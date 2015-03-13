using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data.Entity;
using MusicMaper.Models;

namespace MusicMaper.DAL
{
    public class TrackContexInit : DropCreateDatabaseIfModelChanges<TrackContext>
    {
        protected override void Seed(TrackContext context)
        {
            var players = new List<PlayerNode>
            {
                new PlayerNode{ PhoneID = "1234567890", Latitude = 0, Longitude = 0, Artist = "Artist1", Title = "Title1" },
                new PlayerNode{ PhoneID = "1234567890", Latitude = 0, Longitude = 0, Artist = "Artist2", Title = "Title2" },
            };
            players.ForEach(o => context.Players.Add(o));

            context.SaveChanges();
        }
    }
}