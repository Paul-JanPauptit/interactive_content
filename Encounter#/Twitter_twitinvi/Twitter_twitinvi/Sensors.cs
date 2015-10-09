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
        private static string[] weather_types = new string[] { "Fair", "Partly Cloudy", "Mist", "Rain" };

        private static string Weather_IFTTT_twitter_module_username = "interactivescrn";

        private static string pick_weather_type_from_twitter_stream(string[] weather_types)
        {
            var user = User.GetUserFromScreenName(Weather_IFTTT_twitter_module_username);
            IEnumerable<ITweet> last_tweets = user.GetUserTimeline(1);

            List<ITweet> last_tweet = last_tweets.ToList();
            string tweet_hold = last_tweet[0].ToString();
            string found = "";
            foreach (string weather in weather_types)
            {
                if (tweet_hold.IndexOf(weather) > -1)
                {
                    found = weather;
                    break;
                }
            }

            return found;
        }

        // Method that returns a character representing sunny or cloudy in local region
        static public char SunnyOrCloudy()
        {
             string weather_type = pick_weather_type_from_twitter_stream(weather_types);

             if (weather_type == "Mist" || weather_type == "Rain" || weather_type == "Partly Cloudy" || weather_type == "Cloudy")
             {
                 return 'C';
             }
             else if (weather_type == "Fair")
             {
                 return 'S';
             }

            //Failsafe
            return 'S';
        }
   }
}
