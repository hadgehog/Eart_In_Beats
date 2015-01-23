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

#pragma once

namespace MediaExtension
{
	class AudioEvents
	{
	public:
		AudioEvents();
		~AudioEvents();
		virtual void EndOfRewinding() = 0;
		virtual void EndOfPlaying(int c) = 0;
	};
}

