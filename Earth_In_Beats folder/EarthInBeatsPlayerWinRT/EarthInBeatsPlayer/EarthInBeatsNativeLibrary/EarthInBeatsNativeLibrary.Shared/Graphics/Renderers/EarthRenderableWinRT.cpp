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

		this->nativeRenderer->showSlidersEvent = [=](bool v) {
			concurrency::create_async([=]() {
				this->ShowSlidersEvent(v);
			});
		};

		this->nativeRenderer->horizontalManipulationChanged = [=](float p) {
			concurrency::create_async([=]() {
				this->HorizontalManipulationEvent(p);
			});
		};

		this->nativeRenderer->verticalManipulationChanged = [=](float p) {
			concurrency::create_async([=]() {
				this->VerticalManipulationEvent(p);
			});
		};
	}

	EarthRenderableWinRT::~EarthRenderableWinRT() {
	}

	NativeRenderableContainer ^EarthRenderableWinRT::Get() {
		return this->rendererContainer;
	}

	Windows::Foundation::IAsyncAction ^EarthRenderableWinRT::Load3DModel(Platform::String ^path) {
		return concurrency::create_async([=]() {
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

	void EarthRenderableWinRT::ResetRotationAngle() {
		this->nativeRenderer->ResetRotationAngles();
	}

	void EarthRenderableWinRT::IsPlaylistCreated(bool play) {
		this->nativeRenderer->SetPlayingMode(play);
	}

	float EarthRenderableWinRT::HorisontalRotationAngle::get() {
		return -1.0f;	// no implemented
	}

	void EarthRenderableWinRT::HorisontalRotationAngle::set(float a) {
		this->nativeRenderer->SetHorisontalRotationAngle(a);
	}

	float EarthRenderableWinRT::VericalRotationAngle::get() {
		return -1.0f;	// no implemented
	}

	void EarthRenderableWinRT::VericalRotationAngle::set(float a) {
		this->nativeRenderer->SetVerticalRotationAngle(a);
	}

	bool EarthRenderableWinRT::ManipulationMode::get() {
		return this->nativeRenderer->GetManipulationMode();
	}
}