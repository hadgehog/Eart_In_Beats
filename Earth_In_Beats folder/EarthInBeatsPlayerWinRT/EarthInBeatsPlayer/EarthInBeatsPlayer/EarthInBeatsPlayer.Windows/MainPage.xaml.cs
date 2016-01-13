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
using Windows.UI.Input;
using Windows.Storage.Pickers;
using Windows.Storage.AccessCache;
using Windows.UI;
using Windows.UI.Xaml.Documents;
using Windows.Storage.Streams;
using Windows.Storage;

namespace EarthInBeatsPlayer
{
    public sealed partial class MainPage : Page
    {

        Reader player;
        CreatingPlaylist playList;
        Windows.UI.Core.CoreDispatcher dispatcher;
        bool updateProgress = true;
        bool isPlayingNow = false;

        EarthInBeatsNativeLibrary.Renderer renderer;
        EarthInBeatsNativeLibrary.EarthRenderableWinRT earthRenderable;

        public MainPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = NavigationCacheMode.Required;
            this.sliderVolume.Value = 100;
            this.sliderProgress.Value = 0;

            this.sliderProgress.PointerPressed += sliderProgress_PointerPressed;
            this.sliderProgress.PointerReleased += sliderProgress_PointerReleased;

            this.sliderProgress.AddHandler(PointerPressedEvent, new PointerEventHandler(sliderProgress_PointerPressed), true);
            this.sliderProgress.AddHandler(PointerReleasedEvent, new PointerEventHandler(sliderProgress_PointerReleased), true);
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            this.earthRenderable = new EarthInBeatsNativeLibrary.EarthRenderableWinRT();
            this.renderer = new EarthInBeatsNativeLibrary.Renderer();

            this.renderer.Initialize(Window.Current.CoreWindow, this.swapChainPanel, this.earthRenderable);
            this.renderer.BackgroundColor = Windows.UI.Colors.Black;

            WriteDebugMessage("To select songs press Win + Z", Colors.Yellow);

            await Task.Run(async () =>
             {
                 string path = "Assets\\Earth3.obj";
                 string pathTex = "Assets\\Earth_tex2.dds";
                 string bgTexPath = "Assets\\space_background.dds";

                 await this.earthRenderable.LoadBackgroundTexture(bgTexPath);
                 await this.earthRenderable.LoadModelTexture(pathTex);
                 await this.earthRenderable.Load3DModel(path);
             });
        }

        private void Slider1_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Volume((float)e.NewValue / 100);
            }
        }

        private async void IncreaseProgress()
        {
            this.dispatcher = CoreApplication.MainView.Dispatcher;

            while (this.player != null && this.playList != null && this.player.CurrPos() <= this.player.Duration.Ticks)
            {
                await this.dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    if (this.updateProgress && this.player != null)
                    {
                        var cur = this.player.CurrPos();
                        this.sliderProgress.Value = (cur * 100.0) / (double)this.player.Duration.Ticks;
                    }

                });

                await Task.Delay(TimeSpan.FromSeconds(1.0));
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

                var rewind = (pos / this.sliderProgress.ActualWidth) * this.player.Duration.Ticks;

                if (this.player != null)
                {
                    this.player.Rewinding(rewind);
                }

                this.updateProgress = true;
            }
        }

        private void WriteDebugMessage(string msg)
        {
            this.WriteDebugMessage(msg, Colors.White);
        }

        private void WriteDebugMessage(string msg, Color color, bool clear = false)
        {
            Run run = new Run();
            Paragraph p = new Paragraph();

            run.Text = "::::  " + msg;
            run.FontSize = 16;
            run.Foreground = new SolidColorBrush(color);

            p.Inlines.Add(run);

            if (clear)
            {
                this.DebugTextBlock.Blocks.Clear();
            }

            this.DebugTextBlock.Blocks.Add(p);
        }

        private void swapChainPanel_ManipulationDelta(object sender, ManipulationDeltaRoutedEventArgs e)
        {

        }

        private void Play_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.sliderProgress.Value = 0;
                this.player.Play();
                this.ResetProgress();
                this.IncreaseProgress();
                this.earthRenderable.EarthRotationEnabled = true;
                this.isPlayingNow = true;
            }
        }

        private void Stop_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.earthRenderable.EarthRotationEnabled = false;
                this.player.Stop();
                this.ResetProgress();
                this.sliderProgress.Value = 0;
                this.isPlayingNow = false;
            }
        }

        private void Next_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Next(this.isPlayingNow);
            }
        }

        private void Previous_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Previous(this.isPlayingNow);
            }
        }

        private async void Playlist_Click(object sender, RoutedEventArgs e)
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

            //TODO correct convertion to List !!!!
            var pickedFilesTmp = await filePicker.PickMultipleFilesAsync();
            List<StorageFile> pickedFiles = new List<StorageFile>(pickedFilesTmp);

            if (pickedFiles != null && pickedFiles.Count != 0)
            {
                List<string> songs = new List<string>();
                List<IRandomAccessStream> streams = new List<IRandomAccessStream>();

                this.WriteDebugMessage("Chosen files:", Colors.Yellow);

                for (int i = 0; i < pickedFiles.Count; i++)
                {
                    var file = pickedFiles[i];
                    var path = file.Path;
                    var name = file.Name;

                    songs.Add(name);

                    var stream = await file.OpenStreamForReadAsync();
                    IRandomAccessStream resultStream = stream.AsRandomAccessStream();
                    streams.Add(resultStream);

                    this.WriteDebugMessage(name, Colors.LightGreen);
                }

                if (this.player == null)
                {
                    //create playlist
                    this.playList = new CreatingPlaylist();
                    this.playList.CreatePlayList(songs, streams, pickedFiles);

                    //init players list
                    this.player = new Reader();
                    this.player.InitPlayer(this.playList);
                }
                else
                {
                    this.player.Stop();
                    this.playList.AddTrack(songs, streams, pickedFiles);
                    this.player.InitPlayer(this.playList);
                }

                this.WriteDebugMessage("Playlist successfully created.", Colors.Yellow);
            }
            else
            {
                this.WriteDebugMessage("Wrong chosen files! Chosen files = NULL!!!", Colors.Red);
            }
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.player.Dispose();
            }

            GC.Collect();

            Application.Current.Exit();
        }

        private void ClearPlaylist_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                if (this.playList != null)
                {
                    this.WriteDebugMessage("Playlist successfully cleaned.", Colors.Yellow, true);

                    this.player.Stop();
                    this.player.ClearPlayList();

                    this.ResetProgress();

                    this.sliderProgress.Value = 0.0;
                    this.earthRenderable.EarthRotationEnabled = false;

                    this.player.Dispose();
                    GC.Collect();

                    this.player = null;
                    this.playList = null;
                }
            }
        }

        private void Pause_Click(object sender, RoutedEventArgs e)
        {
            if (this.player != null)
            {
                this.earthRenderable.EarthRotationEnabled = false;
                this.player.Pause();
                this.ResetProgress();
                this.isPlayingNow = false;
            }
        }
    }
}
