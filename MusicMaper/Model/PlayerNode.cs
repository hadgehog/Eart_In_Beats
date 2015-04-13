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


        public string ToStringUrl()
        {
            return PhoneID + "?" + Longitude.ToString() + "?" + Latitude.ToString() + "?" + Artist + "?" + Title;
        }

        static public PlayerNode Parse(string Input)
        {
            PlayerNode answer = new PlayerNode();
            var data = Input.Split(new char[] { '?' });

            answer.PhoneID = data[0];
            answer.Longitude = float.Parse(data[1]);
            answer.Latitude = float.Parse(data[2]);

            if (data.Length == 5)
            {
                answer.Artist = data[3];
                answer.Title = data[4];
            }

            return answer;

        }


    }
}