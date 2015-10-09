using System;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Linq;
using System.Reflection;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Collections.Generic;
using Tweetinvi;
using Tweetinvi.Core;
using Tweetinvi.Core.Enum;
using Tweetinvi.Core.Extensions;
using Tweetinvi.Core.Interfaces;
using Tweetinvi.Core.Interfaces.Controllers;
using Tweetinvi.Core.Interfaces.DTO;
using Tweetinvi.Core.Interfaces.Models;
using Tweetinvi.Core.Interfaces.Models.Parameters;
using Tweetinvi.Core.Interfaces.oAuth;
using Tweetinvi.Core.Interfaces.Streaminvi;
using Tweetinvi.Json;
using Stream = Tweetinvi.Stream;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using System.Xml;

using System.Data;

using CSNamedPipe;
using System.Data;
using Excel;
using Microsoft.Office.Interop.Excel;

using MyClass.WriteToExcel;

using LogWriter;


namespace Twitter_twitinvi
{
    static class Globals
    {
        static public string Weather_IFTTT_twitter_module_username = "interactivescrn";
    }
    
    // This is the Twitter API module for the interactive content project.
    // Takes in input from a JSON file and applies the extracted context profile to analyse 
    // twitter streams with relevant filters 
    
    class Program
    {
    
        static string rootdirectory = @"C:\Users\nikhilbanerjee\Desktop\";

        static void Main(string[] args)
        {
            // Calling function when the application exits to clean up buffer files and folder
            AppDomain.CurrentDomain.ProcessExit += new EventHandler(OnProcessExit);
            //AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(OnUnhandledException);

            try
            {
                RunTwitterApplication();
            }

            catch (Exception e)
            {
                LogWriter.LogWriter.WriteLog(e);
                Process.Start(rootdirectory + @"Encounter#\Twitter_twitinvi\Twitter_twitinvi\bin\Debug\Twitter_twitinvi.exe");
                               
            }
        }

