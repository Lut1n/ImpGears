#include <ImpGears/Descriptors/ImageIO.h>

#include <fstream>




// static const char* BM = "BM"; 

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




IMPGEARS_BEGIN

//--------------------------------------------------------------
void bmp_save(const std::string& filename, const Image::Ptr img);
void tga_save(const std::string& filename, const Image::Ptr img);
Image::Ptr bmp_load(std::string& filename);
Image::Ptr tga_load(std::string& filename, bool flipY);

//--------------------------------------------------------------
ImageIO::ImageIO()
{
}

//--------------------------------------------------------------
ImageIO::~ImageIO()
{
}

//--------------------------------------------------------------
void ImageIO::save(const Image::Ptr image, std::string filename)
{
    save(image,filename,Options());
}

//--------------------------------------------------------------
Image::Ptr ImageIO::load(std::string filename)
{
    return load(filename,Options());
}

//--------------------------------------------------------------
void ImageIO::save(const Image::Ptr image, std::string filename, const Options& opts)
{
    if(opts.fileType == FileType_BMP)
    {
        // todo
        // bmp_save(filename, image);
    }
    else if(opts.fileType == FileType_TGA)
    {
        tga_save(filename, image);
    }
}

//--------------------------------------------------------------
Image::Ptr ImageIO::load(std::string filename, const Options& opts)
{
    Image::Ptr res;
    if(opts.fileType == FileType_BMP)
    {
        res = bmp_load(filename);
    }
    else if(opts.fileType == FileType_TGA)
    {
        res = tga_load(filename, opts._flipY);
    }
    return res;
}


void decode_packet_header(unsigned char packet_header, bool& rl_type, int& len)
{
    unsigned char type_filter = 0b10000000;
    unsigned char size_filter = 0b01111111;

    rl_type = (packet_header & type_filter) == type_filter;
    len = 1 + int(packet_header & size_filter);
}


void readU8(std::ifstream& is, unsigned char* buf, int size)
{
    is.read( reinterpret_cast<char*>(buf), size*sizeof(unsigned char) );
}


void readU8(std::ifstream& is, std::vector<unsigned char>& buf, int size)
{
    buf.resize(size);
    readU8(is,buf.data(),size);
}


void readU8_rle(std::ifstream& is, unsigned char* dst, int size, int chnl)
{
    bool to_repeat; int run_len;
    unsigned char packet_header;
    std::vector<unsigned char> buf;

    int progress = 0;
    while(progress < size)
    {
        is.read(reinterpret_cast<char*>(&packet_header),sizeof(unsigned char));
        decode_packet_header(packet_header, to_repeat, run_len);

        if(to_repeat)
        {
            readU8(is, buf, chnl);
            for(int l=0;l<run_len;++l)
            {
                for(int c=0;c<chnl;++c) dst[progress+c]= buf[c];
                progress += chnl;
            }
        }
        else
        {
            readU8(is, &dst[progress], run_len*chnl);
            progress += run_len*chnl;
        }
    }
}

