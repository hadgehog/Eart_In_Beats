#include "pch.h"
#include "DX11DisplayTexture.h"

namespace DX11{
	namespace DisplayTexture{

		DisplayTexture::DisplayTexture()
			: defaultOrientation(Orientation::None), currentOrientation(Orientation::None),
			logicalDpi(0.0f), scale(0.0f, 0.0f), logicalSize(0.0f, 0.0f),
			physicalSize(0.0f, 0.0f){
		}

		DisplayTexture::~DisplayTexture(){
		}

		Orientation DisplayTexture::GetDefaultOrientation() const{
			return this->defaultOrientation;
		}

		void DisplayTexture::SetDefaultOrientation(Orientation v){
			this->defaultOrientation = v;
		}

		Orientation DisplayTexture::GetCurrentOrientation() const{
			return this->currentOrientation;
		}

		void DisplayTexture::SetCurrentOrientation(Orientation v){
			this->currentOrientation = v;
		}

		float DisplayTexture::GetLogicalDpi() const{
			return this->logicalDpi;
		}

		void DisplayTexture::SetLogicalDpi(float v){
			this->logicalDpi = v;
		}

		const DirectX::XMFLOAT2 &DisplayTexture::GetScale() const{
			return this->scale;
		}

		void DisplayTexture::SetScale(const DirectX::XMFLOAT2 &v){
			this->scale = v;
		}

		const DirectX::XMFLOAT2 &DisplayTexture::GetLogicalSize() const{
			return this->logicalSize;
		}

		void DisplayTexture::SetLogicalSize(const DirectX::XMFLOAT2 &v){
			this->logicalSize = v;
		}

		const DirectX::XMFLOAT2 &DisplayTexture::GetPhysicalSize() const{
			return this->physicalSize;
		}

		void DisplayTexture::SetPhysicalSize(const DirectX::XMFLOAT2 &v){
			this->physicalSize = v;
		}

	}
}