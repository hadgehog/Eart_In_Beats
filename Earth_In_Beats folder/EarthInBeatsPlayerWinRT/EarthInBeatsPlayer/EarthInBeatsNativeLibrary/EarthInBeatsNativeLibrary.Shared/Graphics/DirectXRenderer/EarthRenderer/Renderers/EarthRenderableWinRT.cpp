#include "EarthRenderableWinRT.h"

#include "..\..\Helpers\H.h"

#include <mfidl.h>

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Data::Json;

namespace EarthInBeatsNativeLibrary
{
	EarthRenderableWinRT::EarthRenderableWinRT()
	{
		this->rendererContainer = ref new NativeRenderableContainer();
		this->nativeRenderer = std::make_shared<EarthRendererNative>();

		this->rendererContainer->Set(nativeRenderer);
	}

	EarthRenderableWinRT::~EarthRenderableWinRT(){
	}

	NativeRenderableContainer ^EarthRenderableWinRT::Get(){
		return this->rendererContainer;
	}

	Windows::Foundation::IAsyncAction ^EarthRenderableWinRT::Load3DModel(Windows::Storage::Streams::IRandomAccessStream ^stream_not_disposing){
		return concurrency::create_async([=](){

		});
	}
}