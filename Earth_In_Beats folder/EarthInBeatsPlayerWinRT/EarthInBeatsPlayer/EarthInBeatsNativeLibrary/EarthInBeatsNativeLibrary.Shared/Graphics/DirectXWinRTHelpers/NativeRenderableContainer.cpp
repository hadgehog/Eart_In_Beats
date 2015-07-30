#include "NativeRenderableContainer.h"

namespace EarthInBeatsNativeLibrary
{
	NativeRenderableContainer::NativeRenderableContainer(){
	}

	NativeRenderableContainer::~NativeRenderableContainer(){

	}

	void NativeRenderableContainer::Set(const std::shared_ptr<INativeRenderable> &nativeRender){
		this->nativeRender = nativeRender;
	}

	std::shared_ptr<INativeRenderable> NativeRenderableContainer::Get(){
		return this->nativeRender;
	}
}