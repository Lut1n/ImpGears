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
        bool _flipY;
        Options() : fileType(FileType_TGA), _flipY(false) {}
    };
    
    ImageIO();
    ~ImageIO();
    
    static void save(const Image::Ptr image, std::string filename);
    static void save(const Image::Ptr image, std::string filename, const Options& opts);

    static Image::Ptr load(std::string filename);
    static Image::Ptr load(std::string filename, const Options& opts);
};

IMPGEARS_END

#endif // IMP_IMAGE_IO_H
