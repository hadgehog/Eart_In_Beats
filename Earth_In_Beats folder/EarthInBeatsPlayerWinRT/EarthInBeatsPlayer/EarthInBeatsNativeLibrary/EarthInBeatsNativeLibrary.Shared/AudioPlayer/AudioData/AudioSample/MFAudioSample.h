#pragma once

#include "AudioSample.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <cstdint>
#include <vector>

class MFAudioSample : public AudioSample
{
public:
	MFAudioSample();
	~MFAudioSample();
	virtual LONGLONG GetDuration() override;
	virtual LONGLONG GetSampleTime() override;
	void Initialize(Microsoft::WRL::ComPtr<IMFSample> sample);

protected:
	virtual void Lock(void** buffer, uint64_t* size) override;
	virtual void Unlock(void* buffer, uint64_t size) override;

private:
	LONGLONG sampleDuration, sampleTime;
	Microsoft::WRL::ComPtr<IMFMediaBuffer> sampleBuffer;
};
