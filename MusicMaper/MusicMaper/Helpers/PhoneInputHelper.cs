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
                case "ReadyToPlay":
                    return PhoneStatus.ReadyToPlay;
                case "Playing":
                    return PhoneStatus.Playing;
                case "Offline":
                    return PhoneStatus.Offline;
                default:
                    return PhoneStatus.Missing;
            }
        }
    }
}