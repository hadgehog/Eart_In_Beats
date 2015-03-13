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
                new PlayerNode("1234567890", 0, 0, "Artist1", "Title1"),
                new PlayerNode("1234567890", 0, 0, "Artist2", "Title2"),
            };
            players.ForEach(o => context.Players.Add(o));

            context.SaveChanges();
        }
    }
}