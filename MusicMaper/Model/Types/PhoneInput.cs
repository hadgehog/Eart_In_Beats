using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
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

    public class DeviceInput
    {
        public PhoneStatus Status;
        public DeviceInformation Device;
        public TrackInformation Track;
    }
}