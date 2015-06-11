#include "EarthRenderableWinRT.h"

namespace EarthInBeatsNativeLibrary
{
	EarthRenderableWinRT::EarthRenderableWinRT(){
	}

	EarthRenderableWinRT::~EarthRenderableWinRT(){
	}

	NativeRenderableContainer ^EarthRenderableWinRT::Get(){
		return this->rendererContainer;
	}
}