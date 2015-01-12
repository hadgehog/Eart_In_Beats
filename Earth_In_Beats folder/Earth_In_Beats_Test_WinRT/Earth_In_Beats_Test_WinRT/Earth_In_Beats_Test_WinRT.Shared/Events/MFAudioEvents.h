#pragma once

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
#include "AudioEvents.h"
#include "..\Wrapper\Reader.h"

namespace MediaExtension
{
	class MFAudioEvents : public AudioEvents
	{
	public:
		MFAudioEvents();
		~MFAudioEvents();

		virtual void EndOfPlaying(int c) override;
		virtual void EndOfRewinding() override;
		void InitEvent(Reader ^reader);

	private:
		Reader ^e = ref new Reader();
	};
}

