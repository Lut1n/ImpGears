#ifndef IMP_IMAGE_IO_H
#define IMP_IMAGE_IO_H

#include <Core/Object.h>
#include <Graphics/Image.h>

IMPGEARS_BEGIN

class IMP_API ImageIO : public Object
{
public:
    
    Meta_Class(ImageIO);
    
    enum FileType
    {
        FileType_TGA = 0,
        FileType_BMP
    };
    
    struct Options
    {
        FileType fileType;
        Options() : fileType(FileType_TGA) {}
    };
    
    ImageIO();
    ~ImageIO();
    
    static void save(const Image& image, std::string filename);
    static void save(const Image& image, std::string filename, const Options& opts);

    static void load(Image& image, std::string filename);
    static void load(Image& image, std::string filename, const Options& opts);
};

IMPGEARS_END

#endif // IMP_IMAGE_IO_H
