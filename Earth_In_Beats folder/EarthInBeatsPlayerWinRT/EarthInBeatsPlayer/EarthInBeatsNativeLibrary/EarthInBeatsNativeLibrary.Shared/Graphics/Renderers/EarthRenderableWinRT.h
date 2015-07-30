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

		///////////////  all winrt methods
		void Load3DModel(Platform::String ^path);


	private:
		NativeRenderableContainer ^rendererContainer;
		std::shared_ptr<EarthRendererNative> nativeRenderer;
	};
}