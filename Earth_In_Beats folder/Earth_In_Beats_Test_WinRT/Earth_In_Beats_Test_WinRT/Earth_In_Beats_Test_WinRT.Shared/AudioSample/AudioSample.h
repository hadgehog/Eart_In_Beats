#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <cstdint>

#pragma once

namespace MediaExtension
{
	enum class AudioSampleType	{ FloatType, OtherType };

	class AudioSample
	{
	public:
		AudioSample();
		virtual ~AudioSample();
		virtual LONGLONG GetDuration() = 0;
		virtual LONGLONG GetSampleTime() = 0;

		template<class Callable>
		void GetData(Callable accessor)
		{
			void *buffer = nullptr;
			uint64_t size;

			this->Lock(&buffer, &size);

			accessor(buffer, size);

			this->Unlock(buffer, size);
		}

	protected:
		virtual void Lock(void** buffer, uint64_t* size) = 0;
		virtual void Unlock(void* buffer, uint64_t size) = 0;
	};
}