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
using System.Collections.Generic;

namespace AudioMixApp
{
    public sealed partial class MainPage : Page
    {
        
        Reader player;
        CreatingPlaylist playList;
        double newPosition = 0;
        private long trackLength = 0;
        bool tapped = false;
        private List<Reader> playersList;
        int songNum = 0;

        public MainPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = NavigationCacheMode.Required;
            sliderVolume.Value = 100;
            sliderProgress.Value = 0;
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

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            
        }

        private void OpenButtonClick(Object sender, RoutedEventArgs e)
        {
            // play
            if (player != null)
            {
                player.Play(1);
            }

        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
                sliderProgress.Value = 0;
                newPosition = 0;
                //Stop
                if (player != null)
                {
                    player.Stop();
                }
        }

        private void Slider1_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (player != null)
            {
                //Change volume (float) e.NewValue / 100
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
                //Rewind e.NewValue
                tapped = false;
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            Application.Current.Exit();
        }

        private void Create_Playlist_Button_Click(object sender, RoutedEventArgs e)
        {
            //create playlist
            playList = new CreatingPlaylist();
            playList.CreatePlayList();

            //init players list
            player = new Reader();
            player.InitPlayer(playList);
        }
    }
}