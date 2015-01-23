#include "pch.h"
#include "MFAudioReader.h"
#include "Auto.h"
#include "MFAudioSample.h"
#include "MFAudioEvents.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <vector>
#include <propvarutil.h>
#include <windows.h>


using namespace MediaExtension;
using namespace Platform;
using namespace std;

MFAudioReader::MFAudioReader()
{

}

MFAudioReader::~MFAudioReader()
{

}

uint32 MFAudioReader::GetAudioStreamCount()
{
	return this->audioStreamsVector.size();
}

Microsoft::WRL::ComPtr<IMFMediaType> MFAudioReader::GetType(int index)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IMFMediaType> type;
	int audioStreamIndex = audioStreamsVector[index];
	hr = this->audioReader->GetCurrentMediaType(audioStreamIndex, type.GetAddressOf());
	return type;
}

uint32 MFAudioReader::GetAudioChannelCount(int index)
{
	uint32 channelCount;
	Microsoft::WRL::ComPtr<IMFMediaType> type = GetType(index);
	type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channelCount);
	return channelCount;
}

uint32 MFAudioReader::GetSampleRate(int index)
{
	uint32 sampleRate;
	Microsoft::WRL::ComPtr<IMFMediaType> type = GetType(index);
	type->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sampleRate);
	return sampleRate;
}

AudioSampleType MFAudioReader::GetStreamType(int audioIdx)
{
	HRESULT hr = S_OK;
	GUID tmpGuidType;
	Microsoft::WRL::ComPtr<IMFMediaType> realStreamType = GetType(audioIdx);

	hr = realStreamType->GetGUID(MF_MT_SUBTYPE, &tmpGuidType);

	if (tmpGuidType == MFAudioFormat_Float)
	{
		return AudioSampleType::FloatType;
	}
	else
	{
		return AudioSampleType::OtherType;
	}
}

void MFAudioReader::GetWaveInfo(int audioIdx, WAVEFORMATEX* &waveType, uint32 &waveLength)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IMFMediaType> streamType;
	int audioStreamIndex = audioStreamsVector[audioIdx];
	hr = this->audioReader->GetCurrentMediaType(audioStreamIndex, &streamType);
	hr = MFCreateWaveFormatExFromMFMediaType(streamType.Get(), &waveType, &waveLength);
}

Int64Rational MFAudioReader::GetAudioDuration()
{
	Int64Rational realDur;
	LONGLONG realDuration = this->FindAudioDuration();
	realDur.SetValue(realDuration);
	return realDur;
}


void MFAudioReader::Initialize(Windows::Storage::Streams::IRandomAccessStream ^stream)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IMFByteStream> byteStream;
	Microsoft::WRL::ComPtr<IMFMediaType> mediaType;
	Microsoft::WRL::ComPtr<IMFAttributes> mfAttributes;

	Auto::getInstance();

	hr = MFCreateMFByteStreamOnStreamEx((IUnknown *)stream, byteStream.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		hr = MFCreateAttributes(&mfAttributes, 1);
		hr = mfAttributes->SetUINT32(MF_LOW_LATENCY, TRUE);
		hr = MFCreateSourceReaderFromByteStream(byteStream.Get(), mfAttributes.Get(), this->audioReader.GetAddressOf());
	}

	this->FindAudioStreamIndexes();

	if (!this->audioStreamsVector.empty())
	{
		hr = MFCreateMediaType(mediaType.GetAddressOf());
		hr = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		hr = mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);

		if (SUCCEEDED(hr))
		{
			hr = this->audioReader->SetCurrentMediaType(0, NULL, mediaType.Get());
		}

		if (FAILED(hr))
		{
			throw new exception("hr = this->audioReader->SetCurrentMediaType(0, NULL, mediaType.Get());  -  this string is failed");
		}
	}
}

void MFAudioReader::FindAudioStreamIndexes()
{
	HRESULT hr = S_OK;
	DWORD dwStreamIndex = 0;
	Microsoft::WRL::ComPtr<IMFMediaType> mediaType;
	GUID type;

	while (SUCCEEDED(hr))
	{
		hr = this->audioReader->GetCurrentMediaType(dwStreamIndex, &mediaType);

		if (SUCCEEDED(hr))
		{
			hr = mediaType->GetGUID(MF_MT_MAJOR_TYPE, &type);
		}

		if (type == MFMediaType_Audio && mediaType != NULL)
		{
			this->audioStreamsVector.push_back(dwStreamIndex);
			dwStreamIndex++;
		}
	}
}

Microsoft::WRL::ComPtr<IMFSample> MFAudioReader::ReadSample()
{
	HRESULT hr = S_OK;
	DWORD streamIndex, flags;
	LONGLONG timeStamp;
	Microsoft::WRL::ComPtr<IMFSample> sample;
	Microsoft::WRL::ComPtr<IMFMediaType> tmpType;

	hr = this->audioReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &timeStamp, sample.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		hr = this->audioReader->GetCurrentMediaType(streamIndex, &tmpType);
	}
	else
	{
		sample = nullptr;
	}

	return sample;
}

AudioSample *MFAudioReader::ReadAudioSample()
{
	MFAudioSample *sample = nullptr;
	Microsoft::WRL::ComPtr<IMFSample> s = ReadSample();

	if (s)
	{
		sample = new MFAudioSample();
		sample->Initialize(s);
	}

	return sample;
}

LONGLONG MFAudioReader::FindAudioDuration()
{
	HRESULT hr = S_OK;
	PROPVARIANT var;
	LONGLONG duration = 0;

	hr = this->audioReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);

	if (SUCCEEDED(hr) && var.vt == VARENUM::VT_UI8)
	{
		duration = static_cast<LONGLONG>(var.uhVal.QuadPart);
	}

	return duration;
}

void MFAudioReader::SetPosition(Rational inputRational, double destination)
{
	HRESULT hr = S_OK;
	PROPVARIANT varPos;
	ULONGLONG hnsPos;
	Int64Rational position;

	position.SetRational(inputRational);
	position.SetValue(destination);
	hnsPos = position.Convert(Rational::SEC).value;

	varPos.uhVal.QuadPart = hnsPos;
	varPos.vt = VARENUM::VT_I8;
	
	hr = this->audioReader->SetCurrentPosition(GUID_NULL, varPos);
}