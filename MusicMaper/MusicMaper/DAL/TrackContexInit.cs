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
            context.Players.Add(new PlayerNode("1234567890", 0, 0, "Artist1", "Title1"));
            context.Players.Add(new PlayerNode("1234567890", 0, 0, "Artist2", "Title2"));

            context.SaveChanges();
        }
    }
}