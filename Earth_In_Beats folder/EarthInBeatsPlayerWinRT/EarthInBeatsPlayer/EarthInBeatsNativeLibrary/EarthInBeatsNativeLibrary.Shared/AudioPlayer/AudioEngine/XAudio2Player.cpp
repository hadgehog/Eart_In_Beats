#include "..\AudioData\AudioReader\MFAudioReader.h"
#include "..\AudioData\AudioSample\MFAudioSample.h"
#include "..\AudioHelpers\Auto.h"
#include "XAudio2Player.h"
#include "..\AudioEvents\MFAudioEvents.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <ppltasks.h>
#include <combaseapi.h>
#include <future>

using namespace Platform;

void SourceVoiceDeleter::operator()(IXAudio2SourceVoice *obj)
{
	obj->DestroyVoice();
}

XAudio2Player::XAudio2Player()
{
	this->currentPosition = 0;
	this->stopped = false;
}

XAudio2Player::~XAudio2Player()
{

}

LONGLONG XAudio2Player::GetCurrentPosition()
{
	LONGLONG physCurrPos = 0;
	Int64Rational tmpPos(this->currentPosition, Int64Rational::Unit::HNS);
	Int64Rational pos(static_cast<Int64Rational::Type>(tmpPos.Convert_cr(Int64Rational::Unit::SEC).value), Int64Rational::Unit::SEC);
	physCurrPos = pos.value;

	return physCurrPos;
}

LONGLONG XAudio2Player::GetDuration()
{
	LONGLONG convertValue = 0;
	Int64Rational durationValue = this->reader->GetAudioDuration();
	Int64Rational pos(static_cast<Int64Rational::Type>(durationValue.Convert_cr(Int64Rational::Unit::SEC).value), Int64Rational::Unit::SEC);
	convertValue = pos.value;

	return convertValue;
}

void XAudio2Player::SetVolume(float volume)
{
	HRESULT hr = S_OK;
	hr = this->sourceVoice->SetVolume(volume);
}

void XAudio2Player::SetPosition(const Int64Rational &position)
{
	{
		this->FlushSourceVoice();
		this->reader->SetPosition(position);

		if (this->events)
			this->events->EndOfRewinding();
	}

	this->SubmitBuffer();	//
}

void XAudio2Player::FlushSourceVoice()
{
	std::unique_lock<std::mutex> lock(this->samplesMutex);

	this->sourceVoice->FlushSourceBuffers();

	XAUDIO2_VOICE_STATE voiceState;
	this->sourceVoice->GetState(&voiceState);

	if (voiceState.BuffersQueued != 0)
	{
		this->notifiedRewinding = true;

		while (this->notifiedRewinding == true)
		{
			this->condVar.wait(lock);
		}
	}

	this->samples = std::queue<std::unique_ptr<AudioSample>>();
}

void XAudio2Player::SetAudioData(AudioReader *reader, Microsoft::WRL::ComPtr<IXAudio2> xAudio2)
{
	HRESULT hr = S_OK;
	WAVEFORMATEX *wF;
	uint32 wL;
	IXAudio2SourceVoice *tmpVoice = nullptr;

	this->reader = reader;
	this->xAudio2 = xAudio2;
	this->reader->GetWaveInfo(0, wF, wL);
	hr = this->xAudio2->CreateSourceVoice(&tmpVoice, wF, 0, 2.0f, this);
	this->sourceVoice = std::shared_ptr<IXAudio2SourceVoice>(tmpVoice, SourceVoiceDeleter());

	CoTaskMemFree(wF);

	this->stopped = false;
	//this->SubmitBuffer();
	//hr = this->sourceVoice->Start();
}

void XAudio2Player::Play() {
	HRESULT hr = S_OK;

	if (this->sourceVoice) {
		this->stopped = false;
		this->SubmitBuffer();
		hr = this->sourceVoice->Start();
	}
}

void XAudio2Player::Pause() {
	HRESULT hr = S_OK;

	if (this->sourceVoice) {
		hr = this->sourceVoice->Stop();
		//this->reader->SetPosition(this->currentPosition);	 // check is it need
	}
}

void XAudio2Player::Stop() {
	HRESULT hr = S_OK;

	if (this->sourceVoice) {
		hr = this->sourceVoice->Stop();
		this->FlushSourceVoice();
		this->stopped = true;
	}
}

void XAudio2Player::Initialize(AudioReader *reader, Microsoft::WRL::ComPtr<IXAudio2> iXAudio2, std::shared_ptr<AudioEvents> e)
{
	this->events = e;
	this->SetAudioData(reader, iXAudio2);
}

void XAudio2Player::SubmitBuffer()
{
	HRESULT hr = S_OK;

	if (this->stopped)
	{
		return;
	}

	if (this->notifiedRewinding == false)
	{
		std::unique_ptr<AudioSample> sample = std::unique_ptr<AudioSample>(this->reader->ReadAudioSample());

		if (sample)
		{
			sample->GetData([&](void *buffer, uint64_t size)
			{
				XAUDIO2_BUFFER xbuffer = { 0 };
				xbuffer.AudioBytes = (uint32_t)size;
				xbuffer.pAudioData = (BYTE*)buffer;
				xbuffer.pContext = nullptr;

				hr = this->sourceVoice->SubmitSourceBuffer(&xbuffer);
			});


			this->currentPosition = sample->GetSampleTime();
			this->samples.push(std::move(sample));
		}
		else
		{
			if (this->events)
			{
				this->stopped = true;
				concurrency::create_task([=]()
				{
					this->sourceVoice->Stop();
					this->FlushSourceVoice();
					this->GoToNextSong();
					this->events->EndOfPlaying(this->trackIndex);
				});
			}
		}
	}
	else
	{
		XAUDIO2_VOICE_STATE voiceState;
		this->sourceVoice->GetState(&voiceState);

		if (voiceState.BuffersQueued == 0)
		{
			this->notifiedRewinding = false;
			this->condVar.notify_all();
		}
	}
}

void XAudio2Player::DeleteSamples()
{
	std::unique_lock<std::mutex> lock(this->samplesMutex);
	this->samples.pop();
}

void XAudio2Player::GoToNextSong()
{
	this->trackIndex++;
}