        private static void RunTwitterApplication()
        {
                // BLOCK 1 : Twitter credentials
                // Twitter Credentials: Reading in a json configuration file to set in credential values and other twitter search parameters
                JObject TwitterCredentialsJson = JObject.Parse(File.ReadAllText( rootdirectory + @"Project Encounter\Package\Config_Interfaces\Twitter_context_profile.json"));
                // Locally reading and storing all credential values
                string consumer_key = (string)TwitterCredentialsJson["consumer_key"];
                string consumer_secret = (string)TwitterCredentialsJson["consumer_secret"];
                string access_token = (string)TwitterCredentialsJson["access_token"];
                string access_token_secret = (string)TwitterCredentialsJson["access_token_secret"];
                // Setting twitter credentials
                TwitterCredentials.SetCredentials(access_token, access_token_secret, consumer_key, consumer_secret);



                // BLOCK 2: Populating Landmarks
                // Import excel worksheet for landmark initialization (Name, Categories, Hashtag_trackers, Thematic_Metatags)
                // Reading excel worksheet using exceldatareader nuget package
                FileStream stream = File.Open(rootdirectory + @"Project Encounter\Package\Config_Interfaces\Landmarks_Categories.xlsx", FileMode.Open, FileAccess.Read);
                IExcelDataReader excelReader = ExcelReaderFactory.CreateOpenXmlReader(stream);
                DataSet result = excelReader.AsDataSet();
                // This should initialize all the parameters in the Lanmarks object array using its contructor
                int numberoflandmarks = result.Tables[0].Rows.Count - 1;
                Landmark[] Landmarks = new Landmark[numberoflandmarks];
                for (int i = 0; i < numberoflandmarks; i++)
                {
                    Landmarks[i] = new Landmark(result.Tables[0].Rows[i]);
                }


                //BLOCK 3: Hook population
                // Reading in datatable which contains hooks and their properties

                int numberofhooks = result.Tables[1].Rows.Count - 1;
                Hook[] Hooks = new Hook[numberofhooks];
                for (int i = 0; i < numberofhooks; i++)
                {
                    Hooks[i] = new Hook(result.Tables[1].Rows[i]);
                }


                ////BLOCK 4: Twitter Stream trackers
                //// Creating twitter stream for detecting tweets with landmark names
                var stream_for_landmark = Stream.CreateFilteredStream();
                // Adding trackers for each landmark
                foreach (Landmark obj in Landmarks)
                {
                    string[] landmark_hashtags_store = obj.Hashtag_trackers;
                    // Loop block to assign trackers to each relevant hashtag element in the twitter stream
                    foreach (string hashtag in landmark_hashtags_store)
                    {
                        stream_for_landmark.AddTrack(hashtag);
                    }
                }

                //var stream_for_landmark = Stream.CreateFilteredStream();
                //stream_for_landmark.AddTrack("volkshotel");

                // BLOCK 5: Landing zone and context aware parameters
                LandingZone landingzone = new LandingZone();

                char DayorNight = landingzone.DayOrNight();
                char Weather = Sensors.WeatherSense.SunnyOrCloudy();


                //BLOCK 5: Event trigger for tracked tweets

                // Event trigger block for each new tweet on the above location based landmark triggers
                // String array to store all different weather conditions

                // Hook module : Time of the day - categorize as morning, afternoon and evening
                // Hook is overlayed on the video - in the center of the video

                // Video module : Videos are named as landmark and weather 
                // Weather detection module : Must read through twitter stream of test account -> Assign a hastag weather -> could filter out the

                List<Hook> HooksforExcel = Hooks.ToList<Hook>();

                //ExcelFileWriter<Hook> myExcel = new ExcelWrite();
                //myExcel.WriteDateToExcel(@"C:\TEMP\myExcel.xls", HooksforExcel, "A1", "D1");

                // Landmark is detected by trackers
                // Landmark is pushed to method for extracting content from DB
                // This can happen either here or in openframeworks
                // Once content is extracted it must be sequenced on the basis of :
                // Time to play each item : Once one item is played for a certain preset amount of time extract the next from DB
                // Time calculation in openframeworks:
                // Update loop and draw loop

                // The sensing modules exist on this side. So Hooks are selected here and published onto the interface between OF and Encounter#
                // Hooks are read by OF and sequenced according to time parameters in OF

                //Two kinds of time parameters in OF:
                // Amount of time video must be played for
                // Amount of time a hook should be shown for

                // How does timing fit into the update and draw cycles?
                // Use a timer, counting seconds until a value is reached
                // Validation for time count can be executed at every loop update 

                // Update should contain a timing module where all the timing logic can be placed

                // Objects required in OF
                // OF has to handle video placement, playback and sequencing 
                // OF has to handle Hook placement, sequencing and timing
                // OF has to handle visual blocks with animated elements 


                Console.WriteLine("Stream detection live now:");
                stream_for_landmark.MatchingTweetReceived += (sender, arg) =>
                {

                    // Storing the tweet 
                    string tweet = arg.Tweet.ToString();

                    // Searching through tweet to find relevant landmark
                    Landmark detectedlandmark = Find_tweeted_landmark(Landmarks, tweet);

                    // Getting the hook categories that the landmark satisfies
                    string[] relevantHookCategories = detectedlandmark.Categories;

                    // Writing detected landmark to text file interface between here and OF
                    using (StreamWriter writer = new StreamWriter(rootdirectory + @"Project Encounter\Package\Config_Interfaces\TriggeredLandmark.txt", false))
                    {
                        writer.Write(detectedlandmark.Name);
                    }

                    // Selecting hooks that belong to the relevant categories corresponding to the landmark
                    List<Hook> RelevantHooks = new List<Hook>();
                    foreach (string category in relevantHookCategories)
                    {
                        RelevantHooks.AddRange((from item in Hooks
                                                where item.category == category
                                                select item).ToArray());
                    }

                    // Storing current sensor values
                    DayorNight = landingzone.DayOrNight();
                    Weather = Sensors.WeatherSense.SunnyOrCloudy();

                    // Further filtering hooks based on sensor values
                    List<Hook> RelevantHooksSensorFilters = new List<Hook>();
                    RelevantHooksSensorFilters.AddRange(from hook in RelevantHooks
                                                        where hook.timeofday.Contains(DayorNight) && hook.weather.Contains(Weather)
                                                        select hook);

                    // Writing the selected hooks to an XML to be read in by OF
                    using (XmlWriter writer = XmlWriter.Create(rootdirectory + @"Project Encounter\of_v0.8.4_vs_release\apps\myApps\OpenEncounters\bin\dataHooks.xml"))
                    {
                        writer.WriteStartDocument();
                        writer.WriteStartElement("Filtered_hooks");
                        foreach (Hook item in RelevantHooksSensorFilters)
                        {
                            writer.WriteStartElement("Hook");
                            writer.WriteElementString("text", item.text);
                            writer.WriteElementString("category", item.category);
                            writer.WriteElementString("weather", item.weather);
                            writer.WriteElementString("timeofday", item.timeofday);
                            writer.WriteElementString("font", item.font);
                            writer.WriteEndElement();
                        }
                        writer.WriteEndElement();
                        writer.WriteEndDocument();
                    }

                    Console.WriteLine(tweet);
                    Console.WriteLine();
                    Console.Write("Landmark Triggered:"); Console.Write(detectedlandmark.Name);
                    Console.WriteLine();

                    //System.Threading.Thread.Sleep(
                        //(int)System.TimeSpan.FromSeconds(30).TotalMilliseconds);
                };

                // Starting twitter stream detection
                stream_for_landmark.StartStreamMatchingAllConditions();

            }
        
