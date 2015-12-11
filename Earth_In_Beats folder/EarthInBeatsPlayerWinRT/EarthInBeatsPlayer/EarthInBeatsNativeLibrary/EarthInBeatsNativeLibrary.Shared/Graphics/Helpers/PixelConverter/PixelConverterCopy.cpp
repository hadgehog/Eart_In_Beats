#include "PixelConverterCopy.h"

#include <iostream>

PixelConverterCopy::PixelConverterCopy(uint32_t pixelByteSize)
	: pixelByteSize(pixelByteSize){
}

PixelConverterCopy::~PixelConverterCopy(){
}

void PixelConverterCopy::Convert(void *dst, const void *src, uint32_t pixelCount){
	std::memcpy(dst, src, this->pixelByteSize * pixelCount);
}