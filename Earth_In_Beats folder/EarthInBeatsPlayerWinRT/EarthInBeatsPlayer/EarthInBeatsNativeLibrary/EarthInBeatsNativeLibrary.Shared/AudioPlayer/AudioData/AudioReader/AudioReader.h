#pragma once

#include "..\AudioSample\AudioSample.h"
#include "..\..\AudioEvents\AudioEvents.h"
#include "..\..\AudioHelpers\Rational.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>

class AudioReader
{
public:
	AudioReader();
	virtual ~AudioReader();
	virtual uint32 GetAudioStreamCount() = 0;
	virtual uint32 GetSampleRate(int index) = 0;
	virtual uint32 GetAudioChannelCount(int index) = 0;
	virtual AudioSampleType GetStreamType(int index) = 0;
	virtual void GetWaveInfo(int index, WAVEFORMATEX* &waveType, uint32 &waveLength) = 0;
	virtual Int64Rational GetAudioDuration() = 0;
	virtual AudioSample *ReadAudioSample() = 0;
	virtual void SetPosition(const Int64Rational &position) = 0;
};