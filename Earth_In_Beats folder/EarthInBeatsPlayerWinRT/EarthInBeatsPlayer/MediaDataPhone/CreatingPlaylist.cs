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
        StorageFolder folder;

        public CreatingPlaylist()
        {
        }

        public CreatingPlaylist(List<Track> trackList)
        {
            this.trackList = trackList;
        }

        //global positions sets outside
        public async void CreatePlayList(List<string> songs, string accessFolder)
        {
            folder = await StorageApplicationPermissions.FutureAccessList.GetFolderAsync(accessFolder);

            trackList = new List<Track>();

            for (int i = 0; i < songs.Count; i++)
            {
                AddTrackInPlayList(i + 1, songs[i]);
            }
        }

        public virtual IRandomAccessStream GetStream(int trackNumber)
        {
            var t = this.folder.GetFileAsync(trackList[trackNumber].GetName()).AsTask();
            t.Wait();
            StorageFile file = t.Result;

            var t2 = file.OpenAsync(FileAccessMode.Read).AsTask();
            t2.Wait();

            return t2.Result;
        }

        //return info about track
        public virtual string GetInfoAboutTrack(int trackNumber)
        {
            var t = this.folder.GetFileAsync(trackList[trackNumber].GetName()).AsTask();
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
