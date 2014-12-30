using System;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Background;
using Windows.Foundation.Collections;
using Windows.Media;
using Windows.Media.Playback;
using MediaData;
using MediaExtension;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace BackgroundAudio
{
    public sealed class BackgroundAudio : IBackgroundTask
    {
        private BackgroundTaskDeferral _deferral;
        private SystemMediaTransportControls _systemMediaTransportControl;
        Reader player;
        CreatingPlaylist playList;
        private long currPos, trackLength;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            _systemMediaTransportControl = SystemMediaTransportControls.GetForCurrentView();
            _systemMediaTransportControl.IsEnabled = true;

            BackgroundMediaPlayer.MessageReceivedFromForeground += MessageReceivedFromForeground;

            BackgroundMediaPlayer.Current.MediaOpened += CurrentOnMediaOpened;

            taskInstance.Canceled += OnCanceled;
            taskInstance.Task.Completed += Taskcompleted;

            player = new Reader();
            playList = new CreatingPlaylist();

            Existing("aaa");

            _deferral = taskInstance.GetDeferral();
        }

        void FakePause()
        {
            BackgroundMediaPlayer.Current.Position = new TimeSpan(0);
            BackgroundMediaPlayer.Current.PlaybackRate = 0.0;
        }

        private void CurrentOnMediaOpened(MediaPlayer sender, object args)
        {
            player.Play(playList);
            currPos = player.CurrPos();
            trackLength = player.Duration.Ticks;
            FakePause();

            var messageToForeground = new ValueSet { { "Duration", trackLength } };
            BackgroundMediaPlayer.SendMessageToForeground(messageToForeground);
        }

        private void MessageReceivedFromForeground(object sender, MediaPlayerDataReceivedEventArgs e)
        {
            ValueSet valueSet = e.Data;
            foreach (string key in valueSet.Keys)
            {
                switch (key)
                {
                    case "Play":
                        Play((byte[])valueSet[key]);
                        break;
                    case "Rewind":
                        Rewind((double)valueSet[key]);
                        break;
                    case "Stop":
                        Stop((int)valueSet[key]);
                        break;
                    case "Volume":
                        Volume((float)valueSet[key]);
                        break;
                }
            }
        }

        private async void Play(byte[] serialized)
        {
            MediaPlayer mediaPlayer = BackgroundMediaPlayer.Current;
            mediaPlayer.AutoPlay = true;

            using (var ms = new MemoryStream(serialized))
            {
                playList = CreatingPlaylist.DeSerialize(ms);
            }

            var crutchFile = await Package.Current.InstalledLocation.GetFileAsync("Assets\\silence1sec.mp3");
            mediaPlayer.SetFileSource(crutchFile);

            FakePause();

            //Update the universal volume control
            _systemMediaTransportControl.ButtonPressed += MediaTransportControlButtonPressed;
            _systemMediaTransportControl.IsPauseEnabled = true;
            _systemMediaTransportControl.IsPlayEnabled = false;
            _systemMediaTransportControl.DisplayUpdater.Type = MediaPlaybackType.Music;
            _systemMediaTransportControl.DisplayUpdater.MusicProperties.Title = "Music";
            _systemMediaTransportControl.DisplayUpdater.MusicProperties.Artist = "Different ";
            _systemMediaTransportControl.DisplayUpdater.Update();
        }



        private void Rewind(double pos)
        {
            if(player != null)
            {
                var tmp = player.Duration.Ticks;
                player.Rewinding(pos * (tmp / 100));
            }
        }

        private void Stop(int play)
        {
            if(player != null)
            {
                if(play == 0)
                {
                    BackgroundMediaPlayer.Current.Pause();
                    player.Stop();
                    StopProgress();
                }
            }
        }

        private void StopProgress()
        {
            long stop = 0;
            var messageToForeground = new ValueSet { { "Duration", stop } };
            BackgroundMediaPlayer.SendMessageToForeground(messageToForeground);
        }

        private void Volume(float volume)
        {
            if(player != null)
            {
                player.Volume(volume);
            }
        }

        private void Existing(string s)
        {
            byte[] serialized;

            using (var ms = new MemoryStream())
            {
                serialized = playList.Serialize(ms);
            }

            var messageToForeground = new ValueSet { { "ExistTrue", serialized } };
            BackgroundMediaPlayer.SendMessageToForeground(messageToForeground);
        }

        private void MediaTransportControlButtonPressed(SystemMediaTransportControls sender,
            SystemMediaTransportControlsButtonPressedEventArgs args)
        {
            switch (args.Button)
            {
                case SystemMediaTransportControlsButton.Play:
                    BackgroundMediaPlayer.Current.Play();
                    FakePause();
                    player.Play(playList);
                    break;
                case SystemMediaTransportControlsButton.Pause:
                    BackgroundMediaPlayer.Current.Pause();
                    player.Stop();
                    break;
            }
        }

        private void Taskcompleted(BackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs args)
        {
            BackgroundMediaPlayer.Shutdown();
            _deferral.Complete();
        }

        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            // You get some time here to save your state before process and resources are reclaimed
            BackgroundMediaPlayer.Shutdown();
            _deferral.Complete();
        }
    }
}
