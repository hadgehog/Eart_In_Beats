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

		void ResetRotationAngle();
		
		property bool EarthRotationEnabled {
			bool get();
			void set(bool v);
		}

		property float HorisontalRotationAngle {
			float get();
			void set(float a);
		}

		property float VericalRotationAngle {
			float get();
			void set(float a);
		}

	private:
		NativeRenderableContainer ^rendererContainer;
		std::shared_ptr<EarthRendererNative> nativeRenderer;
	};
}