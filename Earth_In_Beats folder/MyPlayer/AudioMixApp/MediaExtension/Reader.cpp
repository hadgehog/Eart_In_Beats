#include "pch.h"
#include "Reader.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <vector>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <propvarutil.h>
#include <collection.h>
#include "AudioReader.h"
#include "MFAudioReader.h"
#include "MFAudioEvents.h"

using namespace MediaExtension;
using namespace Platform;

Reader::Reader()
{
	this->player = std::shared_ptr<XAudio2Player>(new XAudio2Player());
}

void Reader::Initialize(IPlayList ^playList)
{
	InitMasterVoice::GetInstance();
	Windows::Storage::Streams::IRandomAccessStream ^stream;
	MFAudioReader *reader = new MFAudioReader();
	this->currentPlayList = playList;

	this->currentPlayList->SortPlaylist();
	this->FindGlobalDuration();

	{
		MFAudioEvents *tmppEvents = new MFAudioEvents();
		tmppEvents->InitEvent(this);
		this->events = std::shared_ptr<AudioEvents>(tmppEvents);
	}	

	stream = this->currentPlayList->GetStream(0);

	this->tracksInfo.push_back(this->currentPlayList->GetInfoAboutTrack(0));

	this->xAudio2 = InitMasterVoice::GetInstance().GetXAudio();

	reader->Initialize(stream);

	this->player->Initialize(reader, this->xAudio2, this->events);	//create new player and play

	std::lock_guard<std::mutex> lock(this->lockPlayList);
	this->playersList.push_back(this->player);
}

void Reader::Play(int numPlayer)
{
	int stop = 123;
}

void Reader::Rewinding(double setPosition)
{
	
	for (int i = 0; i < this->playersList.size(); i++)
	{
		this->playersList[i]->SetPosition(Rational::SEC, setPosition);
	}
}

Windows::Foundation::TimeSpan Reader::Duration::get()
{
	Windows::Foundation::TimeSpan duration;
	duration.Duration = this->player->GetDuration();
	return duration;
}

void Reader::Volume(float setVolume)
{
	for (int i = 0; i < this->playersList.size(); i++)
	{
		this->playersList[i]->SetVolume(setVolume);
	}
}

LONGLONG Reader::CurrPos()
{
	return this->player->GetCurrentPosition();
}

void Reader::Stop()
{
	for (int i = 0; i < this->playersList.size(); i++)
	{
		this->playersList[i]->Stop();
	}
	//this->playersList.clear();
}

void Reader::FindGlobalDuration()
{
	this->globalDuration += this->FindSongDurationFromPlayList(0);

	for (int i = 1; i < this->currentPlayList->GetPlayListLength(); i++)
	{
		this->globalDuration += this->FindSongDurationFromPlayList(i);
	}
}

void Reader::EndOfRewindingTrack()
{

}

void Reader::EndOfPlayingTrack(int c)	//begin playing new track in same player
{
	Windows::Storage::Streams::IRandomAccessStream ^stream;
	MFAudioReader *reader = new MFAudioReader();
	int playlistLength = this->currentPlayList->GetPlayListLength();

	if (c < playlistLength)
		if (this->currentPlayList->CheckNext(c - 1))
		{
			stream = this->currentPlayList->GetStream(c);

			this->tracksInfo.push_back(this->currentPlayList->GetInfoAboutTrack(c));

			reader->Initialize(stream);
			this->player->SetAudioData(reader, this->xAudio2);
		}
}

int64_t Reader::FindSongDurationFromPlayList(int numSong)
{
	Windows::Storage::Streams::IRandomAccessStream ^stream;
	MFAudioReader *reader = new MFAudioReader();
	stream = this->currentPlayList->GetStream(numSong);
	reader->Initialize(stream);
	Int64Rational songDuration = reader->GetAudioDuration();
	int64_t convertSongDuration = songDuration.Convert(Rational::HNS).value;
	return convertSongDuration;
}

int Reader::GetGlobalDuration()
{
	return this->globalDuration;
}