#include "pch.h"
#include "Auto.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mftransform.h>
#include <mferror.h>
#include <wrl.h>

using namespace MediaExtension;
using namespace Platform;

Auto& Auto::getInstance()
{
	static Auto instance;
	return instance;
}

Auto::Auto()
{
	MFStartup(MF_VERSION);
}

Auto::~Auto()
{
	MFShutdown();
}

