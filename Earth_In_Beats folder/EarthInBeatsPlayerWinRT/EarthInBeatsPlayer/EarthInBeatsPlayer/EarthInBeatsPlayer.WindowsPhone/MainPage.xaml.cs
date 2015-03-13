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
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using System.Threading.Tasks;
using Windows.UI.Input;
using Windows.Storage.Pickers;
using Windows.Storage.AccessCache;
using Windows.UI;
using Windows.UI.Xaml.Documents;

namespace EarthInBeatsPlayer
{ 
    public sealed partial class MainPage : Page
    {
        Reader player;
        CreatingPlaylist playList;
        Windows.UI.Core.CoreDispatcher dispatcher;
        bool updateProgress = true;
        List<string> songs;
        bool needInitFoulder = true;

        public MainPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = NavigationCacheMode.Required;
            sliderVolume.Value = 100;
            sliderProgress.Value = 0;

            this.sliderProgress.PointerPressed += sliderProgress_PointerPressed;
            this.sliderProgress.PointerReleased += sliderProgress_PointerReleased;

            this.sliderProgress.AddHandler(PointerPressedEvent, new PointerEventHandler(sliderProgress_PointerPressed), true);
            this.sliderProgress.AddHandler(PointerReleasedEvent, new PointerEventHandler(sliderProgress_PointerReleased), true);
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await FolderHelper.InitStorages();

            this.needInitFoulder = FolderHelper.LatestFolder == null;

            if (this.needInitFoulder)
            {
                WriteDebugMessage("Audio folder not found! Please select torrents folder.", Colors.Red);
            }
            else
            {
                WriteDebugMessage("Audio folder is: " + FolderHelper.LatestFolder.Path, Colors.Green);
            }
        }

        private async void Create_Playlist_Button_Click(object sender, RoutedEventArgs e)
        {
            if (!this.needInitFoulder)
            {
                FileOpenPicker filePicker = new FileOpenPicker();

                filePicker.ViewMode = PickerViewMode.List;
                filePicker.SuggestedStartLocation = PickerLocationId.ComputerFolder;
                filePicker.FileTypeFilter.Add(".mp3");
                filePicker.FileTypeFilter.Add(".wav");
                filePicker.FileTypeFilter.Add(".wma");
                filePicker.FileTypeFilter.Add(".aac");
                filePicker.FileTypeFilter.Add(".flac");
                filePicker.FileTypeFilter.Add(".mp4");

                var pickedFiles = await filePicker.PickMultipleFilesAsync();

                if (pickedFiles != null)
                {
                    if (this.songs == null)
                    {
                        this.songs = new List<string>();
                    }

                    for (int i = 0; i < pickedFiles.Count; i++)
                    {
                        var file = pickedFiles[i];
                        var path = file.Path;
                        var name = file.Name;

                        this.songs.Add(name);
                    }

                    if (this.player != null)
                    {
                        this.player.Stop();
                        this.player.Dispose();
                        GC.Collect();
                    }

                    //create playlist
                    this.playList = new CreatingPlaylist();
                    this.playList.CreatePlayList(this.songs);

                    //init players list
                    this.player = new Reader();
                    this.player.InitPlayer(playList);

                }
            }
            else
            {
                WriteDebugMessage("Audio folder not found! Please select torrents folder.", Colors.Red);
            }
        }

        private void OpenButtonClick(object sender, RoutedEventArgs e)
        {
            //play
            if (this.player != null)
            {
                sliderProgress.Value = 0;
                this.player.Play();
                this.ResetProgress();
                this.IncreaseProgress();
            }
        }

        private void Previous_Button_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Previous();
            }
        }

        private void Next_Button_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Next();
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            sliderProgress.Value = 0;

            if (this.player != null)
            {
                this.player.Stop();
                this.ResetProgress();
            }
        }

        private void Slider1_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Volume((float)e.NewValue / 100);
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Dispose();
            }

            GC.Collect();

            Application.Current.Exit();
        }

        private async void IncreaseProgress()
        {
            dispatcher = CoreApplication.MainView.Dispatcher;

            while (this.player.CurrPos() <= this.player.Duration.Ticks)
            {
                await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    if (this.updateProgress && this.player != null)
                    {
                        var cur = this.player.CurrPos();
                        sliderProgress.Value = (cur * 100.0) / (double)this.player.Duration.Ticks;
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

        private void sliderProgress_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.updateProgress = false;
            }
        }

        private void sliderProgress_PointerReleased(object sender, PointerRoutedEventArgs e)
        {
            if (this.player != null)
            {
                var id = e.Pointer.PointerId;
                PointerPoint pt = e.GetCurrentPoint(this.sliderProgress);
                var pos = pt.Position.X;

                var rewind = (pos / this.sliderProgress.Width) * this.player.Duration.Ticks;

                if (this.player != null)
                {
                    this.player.Rewinding(rewind);
                }

                this.updateProgress = true;
            }
        }

        private async void Folder_Button_Click(object sender, RoutedEventArgs e)
        {
            var fPiker = new FolderPicker();
            fPiker.FileTypeFilter.Add("*");
            var folder = await fPiker.PickSingleFolderAsync();

            if (folder != null)
            {
                FolderHelper.AddStorage(folder);

                this.needInitFoulder = FolderHelper.LatestFolder == null;

                if (this.needInitFoulder)
                {
                    WriteDebugMessage("Audio folder not found! Please select torrents folder.", Colors.Red);
                }
                else
                {
                    WriteDebugMessage("Audio folder is: " + FolderHelper.LatestFolder.Path, Colors.Green);
                }
            }
        }

        private void WriteDebugMessage(string msg)
        {
            WriteDebugMessage(msg, Colors.White);
        }

        private void WriteDebugMessage(string msg, Color color)
        {
            Run run = new Run();
            Paragraph p = new Paragraph();

            run.Text = "::::  " + msg;
            run.FontSize = 16;
            run.Foreground = new SolidColorBrush(color);

            p.Inlines.Add(run);

            this.DebugTextBlock.Blocks.Add(p);
        }
    }
}
