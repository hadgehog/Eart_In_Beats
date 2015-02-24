using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using EarthInBeatsNativeLibrary;
using MediaData;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace EarthInBeatsPlayer
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

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            
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

        private void OpenButtonClick(object sender, RoutedEventArgs e)
        {
            //play
            if (player != null)
            {
                sliderProgress.Value = 0;
                player.Play();
            }
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
    }
}
