// ImageReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>
#include <vector>
#include <math.h>
#include <cmath>
#include "HeaderInfo.h"
#include "Pixel.h"
#include <emscripten/emscripten.h>
#include "zlib.h"

#define TYPESIZE 4
#define LENGTHSIZE 4
#define CRCSIZE 4
#define CHUNK 16384

using namespace std;

char* fileContents;
int fileSize = 0;
vector<Pixel> pixelVector;
vector<int> integerPixelVector;
HeaderInfo header;

int EMSCRIPTEN_KEEPALIVE UncompressData(const char* abSrc, int nLenSrc, char* abDst, int nLenDst)
{
	z_stream zInfo = { 0 };
	zInfo.total_in = zInfo.avail_in = nLenSrc;
	zInfo.total_out = zInfo.avail_out = nLenDst;
	zInfo.next_in = (Byte*)abSrc;
	zInfo.next_out = (Byte*)abDst;

	int nErr, nRet = -1;
	nErr = inflateInit(&zInfo);               // zlib function
	if (nErr == Z_OK) {
		nErr = inflate(&zInfo, Z_FINISH);     // zlib function
		if (nErr == Z_STREAM_END) {
			nRet = zInfo.total_out;
		}
	}
	inflateEnd(&zInfo);   // zlib function
	return(nRet); // -1 or len of output
}

void EMSCRIPTEN_KEEPALIVE printBinaryData() {
	for (int i = 0; i <= fileSize; i++) {
		cout << bitset<8>(fileContents[i]) << endl;
	}
}

void EMSCRIPTEN_KEEPALIVE allocateArraySpace(int size) {
	fileSize = size;
	fileContents = new char[size];
}

char* EMSCRIPTEN_KEEPALIVE getFileContentsOffset() {
	return &fileContents[0];
}

int* EMSCRIPTEN_KEEPALIVE getPixelVectorOffset() {
	return &integerPixelVector[0];
}

int EMSCRIPTEN_KEEPALIVE getPixelVectorSize() {
	return integerPixelVector.size();
}

int EMSCRIPTEN_KEEPALIVE getIntegerValue(char* fileContents, int startByte, int endByte) {
	int result = 0;
	for (int i = startByte; i <= endByte; i++) {
		int shiftAmount = (endByte - i) * 8;
		result = result | int((unsigned char)(fileContents[i]) << shiftAmount);
	}

	return result;
}

int EMSCRIPTEN_KEEPALIVE getChunkLength(char* fileContents, int startByte) {
	return getIntegerValue(fileContents, startByte, startByte + 3);
}

int EMSCRIPTEN_KEEPALIVE getImageHeight() {
	return header.getHeight();
}

int EMSCRIPTEN_KEEPALIVE getImageWidth() {
	return header.getWidth();
}

void EMSCRIPTEN_KEEPALIVE FilterReverseSubRgbPixel(Pixel previousPixel, char &red, char &green, char &blue) {
	red = red + (unsigned char)previousPixel.getRed();
	green = green + (unsigned char)previousPixel.getGreen();
	blue = blue + (unsigned char)previousPixel.getBlue();
}

void EMSCRIPTEN_KEEPALIVE FilterReverseSubRgbaPixel(Pixel previousPixel, char &red, char &green, char &blue, char &alpha) {
	FilterReverseSubRgbPixel(previousPixel, red, green, blue);
	alpha = alpha + (unsigned char) previousPixel.getAlpha();
}

void EMSCRIPTEN_KEEPALIVE FilterReverseUpRgbPixel(Pixel previousPixel, char &red, char &green, char &blue) {
	red = red + (unsigned char)previousPixel.getRed();
	green = green + (unsigned char)previousPixel.getGreen();
	blue = blue + (unsigned char)previousPixel.getBlue();
}

void EMSCRIPTEN_KEEPALIVE FilterReverseUpRgbaPixel(Pixel previousPixel, char &red, char &green, char &blue, char &alpha) {
	FilterReverseUpRgbPixel(previousPixel, red, green, blue);
	alpha = alpha + (unsigned char)previousPixel.getAlpha();
}

