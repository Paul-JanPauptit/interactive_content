using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data;

namespace Twitter_twitinvi
{
    // To generalize this further think of what the purpopse of the landmark is

    public class Landmark
    {
        // Use a Landmark contructor to intialize properties such as thematic messages and hashtag trackers by importing values from config files

        public Landmark()
        {

        }

        public Landmark(System.Data.DataRow values)
        {
            // Initialize Name, Categories, thematic_metatags and hashtag_trackers

            this.Name = values[0].ToString();
            this.Categories = Program.splitstring(values[1].ToString());
            this.Thematic_metatags = Program.splitstring(values[2].ToString());
            this.Hashtag_trackers = Program.splitstring(values[3].ToString());
                        
        }

        public string Name {get; set;}
        public string[] Categories;
        public string[] Thematic_metatags; // For open archives video retrieval 
        public string[] Hashtag_trackers;  // Hashtags for twitter stream tracking

                
    }
}
