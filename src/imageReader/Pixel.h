#pragma once
class Pixel
{
	unsigned int _rgba = 0;
public:
	
	Pixel();
	Pixel(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	unsigned int getRGBA();
	unsigned int getRed();
	unsigned int getGreen();
	unsigned int getBlue();
	unsigned int getAlpha();
	void setPixelValues(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
	void zeroPixels();
	~Pixel();
};

