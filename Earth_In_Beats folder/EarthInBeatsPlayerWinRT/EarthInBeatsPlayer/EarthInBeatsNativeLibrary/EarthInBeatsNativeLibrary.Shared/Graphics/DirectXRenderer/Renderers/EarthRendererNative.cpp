#include "EarthRendererNative.h"
#include "..\Helpers\H.h"

#include <fstream>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

EarthRendererNative::EarthRendererNative() : initialized(false){
	DirectX::XMStoreFloat4x4(&this->projection, DirectX::XMMatrixIdentity());
}

EarthRendererNative::~EarthRendererNative(){
}

void EarthRendererNative::Initialize(const std::shared_ptr<GuardedDeviceResources> &dx){
	this->initializeTask = concurrency::create_task([=]() {
		this->dx = dx;
		this->dxResources = std::unique_ptr<MediaRendererDxResources>(new MediaRendererDxResources(this->dx));
		auto dxDev = this->dx->Get();

		concurrency::critical_section::scoped_lock lk(this->dataCs);

		auto d3dDev = dxDev->GetD3DDevice();

		this->quad = this->dxResources->Geometry.GetQuad();
		this->quadVs = this->dxResources->Shader.GetQuadVs();
		this->quadVsCbuffer = std::unique_ptr<QuadVs::Cbuffer>(new QuadVs::Cbuffer(dxDev));
		this->quadPs = this->dxResources->Shader.GetQuadPs();
		this->quadSampler = this->dxResources->Sampler.GetLinearSampler();

		std::unique_lock<std::mutex> lkInit(this->initializedMtx);
		this->initialized = true;
		this->inititalizedCv.notify_all();
	});
}

void EarthRendererNative::CreateDeviceDependentResources(){

}

void EarthRendererNative::ReleaseDeviceDependentResources(){

}

void EarthRendererNative::CreateSizeDependentResources(){
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	auto size = dxDev->GetLogicalSize();
	//resize model
}

void EarthRendererNative::OnRenderThreadStart(){

}

void EarthRendererNative::OnRenderThreadEnd(){

}

void EarthRendererNative::Update(const DX::StepTimer &timer){

}

void EarthRendererNative::Render(){
	this->WaitForInitialization();

	auto dxDev = this->dx->Get();
	concurrency::critical_section::scoped_lock lk(this->dataCs);

	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&this->projection);

	//render model
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

void EarthRendererNative::WaitForInitialization() {
	std::unique_lock<std::mutex> lk(this->initializedMtx);

	while (!this->initialized) {
		this->inititalizedCv.wait(lk);
	}
}
