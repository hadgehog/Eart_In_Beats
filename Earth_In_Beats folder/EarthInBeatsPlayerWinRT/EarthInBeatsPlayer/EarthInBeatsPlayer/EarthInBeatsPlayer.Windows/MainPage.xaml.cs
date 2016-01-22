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
using Windows.UI.ApplicationSettings;

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

            this.sliderVolume.ManipulationStarted += SliderVolume_ManipulationStarted;
            this.sliderVolume.ManipulationCompleted += SliderVolume_ManipulationCompleted;
            this.sliderProgress.ManipulationCompleted += sliderProgress_ManipulationCompleted;

            this.sliderVolume.AddHandler(ManipulationStartedEvent, new ManipulationStartedEventHandler(SliderVolume_ManipulationStarted), true);
            this.sliderVolume.AddHandler(ManipulationCompletedEvent, new ManipulationCompletedEventHandler(SliderVolume_ManipulationCompleted), true);
            this.sliderProgress.AddHandler(ManipulationCompletedEvent, new ManipulationCompletedEventHandler(sliderProgress_ManipulationCompleted), true);

            SettingsPane.GetForCurrentView().CommandsRequested -= this.OnSettingCharmOpen;
            SettingsPane.GetForCurrentView().CommandsRequested += this.OnSettingCharmOpen;
        }

        private void OnSettingCharmOpen(SettingsPane sender, SettingsPaneCommandsRequestedEventArgs args)
        {
            args.Request.ApplicationCommands.Add(new SettingsCommand("Equalizer", "Equalizer", (handler) => this.ShowEqualizerPanel()));
            args.Request.ApplicationCommands.Add(new SettingsCommand("Setings", "Settings", (handler) => this.ShowSettingsPanel()));
            args.Request.ApplicationCommands.Add(new SettingsCommand("Help", "Help", (handler) => this.ShowHelpPanel()));
            args.Request.ApplicationCommands.Add(new SettingsCommand("About", "About", (handler) => this.ShowAboutPanel()));
            args.Request.ApplicationCommands.Add(new SettingsCommand("Exit", "Exit", (handler) => this.ExitApp()));
        }

        private void ShowEqualizerPanel()
        {
            Color backgroundColor = Colors.White;
            Color headerBackgroundColor = Colors.DimGray;

            SettingsFlyout settings = new SettingsFlyout();
            settings.Content = new EqualizerControl();
            settings.Title = "Equalizer";
            settings.HeaderBackground = new SolidColorBrush(headerBackgroundColor);
            settings.Background = new SolidColorBrush(backgroundColor);
            settings.Show();
        }

        private void ShowSettingsPanel()
        {
            Color backgroundColor = Colors.White;
            Color headerBackgroundColor = Colors.DimGray;

            SettingsFlyout settings = new SettingsFlyout();
            settings.Content = new SettingsControl();
            settings.Title = "Settings";
            settings.HeaderBackground = new SolidColorBrush(headerBackgroundColor);
            settings.Background = new SolidColorBrush(backgroundColor);
            settings.Show();
        }

        private void ShowHelpPanel()
        {
            Color backgroundColor = Colors.White;
            Color headerBackgroundColor = Colors.DimGray;

            SettingsFlyout settings = new SettingsFlyout();
            settings.Content = new HelpControl();
            settings.Title = "Help";
            settings.HeaderBackground = new SolidColorBrush(headerBackgroundColor);
            settings.Background = new SolidColorBrush(backgroundColor);
            settings.Show();
        }

        private void ShowAboutPanel()
        {
            Color backgroundColor = Colors.White;
            Color headerBackgroundColor = Colors.DimGray;

            SettingsFlyout settings = new SettingsFlyout();
            settings.Content = new AboutControl();
            settings.Title = "About";
            settings.HeaderBackground = new SolidColorBrush(headerBackgroundColor);
            settings.Background = new SolidColorBrush(backgroundColor);
            settings.Show();
        }

        private void SliderVolume_ManipulationStarted(object sender, ManipulationStartedRoutedEventArgs e)
        {
            int s = 34;
        }

        private void SliderVolume_ManipulationCompleted(object sender, ManipulationCompletedRoutedEventArgs e)
        {
            if (this.earthRenderable != null)
            {
                if (this.sliderVolume.Value > 50)
                {
                    for (int i = (int)this.sliderVolume.Value; i < 100; i++)
                    {
                        this.earthRenderable.VericalRotationAngle = (i / 100) * 360.0f;
                    }
                }
                else
                {
                    for (int i = (int)this.sliderVolume.Value; i >= 0; --i)
                    {
                        if (i < 0)
                        {
                            break;
                        }

                        this.earthRenderable.VericalRotationAngle = (i / 100) * 360.0f;
                    }
                }
            }
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

            if (this.earthRenderable != null)
            {
                this.earthRenderable.VericalRotationAngle = (float)((e.NewValue / 100.0f) * 360.0f);
            }
        }

        private async void IncreaseProgress()
        {
            this.dispatcher = CoreApplication.MainView.Dispatcher;

            while (this.dispatcher != null && this.player != null && this.playList != null && this.player.CurrPos() <= this.player.Duration.Ticks)
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
                this.dispatcher = null;
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

                if (pos < 0)
                {
                    pos = 0.0;
                }

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
                this.player.Volume((float)this.sliderVolume.Value / 100);
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
                this.earthRenderable.ResetRotationAngle();
                this.player.Stop();
                this.ResetProgress();
                this.sliderProgress.Value = 0;
                this.isPlayingNow = false;
                this.sliderProgress.Value = 0;
            }

            this.sliderProgress.Value = 0;
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
                    this.player.InitPlayer(this.playList, this.earthRenderable);
                }
                else
                {
                    this.player.Stop();
                    this.playList.AddTrack(songs, streams, pickedFiles);
                    this.player.InitPlayer(this.playList, this.earthRenderable);
                }

                this.WriteDebugMessage("Playlist successfully created.", Colors.Yellow);
            }
            else
            {
                this.WriteDebugMessage("Wrong chosen files! Chosen files = NULL!!!", Colors.Red);
            }
        }

        private void ExitApp()
        {
            if (this.player != null)
            {
                this.player.Dispose();
            }

            GC.Collect();

            Application.Current.Exit();
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            this.ExitApp();
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

        private async void swapChainPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var value = this.sliderVolume.Value;

            this.sliderVolume.Value = 0;

            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                this.sliderVolume.Value = value;
            });
        }

        private void sliderProgress_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (this.earthRenderable != null && this.isPlayingNow && !this.updateProgress)
            {
                this.earthRenderable.EarthRotationEnabled = false;

                if (e.NewValue > e.OldValue)
                {
                    this.earthRenderable.HorisontalRotationAngle += (float)(e.NewValue / 100.0f) * 30;
                }
                else
                {
                    this.earthRenderable.HorisontalRotationAngle -= (float)(e.NewValue / 100.0f) * 30;
                }
            }
        }

        private void sliderProgress_ManipulationCompleted(object sender, ManipulationCompletedRoutedEventArgs e)
        {
            if (this.earthRenderable != null && this.isPlayingNow)
            {
                this.earthRenderable.EarthRotationEnabled = true;
            }
        }
    }
}
