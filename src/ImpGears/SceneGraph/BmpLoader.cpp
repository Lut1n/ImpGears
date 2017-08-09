#include <Graphics/BmpLoader.h>

#include <cstdlib>
#include <cstring>

void seek(std::ifstream& stream, long offset)
{
	stream.seekg(offset);
}
long pos(std::ofstream& stream)
{
	return stream.tellp();
}

//--------------------------------------------------------------
Bitmap::Bitmap()
    : dib(0)
{
    _internalImageData = new imp::ImageData();
}

//--------------------------------------------------------------
Bitmap::~Bitmap()
{
    delete _internalImageData;
}

//--------------------------------------------------------------
void Bitmap::create(int width, int height, const imp::Pixel& initialColor)
{
    dib = DIB_BITMAPINFOHEADER;

    BitmapInfoHeader_Struct& header = dibHeader.bitmapInfoHeader;

    header.dib = dib;
    header.width = width;
    header.height = height;
    header.numberOfColorPlanes = 1;
    header.bpp = 24;
    header.compressionMethod = 0;
    header.rawDataSize = 0;
    header.horizontalResolution = 0;
    header.verticalResolution = 0;
    header.paletteColorNumber = 0;
    header.importantColorUsedNumber = 0;

    // int rawSize = (int)( (header.bpp * header.width + 31)/32 ) * 4;
    // int bufferSize = rawSize * header.height;

    _internalImageData->create(width, height, imp::PixelFormat_BGR8);
    _internalImageData->fill( initialColor );
}

//--------------------------------------------------------------
void Bitmap::saveToFile(std::string filename)
{
    int rawSize = (int)( (dibHeader.bitmapInfoHeader.bpp * dibHeader.bitmapInfoHeader.width + 31)/32 ) * 4;
    int bufferSize = rawSize * dibHeader.bitmapInfoHeader.height;

    fileHeader.magicNumber = 0x424d;
    fileHeader.dataOffset = 54;
    fileHeader.fileSize = fileHeader.dataOffset + bufferSize;

    std::ofstream os(filename.c_str(), std::ifstream::binary);
    if(os)
    {
        // bmp header
        //write<short>(os, fileHeader.magicNumber);
        write<char>(os, 0x42);
        write<char>(os, 0x4d);

        write<int>(os, fileHeader.fileSize);
        write<short>(os, 0);
        write<short>(os, 0);
        write<unsigned int>(os, fileHeader.dataOffset);

        // dib header
        BitmapInfoHeader_Struct& header = dibHeader.bitmapInfoHeader;
        write<unsigned int>(os, header.dib);
        write<int>(os, header.width);
        write<int>(os, header.height);
        write<short>(os, header.numberOfColorPlanes);
        write<short>(os, header.bpp);
        write<int>(os, header.compressionMethod);
        write<int>(os, header.rawDataSize);
        write<int>(os, header.horizontalResolution);
        write<int>(os, header.verticalResolution);
        write<int>(os, header.paletteColorNumber);
        write<int>(os, header.importantColorUsedNumber);

        char buffer[rawSize];
		int sizeToWrite = _internalImageData->getDataRawSize();
        for(int i=0; i<header.height; ++i)
        {
			unsigned char* dataPtr = &(_internalImageData->getData()[i*sizeToWrite]);
			memset(buffer, 0, rawSize);
            memcpy(buffer, dataPtr, sizeToWrite);
            os.write((char*)buffer, rawSize); 
        }
        os.flush();
        os.close();

		printInfoLog();
		// dumpData();
    }
}

//--------------------------------------------------------------
void Bitmap::loadFromFile(std::string filename)
{
    std::ifstream is(filename.c_str(), std::ifstream::binary);
    if (is) {

        // check bmp file
        if(read<char>(is) == 'B' && read<char>(is) == 'M')
        {
            std::cout << "bmp magic number OK" << std::endl;
        }
        else
        {
            std::cerr << "bmp magic number FAILED" << std::endl;
            exit(1);
        }

        // read file header
        fileHeader.fileSize = read<int>(is);

        read<short>(is); // reserved 1
        read<short>(is); // reserved 2

        fileHeader.dataOffset = read<unsigned int>(is);

        dib = read<int>(is);
        if(dib == DIB_BITMAPINFOHEADER
			|| dib == DIB_BITMAPV4HEADER
			|| dib == DIB_BITMAPV5HEADER)
        {
            BitmapInfoHeader_Struct& header = dibHeader.bitmapInfoHeader;
            header.dib = dib;
            header.width = read<int>(is);
            header.height = read<int>(is);
            header.numberOfColorPlanes = read<short>(is);
            header.bpp = read<short>(is);
            header.compressionMethod = read<int>(is);
            header.rawDataSize = read<int>(is);
            header.horizontalResolution = read<int>(is);
            header.verticalResolution = read<int>(is);
            header.paletteColorNumber = read<int>(is);
            header.importantColorUsedNumber = read<int>(is);

            unsigned int rawSize = (unsigned int)( (header.bpp * header.width + 31)/32 ) * 4; // take floor of [(bpp*width+31)/32]
            // unsigned int bufferSize = rawSize * header.height; // should take absolute value of height

			imp::PixelFormat format = imp::PixelFormat_Unknown;
			if(header.bpp == 24)
				format = imp::PixelFormat_BGR8;
			else if(header.bpp == 32)
				format = imp::PixelFormat_BGRA8;
			
			// go to pixel data
			seek(is, fileHeader.dataOffset);
			
            _internalImageData->create(header.width, header.height, header.bpp, format);
            unsigned char buffer[rawSize];
            for(int i=0; i<header.height; ++i)
            {
                is.read((char*)buffer, rawSize);
                memcpy(&(_internalImageData->getData()[i*_internalImageData->getDataRawSize()]), buffer, _internalImageData->getDataRawSize());
            }
        }
		else
		{
			std::cout << "DIB not supported : " << dib << "\n";
		}

        is.close();
    }
    else
    {
        std::cout << "Could not open " << filename << std::endl;
    }
}

