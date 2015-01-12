#include "pch.h"
#include "..\AudioReader\MFAudioReader.h"
#include "..\AudioSample\MFAudioSample.h"
#include "Auto.h"
#include "XAudio2Player.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <combaseapi.h>

using namespace MediaExtension;
using namespace Platform;

void XAudio2Player::OnBufferEnd(void* pContext)
{
	if (!this->samples.empty())
		this->DeleteSamples();
	SubmitBuffer();
}


