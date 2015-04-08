using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using MusicMaper.Helpers;
using System.Globalization;

namespace MusicMaper.Models
{
    public enum PhoneStatus
    {
        Connect,
        Disconnect,
        Missing,
        Stop,
        Play
    }

    public class PhoneInput
    {
        public PhoneStatus Status;
        public PlayerNode Player;

        //@todo: add check phoneId
        static public PhoneInput Parse(string Input)
        {
            var split = Input.Split(new char[] { '?' });
            PhoneInput result = new PhoneInput();
            if (split.Length > 6 || split.Length == 0)
                throw new FormatException("Can not parse PhoneInput from string! Wrong input string.");

            result.Status = PhoneInputHelper.StringToPhoneStatus(split[0]);
            result.Player = new PlayerNode();

            switch (result.Status)
            {
                case PhoneStatus.Missing:
                    throw new FormatException("Could parse phone status!");
                case PhoneStatus.Disconnect:
                case PhoneStatus.Connect:
                case PhoneStatus.Stop:
                    if (split.Length != 4)
                        throw new FormatException("Wrong input data! Must be: Data=<PhoneStatus>?<PhoneId>?<Lantitude>?<Longtitude>");
                    result.Player.PhoneID = split[1];//need check
                    result.Player.Latitude = float.Parse(split[2], CultureInfo.InvariantCulture);
                    result.Player.Longitude = float.Parse(split[3], CultureInfo.InvariantCulture);
                    break;
                case PhoneStatus.Play:
                    if (split.Length != 6)
                        throw new FormatException("Wrong input data! Must be: Data=<PhoneStatus>?<PhoneId>?<Lantitude>?<Longtitude>?<Artist>?<Title>");
                    result.Player.PhoneID = split[1];//need check
                    result.Player.Latitude = float.Parse(split[2], CultureInfo.InvariantCulture);
                    result.Player.Longitude = float.Parse(split[3], CultureInfo.InvariantCulture);
                    result.Player.Artist = split[4];
                    result.Player.Title = split[5];
                    break;
            }

            return result;
        }
    }
}