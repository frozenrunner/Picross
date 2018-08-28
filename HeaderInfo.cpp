#include "stdafx.h"
#include "HeaderInfo.h"

HeaderInfo::HeaderInfo EMSCRIPTEN_KEEPALIVE(){
	_width = 0;
	_height = 0;
	_bitDepth = 0;
	_colourType = 0;
	_compressType = 0;
	_filterMethod = 0;
	_interlaceMethod = 0;
}

HeaderInfo::HeaderInfo EMSCRIPTEN_KEEPALIVE (int width, int height, int bitDepth, int colourType, int compressType, int filterMethod, int interlaceMethod)
{
	setValues(width, height, bitDepth, colourType, compressType, filterMethod, interlaceMethod);
}

HeaderInfo::~HeaderInfo EMSCRIPTEN_KEEPALIVE()
{
}

void HeaderInfo::setValues(int width, int height, int bitDepth, int colourType, int compressType, int filterMethod, int interlaceMethod) {
	_width = width;
	_height = height;
	_bitDepth = bitDepth;
	_colourType = colourType;
	_compressType = compressType;
	_filterMethod = filterMethod;
	_interlaceMethod = interlaceMethod;
}

int EMSCRIPTEN_KEEPALIVE HeaderInfo::getWidth() {
	return _width;
}

int EMSCRIPTEN_KEEPALIVE HeaderInfo::getHeight() {
	return _height;
}

int HeaderInfo::getBitDepth()
{
	return _bitDepth;
}

int HeaderInfo::getColourType()
{
	return _colourType;
}

int HeaderInfo::getCompressType()
{
	return _compressType;
}

int HeaderInfo::getFilterMethod()
{
	return _filterMethod;
}

int HeaderInfo::getInterlaceMethod()
{
	return _interlaceMethod;
}