Image::Ptr tga_load(std::string& filename, bool flipY)
{
    unsigned short w, h;
    unsigned char pixel_depth;

    std::ifstream istrm(filename,std::ios::binary);
    unsigned char id_len = 0;
    unsigned char colormap;
    unsigned char image_type = 0;
    istrm.read(reinterpret_cast<char*>(&id_len), sizeof id_len); //ID length (1o)
    istrm.read(reinterpret_cast<char*>(&colormap), sizeof colormap); // Color map type (1o)
    istrm.read(reinterpret_cast<char*>(&image_type), sizeof image_type); // Image type (1o)

    static const int NO_IMAGE = 0;
    static const int INDEXED = 1;
    // static const int TRUE_COLOR = 2;
    // static const int GRAYSCALE = 3;
    static const int INDEXED_RLE = 9;
    static const int TRUE_COLOR_RLE = 10;
    static const int GRAYSCALE_RLE = 11;
    if(image_type == INDEXED || image_type == INDEXED_RLE)
        std::cout << "TGA loader : warning - indexed color (not supported)" << std::endl;
    if(image_type == NO_IMAGE)
        std::cout << "TGA loader : warning - no image" << std::endl;

    bool rle = image_type==INDEXED_RLE || image_type==TRUE_COLOR_RLE || image_type==GRAYSCALE_RLE;

    // if(image_type == 3) chnl = 1; // black and white
    // if(image_type == 2) chnl = 3; // TrueColor

    std::vector<unsigned char> ignored;
    readU8(istrm, ignored, 5); // colormap info (ignored)

    unsigned short x_minus_o = 0;
    unsigned short y_minus_o = 0;
    istrm.read(reinterpret_cast<char*>(&x_minus_o), sizeof x_minus_o); // X-origin (2o)
    istrm.read(reinterpret_cast<char*>(&y_minus_o), sizeof y_minus_o); // Y-origin (2o)

    istrm.read(reinterpret_cast<char*>(&w), sizeof w); // width (2o)
    istrm.read(reinterpret_cast<char*>(&h), sizeof h); // height (2o)

    istrm.read(reinterpret_cast<char*>(&pixel_depth), sizeof pixel_depth); // px depth (1o)
    int chnl = pixel_depth / 8;	// 24 for RGB; 32 for RGBA


    // Image description
    // bits 0,1,2,3 : alpha channel depth (ignored)
    // bits 4,5 : flip X and Y
    // bits 6,7 : unused (ignored)
    char id_info;
    istrm.read(&id_info, sizeof(char));
    char flip_y_bit = 0b00100000;
    bool need_flipY = (id_info & flip_y_bit) == flip_y_bit;
    if(need_flipY) std::cout << "need flip Y" << std::endl;
    flipY ^= need_flipY;

    Image::Ptr result = Image::create(w,h,chnl);
    Image::Ptr tmp_img = Image::create(w,h,chnl);
    int totalSize = result->width() * result->height() * result->channels();

    if(rle) // compression run-length encoding
    {
        readU8_rle(istrm, tmp_img->data(), totalSize, chnl);

    }
    else // no compression - just read
    {
        readU8(istrm, tmp_img->data(), totalSize);
    }

    bool effective_flip_y = !flipY; // imp::Image seems to be flipped on screen (has to be confirmed)
    if(chnl == 1) result->copy(tmp_img, {0,1,2,3}, effective_flip_y);
    else if(chnl == 2) result->copy(tmp_img, {1,0}, effective_flip_y);
    else if(chnl == 3) result->copy(tmp_img, {2,1,0}, effective_flip_y);
    else if(chnl == 4) result->copy(tmp_img, {2,1,0,3}, effective_flip_y);

    return result;
}

//--------------------------------------------------------------
void tga_save(const std::string& filename, const Image::Ptr img)
{
    std::ofstream ostrm(filename,std::ios::binary);
    unsigned char chnl = img->channels();
    unsigned char v = 0;
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); //ID length (1o)
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Color map type (1o)
    if(chnl == 1) v = 3; // black and white
    if(chnl == 3 || chnl == 4) v = 2;	// TrueColor
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Image type (1o)
    v = 0;	// no color map
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Colormap spec (5o)
    unsigned short d = 0;
    ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // X-origin (2o)
    ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // Y-origin (2o)
    d = img->width();
    ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // width (2o)
    d = img->height();
    ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // height (2o)
    // v = 8;	// default for black and white
    v = chnl * 8;	// 24 for RGB; 32 for RGBA
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // px depth (1o)
    v = 0;
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // description (1o)
    
    int totalSize = img->width() * img->height() * chnl;

    Image::Ptr tosave = Image::create(img->width(),img->height(),chnl);
    if(chnl == 1) tosave->copy(img);
    else if(chnl == 2) tosave->copy(img, {1,0});
    else if(chnl == 3) tosave->copy(img, {2,1,0});
    else if(chnl == 4) tosave->copy(img, {2,1,0,3});
    ostrm.write(reinterpret_cast<char*>(tosave->data()),totalSize); // image data
}

