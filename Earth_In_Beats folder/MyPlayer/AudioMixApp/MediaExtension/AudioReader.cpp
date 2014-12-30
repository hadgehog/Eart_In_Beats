#include "pch.h"
#include "AudioReader.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>

using namespace MediaExtension;
using namespace Platform;

const Rational Rational::HNS(1, 10000000);
const Rational Rational::SEC(10000000, 1);

AudioReader::AudioReader()
{
}



AudioReader::~AudioReader()
{
}