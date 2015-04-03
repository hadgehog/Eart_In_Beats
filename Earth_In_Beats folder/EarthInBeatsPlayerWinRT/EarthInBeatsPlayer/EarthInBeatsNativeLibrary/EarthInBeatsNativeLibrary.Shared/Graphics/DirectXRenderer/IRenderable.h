#pragma once

#include "NativeRenderableContainer.h"

namespace EarthInBeatsNativeLibrary
{
	//calls in C# but implementing in C++
	public interface class IRenderable{
		NativeRenderableContainer ^Get();
	};
}