using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using MusicMaper.Models;

namespace MusicMaper.Helpers
{
    public static class PhoneInputHelper
    {
        static public PhoneStatus StringToPhoneStatus(string Input)
        {
            switch (Input)
            {
                case "Connect":
                    return PhoneStatus.Connect;
                case "Play":
                    return PhoneStatus.Play;
                case "Stop":
                    return PhoneStatus.Stop;
                case "Disconnect":
                    return PhoneStatus.Disconnect;
                default:
                    return PhoneStatus.Missing;
            }
        }
    }
}