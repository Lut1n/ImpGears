#include <OGLPlugin/CubeMap.h>
#include <OGLPlugin/GlError.h>

#include <iostream>

IMPGEARS_BEGIN

std::uint32_t CubeMap::_s_count = 0;

//--------------------------------------------------------------
CubeMap::CubeMap(const std::string& name)
    : _videoID(0)
    , _name(name)
{
    glGenTextures(1, &_videoID);
    GL_CHECKERROR("gen CubeMap");
    _s_count++;
}

//--------------------------------------------------------------
CubeMap::~CubeMap()
{
    glDeleteTextures(1, &_videoID);
    GL_CHECKERROR("delete CubeMap");
    _videoID = 0;
    _s_count--;
}

//--------------------------------------------------------------
void CubeMap::loadFromMemory(std::uint8_t* buf, std::uint32_t width, std::uint32_t height, int chnls, int faceID)
{
    update();

    std::int32_t glInternalFormat = 0;
    std::int32_t glDataFormat = 0;
    std::int32_t glDataType = GL_UNSIGNED_BYTE;

    if(chnls == 1)
    {
    // case PixelFormat_R16 :
        glDataFormat = GL_RED;
        glInternalFormat = GL_DEPTH_COMPONENT16;
    }
    else if(chnls == 3)
    {
        glDataFormat = GL_RGB;
        glInternalFormat = GL_RGB8;
    }
    else if(chnls == 4)
    {
        glDataFormat = GL_RGBA;
        glInternalFormat = GL_RGBA8;
    }
    else if(chnls == 2)
    {
        // case PixelFormat_R16 :
        // glDataFormat = GL_DEPTH_COMPONENT;
        // glInternalFormat = GL_DEPTH_COMPONENT16;
    }
    else
    {
        std::cerr << "impError : " << _name << " CubeMap format error (" << chnls << " chnls)" << std::endl;
    }

    glActiveTexture(GL_TEXTURE0);
    bind();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+faceID, 0, glInternalFormat, width, height,0, glDataFormat, glDataType, buf);
    GL_CHECKERROR("CubeMap update gpu Texture");
    unbind();
}

//--------------------------------------------------------------
void CubeMap::saveToMemory(std::uint8_t* buf, std::uint32_t width, std::uint32_t height, int chnls, int faceID)
{
    std::int32_t glDataFormat = 0;
    std::int32_t glDataType = GL_UNSIGNED_BYTE;

    if(chnls == 1)
    {
        glDataFormat = GL_RED;
        // glInternalFormat = GL_DEPTH_COMPONENT16;
    }
    else if(chnls == 3)
    {
        glDataFormat = GL_RGB;
        // glInternalFormat = GL_RGB8;
    }
    else if(chnls == 4)
    {
        glDataFormat = GL_RGBA;
        // glInternalFormat = GL_RGBA8;
    }

    glActiveTexture(GL_TEXTURE0);
    bind();
    glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X+faceID, 0, glDataFormat, glDataType, buf);
    unbind();
}

//--------------------------------------------------------------
void CubeMap::loadFromImage(const std::vector<Image::Ptr>& img)
{
    _sources = img;
    for(int i=0; i<6; ++i)
        loadFromMemory(img[i]->data(), img[i]->width(),img[i]->height(),img[i]->channels(), i);
}

//--------------------------------------------------------------
void CubeMap::saveToImage(std::vector<Image::Ptr>& img)
{
    for(int i=0; i<6; ++i)
    {
        saveToMemory(img[i]->data(), img[i]->width(),img[i]->height(),img[i]->channels(), i);
        GL_CHECKERROR("CubeMap::saveToImage");
    }
}

//--------------------------------------------------------------
void CubeMap::update()
{
    std::int32_t glFilterMagValue = GL_LINEAR;
    std::int32_t glWrapMode = GL_CLAMP_TO_EDGE;

    GLint glFilterMinValue = glFilterMagValue;

    glActiveTexture(GL_TEXTURE0);
    bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, glWrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, glWrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, glWrapMode);
    GL_CHECKERROR("CubeMap update 1");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, glFilterMagValue);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, glFilterMinValue);
    GL_CHECKERROR("CubeMap update 2");

    unbind();
}

//--------------------------------------------------------------
void CubeMap::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, _videoID);
    GL_CHECKERROR("bind CubeMap");
}

//--------------------------------------------------------------
void CubeMap::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

IMPGEARS_END
