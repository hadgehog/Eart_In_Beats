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

        public MainPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = NavigationCacheMode.Required;
            sliderVolume.Value = 100;
            sliderProgress.Value = 0;
        }

        private async void SetTrackDuration()
        {
            trackLength = player.Duration.Ticks;

            #if WINDOWS_PHONE_APP
              var dispatcher = CoreApplication.MainView.CoreWindow.Dispatcher;
            #else
              var dispatcher = CoreApplication.MainView.Dispatcher;
            #endif

            if (player != null)
            {
                while (player.CurrPos() <= player.Duration.Ticks)
                {
                    await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        sliderProgress.Value += 100.0 / trackLength;
                    });
                    await Task.Delay(TimeSpan.FromSeconds(1));
                }
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
                sliderProgress.Value = 0;
                player.Play();
                this.SetTrackDuration();
            }

        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
                sliderProgress.Value = 0;
                newPosition = 0;
                if (player != null)
                {
                    player.Stop();
                }
        }

        private void Slider1_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (player != null)
            {
                player.Volume((float)e.NewValue / 100);
            }
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
                var tmp = player.Duration.Ticks;
                player.Rewinding(e.NewValue * (tmp / 100));
                tapped = false;
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            player.Dispose();
            GC.Collect();

            Application.Current.Exit();
        }

        private void Create_Playlist_Button_Click(object sender, RoutedEventArgs e)
        {
            if (player != null)
            {
                player.Stop();
                player.Dispose();
                GC.Collect();
            }

            //create playlist
            playList = new CreatingPlaylist();
            playList.CreatePlayList();

            //init players list
            player = new Reader();
            player.InitPlayer(playList);
        }

        private void Previous_Button_Click(object sender, RoutedEventArgs e)
        {
            if (player != null)
            {
                player.Previous();
            }
        }

        private void Next_Button_Click(object sender, RoutedEventArgs e)
        {
            if (player != null)
            {
                player.Next();
            }
        }
    }
}