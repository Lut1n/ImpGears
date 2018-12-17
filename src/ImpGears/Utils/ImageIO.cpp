#include <Utils/ImageIO.h>

#include <fstream>


IMPGEARS_BEGIN

//--------------------------------------------------------------
void tga_save(const std::string& filename, const Image& img);


//--------------------------------------------------------------
ImageIO::ImageIO()
{
}

//--------------------------------------------------------------
ImageIO::~ImageIO()
{
}

//--------------------------------------------------------------
void ImageIO::save(const Image& image, std::string filename)
{
    save(image,filename,Options());
}

//--------------------------------------------------------------
void ImageIO::load(Image& image, std::string filename)
{
    load(image,filename,Options());
}

//--------------------------------------------------------------
void ImageIO::save(const Image& image, std::string filename, const Options& opts)
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
void ImageIO::load(Image& image, std::string filename, const Options& opts)
{
    if(opts.fileType == FileType_BMP)
    {
        //todo
        // bmp_load(filename, image);
    }
    else if(opts.fileType == FileType_TGA)
    {
        // todo
        // tga_load(filename, image);
    }
}

//--------------------------------------------------------------
void tga_save(const std::string& filename, const Image& img)
{
    std::ofstream ostrm(filename,std::ios::binary);
    unsigned char v = 0;
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); //ID length (1o)
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // Color map type (1o)
    if(img.channels() == 1) v = 3; // black and white
    if(img.channels() == 3 || img.channels() == 4) v = 2;	// TrueColor
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
    d = img.width();
    ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // width (2o)
    d = img.height();
    ostrm.write(reinterpret_cast<char*>(&d), sizeof d); // height (2o)
    // v = 8;	// default for black and white
    v = img.channels() * 8;	// 24 for RGB; 32 for RGBA
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // px depth (1o)
    v = 0;
    ostrm.write(reinterpret_cast<char*>(&v), sizeof v); // description (1o)
    ostrm.write(reinterpret_cast<char*>(img.asGrid()->data()), (img.width()*img.height()*img.channels())); // image data
}

IMPGEARS_END
