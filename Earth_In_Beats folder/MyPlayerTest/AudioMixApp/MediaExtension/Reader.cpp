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
	//this->player = std::shared_ptr<XAudio2Player>(new XAudio2Player());
}

void Reader::InitPlayer(IPlayList ^playList)
{
	this->playersList.clear();

	this->currentPlayList = playList;
	this->currentPlayList->SortPlaylist();
	this->FindGlobalDuration();	

	for (size_t i = 0; i < currentPlayList->GetPlayListLength(); i++)
	{
		auto player = std::shared_ptr<XAudio2Player>(new XAudio2Player());
		Windows::Storage::Streams::IRandomAccessStream ^stream;
		InitMasterVoice::GetInstance();
		MFAudioReader *reader = new MFAudioReader();
		this->xAudio2 = InitMasterVoice::GetInstance().GetXAudio();

		{
			MFAudioEvents *tmppEvents = new MFAudioEvents();
			tmppEvents->InitEvent(this);
			this->events = std::shared_ptr<AudioEvents>(tmppEvents);
		}

		stream = this->currentPlayList->GetStream(i);

		this->tracksInfo.push_back(this->currentPlayList->GetInfoAboutTrack(i));

		reader->Initialize(stream);

		player->Initialize(reader, this->xAudio2, this->events);	//create new player

		std::lock_guard<std::mutex> lock(this->lockPlayList);
		this->playersList.push_back(player);
	}
}

void Reader::Play()
{
	if (this->playersList[this->currentPlayerNum])
	{
		this->playersList[this->currentPlayerNum]->Stop();
		//if needed always play from 0 position
		//this->playersList[i]->SetPosition(Rational::SEC, 0);
		this->playersList[this->currentPlayerNum]->Play();
	}
}

void Reader::Next(){
	if (this->playersList[this->currentPlayerNum])
	{
		this->playersList[this->currentPlayerNum]->Stop();
		this->currentPlayerNum++;
		this->playersList[this->currentPlayerNum]->Play();
	}
}

void Reader::Previous(){
	if (this->playersList[this->currentPlayerNum - 1])
	{
		this->playersList[this->currentPlayerNum]->Stop();
		this->currentPlayerNum--;
		this->playersList[this->currentPlayerNum]->Play();
	}
}

void Reader::Rewinding(double setPosition)
{	
	if (this->playersList[this->currentPlayerNum])
	{
		this->playersList[this->currentPlayerNum]->SetPosition(Rational::SEC, setPosition);
	}
}

Windows::Foundation::TimeSpan Reader::Duration::get()
{
	Windows::Foundation::TimeSpan duration;
	duration.Duration = this->playersList[this->currentPlayerNum]->GetDuration();
	return duration;
}

void Reader::Volume(float setVolume)
{
	for (int i = 0; i < this->playersList.size(); i++)
	{
		if (this->playersList[i])
		{
			this->playersList[i]->SetVolume(setVolume);
		}
	}
}

LONGLONG Reader::CurrPos()
{
	return this->playersList[this->currentPlayerNum]->GetCurrentPosition();
}

void Reader::Stop()
{
	for (int i = 0; i < this->playersList.size(); i++)
	{
		if (this->playersList[i])
		{
			this->playersList[i]->Stop();
			this->playersList[i]->SetPosition(Rational::SEC, 0);
		}
	}
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

void Reader::EndOfPlayingTrack(int c)	//start new player incrementing currentPlayerNum
{
	this->playersList[this->currentPlayerNum]->Stop();
	this->currentPlayerNum++;

	if (this->currentPlayerNum < this->playersList.size())
	{
		if (this->playersList[this->currentPlayerNum])
		{
			this->playersList[this->currentPlayerNum]->Stop();
			this->playersList[this->currentPlayerNum]->Play();
		}
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