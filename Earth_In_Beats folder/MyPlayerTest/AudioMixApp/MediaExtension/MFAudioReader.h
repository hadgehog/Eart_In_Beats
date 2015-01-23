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
#include "AudioReader.h"

#pragma once

using namespace std;

namespace MediaExtension
{
	class MFAudioReader : public AudioReader
	{
	public:	
		MFAudioReader();
		~MFAudioReader();
		virtual uint32 GetAudioStreamCount() override;
		virtual uint32 GetSampleRate(int index) override;
		virtual uint32 GetAudioChannelCount(int index) override;
		virtual AudioSampleType GetStreamType(int index) override;
		virtual void GetWaveInfo(int audioIdx, WAVEFORMATEX* &waveType, uint32 &waveLength) override;
		virtual Int64Rational GetAudioDuration() override;
		void Initialize(Windows::Storage::Streams::IRandomAccessStream ^stream);
		virtual AudioSample *ReadAudioSample() override;
		virtual void SetPosition(Rational inputRational, double destination) override;

	private:
		Microsoft::WRL::ComPtr<IMFSourceReader> audioReader;
		vector<int> audioStreamsVector;

		Microsoft::WRL::ComPtr<IMFMediaType> GetType(int index);
		void FindAudioStreamIndexes();
		Microsoft::WRL::ComPtr<IMFSample> ReadSample();
		LONGLONG FindAudioDuration();
	};
}
