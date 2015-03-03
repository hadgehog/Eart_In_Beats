using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace MusicMaper.Models
{
    public class PlayerNode
    {
        public PlayerNode(string PhoneID, float Latitude, float Longitude, string Artist, string Title)
        {
            this.PhoneID = PhoneID;
            this.Latitude = Latitude;
            this.Longitude = Longitude;
            this.Artist = Artist;
            this.Title = Title;
        }
        public string PhoneID { get; set; }
        public float Latitude { get; set; }
        public float Longitude { get; set; }
        public string Artist { get; set; }
        public string Title { get; set; }
    }
}