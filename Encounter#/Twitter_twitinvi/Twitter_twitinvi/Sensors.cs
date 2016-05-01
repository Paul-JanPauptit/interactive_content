using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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

namespace Sensors
{
    // Weather sensing module : IFTTT and twitter stream combination
    // The IFTTT module triggers a tweet on the twitter feed dedicated for this purpose
    // The method then reads the most recent tweet on the feed and detects the weather

    public static class WeatherSense
    {
        private static string[] weather_types = new string[] { "Fair", "Clear", "Cloudy", "Mist", "Rain", "Snow" };

        private static int lastCallMSec = 0;
        private static string lastTweet = "";

        private static string PickWeatherTypeFromTwitterStream(string accountName, string[] weather_types)
        {
            string found = "";
            int now = Environment.TickCount;
            if (now - lastCallMSec > 60 * 1000)
            {
                var user = User.GetUserFromScreenName(accountName);
                if (user == null)
                {
                    var exceptionDetails = ExceptionHandler.GetLastException();
                    Console.WriteLine("ERROR: Cannot find Twitter user '" + accountName + "', ignoring weather type: " + exceptionDetails.ToString());
                }
                else
                {
                    lastCallMSec = now;
                    IEnumerable<ITweet> last_tweets = user.GetUserTimeline(1);

                    List<ITweet> last_tweet = last_tweets.ToList();
                    lastTweet = last_tweet[0].ToString();
                    Console.WriteLine("Found Tweet '" + lastTweet + "'");
                }
            }
            foreach (string weather in weather_types)
            {
                if (lastTweet.IndexOf(weather) > -1)
                {
                    found = weather;
                    break;
                }
            }
            return found;
        }

        // Method that returns a character representing sunny or cloudy in local region
        static public char SunnyOrCloudy(string accountName)
        {
            string weather_type = PickWeatherTypeFromTwitterStream(accountName, weather_types);

            if (weather_type == "Mist" || weather_type == "Rain" || weather_type == "Cloudy" || weather_type == "Snow")
            {
                return 'C';
            }
            else 
                return 'S';
        }
   }
}
