#include "stdafx.h"
#include "Pixel.h"

//#define DEBUG
#ifdef DEBUG
	#include <string>
	#include <iostream>
	#include <iomanip>
	#include <bitset>
#include <vector>
using namespace std;
#endif

#define REDMASK 4278190080
#define REDSHIFT 24

#define GREENMASK 16711680
#define GREENSHIFT 16

#define BLUEMASK 65280
#define BLUESHIFT 8

#define ALPHAMASK 255
unsigned int _rgba;
Pixel::Pixel()
{
	_rgba = 0;
}

Pixel::Pixel(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	setPixelValues(red, green, blue, alpha);
}

unsigned int Pixel::getRGBA() {
	return _rgba;
}

unsigned int Pixel::getRed() {
	return (unsigned int) (REDMASK & _rgba) >> REDSHIFT;
}

unsigned int Pixel::getGreen() {
	return (unsigned int) ((GREENMASK & _rgba) << BLUESHIFT) >> REDSHIFT;
}

unsigned int Pixel::getBlue() {
	return (unsigned int) ((BLUEMASK & _rgba) << GREENSHIFT) >> REDSHIFT;
}

unsigned int Pixel::getAlpha() {
	return (unsigned int) ((ALPHAMASK & _rgba) << REDSHIFT) >> REDSHIFT;
}

void Pixel::setPixelValues(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
#ifdef DEBUG
	cout << "RGB" << endl;
	cout << bitset<8>(red) << endl;
	cout << bitset<8>(green) << endl;
	cout << bitset<8>(blue) << endl;
	cout << bitset<8>(alpha) << endl;
	cout << "INIT RGB" << endl;
	cout << bitset<32>(_rgba) << endl;
#endif
	_rgba = _rgba | (unsigned int) red << REDSHIFT;
	_rgba = _rgba | (unsigned int) green << GREENSHIFT;
	_rgba = _rgba | (unsigned int) blue << BLUESHIFT;
	_rgba = _rgba | (unsigned int) alpha;
#ifdef DEBUG
	cout << bitset<32>(_rgba) << endl;
#endif
}

void Pixel::zeroPixels() {
	_rgba = 0;
}

Pixel::~Pixel()
{
}
