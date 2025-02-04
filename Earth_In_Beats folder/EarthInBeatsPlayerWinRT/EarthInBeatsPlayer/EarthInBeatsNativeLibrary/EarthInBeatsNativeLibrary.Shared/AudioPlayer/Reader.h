#pragma once

#include "AudioData\AudioReader\MFAudioReader.h"
#include "AudioData\AudioSample\MFAudioSample.h"
#include "AudioEngine\XAudio2Player.h"
#include "AudioHelpers\InitMasterVoice.h"

#include "..\Graphics\Renderers\EarthRenderableWinRT.h"

#include <list>

namespace EarthInBeatsNativeLibrary
{
	public interface class ITrack
	{
	public:
		int GetPosition();
		Platform::String ^GetName();
	};

	public interface class IPlayList
	{
	public:
		void SortPlaylist();
		ITrack ^GetTrack(int index);
		Windows::Storage::Streams::IRandomAccessStream ^GetStream(int trackNumber);
		Platform::String ^GetInfoAboutTrack(int trackNumber);
		bool CheckNext(int currentNumber);
		int GetPlayListLength();
	};

	public interface class INetworking
	{
	public:
		Platform::String ^GetMyLocation();
		void SendMyLocation();
	};

	public ref class Reader sealed
	{
		struct compare
		{
			bool operator() (ITrack ^song1, ITrack ^song2) const
			{
				return song1->GetPosition() < song2->GetPosition();
			}
		};

	public:
		Reader();
		virtual ~Reader();
		void InitPlayer(IPlayList ^playList, EarthInBeatsNativeLibrary::EarthRenderableWinRT ^renderer);
		void Play();
		void Pause();
		void Next(bool isPlayingNow);
		void Previous(bool isPlayingNow);
		void Rewinding(double setPosition);
		property Windows::Foundation::TimeSpan Duration	{ Windows::Foundation::TimeSpan get(); }
		void Volume(float setVolume);
		LONGLONG CurrPos();
		void Stop();
		void FindGlobalDuration();
		int64_t GetGlobalDuration();
		void ClearPlayList();

		void EndOfRewindingTrack();
		void EndOfPlayingTrack(int c);

	private:
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
		IPlayList ^currentPlayList;
		std::shared_ptr<AudioEvents> events;
		std::vector<std::shared_ptr<XAudio2Player>> playersList;
		int64_t globalDuration = 0;
		std::mutex lockPlayList;
		int currentPlayerNum = 0;
		std::vector<Platform::String ^> tracksInfo;
		bool isPauseOccurs;
		EarthInBeatsNativeLibrary::EarthRenderableWinRT ^rendererPointer;

		int64_t FindSongDurationFromPlayList(int numSong);
	};
}

