#pragma once

#include "AudioEvents.h"
#include "..\Reader.h"

#include <initguid.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <memory>

class MFAudioEvents : public AudioEvents
{
public:
	MFAudioEvents();
	~MFAudioEvents();

	virtual void EndOfPlaying(int c) override;
	virtual void EndOfRewinding() override;
	void InitEvent(EarthInBeatsNativeLibrary::Reader ^reader);

private:
	EarthInBeatsNativeLibrary::Reader ^e = ref new EarthInBeatsNativeLibrary::Reader();
};

