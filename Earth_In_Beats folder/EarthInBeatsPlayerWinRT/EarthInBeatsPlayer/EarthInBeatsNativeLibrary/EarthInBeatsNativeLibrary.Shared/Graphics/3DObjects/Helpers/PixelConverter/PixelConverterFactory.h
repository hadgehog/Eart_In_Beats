#pragma once
#include "..\H.h"
#include "PixelConverter.h"

#include <unordered_map>
#include <functional>
#include <memory>

struct PixelFormatConversionDesc{
	GUID SourceFormat;
	GUID DestinationFormat;
};

bool operator==(const PixelFormatConversionDesc &_this, const PixelFormatConversionDesc &other);

struct PixelFormatConversionDescHash{
	std::size_t operator()(const PixelFormatConversionDesc &v) const;
};

class PixelConverterFactory{
	static std::unordered_map <
		PixelFormatConversionDesc,
		std::function<PixelConverter *()>,
		PixelFormatConversionDescHash > creators;
	static int StaticCtorRes;
	static int StaticCtor();
	static void InitCreators();
public:
	static PixelConverter *CreateConverter(const GUID &src, const GUID &dst);
};