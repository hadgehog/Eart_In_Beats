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

///////////Init only last player!!! Need always recreate player in loop !!
void Reader::InitPlayer(IPlayList ^playList)
{
	this->currentPlayList = playList;

	this->currentPlayList->SortPlaylist();
	this->FindGlobalDuration();	

	for (size_t i = 0; i < currentPlayList->GetPlayListLength(); i++)
	{
		this->player = std::shared_ptr<XAudio2Player>(new XAudio2Player());
		Windows::Storage::Streams::IRandomAccessStream ^stream;
		InitMasterVoice::GetInstance();
		MFAudioReader *reader = new MFAudioReader();
		this->xAudio2 = InitMasterVoice::GetInstance().GetXAudio();

		{
			MFAudioEvents *tmppEvents = new MFAudioEvents();
			tmppEvents->InitEvent(this);
			this->events = std::shared_ptr<AudioEvents>(tmppEvents);
		}

		auto a = this->currentPlayList->GetTrack(i);
		auto n = a->GetName();
		auto p = a->GetPosition();

		stream = this->currentPlayList->GetStream(i);

		this->tracksInfo.push_back(this->currentPlayList->GetInfoAboutTrack(i));

		reader->Initialize(stream);

		this->player->Initialize(reader, this->xAudio2, this->events);	//create new player

		std::lock_guard<std::mutex> lock(this->lockPlayList);
		this->playersList.push_back(this->player);
	}
}

void Reader::Play(int num)
{
	if (this->playersList[num - 1])
	{
		this->playersList[num - 1]->Stop();
		//if needed always play from 0 position
		//this->playersList[i]->SetPosition(Rational::SEC, 0);
		this->playersList[num - 1]->Play();
	}
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
	duration.Duration = this->playersList[0]->GetDuration();	//tmp
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
	return this->playersList[0]->GetCurrentPosition();	//tmp
}

void Reader::Stop()
{
	for (int i = 0; i < this->playersList.size(); i++)
	{
		this->playersList[i]->Stop();
		this->playersList[i]->SetPosition(Rational::SEC, 0);
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
			//this->player->SetAudioData(reader, this->xAudio2);	//tmp
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