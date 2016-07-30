#include "Graphics/Texture.h"
#include "Graphics/GLcommon.h"

#include <cstring>
#include <iostream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Texture::Texture():
    m_videoID(0),
    m_syncMode(MemorySyncMode_None),
    m_videoMemLastModified(false),
    m_isSmooth(false),
    m_isRepeated(false),
    m_hasMipmap(false),
    m_mipmapMaxLevel(1000)
{
    glGenTextures(1, &m_videoID);
    GL_CHECKERROR("gen texture");
}

//--------------------------------------------------------------
Texture::~Texture()
{
    destroy();

    glDeleteTextures(1, &m_videoID);
    GL_CHECKERROR("delete texture");
    m_videoID = 0;
}

//--------------------------------------------------------------
void Texture::loadFromMemory(char* data, Uint32 width, Uint32 height, PixelFormat format)
{
    m_data.create(width, height, 32, format, reinterpret_cast<Uint8*>(data) );
    updateVideoParams();
    updateVideoMemory();
}

void Texture::loadFromImageData(const ImageData* data)
{
    create(data->getWidth(), data->getHeight(), data->getFormat());
    m_data.clone(*data);
    updateVideoMemory();
}

//--------------------------------------------------------------
void Texture::create(Uint32 width, Uint32 height, PixelFormat format)
{
    m_data.create(width, height, 32, format);

    updateVideoParams();
    updateVideoMemory();
}

//--------------------------------------------------------------
void Texture::destroy()
{
    m_data.destroy();
}

//--------------------------------------------------------------
void Texture::getImageData(ImageData* data) const
{
    if(data == IMP_NULL)
        return;

    data->clone(m_data);
}

//--------------------------------------------------------------
void Texture::synchronize()
{
    if(m_syncMode == MemorySyncMode_None)
        return;

    if(m_videoMemLastModified)
    {
        if(m_syncMode != MemorySyncMode_VideoOnLocal)
            updateLocalMemory();
    }
    else
    {
        if(m_syncMode != MemorySyncMode_LocalOnVideo)
            updateVideoMemory();
    }
}

//--------------------------------------------------------------
void Texture::updateVideoMemory()
{
    Int32 glInternalFormat = 0;
    Int32 glDataFormat = 0;
    Int32 glDataType = GL_UNSIGNED_BYTE;

    switch(m_data.getFormat())
    {
        case PixelFormat_RGBA8 :
            glDataFormat = GL_RGBA;
            glInternalFormat = GL_RGBA8;
        break;
        case PixelFormat_RGB8 :
        case PixelFormat_BGR8 :
            glDataFormat = GL_RGB;
            glInternalFormat = GL_RGB8;
        break;
        case PixelFormat_R16 :
            glDataFormat = GL_DEPTH_COMPONENT;
            glInternalFormat = GL_DEPTH_COMPONENT16;
            //glDataType = GL_UNSIGNED_BYTE;
        break;
        default:
            fprintf(stderr, "impError : texture format error (%d)\n", m_data.getFormat());
        break;
    }

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_data.getWidth(), m_data.getHeight(),
                 0, glDataFormat, glDataType, m_data.getBuffer());
    GL_CHECKERROR("texture update gpu Texture");
    unbind();
}

void Texture::updateVideoParams()
{
    Int32 glFilterMagValue = m_isSmooth? GL_NEAREST : GL_LINEAR;
    Int32 glWrapMode = m_isRepeated? GL_REPEAT : GL_CLAMP_TO_EDGE;

    GLint glFilterMinValue = glFilterMagValue;
    if(m_hasMipmap)
    {
        if(glFilterMinValue == GL_NEAREST)
            glFilterMinValue = GL_NEAREST_MIPMAP_NEAREST; // GL_NEAREST_MIPMAP_LINEAR;
        else if(glFilterMinValue == GL_LINEAR)
            glFilterMinValue = GL_LINEAR_MIPMAP_LINEAR;
    }

    bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode);
    GL_CHECKERROR("texture update 1");
    if(m_hasMipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_mipmapMaxLevel);
    GL_CHECKERROR("texture update 2");
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMagValue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMinValue);
    GL_CHECKERROR("texture update 3");

    unbind();
}

void Texture::updateLocalMemory()
{
    // to implement
}

//--------------------------------------------------------------
void Texture::bind() const
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(m_videoID));
}

//--------------------------------------------------------------
void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

IMPGEARS_END
