using System;
using System.IO;
using System.Threading;
using Windows.ApplicationModel;
using Windows.Foundation.Collections;
using Windows.Media.Playback;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Navigation;
using Windows.Graphics.Imaging;
using MediaData;
using MediaExtension;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;

namespace AudioMixApp
{
    public sealed partial class MainPage : Page
    {
        
        Reader player;
        CreatingPlaylist playList;
        double newPosition = 0;
        private MediaPlayer mediaPlayer;
        private long trackLength = 0;
        bool tapped = false;

        public MainPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = NavigationCacheMode.Required;
            sliderVolume.Value = 100;
            sliderProgress.Value = 0;
        }

        private void MessageReceivedFromBackground(object sender, MediaPlayerDataReceivedEventArgs e)
        {
            ValueSet valueSet = e.Data;
            foreach (string key in valueSet.Keys)
            {
                switch (key)
                {
                    case "ExistTrue":
                        CheckPlayList((byte[])valueSet[key]);
                        break;
                    case "Duration":
                        SetTrackDuration((long)valueSet[key]);                   
                        break;
                }
            }
        }

        private async void SetTrackDuration(long duration)
        {
            trackLength = duration;

            #if WINDOWS_PHONE_APP
                        var dispatcher = CoreApplication.MainView.CoreWindow.Dispatcher;
            #else
              var dispatcher = CoreApplication.MainView.Dispatcher;
            #endif

            for (int i = 1; i < trackLength; i++)
            {
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    sliderProgress.Value += 100.0 / trackLength;
                });
                await Task.Delay(TimeSpan.FromSeconds(1));
            }
        }

        private void CheckPlayList(byte[] serialized) 
        {
            using (var ms = new MemoryStream(serialized))
            {
                CreatingPlaylist tmp = CreatingPlaylist.DeSerialize(ms);

                if (tmp.Tracklist != null)
                {
                    if (tmp.Tracklist.Count != 0)
                    {
                        playList = CreatingPlaylist.DeSerialize(ms);
                    }
                }
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            mediaPlayer = BackgroundMediaPlayer.Current;
            var messageToBackground = new ValueSet { { "Background", "Is background existing" } };
            BackgroundMediaPlayer.SendMessageToBackground(messageToBackground);
            BackgroundMediaPlayer.MessageReceivedFromBackground += MessageReceivedFromBackground;
        }

        private void OpenButtonClick(Object sender, RoutedEventArgs e)
        {
            byte[] serialized;

            if (playList == null)
            {
                playList = new CreatingPlaylist();
                playList.CreatePlayList();
            }

            using (var ms = new MemoryStream())
            {
                serialized = playList.Serialize(ms);
            }

            var messageToBackground = new ValueSet { { "Play", serialized } };
            BackgroundMediaPlayer.SendMessageToBackground(messageToBackground);

            player = new Reader();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
                sliderProgress.Value = 0;
                newPosition = 0;
                var messageToBackground = new ValueSet { { "Stop", 0 } };
                BackgroundMediaPlayer.SendMessageToBackground(messageToBackground);
        }

        private void Slider1_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (player != null)
            {
                var messageToBackground = new ValueSet {{"Volume", (float) e.NewValue / 100}};
                BackgroundMediaPlayer.SendMessageToBackground(messageToBackground);
            }
        }

        public void ResetProgress(long duration)
        {
            sliderProgress.Value = 0;
            trackLength = duration;
        }

        private void sliderProgress_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            tapped = true;
        }

        private void Slider2_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (player != null && tapped)
            {
                newPosition = e.NewValue;
                var messageToBackground = new ValueSet { { "Rewind", e.NewValue } };
                BackgroundMediaPlayer.SendMessageToBackground(messageToBackground);
                tapped = false;
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            Application.Current.Exit();
        }
    }
}