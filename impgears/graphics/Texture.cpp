#include "Texture.h"
#include <cstring>
#include <iostream>
#include "GLcommon.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
Texture::Texture():
    m_data(IMP_NULL),
    m_width(0),
    m_height(0),
    m_format(Format_RGBA),
    m_memoryMode(MemoryMode_ramAndVideo),
    m_videoID(0),
    m_isSmooth(false),
    m_isRepeated(false),
    m_hasMipmap(false),
    m_mipmapMaxLevel(1000),
    m_videoMemLastModified(false)
{
    //ctor
}

//--------------------------------------------------------------
Texture::~Texture()
{
    destroy();
}

//--------------------------------------------------------------
void Texture::loadFromMemory(char* data, Uint32 width, Uint32 height, Format format, MemoryMode memoryMode)
{
    create(width, height, format, memoryMode);

    Uint32 bpp = 32;
    switch(format)
    {
        case Format_RGB:
            bpp = 24;
            break;
        case Format_RGBA:
            bpp = 32;
            break;
        default:
            fprintf(stderr, "[impError] format not supported.\n");
            break;
    }

    Uint32 size = width*height*(bpp/8);

    memcpy(m_data, data, size);

    m_videoMemLastModified = false;
}

//--------------------------------------------------------------
void Texture::create(Uint32 width, Uint32 height, Format format, MemoryMode memoryMode)
{
    destroy();

    m_width = width;
    m_height = height;
    m_format = format;

    Uint32 bpp = 32;
    switch(format)
    {
        case Format_RGB:
            bpp = 24;
            break;
        case Format_RGBA:
            bpp = 32;
            break;
        case Format_Depth16:
            bpp = 16;
        default:
            break;
    }

    Uint32 size = width*height*(bpp/8);

    m_data = new char[size];
    memset(m_data, 0, size);

    m_videoMemLastModified = false;
}

//--------------------------------------------------------------
void Texture::destroy()
{
    if(m_data != IMP_NULL)
    {
        delete [] m_data;
        m_data = IMP_NULL;
    }

    if(m_videoID > 0)
    {
        GLuint id = static_cast<GLuint>(m_videoID);
        glDeleteTextures(1, &id);
        GL_CHECKERROR("delete texture");
        m_videoID = 0;
    }

    m_videoMemLastModified = false;
}

//--------------------------------------------------------------
void Texture::updateGlTex()
{
    if(m_videoID == 0)
    {
        GLuint id = 0;
        glGenTextures(1, &id);
        m_videoID = static_cast<Uint32>(id);
    }

    if(m_data != IMP_NULL)
    {
        int texture_format;
        int gl_texture_format;
        int component_format = GL_UNSIGNED_BYTE;

        if (m_format == Format_RGBA)     // alpha
        {
            texture_format = GL_RGBA;
            gl_texture_format = GL_RGBA8;
        }
        else if(m_format == Format_RGB)  // no alpha
        {
            texture_format = GL_RGB;
            gl_texture_format = GL_RGB8;
        }
        else if(m_format == Format_Depth16)
        {
            texture_format = GL_DEPTH_COMPONENT;
            gl_texture_format = GL_DEPTH_COMPONENT16;
            component_format = GL_UNSIGNED_BYTE;
        }
        else
        {
            fprintf(stderr, "impError : texture format error\n");
        }

        GLint glFilterMagValue = m_isSmooth? GL_NEAREST : GL_LINEAR;
        GLint glWrapValue = m_isRepeated? GL_REPEAT : GL_CLAMP_TO_EDGE;

        GLint glFilterMinValue = glFilterMagValue;
        if(m_hasMipmap)
        {
            if(glFilterMinValue == GL_NEAREST)
                glFilterMinValue = GL_NEAREST_MIPMAP_NEAREST; // GL_NEAREST_MIPMAP_LINEAR;
            else if(glFilterMinValue == GL_LINEAR)
                glFilterMinValue = GL_LINEAR_MIPMAP_LINEAR;
        }

        glBindTexture(GL_TEXTURE_2D, m_videoID);
        if(m_format == Format_Depth16)
        {
            delete [] m_data;
            m_data = IMP_NULL;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, gl_texture_format, m_width, m_height, 0, texture_format, component_format, m_data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapValue);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapValue);
        if(m_hasMipmap)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_mipmapMaxLevel);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMagValue);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMinValue);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    GL_CHECKERROR("texture update");
}

