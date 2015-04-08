using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace MusicMaper.Models
{
    public class PlayerNode
    {
        public PlayerNode()
        {

        }
        public PlayerNode(string PhoneID, float Latitude, float Longitude, string Artist, string Title)
        {
            this.PhoneID = PhoneID;
            this.Latitude = Latitude;
            this.Longitude = Longitude;
            this.Artist = Artist;
            this.Title = Title;
        }
        [Key]
        public int Id { get; set; }
        public string PhoneID { get; set; }
        public float Latitude { get; set; }
        public float Longitude { get; set; }
        public string Artist { get; set; }
        public string Title { get; set; }
    }
}