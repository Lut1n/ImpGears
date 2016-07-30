#include <Graphics/BmpLoader.h>

#include <cstdlib>
#include <cstring>

Bitmap::Bitmap()
    : dib(0)
{
    _internalImageData = new imp::ImageData();
}

Bitmap::~Bitmap()
{
    delete _internalImageData;
}

void Bitmap::create(int width, int height, ColorRGB initialColor)
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

    _internalImageData->create(width, height, 24, imp::PixelFormat_BGR8);
	imp::Pixel pix;
	pix.red = initialColor.r;
	pix.green = initialColor.g;
	pix.blue = initialColor.b;
	pix.alpha = 255;
    _internalImageData->fill( pix );
}

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
        for(int i=0; i<header.height; ++i)
        {
            memcpy(buffer, &(_internalImageData->getData()[i*_internalImageData->getDataRawSize()]), _internalImageData->getDataRawSize());
            memset(&(buffer[_internalImageData->getDataRawSize()]), 0, rawSize-_internalImageData->getDataRawSize() );
            os.write((char*)buffer, rawSize); 
        }
        os.flush();
        os.close();
    }
}

void Bitmap::loadFromFile(std::string filename)
{
    std::ifstream is(filename.c_str(), std::ifstream::binary);
    if (is) {

        // check bmp file
        if(read<char>(is) == BM[0] && read<char>(is) == BM[1])
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
        if(dib == DIB_BITMAPINFOHEADER)
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

            _internalImageData->create(header.width, header.height, header.bpp, imp::PixelFormat_RGBA8);
            unsigned char buffer[rawSize];
            for(int i=0; i<header.height; ++i)
            {
                is.read((char*)buffer, rawSize);
                memcpy(&(_internalImageData->getData()[i*_internalImageData->getDataRawSize()]), buffer, _internalImageData->getDataRawSize());
            }
        }

        is.close();
    }
    else
    {
        std::cout << "Could not open " << filename << std::endl;
    }
}

void Bitmap::printInfoLog()
{
    std::cout << "file size = " << (fileHeader.fileSize) << std::endl;

    if(dib == DIB_BITMAPCOREHEADER)
        std::cout << "dib = " << "bitmapCoreHeader" << std::endl;
    else if(dib == DIB_BITMAPINFOHEADER)
        std::cout << "dib = " << "bitmapInfoHeader" << std::endl;
    else
        std::cout << "dib = " << "unsupported" << std::endl;

    if(dib == DIB_BITMAPINFOHEADER)
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

void Bitmap::getPixelColor(int x, int y, ColorRGB& color)
{
	imp::Pixel pix = _internalImageData->getPixel(x,y);
	color.r = pix.red;
	color.g = pix.green;
	color.b = pix.blue;
}

void Bitmap::setPixelColor(int x, int y, ColorRGB& color)
{
	imp::Pixel pix;
	pix.red = color.r;
	pix.green = color.g;
	pix.blue = color.b;
	pix.alpha = 255;
    _internalImageData->setPixel(x,y,pix);
}

void Bitmap::resize(int w, int h, ColorRGB& color, int xrel, int yrel)
{
    
}

void Bitmap::clone(Bitmap& other)
{
    fileHeader = other.fileHeader;
    dibHeader = other.dibHeader;
    dib = other.dib;

    imp::ImageData& data = *(other._internalImageData);
    _internalImageData->clone(data);
}

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





imp::Texture* BmpLoader::loadFromFile(const char* filename)
{
	Bitmap bmp;
	bmp.loadFromFile( filename );
	bmp.printInfoLog();
	std::cout << "test : format = " << bmp.getInternalImageData()->getFormat() << "\n";
	
	imp::Texture* tex = new imp::Texture();
	tex->loadFromImageData( bmp.getInternalImageData() );
	return tex;
}

