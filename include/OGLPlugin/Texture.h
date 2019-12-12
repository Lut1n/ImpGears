#ifndef IMP_TEXTURE_H
#define IMP_TEXTURE_H

#include <Core/Object.h>
#include <Graphics/Image.h>

#include <string>

IMPGEARS_BEGIN

/// \brief Defines a texture.
class IMP_API Texture : public Object
{
public:

    enum MemorySyncMode
    {
        MemorySyncMode_None = 0,
        MemorySyncMode_VideoOnLocal,
        MemorySyncMode_LocalOnVideo,
        MemorySyncMode_Bidirectional
    };

    Meta_Class(Texture)

    Texture(const std::string& name = "");

    virtual ~Texture();

    void loadFromMemory(std::uint8_t* data, std::uint32_t width, std::uint32_t height, int chnls = 3);

    void loadFromImage(const Image::Ptr data);

    void saveToImage(Image::Ptr& data);

    void bind() const;
    void unbind() const;

    std::uint32_t getVideoID() const{return _videoID;}

    bool isSmooth() const{return _isSmooth;}
    void setSmooth(bool smooth){_isSmooth = smooth; update();}

    bool isRepeated() const{return _isRepeated;}
    void setRepeated(bool repeated){_isRepeated = repeated; update();}

    bool hasMimap() const{return _hasMipmap;}
    void setMipmap(bool mipmap, std::uint32_t maxLevel = 1000){_hasMipmap = mipmap; _mipmapMaxLevel = maxLevel; update();}

    void update();

    int width() const {return _width;}
    int height() const {return _height;}

    static std::uint32_t s_count() { return _s_count; }

protected:
private:

    std::uint32_t _videoID;
    static std::uint32_t _s_count;

    int _width;
    int _height;

    bool _isSmooth;
    bool _isRepeated;
    bool _hasMipmap;

    std::uint32_t _mipmapMaxLevel;

    std::string _name;
};


IMPGEARS_END

#endif // IMP_TEXTURE_H
