using MusicMaper.DAL;
using MusicMaper.Models;
using System.Data.Entity;

namespace MusicMaper.Logic
{
    static public class TrackManager
    {
        static TrackContext db = new TrackContext();

        static public void Update(PhoneInput Phone)
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

        static void Add(PlayerNode Player)
        {
            var player = db.Players.Find(Player.PhoneID);
            if (player == null)
            {
                db.Players.Add(Player);
                db.SaveChanges();
            }
        }
        static void Edit(PlayerNode Player)
        {
                var player = db.Players.Find(Player.PhoneID);
                if (player != null)
                {
                    db.Entry(Player).State = EntityState.Modified;
                    db.SaveChanges();
                }
        }
        static void Remove(PlayerNode Player)
        {
            var player = db.Players.Find(Player.PhoneID);
            if (player != null)
            {
                db.Players.Remove(Player);
                db.SaveChanges();
            }
        }
    }
}