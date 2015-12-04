using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using Windows.ApplicationModel;
using Windows.Storage;
using Windows.Storage.Streams;
using EarthInBeatsNativeLibrary;
using ProtoBuf;
using Windows.Storage.FileProperties;
using System.Text;
using Windows.Storage.AccessCache;

namespace MediaData
{
    public class CreatingPlaylist : IPlayList
    {
        private List<Track> trackList;
        private string infoAboutTracks = "";
        List<IRandomAccessStream> streamsToSongs;
        List<StorageFile> files;

        public CreatingPlaylist()
        {
        }

        public CreatingPlaylist(List<Track> trackList)
        {
            this.trackList = trackList;
        }

        //send a vector with song names
        public void CreatePlayList(List<string> songs, List<IRandomAccessStream> streams, List<StorageFile> files)
        {
            this.trackList = new List<Track>();
            this.streamsToSongs = streams;
            this.files = files;

            for (int i = 0; i < songs.Count; i++)
            {
                this.AddTrackInPlayList(i + 1, songs[i]);
            }
        }

        public void AddTrack(List<string> songs, List<IRandomAccessStream> streams, List<StorageFile> files)
        {
            if(this.trackList != null)
            {
                foreach(var stream in streams)
                {
                    this.streamsToSongs.Add(stream);
                }

                foreach(var file in files)
                {
                    this.files.Add(file);
                }
 
                for(int i = 0; i< songs.Count; i++)
                {
                    this.AddTrackInPlayList(this.trackList.Count + i + 1, songs[i]);
                }
            }
        }

        public virtual IRandomAccessStream GetStream(int trackNumber)
        {
            IRandomAccessStream stream = null;

            if (this.streamsToSongs.Count != 0)
            {
                stream = this.streamsToSongs[trackNumber];
            }
            
            return stream;
        }

        //return info about track
        public virtual string GetInfoAboutTrack(int songIndex)
        {
            StorageItemContentProperties contentProperties = files[songIndex].Properties;

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
            this.trackList.Add(track);
        }

        public virtual ITrack GetTrack(int index)
        {
            return this.trackList[index];
        }

        public void SortPlaylist()
        {
            this.trackList.Sort((x, y) => x.Position - y.Position);
        }
    }
}