void EMSCRIPTEN_KEEPALIVE FilterReverseAverageRgbPixel(Pixel previousLeftPixel, Pixel previousUpPixel, char &red, char &green, char &blue) {
	red = red + floor(((unsigned char)previousLeftPixel.getRed() + (unsigned char)previousUpPixel.getRed())/2);
	green = green + floor(((unsigned char)previousLeftPixel.getGreen() + (unsigned char)previousUpPixel.getGreen())/2);
	blue = blue + floor(((unsigned char)previousLeftPixel.getBlue() + (unsigned char)previousUpPixel.getBlue())/2);
}

void EMSCRIPTEN_KEEPALIVE FilterReverseAverageRgbaPixel(Pixel previousLeftPixel, Pixel previousUpPixel, char &red, char &green, char &blue, char &alpha) {
	FilterReverseAverageRgbPixel(previousLeftPixel, previousUpPixel, red, green, blue);
	alpha = alpha + floor(((unsigned char)previousLeftPixel.getAlpha() + (unsigned char)previousUpPixel.getAlpha())/2);
}

//Do each channel separately
Pixel EMSCRIPTEN_KEEPALIVE PaethPredictor(Pixel pixelLeft, Pixel pixelUp, Pixel pixelUpLeft) {
	Pixel paethPixel;
	unsigned int paethRed;
	unsigned int paethGreen;
	unsigned int paethBlue;
	unsigned int paethAlpha;
	
	unsigned int pRed = (pixelLeft.getRed() + pixelUp.getRed() - pixelUpLeft.getRed());
	unsigned int pGreen = (pixelLeft.getGreen() + pixelUp.getGreen() - pixelUpLeft.getGreen());
	unsigned int pBlue = (pixelLeft.getBlue() + pixelUp.getBlue() - pixelUpLeft.getBlue());
	unsigned int pAlpha = (pixelLeft.getAlpha() + pixelUp.getAlpha() - pixelUpLeft.getAlpha());

	unsigned int paRed = abs(int (pRed) - (int) pixelLeft.getRed());
	unsigned int pbRed = abs(int (pRed) - (int) pixelUp.getRed());
	unsigned int pcRed = abs(int (pRed) - (int) pixelUpLeft.getRed());

	unsigned int paGreen = abs(int (pGreen) - (int) pixelLeft.getGreen());
	unsigned int pbGreen = abs(int (pGreen) - (int) pixelUp.getGreen());
	unsigned int pcGreen = abs(int (pGreen) - (int) pixelUpLeft.getGreen());

	unsigned int paBlue = abs(int (pBlue) - (int) pixelLeft.getBlue());
	unsigned int pbBlue = abs(int (pBlue) - (int) pixelUp.getBlue());
	unsigned int pcBlue = abs(int (pBlue) - (int) pixelUpLeft.getBlue());

	cout << "pBlue" << (int) pBlue << endl;
	cout << "pixelLeftBlue" << (int) pixelLeft.getBlue() << endl;
	cout << "pixelUpBlue" << (int) pixelUp.getBlue() << endl;
	cout << "pixelUpLeftBlue" << (int) pixelUpLeft.getBlue() << endl;
	cout << "paBlue" << abs(int (pBlue) - (int) pixelLeft.getBlue()) << endl;
	cout << "pbBlue" << abs(int (pBlue) - (int) pixelUp.getBlue()) << endl;
	cout << "pcBlue" << abs(int (pBlue) - (int) pixelUpLeft.getBlue()) << endl;

	unsigned int paAlpha = abs(int (pAlpha) - (int) pixelLeft.getAlpha());
	unsigned int pbAlpha = abs(int (pAlpha) - (int) pixelUp.getAlpha());
	unsigned int pcAlpha = abs(int (pAlpha) - (int) pixelUpLeft.getAlpha());

	if (paRed <= pbRed && paRed <= pcRed) {
		paethRed = pixelLeft.getRed();
	}
	else if (pbRed <= pcRed) {
		paethRed = pixelUp.getRed();
	}
	else {
		paethRed = pixelUpLeft.getRed();
	}

	if (paGreen <= pbGreen && paGreen <= pcGreen) {
		paethGreen = pixelLeft.getGreen();
	}
	else if (pbGreen <= pcGreen) {
		paethGreen = pixelUp.getGreen();
	}
	else {
		paethGreen = pixelUpLeft.getGreen();
	}

	if (paBlue <= pbBlue && paBlue <= pcBlue) {
		paethBlue = pixelLeft.getBlue();
	}
	else if (pbBlue <= pcBlue) {
		paethBlue = pixelUp.getBlue();
	}
	else {
		paethBlue = pixelUpLeft.getBlue();
	}

	cout << "paethBlue" << (int) paethBlue << endl;

	if (paAlpha <= pbAlpha && paAlpha <= pcAlpha) {
		paethAlpha = pixelLeft.getAlpha();
	}
	else if (pbAlpha <= pcAlpha) {
		paethAlpha = pixelUp.getAlpha();
	}
	else {
		paethAlpha = pixelUpLeft.getAlpha();
	}

	paethPixel.setPixelValues((unsigned char) paethRed, (unsigned char) paethGreen, (unsigned char) paethBlue, (unsigned char) paethAlpha);
	return paethPixel;
}

