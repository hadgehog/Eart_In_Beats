#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include "AudioSample.h"
#include "AudioEvents.h"

#pragma once

struct Rational
{
	static const Rational HNS;
	static const Rational SEC;

	Rational(): num(1), den(1){}

	Rational(int32_t num, int32_t den)
	{
		this->num = num;
		this->den = den;
	}

	void SetRational(int32_t num, int32_t den)
	{
		this->num = num;
		this->den = den;
	}
	
	int32_t num;
	int32_t den;
};

template<class T>
class RationalValue
{
public:
	Rational unit;
	T value;

	void SetValue(T inputValue)
	{
		this->value = inputValue;
	}

	void SetRational(Rational inputUnit)
	{
		this->unit = inputUnit;
	}

	RationalValue &Convert(const Rational &r)
	{
		if (unit.den != r.den && unit.num != r.num)
			value = value * unit.num * r.den / unit.den * r.num;
		else
			value = value * r.num / r.den;
		unit = r;
		return *this;
	}
};

typedef RationalValue<int64_t> Int64Rational;
typedef RationalValue<double>  DoubleRational;

namespace MediaExtension
{
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
		virtual void SetPosition(Rational inputRational, double destination) = 0;
	};
}