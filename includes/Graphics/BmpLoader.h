#ifndef IMP_BMPLOADER_H
#define IMP_BMPLOADER_H

#include <Core/impBase.h>
#include <Graphics/ImageData.h>
#include "Graphics/Texture.h"

#include <iostream>
#include <fstream>

static const char* BM = "BM"; 

struct BitmapFileHeader_Struct
{
	short magicNumber;         // magic number : BM = 0x42 0x4D (others are possibles...)
	int fileSize;              // size of the bmp file
	short editorReserved1;     // ID editor
	short editorReserved2;     // ID editor
	unsigned int dataOffset;// offset of data
};

// BMP HEADER SIZE
static const unsigned int DIB_BITMAPCOREHEADER = 12;
static const unsigned int DIB_OS21XBITMAPHEADER = DIB_BITMAPCOREHEADER;
static const unsigned int DIB_OS22XBITMAPHEADER = 64;
static const unsigned int DIB_BITMAPINFOHEADER = 40;
static const unsigned int DIB_BITMAPV2INFOHEADER = 52;
static const unsigned int DIB_BITMAPV3INFOHEADER = 56;
static const unsigned int DIB_BITMAPV4HEADER = 108;
static const unsigned int DIB_BITMAPV5HEADER = 124;

struct BitmapCoreHeader_Struct
{
	unsigned int dib;	//offset 0x0E
	unsigned short width;	// offset 0x12
	unsigned short height;	// offset 0x14
	unsigned short numberOfColorPlanes;	// (1) offset 0x16
	unsigned short bpp;	// 0x18
};

static const unsigned int BI_RGB = 0;	// none
/*static const unsigned int BI_RLE8;
static const unsigned int BI_RLE4;
static const unsigned int BI_BITFIELDS;
static const unsigned int BI_JPEG;
static const unsigned int BI_PNG;
static const unsigned int BI_ALPHABITFIELDS;
static const unsigned int BI_CMYK;
static const unsigned int BI_CMYKRLE8;
static const unsigned int BI_CMYKRLE4;*/

struct BitmapInfoHeader_Struct
{
	unsigned int dib;	// offset 0x0E
	signed int width;
	signed int height;
	unsigned short numberOfColorPlanes; // (1)
	unsigned short bpp;
	int compressionMethod; // (0 for none)
	int rawDataSize; // (0 for BI_RGB)
	int horizontalResolution;	// pixel per meter
	int verticalResolution;	// pixel per meter
	int paletteColorNumber; // (0 by default)
	int importantColorUsedNumber; // (0 by default)
};

union DibHeader_Union
{
    BitmapCoreHeader_Struct bitmapCoreHeader;
    BitmapInfoHeader_Struct bitmapInfoHeader;
};

// template for file reading
template<class T>
T read(std::ifstream & stream)
{
    T value;
    stream.read( (char*)&value, sizeof(T) );
    return value;
}
template<class T>
void write(std::ofstream & stream, T value)
{
    stream.write( (char*)&value, sizeof(T) );
}
void seek(std::ifstream& stream, long offset)
{
	stream.seekg(offset);
}
long pos(std::ofstream& stream)
{
	return stream.tellp();
}

class IMP_API Bitmap
{
public:
    Bitmap();

    ~Bitmap();

    void create(int width, int height, const imp::Pixel& initialColor);

    void saveToFile(std::string filename);

    void loadFromFile(std::string filename);

    void printInfoLog();

	void dumpData();

    void getPixelColor(int x, int y, imp::Pixel& color);

    void setPixelColor(int x, int y, imp::Pixel& color);

    void resize(int w, int h, imp::Pixel& color, int xrel=0, int yrel=0);

    void clone(Bitmap& other);

    void copyFrom(Bitmap& src, int xsrc, int ysrc, int xdest, int ydest, int w, int h);

    int getWidth(){ return dibHeader.bitmapInfoHeader.width; }
    int getHeight(){ return dibHeader.bitmapInfoHeader.height; }
    int getBpp(){ return dibHeader.bitmapInfoHeader.bpp; }

    imp::ImageData* getInternalImageData(){ return _internalImageData; }

private:
    BitmapFileHeader_Struct fileHeader;
    DibHeader_Union dibHeader;
    int dib;

    imp::ImageData* _internalImageData;
};

class IMP_API BmpLoader
{
public:
	static imp::Texture* loadFromFile(const char* filename);
	static void loadFromFile(const char* filename, imp::ImageData**);
	static void saveToFile(const imp::ImageData* image, const char* filename);
};

#endif // IMP_BMPLOADER_H
