#include <SceneGraph/Texture.h>
#include <SceneGraph/OpenGL.h>

#include <iostream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Texture::Texture(const std::string& name):
    _videoID(0),
    _isSmooth(false),
    _isRepeated(false),
    _hasMipmap(false),
    _mipmapMaxLevel(1000)
	, _name(name)
{
    glGenTextures(1, &_videoID);
    GL_CHECKERROR("gen texture");
}

//--------------------------------------------------------------
Texture::~Texture()
{
    glDeleteTextures(1, &_videoID);
    GL_CHECKERROR("delete texture");
    _videoID = 0;
}

//--------------------------------------------------------------
void Texture::loadFromMemory(std::uint8_t* buf, std::uint32_t width, std::uint32_t height, int chnls)
{
    update();
	
    std::int32_t glInternalFormat = 0;
    std::int32_t glDataFormat = 0;
    std::int32_t glDataType = GL_UNSIGNED_BYTE;
	
	if(chnls == 3)
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
		std::cerr << "impError : " << _name << " texture format error (" << chnls << " chnls)" << std::endl;
	}

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height,0, glDataFormat, glDataType, buf);
    GL_CHECKERROR("texture update gpu Texture");
    unbind();
}

//--------------------------------------------------------------
void Texture::loadFromImage(const Image::Ptr img)
{
    loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
}

//--------------------------------------------------------------
void Texture::update()
{
    std::int32_t glFilterMagValue = _isSmooth? GL_NEAREST : GL_LINEAR;
    std::int32_t glWrapMode = _isRepeated? GL_REPEAT : GL_CLAMP_TO_EDGE;

    GLint glFilterMinValue = glFilterMagValue;
    if(_hasMipmap)
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
    if(_hasMipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, _mipmapMaxLevel);
    GL_CHECKERROR("texture update 2");
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMagValue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMinValue);
    GL_CHECKERROR("texture update 3");

    unbind();
}

//--------------------------------------------------------------
void Texture::bind() const
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(_videoID));
}

//--------------------------------------------------------------
void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

IMPGEARS_END