//--------------------------------------------------------------
void bmp_save(const std::string& filename, const Image::Ptr img)
{
    BitmapFileHeader_Struct fileHeader;
    DibHeader_Union dibHeader;
    int dib;

    dib = DIB_BITMAPINFOHEADER;

    BitmapInfoHeader_Struct& header = dibHeader.bitmapInfoHeader;

    header.dib = dib;
    header.width = img->width();
    header.height = img->height();
    header.numberOfColorPlanes = 1;
    header.bpp = img->channels()*8;
    header.compressionMethod = 0;
    header.rawDataSize = 0;
    header.horizontalResolution = 0;
    header.verticalResolution = 0;
    header.paletteColorNumber = 0;
    header.importantColorUsedNumber = 0;

    const int rawSize = (int)( (dibHeader.bitmapInfoHeader.bpp * dibHeader.bitmapInfoHeader.width + 31)/32 ) * 4;
    int bufferSize = rawSize * dibHeader.bitmapInfoHeader.height;

    fileHeader.magicNumber = 0x424d;
    fileHeader.dataOffset = 54;
    fileHeader.fileSize = fileHeader.dataOffset + bufferSize;

    std::ofstream os(filename.c_str(), std::ifstream::binary);
    if(os)
    {
        // bmp header
        //magicNumber
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
            unsigned char* dataPtr = &(img->asGrid()->data()[i*rawSize]);
            memset(buffer, 0, rawSize);
            memcpy(buffer, dataPtr, rawSize);
            os.write((char*)buffer, rawSize);
        }
        os.flush();
        os.close();
    }
}

//--------------------------------------------------------------
Image::Ptr bmp_load(std::string& filename)
{
    BitmapFileHeader_Struct fileHeader;
    DibHeader_Union dibHeader;
    int dib;

    std::ifstream is(filename.c_str(), std::ifstream::binary);
    if (!is)
    {
        std::cout << "bmp loading failed \"" << filename << "\" : Could not open " << filename << std::endl;
        return nullptr;
    }

    // check bmp file
    if(read<char>(is) != 'B' || read<char>(is) != 'M')
    {
        std::cerr << "bmp loading failed \"" << filename << "\" : bmp magic number FAILED" << std::endl;
        return nullptr;
    }

    // read file header
    fileHeader.fileSize = read<int>(is);
    read<short>(is); // reserved 1
    read<short>(is); // reserved 2
    fileHeader.dataOffset = read<unsigned int>(is);

    dib = read<int>(is);
    if(dib != DIB_BITMAPINFOHEADER && dib != DIB_BITMAPV4HEADER && dib != DIB_BITMAPV5HEADER)
    {
        std::cerr << "bmp loading failed \"" << filename << "\" : DIB not supported : " << dib << std::endl;
        return nullptr;
    }

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

    const unsigned int rawSize = (unsigned int)( (header.bpp * header.width + 31)/32 ) * 4; // take floor of [(bpp*width+31)/32]
    // unsigned int bufferSize = rawSize * header.height; // should take absolute value of height

    int chnls = 3;
    if(header.bpp == 24)
        chnls = 3;
    else if(header.bpp == 32)
        chnls = 4;

    // go to pixel data
    seek(is, fileHeader.dataOffset);

    Image::Ptr loaded = Image::create(header.width, header.height, chnls);
    unsigned char buffer[rawSize];
    for(int i=0; i<header.height; ++i)
    {
        is.read((char*)buffer, rawSize);
        memcpy(&(loaded->asGrid()->data()[i*rawSize]), buffer, rawSize);
    }
    return loaded;
}

IMPGEARS_END