void EMSCRIPTEN_KEEPALIVE FilterReversePaethRgbPixel(Pixel previousLeftPixel, Pixel previousUpPixel, Pixel previousUpLeftPixel, char &red, char &green, char &blue) {
	Pixel paethPixel = PaethPredictor(previousLeftPixel, previousUpPixel, previousUpLeftPixel);
	red = red + paethPixel.getRed();
	green = green + paethPixel.getGreen();
	blue = blue + paethPixel.getBlue();
}

void EMSCRIPTEN_KEEPALIVE FilterReversePaethRgbaPixel(Pixel previousLeftPixel, Pixel previousUpPixel, Pixel previousUpLeftPixel, char &red, char &green, char &blue, char &alpha) {
	Pixel paethPixel = PaethPredictor(previousLeftPixel, previousUpPixel, previousUpLeftPixel);
	red = red + paethPixel.getRed();
	green = green + paethPixel.getGreen();
	blue = blue + paethPixel.getBlue();
	alpha = alpha + paethPixel.getAlpha();
}

int EMSCRIPTEN_KEEPALIVE readHeader(char* fileContents, HeaderInfo &header) {
	//Header information
	cout << "HEADER: " << endl;
	char fileTypeCharacter[] = { fileContents[1], fileContents[2], fileContents[3], '\0' };
	string fileTypeString(fileTypeCharacter);
	cout << fileTypeString << endl;
#ifdef DEBUG
	printBinaryData(fileContents, 0, 7);
#endif

	//Chunk 1
#ifdef DEBUG
	//Length
	cout << "LENGTH: " << endl;
	int dataLength = getChunkLength(fileContents, 8);
	cout << dataLength << endl;
	printBinaryData(fileContents, 8, 11);

	//Type
	cout << "TYPE: " << fileContents[12] << fileContents[13] << fileContents[14] << fileContents[15] << endl;
	printBinaryData(fileContents, 12, 15);
#endif

	//Data
	cout << "DATA: " << endl;

	//Width
	int startWidthByte = 16;
	int endWidthByte = startWidthByte + 3;
	cout << "WIDTH: " << endl;
	int width = getIntegerValue(fileContents, startWidthByte, endWidthByte);
	cout << width << endl;
#ifdef DEBUG
	printBinaryData(fileContents, startWidthByte, endWidthByte);
#endif

	//Height
	int startHeightByte = endWidthByte + 1;
	int endHeightByte = startHeightByte + 3;
	cout << "HEIGHT: " << endl;
	int height = getIntegerValue(fileContents, startHeightByte, endHeightByte);
	cout << height << endl;
#ifdef DEBUG
	printBinaryData(fileContents, startHeightByte, endHeightByte);
#endif

	//Bit Depth
	int bitDepthByte = endHeightByte + 1;
	cout << "BIT DEPTH: " << endl;
	int bitDepth = getIntegerValue(fileContents, bitDepthByte, bitDepthByte);
	cout << bitDepth << endl;
#ifdef DEBUG
	printBinaryData(fileContents, bitDepthByte, bitDepthByte);
#endif

	//Colour type
	int colorTypeByte = bitDepthByte + 1;
	cout << "COLOUR TYPE: " << endl;
	int colourType = getIntegerValue(fileContents, colorTypeByte, colorTypeByte);
	cout << colourType << endl;
#ifdef DEBUG
	printBinaryData(fileContents, colorTypeByte, colorTypeByte);
#endif

	//Compression Type
	int compressionTypeByte = colorTypeByte + 1;
	cout << "COMPRESSION TYPE: " << endl;
	int compressType = getIntegerValue(fileContents, compressionTypeByte, compressionTypeByte);
	cout << compressType << endl;
#ifdef DEBUG
	printBinaryData(fileContents, compressionTypeByte, compressionTypeByte);
#endif

	//Filter Method
	int filterMethodByte = compressionTypeByte + 1;
	cout << "FILTER METHOD: " << endl;
	int filterMethod = getIntegerValue(fileContents, filterMethodByte, filterMethodByte);
	cout << filterMethod << endl;
#ifdef DEBUG
	printBinaryData(fileContents, filterMethodByte, filterMethodByte);
#endif

	//Interlace Method
	int interlaceMethodByte = filterMethodByte + 1;
	cout << "INTERLACE METHOD: " << endl;
	int interlaceMethod = getIntegerValue(fileContents, interlaceMethodByte, interlaceMethodByte);
	cout << interlaceMethod << endl;
#ifdef DEBUG
	printBinaryData(fileContents, interlaceMethodByte, interlaceMethodByte);
#endif

	header.setValues(width, height, bitDepth, colourType, compressType, filterMethod, interlaceMethod);

	//CRC
	int crcStartByte = interlaceMethodByte + 1;
	int crcEndByte = crcStartByte + 3;
#ifdef DEBUG
	cout << "CRC: " << endl;
	printBinaryData(fileContents, crcStartByte, crcEndByte);
#endif

	return crcEndByte + 1;
}

