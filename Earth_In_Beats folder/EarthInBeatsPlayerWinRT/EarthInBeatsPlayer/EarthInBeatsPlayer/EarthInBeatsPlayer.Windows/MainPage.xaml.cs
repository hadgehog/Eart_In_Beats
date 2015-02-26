using EarthInBeatsNativeLibrary;
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
using MediaData;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using System.Threading.Tasks;

namespace EarthInBeatsPlayer
{
    public sealed partial class MainPage : Page
    {

        Reader player;
        CreatingPlaylist playList;
        private long dur = 0;
        private double cur = 0;
        private double curRewind = 0;
        bool tapped = false;
        Windows.UI.Core.CoreDispatcher dispatcher;

        public MainPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = NavigationCacheMode.Required;
            sliderVolume.Value = 100;
            sliderProgress.Value = 0;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            int stop = 234;
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

        private void OpenButtonClick(object sender, RoutedEventArgs e)
        {
            //play
            if (player != null)
            {
                sliderProgress.Value = 0;
                player.Play();
                this.ResetProgress();
                this.IncreaseProgress();
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
            tapped = false;

            if (player != null)
            {
                player.Stop();
                this.ResetProgress();
            }
        }

        private void Slider1_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (player != null)
            {
                player.Volume((float)e.NewValue / 100);
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            player.Dispose();
            GC.Collect();

            Application.Current.Exit();
        }

        private void sliderProgress_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            tapped = true;
        }

        private void Slider2_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (player != null && tapped)
            {
                curRewind = e.NewValue;
                player.Rewinding((curRewind * dur) / 100);
                tapped = false;
            }
        }

        private async void IncreaseProgress()
        {
            this.dur = player.Duration.Ticks;

            dispatcher = CoreApplication.MainView.Dispatcher;

            while (player.CurrPos() <= this.dur)
            {
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    if (this.tapped)
                    {
                        if (this.curRewind != 0)
                        {
                            sliderProgress.Value = this.curRewind;
                        }
                    }
                    else
                    {
                        cur = player.CurrPos();
                        sliderProgress.Value = (cur * 100.0) / (double)this.dur;
                    }

                });
                await Task.Delay(TimeSpan.FromSeconds(1));
            }

        }

        void ResetProgress()
        {
            if (this.dispatcher != null)
            {
                this.dispatcher.StopProcessEvents();
            }
        }
    }
}
