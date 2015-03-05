using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using MusicMaper.Helpers;

namespace MusicMaper.Models
{
    public enum PhoneStatus 
    {
        ReadyToPlay,
        Playing,
        Offline,
        Missing
    }

    public class PhoneInput
    {
        public PhoneStatus Status;
        public PlayerNode Player;

        static public PhoneInput Parse(string Input)
        {
            var split = Input.Split(new char[] { '?' });
            PhoneInput result = new PhoneInput();
            if (split.Length != 6)
                throw new FormatException("Can not parse PhoneInput from string! Wrong input string.");

            result.Status = PhoneInputHelper.StringToPhoneStatus(split[0]);

            PlayerNode player = new PlayerNode(
                split[1],
                float.Parse(split[2]),
                float.Parse(split[3]),
                split[4],
                split[5]
                );

            result.Player = player;
                
            return result;
        }
    }
}