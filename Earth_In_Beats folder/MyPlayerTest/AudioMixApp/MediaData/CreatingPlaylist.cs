using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using Windows.ApplicationModel;
using Windows.Storage;
using Windows.Storage.Streams;
using MediaExtension;
using ProtoBuf;
using Windows.Storage.FileProperties;
using System.Text;

namespace MediaData
{
    public class CreatingPlaylist : IPlayList
    {
        private List<Track> trackList;
        private string infoAboutTracks = "";

        public CreatingPlaylist()
        {
        }

        public CreatingPlaylist(List<Track> trackList)
        {
            this.trackList = trackList;
        }

        //global positions sets outside
        public void CreatePlayList()
        {
            trackList = new List<Track>();

            AddTrackInPlayList(2, "Assets\\02 Quutamo.mp3");
            AddTrackInPlayList(3, "Assets\\02 - Master of Puppets.mp3");
            AddTrackInPlayList(1, "Assets\\1 Caroline Duris - Barrage(original mix).mp3");
            AddTrackInPlayList(5, "Assets\\9d64d647715f9e.mp3");
            AddTrackInPlayList(4, "Assets\\a9d220db4fcf.mp3");
        }

        public virtual IRandomAccessStream GetStream(int trackNumber)
        {
            var t = Package.Current.InstalledLocation.GetFileAsync(trackList[trackNumber].GetName()).AsTask();
            t.Wait();
            StorageFile file = t.Result;

            var t2 = file.OpenAsync(FileAccessMode.Read).AsTask();
            t2.Wait();

            return t2.Result;
        }

        //return info about track
        public virtual string GetInfoAboutTrack(int trackNumber)
        {
            var t = Package.Current.InstalledLocation.GetFileAsync(trackList[trackNumber].GetName()).AsTask();
            t.Wait();
            StorageFile file = t.Result;

            StorageItemContentProperties contentProperties = file.Properties;

            var musicProperties = contentProperties.GetMusicPropertiesAsync().AsTask();
            musicProperties.Wait();

            MusicProperties mP = musicProperties.Result;
            infoAboutTracks = "Track name: " + mP.Title + ", Artist: " + mP.Artist + ", Album: " + mP.Album;

            return infoAboutTracks;
        }

        public virtual bool CheckNext(int currentNumber)
        {
            if (trackList[currentNumber + 1].GetName() != "" && trackList[currentNumber + 1] != null)
                return true;
            return false;
        }

        public virtual int GetPlayListLength()
        {
            return trackList.Count;
        }

        private void AddTrackInPlayList(int trackNumber, string trackName)
        {
            var track = new Track(trackNumber, trackName);
            trackList.Add(track);
        }


        public ITrack GetTrack(int index)
        {
            return this.trackList[index];
        }

        public void SortPlaylist()
        {
            this.trackList.Sort((x, y) => x.Position - y.Position);
        }
    }
}
