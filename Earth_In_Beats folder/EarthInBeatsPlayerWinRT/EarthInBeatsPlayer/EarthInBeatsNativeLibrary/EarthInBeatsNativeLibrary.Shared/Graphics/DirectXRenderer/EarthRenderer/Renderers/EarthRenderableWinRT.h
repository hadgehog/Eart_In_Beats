#pragma once

#include "Graphics\DirectXRenderer\DirectXWinRTHelpers\IRenderable.h"
#include "EarthRendererNative.h"

namespace EarthInBeatsNativeLibrary
{
	public ref class EarthRenderableWinRT sealed : public IRenderable{
	public:
		EarthRenderableWinRT();
		virtual ~EarthRenderableWinRT();

		virtual NativeRenderableContainer ^Get();

		///////////////  all winrt methods
		Windows::Foundation::IAsyncAction ^Load3DModel(Windows::Storage::Streams::IRandomAccessStream ^stream_not_disposing);

		//Windows::Foundation::IAsyncOperation<Windows::UI::Color> ^GetBackgroundColorAsync();
		//Windows::Foundation::IAsyncAction ^SetBackgroundColorAsync(Windows::UI::Color v);

	private:
		NativeRenderableContainer ^rendererContainer;
		std::shared_ptr<EarthRendererNative> nativeRenderer;
	};
}