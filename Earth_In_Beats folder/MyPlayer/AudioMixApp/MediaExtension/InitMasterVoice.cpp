#include "pch.h"
#include "InitMasterVoice.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>

using namespace MediaExtension;
using namespace Platform;

InitMasterVoice::InitMasterVoice()
{
	HRESULT hr = S_OK;
	hr = XAudio2Create(&this->xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	hr = this->xAudio2->CreateMasteringVoice(&this->masterVoice, XAUDIO2_DEFAULT_CHANNELS, 0, 0, nullptr, nullptr, AudioCategory_BackgroundCapableMedia);
}

InitMasterVoice::~InitMasterVoice()
{
	this->masterVoice->DestroyVoice();
}

InitMasterVoice &InitMasterVoice::GetInstance()
{
	static InitMasterVoice instance;
	return instance;
}

Microsoft::WRL::ComPtr<IXAudio2> InitMasterVoice::GetXAudio()
{
	return this->xAudio2;
}
