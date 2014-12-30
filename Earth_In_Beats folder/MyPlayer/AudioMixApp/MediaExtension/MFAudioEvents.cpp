#include "pch.h"
#include "MFAudioEvents.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>
#include <combaseapi.h>

using namespace MediaExtension;
using namespace Platform;

MFAudioEvents::MFAudioEvents()
{

}

MFAudioEvents::~MFAudioEvents()
{

}

void MFAudioEvents::EndOfPlaying(int c)
{
	this->e->EndOfPlayingTrack(c);
}

void MFAudioEvents::EndOfRewinding()
{
	this->e->EndOfRewindingTrack();
}

void MFAudioEvents::InitEvent(Reader ^reader)
{
	this->e = reader;
}
