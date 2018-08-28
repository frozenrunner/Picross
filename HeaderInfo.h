#pragma once
class HeaderInfo
{
	int _width, _height,
		_bitDepth, _colourType, _compressType,
		_filterMethod, _interlaceMethod;

	public:
		HeaderInfo();
		HeaderInfo(int width, int height, int bitDepth, int colourType,
			int compressType, int filterMethod, int interlaceMethod);
		~HeaderInfo();
		void setValues(int width, int height, int bitDepth, int colourType,
			int compressType, int filterMethod, int interlaceMethod);
		int getWidth();
		int getHeight();
		int getBitDepth();
		int getColourType();
		int getCompressType();
		int getFilterMethod();
		int getInterlaceMethod();
};

