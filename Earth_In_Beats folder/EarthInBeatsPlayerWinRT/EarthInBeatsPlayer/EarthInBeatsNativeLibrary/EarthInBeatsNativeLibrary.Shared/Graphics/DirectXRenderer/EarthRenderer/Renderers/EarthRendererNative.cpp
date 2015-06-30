#include "EarthRendererNative.h"
#include "..\..\Helpers\H.h"

#include <fstream>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

EarthRendererNative::EarthRendererNative(){
}

EarthRendererNative::~EarthRendererNative(){
}

void EarthRendererNative::Initialize(const std::shared_ptr<DX::DeviceResources> &dxDev){

}

void EarthRendererNative::CreateDeviceDependentResources(){

}

void EarthRendererNative::ReleaseDeviceDependentResources(){

}

void EarthRendererNative::CreateSizeDependentResources(){

}

void EarthRendererNative::OnRenderThreadStart(){

}

void EarthRendererNative::OnRenderThreadEnd(){

}

void EarthRendererNative::Update(const DX::StepTimer &timer){

}

void EarthRendererNative::Render(){

}

void EarthRendererNative::PointerPressed(Windows::UI::Input::PointerPoint ^ppt){

}

void EarthRendererNative::PointerMoved(Windows::UI::Input::PointerPoint ^ppt){

}

void EarthRendererNative::PointerReleased(Windows::UI::Input::PointerPoint ^ppt){

}

void EarthRendererNative::LoadModel(std::string path){	//later stream
	//where will be works assimp
	Assimp::Importer modelImporter;
	const aiScene *scene = NULL;
	const aiMesh *mesh = NULL;

	scene = modelImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (scene){

	}
	else{
		H::System::DebuggerBreak();
	}
}
