#pragma once

#include "MFAudioReader.h"
#include "MFAudioSample.h"
#include "XAudio2Player.h"
#include "InitMasterVoice.h"
#include <list>

namespace MediaExtension
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
		Platform::String ^GetInfoAboutTrack(int trackNumber);	//return info about track
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
		void InitPlayer(IPlayList ^playList);
		void Play();
		void Rewinding(double setPosition);
		property Windows::Foundation::TimeSpan Duration	{ Windows::Foundation::TimeSpan get(); }
		void Volume(float setVolume);
		LONGLONG CurrPos();
		void Stop();
		void FindGlobalDuration();
		int GetGlobalDuration();

		void EndOfRewindingTrack();
		void EndOfPlayingTrack(int c);

	private:
		//std::shared_ptr<XAudio2Player> player;
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
		IPlayList ^currentPlayList;
		std::shared_ptr<AudioEvents> events;
		std::vector<std::shared_ptr<XAudio2Player>> playersList;
		int64_t globalDuration = 0;
		std::mutex lockPlayList;
		int currentPlayerNum = 0;

		int64_t FindSongDurationFromPlayList(int numSong);
		std::vector<Platform::String ^> tracksInfo;
	};
}