void EMSCRIPTEN_KEEPALIVE ProcessImageData() {
		pixelVector.reserve(4);
		integerPixelVector.reserve(4);
		//Header
		int nextChunkStart = readHeader(fileContents, header);

		for (int i = nextChunkStart; i < fileSize; i++) {
			cout << "-----------------------------------------------------" << endl;
			int length = getChunkLength(fileContents, i);

			int lengthEnd = i + 3;

			int typeStart = lengthEnd + 1;
			int typeEnd = typeStart + 3;

			int chunkDataStart = typeEnd + 1;
			int chunkDataEnd = chunkDataStart + length - 1;

			int crcStart = chunkDataEnd + 1;
			int crcEnd = crcStart + 3;

			char typeCharacter[] = { fileContents[typeStart], fileContents[typeStart + 1], fileContents[typeStart + 2], fileContents[typeStart + 3], '\0' };
			string typeString(typeCharacter);

			cout << "TYPE: " << typeString << endl;
			if (typeString == "IDAT") {

				int compressionFlagByte = chunkDataStart;
				int additionalCheckByte = compressionFlagByte + 1;
				int dataStart = additionalCheckByte + 1;
				int dataLength = dataStart + length - 6;
				int blockLength = dataLength - dataStart;
				int dataCheck = dataStart + dataLength - 1;

#ifdef DEBUG
				//Length
				cout << "LENGTH: " << endl;
				printBinaryData(fileContents, i, lengthEnd);
				cout << length << endl;

				//Type
				cout << "TYPE: " << endl;
				printBinaryData(fileContents, typeStart, typeEnd);
#endif

				//Data
				cout << "DATA: " << endl;
				cout << "COMPRESSION FLAG: " << endl;
				int compressionMethod = getIntegerValue(fileContents, compressionFlagByte, compressionFlagByte);
				cout << compressionMethod << endl;
				cout << fileContents[compressionFlagByte] << endl;
#ifdef DEBUG
				printBinaryData(fileContents, compressionFlagByte, compressionFlagByte);
#endif

				cout << "ADDITIONAL FLAG: " << endl;
				int additionalFlag = getIntegerValue(fileContents, additionalCheckByte, additionalCheckByte);
				cout << additionalFlag << endl;
#ifdef DEBUG
				printBinaryData(fileContents, additionalCheckByte, additionalCheckByte);
#endif

				cout << "DATA BLOCK" << endl;
#ifdef DEBUG
				printBinaryData(fileContents, dataStart, dataLength);
#endif

				char* dataCompressed = &fileContents[compressionFlagByte];

#ifdef DEBUG
				cout << "UNCOMPRESSED DATA" << endl;
#endif
				char* pbUnpacked = new char[CHUNK];
				int uncompressedLength = UncompressData(dataCompressed, chunkDataEnd - chunkDataStart + 1, pbUnpacked, CHUNK);
#ifdef DEBUG
				printBinaryData(pbUnpacked, 0, uncompressedLength - 1);
#endif

				int bpp; //Bytes per pixel.
				int bps; //Bytes per scanline minus 1.

				if (header.getColourType() == 2) {
					if (header.getBitDepth() == 8) { //RGB Represented by 1 byte each.
						bpp = 3;
					}
					else if (header.getBitDepth() == 16) { //RGB Represented by 2 bytes each.
						bpp = 6;
					}
					bps = (header.getWidth() * bpp) - 1;
				}
				else if (header.getColourType() == 6) {
					if (header.getBitDepth() == 8) { //RGB Represented by 1 byte each.
						bpp = 4;
					}
					else if (header.getBitDepth() == 16) { //RGB Represented by 2 bytes each.
						bpp = 8;
					}
				}

				//first byte should be scanline filter type
				for (int i = 0; i < header.getHeight(); i++) {
					int filterType = 0;
					int filterByte = 0;
					if (i == 0) {
						filterType = getIntegerValue(pbUnpacked, 0, 0);
					}
					else {
						filterByte = i * (1 + header.getWidth() * bpp);
						filterType = getIntegerValue(pbUnpacked, filterByte, filterByte);
					}

					cout << "SCANLINE " << i << " FILTERTYPE" << endl;
					cout << filterType << endl;

					//First colour value will be filterByte + 1
					for (int j = 0; j < header.getWidth(); j++) {

						Pixel newPixel;
						if (header.getColourType() == 6) {
							char red = pbUnpacked[filterByte + (j * 4) + 1];
							char green = pbUnpacked[filterByte + (j * 4) + 2];
							char blue = pbUnpacked[filterByte + (j * 4) + 3];
							char alpha = pbUnpacked[filterByte + (j * 4) + 4];

							//Filter type is SUB
							if (filterType == 1 && j > 0) {
								FilterReverseSubRgbaPixel(pixelVector[(i * header.getWidth()) + j - 1], red, green, blue, alpha);
							}
							else if (filterType == 2 && i > 0) {
								FilterReverseUpRgbaPixel(pixelVector[((i - 1) * header.getWidth()) + j], red, green, blue, alpha);
							}
							//This needs more conditions
							else if (filterType == 3) {
								Pixel pixelLeft = pixelVector[(i * header.getWidth()) + j - 1];
								Pixel pixelUp = pixelVector[((i - 1) * header.getWidth()) + j];
								if ( j > 0 ) {
									pixelLeft.setPixelValues(0,0,0,0);
								}
								if ( i > 0 ) {
									pixelUp.setPixelValues(0,0,0,0);
								}

								FilterReverseAverageRgbaPixel(pixelLeft, pixelUp, red, green, blue, alpha);
							}
							else if (filterType == 4) {
								cout << "Filter 4" << endl;
								Pixel pixelLeft = pixelVector[(i * header.getWidth()) + j - 1];
								Pixel pixelUp = pixelVector[((i - 1) * header.getWidth()) + j];
								Pixel pixelUpLeft = pixelVector[((i - 1) * header.getWidth()) + j - 1];
								if ( j > 0 ) {
									pixelLeft.setPixelValues(0,0,0,0);
									pixelUp.setPixelValues(0,0,0,0);
								}
								if ( i > 0 ) {
									pixelUp.setPixelValues(0,0,0,0);
									pixelUpLeft.setPixelValues(0,0,0,0);
								}

								FilterReversePaethRgbaPixel(pixelLeft, pixelUp, pixelUpLeft, red, green, blue, alpha);
							}

							newPixel.setPixelValues(red, green, blue, alpha);
							cout << newPixel.getRed() << endl;
							cout << newPixel.getGreen() << endl;
							cout << newPixel.getBlue() << endl;
							cout << newPixel.getAlpha() << endl;
							pixelVector.push_back(newPixel);
							integerPixelVector.push_back(newPixel.getRGBA());

						}
						else if (header.getColourType() == 2) {

							char red = pbUnpacked[filterByte + (j * 3) + 1];
							char green = pbUnpacked[filterByte + (j * 3) + 2];
							char blue = pbUnpacked[filterByte + (j * 3) + 3];

							//Filter type is SUB
							if (filterType == 1 && j > 0) {
								FilterReverseSubRgbPixel(pixelVector[(i * header.getWidth()) + j - 1], red, green, blue);
							}
							else if (filterType == 2 && i > 0) {
								FilterReverseUpRgbPixel(pixelVector[((i - 1) * header.getWidth()) + j], red, green, blue);
							}
							//This needs more conditions
							else if (filterType == 3) {
								Pixel pixelLeft = pixelVector[(i * header.getWidth()) + j - 1];
								Pixel pixelUp = pixelVector[((i - 1) * header.getWidth()) + j];
								if ( j == 0 ) {
									pixelLeft.setPixelValues(0,0,0,0);
								}
								if ( i == 0 ) {
									pixelUp.setPixelValues(0,0,0,0);
								}
								FilterReverseAverageRgbPixel(pixelLeft, pixelUp, red, green, blue);
							}
							else if (filterType == 4) {
								Pixel pixelLeft = pixelVector[(i * header.getWidth()) + j - 1];
								Pixel pixelUp = pixelVector[((i - 1) * header.getWidth()) + j];
								Pixel pixelUpLeft = pixelVector[((i - 1) * header.getWidth()) + j - 1];
								if ( j == 0 ) {
									pixelLeft.setPixelValues(0,0,0,0);
									pixelUp.setPixelValues(0,0,0,0);
								}
								if ( i == 0 ) {
									pixelUp.setPixelValues(0,0,0,0);
									pixelUpLeft.setPixelValues(0,0,0,0);
								}
								FilterReversePaethRgbPixel(pixelLeft, pixelUp, pixelUpLeft, red, green, blue);
							}

							newPixel.setPixelValues(red, green, blue, (char)0);
							cout << newPixel.getRed() << endl;
							cout << newPixel.getGreen() << endl;
							cout << newPixel.getBlue() << endl;

							pixelVector.push_back(newPixel);
							integerPixelVector.push_back(newPixel.getRGBA());
						}
					}
				}
#ifdef DEBUG
				cout << "DECOMPRESSED PIXEL VALUES" << endl;
				for (int j = 0; j < pixelVector.size(); j++) {
					cout << "PIXEL " << j << endl;
					cout << "R: " << pixelVector[j].getRed() << endl;
					cout << "G: " << pixelVector[j].getGreen() << endl;
					cout << "B: " << pixelVector[j].getBlue() << endl;
					cout << "A: " << pixelVector[j].getAlpha() << endl;
				}

				//CRC
				cout << "CRC: " << endl;
				printBinaryData(fileContents, crcStart, crcEnd);
#endif			
				delete[] pbUnpacked;
			}
			i = crcEnd;
		}

		delete[] fileContents;
	}