        // To be called at the end of runtime to clear out interface files
        static void OnProcessExit(object sender, EventArgs e)
        {
            Console.WriteLine("Starting clean up...");

            // Cleaning up text file for landmark buffer
            File.WriteAllText( rootdirectory + @"Project Encounter\Package\Config_Interfaces\TriggeredLandmark.txt", String.Empty);

        }

        // Method for comma seperated value extration
        static public string[] splitstring(string csv)
        {
            string[] values = csv.Split(',');
            for (int i = 0; i < values.Length; i++)
            {
                values[i] = values[i].Trim();
            }

            return values;
        }

        // Class definition for storing location coordinates
        class Coordinates : ICoordinates
        {
            public double Latitude { get; set; }
            public double Longitude { get; set; }

            public Coordinates(double p1, double p2)
            {
                this.Latitude = p1;
                this.Longitude = p2;
            }
        }
        
        //Class definition for storing locations
        class Location : ILocation
        {
            public ICoordinates Coordinate1 { get; set; }
            public ICoordinates Coordinate2 { get; set; }

            public Location(Coordinates c1, Coordinates c2)
            {
                this.Coordinate1 = c1;
                this.Coordinate2 = c2;
            }
        }

        // Method for returning the location/landmark for the mathched hashtag : return type string for landmark that will be matched in the switch statment
        static public Landmark Find_tweeted_landmark(Landmark[] landmarks, string tweet)
        {
            tweet = tweet.ToLower();
            Landmark found = new Landmark();
            // Searching for each hashtag trigger for every landmark in tweet to find the associated landmark to trigger videos and hooks
            foreach (Landmark landmark in landmarks )
            {
                foreach (string hashtag in landmark.Hashtag_trackers)
                {
                    if (tweet.IndexOf(hashtag) > -1)
                    {
                        found = landmark;
                        break;
                    }
                }
                
            }
                       
            return found;
        }

    }
}





// TWITTER LOCATION STUFF FOR FUTURE USE

//Location mapping
//Array.Clear(values, 0, values.Length);

//string top_left_coordinates = (string)JsonContextProfile["location_topleft"];
//values = splitstring(top_left_coordinates);

//double top_left_Latitude = double.Parse(values[0], System.Globalization.CultureInfo.InvariantCulture);
//double top_left_Longitude = double.Parse(values[1], System.Globalization.CultureInfo.InvariantCulture);

//var coordinate1 = new Coordinates(top_left_Latitude, top_left_Longitude);

//Array.Clear(values, 0, values.Length);

//string bottom_right_coordinates = (string)JsonContextProfile["location_bottomright"];
//values = splitstring(bottom_right_coordinates);

//double bottom_right_Latitude = double.Parse(values[0], System.Globalization.CultureInfo.InvariantCulture);
//double bottom_right_Longitude = double.Parse(values[1], System.Globalization.CultureInfo.InvariantCulture);

//var coordinate2 = new Coordinates(bottom_right_Latitude, bottom_right_Longitude);

//var location = new Location(coordinate1, coordinate2);
//stream.AddLocation(location);