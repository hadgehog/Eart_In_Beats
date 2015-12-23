#include "EarthRenderableWinRT.h"

#include "..\Helpers\H.h"

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

	Windows::Foundation::IAsyncAction ^EarthRenderableWinRT::Load3DModel(Platform::String ^path){
		return concurrency::create_async([=](){
			std::string strPath = H::Text::ConvertToUTF8(path);
			this->nativeRenderer->LoadModel(strPath);
		});
	}

	Windows::Foundation::IAsyncAction ^EarthRenderableWinRT::LoadModelTexture(Platform::String ^path) {
		return concurrency::create_async([=]() {
			this->nativeRenderer->LoadModelTexture(path->Data());
		});
	}

	Windows::Foundation::IAsyncAction ^EarthRenderableWinRT::LoadBackgroundTexture(Platform::String ^path) {
		return concurrency::create_async([=]() {
			this->nativeRenderer->LoadBackgroundTexture(path->Data());
		});
	}

	bool EarthRenderableWinRT::EarthRotationEnabled::get() {
		return this->nativeRenderer->GetEarthRotationEnabled();
	}

	void EarthRenderableWinRT::EarthRotationEnabled::set(bool v) {
		this->nativeRenderer->SetEarthRotationEnabled(v);
	}
}