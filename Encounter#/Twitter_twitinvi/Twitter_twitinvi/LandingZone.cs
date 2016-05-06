using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sensors;

namespace Twitter_twitinvi
{
    public class LandingZone
    {
        // This object contains information about the landing zone and also sensory data

        // Two sensor values for now: Time and Weather

        public char DayOrNight()
        {
            if (DateTime.Now.Hour < 20)
                return 'D';
            else
                return 'N';
        }


        public char MorningOrAfternoon()
        {
            if (DateTime.Now.Hour < 12)
                return 'M';
            else
                return 'A';
        }
    }
}
