#pragma once
#include "H.h"
#include "ImageUtilsEncodeOptions.h"

#include <wincodec.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <unordered_map>

typedef GUID GUID_ContainerFormat;
typedef GUID GUID_WICPixelFormat;

// rotations are clock-wise
// exif flags starts from 1
enum class ExifRotationFlag : uint16_t{
	NoTransform = 1,
	FlipHorizontal = 2,
	Rotate180 = 3,
	FlipVertcal = 4,
	Rotate90FlipHorizontal = 5,
	Rotate270 = 6,
	Rotate90FlipVertcal = 7,
	Rotate90 = 8,
};

class ImageUtils{
private:
	static std::unordered_map<GUID_WICPixelFormat, uint32_t, GUIDHash> WICPixelFormatBitSize;

	static int StaticCtorTmp;
	static int StaticCtor();
public:
	ImageUtils();
	~ImageUtils();

	uint32_t GetFrameCount(IWICBitmapDecoder *decoder) const;
	uint32_t GetPixelBitSize(IWICBitmapSource *frame) const;
	uint32_t GetPixelBitSize(const GUID_WICPixelFormat &fmt) const;
	uint32_t GetFrameStride(IWICBitmapSource *frame) const;
	uint32_t GetFrameByteSize(IWICBitmapSource *frame) const;
	DirectX::XMUINT2 GetFrameSize(IWICBitmapSource *frame) const;
	ExifRotationFlag GetExifRotation(IWICBitmapFrameDecode *frame) const;
	WICBitmapTransformOptions GetInversedRotationFlipOptions(ExifRotationFlag v) const;
	WICBitmapTransformOptions GetRotationFlipOptions(ExifRotationFlag v) const;

	void Initialize();

	// Decoding:

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> CreateDecoder(Windows::Storage::Streams::IRandomAccessStream ^stream) const;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> CreateFrameForDecode(IWICBitmapDecoder *decoder, uint32_t idx = 0) const;

	void DecodePixels(IWICBitmapSource *frame, uint32_t pixelsByteSize, void *pixels, const WICRect *rect = nullptr) const;

	Microsoft::WRL::ComPtr<IWICBitmap> LoadToMemory(IWICBitmapSource *frame) const;

	// Encoding:

	Microsoft::WRL::ComPtr<IWICBitmapEncoder> CreateEncoder(
		Windows::Storage::Streams::IRandomAccessStream ^stream, 
		const GUID_ContainerFormat &containerFormat) const;

	Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> CreateFrameForEncode(IWICBitmapEncoder *encoder);

	template<class OptsImpl>
	Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> CreateFrameForEncode(
		IWICBitmapEncoder *encoder,
		ImageUtilsEncodeOptions<OptsImpl> &options) const
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<IPropertyBag2> props;
		Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> encodeFrame;

		hr = encoder->CreateNewFrame(encodeFrame.GetAddressOf(), props.GetAddressOf());
		H::System::ThrowIfFailed(hr);

		hr = options.Save(props.Get());
		H::System::ThrowIfFailed(hr);

		hr = encodeFrame->Initialize(props.Get());
		H::System::ThrowIfFailed(hr);
		
		return encodeFrame;
	}

	void EncodeAllocPixels(
		IWICBitmapFrameEncode *encodeFrame,
		const DirectX::XMUINT2 &size,
		const GUID_WICPixelFormat &fmt) const;
	// actualFmt can differ from fmt
	void EncodeAllocPixels(
		IWICBitmapFrameEncode *encodeFrame,
		const DirectX::XMUINT2 &size,
		const GUID_WICPixelFormat &fmt,
		GUID_WICPixelFormat &actualFmt) const;
	void EncodePixels(
		IWICBitmapFrameEncode *encodeFrame,
		uint32_t lineCount, uint32_t stride, 
		uint32_t pixelsByteSize, const void *pixels) const;
	void EncodeFrame(
		IWICBitmapFrameEncode *encodeFrame,
		IWICBitmapSource *frame,
		WICRect *rect = nullptr) const;
	void EncodeCommit(IWICBitmapFrameEncode *encodeFrame) const;
	void EncodeCommit(IWICBitmapEncoder *encoder) const;

	// Transforms:

	Microsoft::WRL::ComPtr<IWICFormatConverter> ConvertPixelFormatInMemory(
		IWICBitmapSource *frame, 
		const GUID_WICPixelFormat &destFmt
		) const;
	Microsoft::WRL::ComPtr<IWICBitmapScaler> Scale(
		IWICBitmapSource *frame,
		const DirectX::XMUINT2 &destSize,
		WICBitmapInterpolationMode interpolationMode = WICBitmapInterpolationMode::WICBitmapInterpolationModeLinear
		) const;
	Microsoft::WRL::ComPtr<IWICBitmapFlipRotator> RotateFlipInMemory(
		IWICBitmapSource *frame,
		WICBitmapTransformOptions options
		) const;
	Microsoft::WRL::ComPtr<IWICBitmapClipper> Clip(
		IWICBitmapSource *frame,
		const WICRect *prc
		) const;
private:
	Microsoft::WRL::ComPtr<IWICImagingFactory2>	wicFactory;

	static WICBitmapTransformOptions RotationFlipOptionsFromExif(ExifRotationFlag v);
	static WICBitmapTransformOptions RotationFlipOptionsFromExifInversed(ExifRotationFlag v);
};