//--------------------------------------------------------------
void Texture::synchronize()
{
    if(m_memoryMode != MemoryMode_ramAndVideo)
        return;

    if(m_videoMemLastModified)
    {
        //glReadBuffer(GL_COLOR_ATTACHMENT0+buffer);
        //glReadPixels(0, 0, WIN_W, WIN_H, internal_format, GL_UNSIGNED_BYTE, &pixels[0]);
        m_videoMemLastModified = false;
    }
    else
    {
        updateGlTex();
    }

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

//--------------------------------------------------------------
Pixel Texture::getPixel(Uint32 _x, Uint32 _y) const
{
    Uint32 channels = 4;
    if(m_format == Format_RGB)
        channels = 3;

	Pixel pixel;
	pixel.red = m_data[_y*m_width*channels + _x*channels];
	pixel.green = m_data[_y*m_width*channels + _x*channels+1];
	pixel.blue = m_data[_y*m_width*channels + _x*channels+2];

	if(channels == 4)
		pixel.alpha = m_data[_y*m_width*channels + _x*channels+3];
	else
		pixel.alpha = 1;

	return pixel;
}

//--------------------------------------------------------------
void Texture::setPixel(Uint32 _x, Uint32 _y, Pixel _pixel)
{
    Uint32 channels = 4;
    if(m_format == Format_RGB)
        channels = 3;

    m_data[_y*m_width*channels + _x*channels] = _pixel.red;
    m_data[_y*m_width*channels + _x*channels+1] = _pixel.green;
    m_data[_y*m_width*channels + _x*channels+2] = _pixel.blue;

    if(channels == 4)
        m_data[_y*m_width*channels + _x*channels+3] = _pixel.alpha;

    m_videoMemLastModified = false;

	if(m_pixelsLocked == false)
    	synchronize();
}

//--------------------------------------------------------------
void Texture::getPixels(Uint32 _x, Uint32 _y, Uint32 _w, Uint32 _h, Pixel* _data) const
{
    Uint32 channels = 4;
    if(m_format == Format_RGB)
        channels = 3;

	int index = 0;
	for(Uint32 y=0; y<_h; ++y)
	for(Uint32 x=0; x<_w; ++x)
	{
		_data[index].red = m_data[(_y+y)*m_width*channels + (_x+x)*channels];
		_data[index].green = m_data[(_y+y)*m_width*channels + (_x+x)*channels+1];
		_data[index].blue = m_data[(_y+y)*m_width*channels + (_x+x)*channels+2];

		if(channels == 4)
			_data[index].alpha = m_data[(_y+y)*m_width*channels + (_x+x)*channels+3];
		else
			_data[index].alpha = 1;

		++index;
	}
}

//--------------------------------------------------------------
void Texture::setPixels(Uint32 _x, Uint32 _y, Uint32 _w, Uint32 _h, const Pixel* _data)
{
    Uint32 channels = 4;
    if(m_format == Format_RGB)
        channels = 3;

	int index = 0;
	for(Uint32 y=0; y<_h; ++y)
	for(Uint32 x=0; x<_w; ++x)
	{
		m_data[(_y+y)*m_width*channels + (_x+x)*channels] = _data[index].red;
		m_data[(_y+y)*m_width*channels + (_x+x)*channels+1] = _data[index].green;
		m_data[(_y+y)*m_width*channels + (_x+x)*channels+2] = _data[index].blue;

		if(channels == 4)
			m_data[(_y+y)*m_width*channels + (_x+x)*channels+3] = _data[index].alpha;

		++index;
	}

	m_videoMemLastModified = false;

	if(m_pixelsLocked == false)
	{
	    GLuint videoFormat = GL_RGBA;
        if(channels == 3)
            videoFormat = GL_RGB;

		glBindTexture(GL_TEXTURE_2D,m_videoID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, _x, _y, _w, _h, videoFormat, GL_UNSIGNED_BYTE, (GLvoid*)_data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//--------------------------------------------------------------
void Texture::lockPixels()
{
	m_pixelsLocked = true;
}

//--------------------------------------------------------------
void Texture::unlockPixels()
{
	if(m_pixelsLocked)
	{
		synchronize();
		m_pixelsLocked = false;
	}
}

//--------------------------------------------------------------
void Texture::draw2D(Uint32 x, Uint32 y, float alpha) const
{
	draw2D(x, y, m_width+x, m_height+y, alpha);
}

//--------------------------------------------------------------
void Texture::draw2D(Uint32 x, Uint32 y, Uint32 x2, Uint32 y2, float alpha) const
{
	bind();

	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	glTexCoord2f(0.f,1.f);
	glVertex2i(x, y);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	glTexCoord2f(1.f,1.f);
	glVertex2i(x2, y);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	glTexCoord2f(1.f,0.f);
	glVertex2i(x2, y2);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	glTexCoord2f(0.f,0.f);
	glVertex2i(x, y2);
	glEnd();
}

//--------------------------------------------------------------
void Texture::copyPixelsFrom(const Texture* _src,
                    Uint32 _srcx, Uint32 _srcy, Uint32 _srcw, Uint32 _srch,
                    Uint32 _dstx, Uint32 _dsty)
{
	Pixel buffer[_srcw*_srch];

	_src->getPixels(_srcx, _srcy, _srcw, _srch, buffer);
	setPixels(_dstx, _dsty, _srcw, _srch, buffer);
}

//--------------------------------------------------------------
void Texture::notifyVideoMemModified()
{
    m_videoMemLastModified = true;
}

IMPGEARS_END
