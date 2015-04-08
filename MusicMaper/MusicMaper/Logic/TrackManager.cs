using MusicMaper.DAL;
using MusicMaper.Models;
using System.Data.Entity;
using System.Data.Entity.Core;
using System.Data.Entity.Core.Objects;
using System.Linq;

namespace MusicMaper.Logic
{
    public class TrackManager
    {
        TrackContext db = new TrackContext();

        public void Update(PhoneInput Phone)
        {
            switch (Phone.Status)
            {
                case PhoneStatus.Connect:
                    Add(Phone.Player);
                    break;
                case PhoneStatus.Play:
                    Edit(Phone.Player);
                    break;
                case PhoneStatus.Stop:
                    Edit(Phone.Player);
                    break;
                case PhoneStatus.Disconnect:
                    Remove(Phone.Player);
                    break;
                default:
                    break;
            }
        }

        void Add(PlayerNode Player)
        {
            var player = from o in db.Players
                         where o.PhoneID == Player.PhoneID
                         select o;

            if (player.Count() == 0)
            {
                db.Players.Add(Player);
                db.SaveChanges();
            }
        }
        void Edit(PlayerNode Player)
        {
            var db = new TrackContext();
            var player = (from o in db.Players
                         where o.PhoneID == Player.PhoneID
                         select o).FirstOrDefault();

            player.Artist = Player.Artist;
            player.Latitude = Player.Latitude;
            player.Longitude = Player.Longitude;
            player.Title = Player.Title;
                
            db.SaveChanges();
        }
        void Remove(PlayerNode Player)
        {
            var player = (from o in db.Players
                          where o.PhoneID == Player.PhoneID
                          select o).FirstOrDefault();

            if (player != null)
            {
                db.Players.Remove(player);
                db.SaveChanges();
            }
        }
    }
}