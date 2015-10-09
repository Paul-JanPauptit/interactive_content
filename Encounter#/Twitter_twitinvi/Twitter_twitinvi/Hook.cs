using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Twitter_twitinvi
{
    public class Hook
    {
        
        public Hook(System.Data.DataRow values)
        {
            this.text = values[0].ToString();
            this.category = values[1].ToString();
            this.weather = values[2].ToString();
            this.timeofday = values[3].ToString();
            this.font = values[4].ToString();

        }

        public string text { get; set; }
        public string font { get; set; }
        public string category { get; set; }
        public string[] associatedLandmarks { get; set; }

        public string weather { get; set; }
        public string timeofday { get; set; }
        
    }
}
