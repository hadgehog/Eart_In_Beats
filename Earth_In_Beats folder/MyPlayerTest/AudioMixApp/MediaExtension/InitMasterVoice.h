#pragma once

#include "Reader.h"

namespace MediaExtension
{
	// based on http://stackoverflow.com/questions/1008019/c-singleton-design-pattern
	class InitMasterVoice
	{
		InitMasterVoice();
		~InitMasterVoice();

		InitMasterVoice(InitMasterVoice const&);              // Don't Implement
		void operator=(InitMasterVoice const&);		//Don't implement

	public:
		static InitMasterVoice &GetInstance();
		Microsoft::WRL::ComPtr<IXAudio2> GetXAudio();

	private:
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
		IXAudio2MasteringVoice *masterVoice;
	};
}

