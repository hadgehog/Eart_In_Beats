using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicMaper.Models;

namespace Business.DAL
{
    public class DefaultRepository
    {
        DataBaseContext context = new DataBaseContext();

        public void Processing(DeviceInput Data)
        {
            switch (Data.Status)
            {
                case PhoneStatus.Connect:
                    connect(Data);
                    break;
                case PhoneStatus.Disconnect:
                    disconnect(Data);
                    break;
                case PhoneStatus.Stop:
                    stop(Data);
                    break;
                case PhoneStatus.Play:
                    play(Data);
                    break;
                case PhoneStatus.Missing:
                    break;
                default:
                    break;
            }
        }
        public TrackInformation[] GetTacks(DeviceInformation Device, float Latitude, float Longitude, float Radius)
        {
            var session = context.Sessions.FirstOrDefault(o => o.Device.PhoneID == Device.PhoneID);
            if (session == null)
            {
                return new TrackInformation[0];
            }

            var tracks = from o in context.Sessions
                         where o.Track.Artist == session.Track.Artist &&
                                o.Track.Title == session.Track.Title &&
                                Math.Sqrt((o.Device.Latitude - Device.Latitude) * (o.Device.Latitude - Device.Latitude) + (o.Device.Longitude - Device.Longitude) * (o.Device.Longitude - Device.Longitude)) < Radius
                         select o.Track;

            return tracks.ToArray();

        }
        void connect(DeviceInput Data)
        {
            var session = context.Sessions.FirstOrDefault(o => o.Device.PhoneID == Data.Device.PhoneID);
            if (session != null)
                disconnect(Data);

            Session newSession = new Session()
            {
                Connected = DateTime.Now,
                Device = Data.Device,
                Track = new TrackInformation()
            };

            context.Sessions.Add(newSession);
            context.SaveChanges();
        }
        void disconnect(DeviceInput Data)
        {
            var session = context.Sessions.FirstOrDefault(o => o.Device.PhoneID == Data.Device.PhoneID);
            if (session == null)
                throw new InvalidOperationException("Session does not exist.");

            context.Sessions.Remove(session);
            context.SaveChanges();
        }
        void stop(DeviceInput Data)
        {
            var session = context.Sessions.FirstOrDefault(o => o.Device.PhoneID == Data.Device.PhoneID);
            if (session == null)
            {
                connect(Data);
                session = context.Sessions.FirstOrDefault(o => o.Device.PhoneID == Data.Device.PhoneID);
            }

            session.Track = null;
            context.SaveChanges();
        }
        void play(DeviceInput Data)
        {
            var session = context.Sessions.FirstOrDefault(o => o.Device.PhoneID == Data.Device.PhoneID);
            if (session == null)
            {
                connect(Data);
                session = context.Sessions.FirstOrDefault(o => o.Device.PhoneID == Data.Device.PhoneID);
            }

            session.Track = Data.Track;
            context.SaveChanges();
        }
    }
}
