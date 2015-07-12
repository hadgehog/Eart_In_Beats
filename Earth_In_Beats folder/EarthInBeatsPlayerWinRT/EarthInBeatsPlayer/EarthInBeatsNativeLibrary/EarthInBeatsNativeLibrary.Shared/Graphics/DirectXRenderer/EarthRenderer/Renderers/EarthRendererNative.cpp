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

void EarthRendererNative::LoadModel(std::string path){
	//where will be works assimp
	Assimp::Importer modelImporter;
	const aiScene *scene = NULL;
	const aiMesh *mesh = NULL;
	const aiFace *faces = NULL;
	int indicesNum = 0;
	std::vector<int> materialIndices;

	scene = modelImporter.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (scene){
		mesh = scene->mMeshes[0];

		if (mesh){

			if (mesh->HasFaces()){
				indicesNum = mesh->mNumFaces * 3;
				materialIndices.resize(indicesNum);
				faces = mesh->mFaces;

				for (size_t i = 0; i < mesh->mNumFaces; i++){
					if (faces && faces[i].mNumIndices == 3){
						for (size_t j = 0; j < 3; j++){
							materialIndices[i * 3 + j] = faces[i].mIndices[j];
						}
					}
					else{
						H::System::DebuggerBreak();
					}
				}
			}

			if (mesh->HasPositions()){
				int verticesNum = 0;
				std::vector<float> vertices;
				
				verticesNum = mesh->mNumVertices;
				vertices.resize(verticesNum);

				for (size_t i = 0; i < verticesNum; i++){

				}
			}

		}
		else{
			H::System::DebuggerBreak();
		}
	}
	else{
		H::System::DebuggerBreak();
	}
}
