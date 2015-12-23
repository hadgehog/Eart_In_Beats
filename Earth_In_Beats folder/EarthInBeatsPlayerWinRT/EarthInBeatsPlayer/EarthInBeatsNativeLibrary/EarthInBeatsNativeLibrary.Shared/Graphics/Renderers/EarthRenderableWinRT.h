#pragma once

#include "Graphics\DirectXWinRTHelpers\IRenderable.h"
#include "EarthRendererNative.h"

namespace EarthInBeatsNativeLibrary
{
	public ref class EarthRenderableWinRT sealed : public IRenderable{
	public:
		EarthRenderableWinRT();
		virtual ~EarthRenderableWinRT();

		virtual NativeRenderableContainer ^Get();

		Windows::Foundation::IAsyncAction ^Load3DModel(Platform::String ^path);
		Windows::Foundation::IAsyncAction ^LoadModelTexture(Platform::String ^path);
		Windows::Foundation::IAsyncAction ^LoadBackgroundTexture(Platform::String ^path);
		
		property bool EarthRotationEnabled {
			bool get();
			void set(bool v);
		}

	private:
		NativeRenderableContainer ^rendererContainer;
		std::shared_ptr<EarthRendererNative> nativeRenderer;
	};
}