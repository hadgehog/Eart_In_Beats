using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace MusicMaper.Models
{
    public class DeviceInformation
    {
        public string PhoneID { get; set; }
        public float Latitude { get; set; }
        public float Longitude { get; set; }
    }
}