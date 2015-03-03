using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data.Entity;
using MusicMaper.Models;
using MusicMaper.DAL;

namespace MusicMaper.Logic
{
    static public class TrackManager
    {
        static TrackContext db = new TrackContext();

        static public void Update(PhoneInput Phone)
        {
            switch (Phone.Status)
            {
                case PhoneStatus.ReadyToPlay:
                    Add(Phone.Player);
                    break;
                case PhoneStatus.Playing:
                    Edit(Phone.Player);
                    break;
                case PhoneStatus.Offline:
                    Remove(Phone.Player);
                    break;
                default:
                    break;
            }
        }

        static void Add(PlayerNode Player)
        {
            db.Players.Add(Player);
            db.SaveChanges();
        }
        static void Edit(PlayerNode Player)
        {

        }
        static void Remove(PlayerNode Player)
        {
            db.Players.Remove(Player);
            db.SaveChanges();
        }
    }
}