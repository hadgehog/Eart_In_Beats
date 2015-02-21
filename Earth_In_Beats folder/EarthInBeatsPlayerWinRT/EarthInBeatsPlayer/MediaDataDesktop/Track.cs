using EarthInBeatsNativeLibrary;
using ProtoBuf;

namespace MediaData
{
    //make an interfase in Reader. sent List of Track instead of IPlayList
    public class Track : ITrack
    {
        private int globalPosInPlayList; //song position relatively begin of global playlist duration
        private string trackName;

        public Track()
        {
        }

        public Track(int globalPos, string name)
        {
            globalPosInPlayList = globalPos;
            trackName = name;
        }

        public int Position { get { return globalPosInPlayList; } set { this.globalPosInPlayList = value; } }

        public string Name { get { return trackName; } set { this.trackName = value; } }

        public int GetPosition()
        {
            return globalPosInPlayList;
        }

        public string GetName()
        {
            return trackName;
        }
    }
}
