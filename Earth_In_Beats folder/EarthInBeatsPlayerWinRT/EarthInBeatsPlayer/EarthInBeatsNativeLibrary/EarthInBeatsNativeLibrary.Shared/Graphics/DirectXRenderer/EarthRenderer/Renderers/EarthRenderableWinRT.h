#pragma once

#include "Graphics\DirectXRenderer\IRenderable.h"

namespace EarthInBeatsNativeLibrary
{
	public ref class EarthRenderableWinRT sealed : IRenderable{
	public:
		EarthRenderableWinRT();
		virtual ~EarthRenderableWinRT();

		virtual NativeRenderableContainer ^Get();

	private:
		NativeRenderableContainer ^rendererContainer;

	};
}