//--------------------------------------------------------------
void Bitmap::printInfoLog()
{
    std::cout << "file size = " << (fileHeader.fileSize) << std::endl;
	std::cout << "dataOffset = " << fileHeader.dataOffset << std::endl;

    if(dib == DIB_BITMAPCOREHEADER)
        std::cout << "dib = " << "bitmapCoreHeader" << std::endl;
    else if(dib == DIB_BITMAPINFOHEADER)
        std::cout << "dib = " << "bitmapInfoHeader" << std::endl;
    else if(dib == DIB_BITMAPV4HEADER)
        std::cout << "dib = " << "bitmap V4 header" << std::endl;
    else if(dib == DIB_BITMAPV5HEADER)
        std::cout << "dib = " << "bitmap V5 header" << std::endl;
    else
        std::cout << "dib = " << "unsupported" << std::endl;

    if(dib == DIB_BITMAPINFOHEADER
		|| dib == DIB_BITMAPV4HEADER
		|| dib == DIB_BITMAPV5HEADER)
    {
        BitmapInfoHeader_Struct& header = dibHeader.bitmapInfoHeader;

        std::cout << "width = " << header.width << std::endl;
        std::cout << "height = " << header.height << std::endl;
        std::cout << "numberOfColorPlanes = " << header.numberOfColorPlanes << std::endl;
        std::cout << "bpp = " << header.bpp << std::endl;
        std::cout << "compressionMethod = " << header.compressionMethod << std::endl;
        std::cout << "rawDataSize = " << header.rawDataSize << std::endl;
        std::cout << "horizontalResolution = " << header.horizontalResolution << std::endl;
        std::cout << "verticalResolution = " << header.verticalResolution << std::endl;
        std::cout << "paletteColorNumber = " << header.paletteColorNumber << std::endl;
        std::cout << "importantColorUsedNumber = " << header.importantColorUsedNumber << std::endl;

        // int rawSize = (int)( (header.bpp * header.width + 31)/32 ) * 4; // take floor of [(bpp*width+31)/32]
    }
}

//--------------------------------------------------------------
void Bitmap::dumpData()
{
	BitmapInfoHeader_Struct& header = dibHeader.bitmapInfoHeader;
	for(int j=0; j<header.height; ++j)
	{
		for(int i=0; i<header.width; i+=3)
		{
			unsigned char* ptr = &(_internalImageData->getData()[j*_internalImageData->getDataRawSize()+i]);
			std::cout << "{" << (unsigned int)ptr[0] << ";" << (unsigned int)ptr[1] << ";" << (unsigned int)ptr[2] << "} ";
		}
		std::cout << "\n";
	}
}

//--------------------------------------------------------------
void Bitmap::getPixelColor(int x, int y, imp::Pixel& color)
{
	color = _internalImageData->getPixel(x,y);
}

//--------------------------------------------------------------
void Bitmap::setPixelColor(int x, int y, imp::Pixel& color)
{
    _internalImageData->setPixel(x,y,color);
}

//--------------------------------------------------------------
void Bitmap::resize(int w, int h, imp::Pixel& color, int xrel, int yrel)
{
    
}

//--------------------------------------------------------------
void Bitmap::clone(Bitmap& other)
{
    fileHeader = other.fileHeader;
    dibHeader = other.dibHeader;
    dib = other.dib;

    imp::ImageData& data = *(other._internalImageData);
    _internalImageData->clone(data);
}

//--------------------------------------------------------------
void Bitmap::copyFrom(Bitmap& src, int xsrc, int ysrc, int xdest, int ydest, int w, int h)
{
    imp::Rect rs, rd;
    rs.x = xsrc;
    rs.y = ysrc;
    rs.w = w;
    rs.h = h;

    rd.x = xdest;
    rd.y = ydest;
    rd.w = w;
    rd.h = h;

    imp::ImageData& data = *(src._internalImageData);
    _internalImageData->draw(data, rs, rd);
}

//--------------------------------------------------------------
imp::Texture* BmpLoader::loadFromFile(const char* filename)
{
	Bitmap bmp;
	bmp.loadFromFile( filename );
	bmp.printInfoLog();
	imp::ImageData* data = bmp.getInternalImageData();
	
	imp::Texture* tex = new imp::Texture( filename );
	
	if(data == IMP_NULL)
		std::cout << "no image data\n";
	else
	{
		tex->loadFromImageData( data );
	}
	return tex;
}
void BmpLoader::loadFromFile(const char* filename, imp::ImageData** image)
{
	Bitmap bmp;
	bmp.loadFromFile( filename );
	bmp.printInfoLog();
	imp::ImageData* data = bmp.getInternalImageData();
	
	(*image) = NULL;

	if(data == IMP_NULL)
		std::cout << "no image data\n";
	else
	{
		(*image) = new imp::ImageData();
		(*image)->clone(*data);
	}
}

void BmpLoader::saveToFile(const imp::ImageData* image, const char* filename)
{
	imp::Pixel initColor = {255, 255, 255, 255};

	Bitmap bitmap;
	bitmap.create(image->getWidth(), image->getHeight(), initColor);
	bitmap.getInternalImageData()->clone(*image);
	bitmap.saveToFile(filename);
}
