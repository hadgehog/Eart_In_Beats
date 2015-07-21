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
	std::vector<VertexTextureNormal> posTexNormList;

	scene = modelImporter.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (scene){
		mesh = scene->mMeshes[0];

		if (mesh){
			if (mesh->HasFaces()){
				faces = mesh->mFaces;
				indicesNum = mesh->mNumFaces * 3;
				materialIndices.resize(indicesNum);

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
				verticesNum = mesh->mNumVertices;

				posTexNormList.resize(verticesNum);

				for (size_t i = 0; i < verticesNum; i++){
					posTexNormList[i].Position = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

					//DirectX::XMStoreFloat3(&posTexNormList[i].Position, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&posTexNormList[i].Position), mLocalWorld));
				}

				if (mesh->HasNormals()){
					for (size_t i = 0; i < verticesNum; i++){
						DirectX::XMVECTOR xvNormal = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&mesh->mNormals[i]);
						xvNormal = DirectX::XMVector3Normalize(xvNormal);

						DirectX::XMStoreFloat3(&posTexNormList[i].Normal, xvNormal);
					}
				}

				if (mesh->HasTextureCoords(0)) {
					for (size_t i = 0; i < verticesNum; i++){
						posTexNormList[i].TextureCoord= DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
					}
				}
			}
			int stop = 3;
		}
		else{
			H::System::DebuggerBreak();
		}
	}
	else{
		H::System::DebuggerBreak();
	}
}
