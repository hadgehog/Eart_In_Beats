#include "Reader.h"
#include "AudioData/AudioReader/AudioReader.h"
#include "AudioEvents/MFAudioEvents.h"

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

using namespace EarthInBeatsNativeLibrary;
using namespace Platform;

Reader::Reader()
{
	//this->player = std::shared_ptr<XAudio2Player>(new XAudio2Player());
	this->currentPlayerNum = 0;
	this->globalDuration = 0;
}

Reader::~Reader()
{

}

void Reader::InitPlayer(IPlayList ^playList)
{
	this->playersList.clear();

	this->currentPlayList = playList;
	this->currentPlayList->SortPlaylist();
	this->FindGlobalDuration();	

	for (uint16_t i = 0; i < currentPlayList->GetPlayListLength(); i++)
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

		player->Initialize(reader, this->xAudio2, this->events);	//create new players

		std::lock_guard<std::mutex> lock(this->lockPlayList);
		this->playersList.push_back(player);
	}
}

void Reader::Play()
{
	if (this->playersList[this->currentPlayerNum])
	{
		this->playersList[this->currentPlayerNum]->Stop();
		//if needed always play from 0 position to do SetPosition(0)
		this->playersList[this->currentPlayerNum]->Play();
	}
}

void Reader::Next(){
	if (this->currentPlayerNum < playersList.size() - 1)
	{
		if (this->playersList[this->currentPlayerNum])
		{
			this->playersList[this->currentPlayerNum]->Stop();

			DoubleRational tmpPos(static_cast<double>(0.0), DoubleRational::Unit::SEC);
			Int64Rational pos(static_cast<Int64Rational::Type>(tmpPos.Convert_cr(DoubleRational::Unit::HNS).value), Int64Rational::Unit::HNS);

			this->playersList[this->currentPlayerNum]->SetPosition(pos);
			this->currentPlayerNum++;
			this->playersList[this->currentPlayerNum]->Play();
		}
	}
}

void Reader::Previous(){
	if (this->currentPlayerNum - 1 >= 0)
	{
		if (this->playersList[this->currentPlayerNum - 1])
		{
			this->playersList[this->currentPlayerNum]->Stop();

			DoubleRational tmpPos(static_cast<double>(0.0), DoubleRational::Unit::SEC);
			Int64Rational pos(static_cast<Int64Rational::Type>(tmpPos.Convert_cr(DoubleRational::Unit::HNS).value), Int64Rational::Unit::HNS);

			this->playersList[this->currentPlayerNum]->SetPosition(pos);
			this->currentPlayerNum--;
			this->playersList[this->currentPlayerNum]->Play();
		}
	}
}

void Reader::Rewinding(double setPosition)
{	
	if (this->playersList[this->currentPlayerNum])
	{
		DoubleRational tmpPos(static_cast<double>(setPosition), DoubleRational::Unit::SEC);
		Int64Rational pos(static_cast<Int64Rational::Type>(tmpPos.Convert_cr(DoubleRational::Unit::HNS).value), Int64Rational::Unit::HNS);

		this->playersList[this->currentPlayerNum]->SetPosition(pos);
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
	for (uint16_t i = 0; i < this->playersList.size(); i++)
	{
		if (this->playersList[i])
		{
			this->playersList[i]->SetVolume(setVolume);
		}
	}
}

LONGLONG Reader::CurrPos()
{
	if (this->playersList[this->currentPlayerNum])
	{
		return this->playersList[this->currentPlayerNum]->GetCurrentPosition();
	}
	else
	{
		return 0;
	}
}

void Reader::Stop()
{
	for (uint16_t i = 0; i < this->playersList.size(); i++)
	{
		if (this->playersList[i])
		{
			this->playersList[i]->Stop();

			DoubleRational tmpPos(static_cast<double>(0.0), DoubleRational::Unit::SEC);
			Int64Rational pos(static_cast<Int64Rational::Type>(tmpPos.Convert_cr(DoubleRational::Unit::HNS).value), Int64Rational::Unit::HNS);

			this->playersList[i]->SetPosition(pos);
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

int64_t Reader::GetGlobalDuration()
{
	return this->globalDuration;
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
	int64_t convertSongDuration = 0;
	Int64Rational pos(songDuration.Convert_cr(Int64Rational::Unit::SEC).value, Int64Rational::Unit::HNS);
	convertSongDuration = pos.value;

	return convertSongDuration;
}