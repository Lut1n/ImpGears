#ifndef IMP_CUBEMAP_H
#define IMP_CUBEMAP_H

#include <Core/Object.h>
#include <Graphics/Image.h>

#include <string>

IMPGEARS_BEGIN

class IMP_API CubeMap : public Object
{
public:

    Meta_Class(CubeMap)

    CubeMap(const std::string& name = "");

    virtual ~CubeMap();

    void loadFromMemory(std::uint8_t* buf, std::uint32_t width, std::uint32_t height, int chnls, int faceID);

    void saveToMemory(std::uint8_t* buf, std::uint32_t width, std::uint32_t height, int chnls, int faceID);

    void loadFromImage(const std::vector<Image::Ptr>& images);

    void saveToImage(std::vector<Image::Ptr>& images);

    void bind() const;
    void unbind() const;

    std::uint32_t getVideoID() const{return _videoID;}

    void update();

    static std::uint32_t s_count() { return _s_count; }

protected:
private:

    std::uint32_t _videoID;
    static std::uint32_t _s_count;
    std::vector<Image::Ptr> _sources;

    std::string _name;
};


IMPGEARS_END

#endif // IMP_CUBEMAP_H
