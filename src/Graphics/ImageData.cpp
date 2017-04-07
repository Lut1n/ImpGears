#include "Graphics/ImageData.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

IMPGEARS_BEGIN

	void resize(int w, int h, int xrel=0, int yrel=0);


//--------------------------------------------------------------
ImageData::ImageData()
	: m_buffer(IMP_NULL)
	, m_bufferSize(0)
	, m_rawSize(0)
    , m_width(0)
    , m_height(0)
    , m_bpp(32)
    , m_channels(4)
    , m_format(PixelFormat_Unknown)
    // , m_type(PixelType_Unknown)
{
}

//--------------------------------------------------------------
ImageData::~ImageData()
{
    destroy();
}

//--------------------------------------------------------------
/*
void ImageData::loadFromMemory(const void* data, Uint32 width, Uint32 height, PixelFormat format)
{
    create(width, height, format);
    memcpy(m_buffer, data, m_bufferSize);
}*/

//--------------------------------------------------------------
void ImageData::create(Uint32 width, Uint32 height, Uint32 bpp, PixelFormat format, Uint8* srcBuffer)
{
    create(width, height, format, srcBuffer);
}

//--------------------------------------------------------------
void ImageData::create(Uint32 width, Uint32 height, PixelFormat format, Uint8* srcBuffer)
{
    destroy();

	m_width = width;
	m_height = height;
	m_bpp = 32;
    m_channels = 4;
    // m_type = PixelType_Uint8;

	m_format = format;
	
    switch(format)
    {
        case PixelFormat_RGB8:
            // m_type = PixelType_Uint8;
            m_bpp = 24;
            m_channels = 3;
            break;
        case PixelFormat_BGR8:
            // m_type = PixelType_Uint8;
            m_bpp = 24;
            m_channels = 3;
            break;
        case PixelFormat_RGBA8:
            // m_type = PixelType_Uint8;
            m_bpp = 32;
            m_channels = 4;
            break;
        case PixelFormat_BGRA8:
            // m_type = PixelType_Uint8;
            m_bpp = 32;
            m_channels = 4;
            break;
        case PixelFormat_RG16:
            // m_type = PixelType_Uint16;
            m_bpp = 32;
            m_channels = 2;
            break;
        case PixelFormat_R8:
            // m_type = PixelType_Uint16;
            m_bpp = 8;
            m_channels = 1;
            break;
        case PixelFormat_R16:
            // m_type = PixelType_Uint16;
            m_bpp = 16;
            m_channels = 1;
            break;
        default:
            break;
    }

	m_rawSize = m_bpp/8 * m_width;
	m_bufferSize = m_rawSize * m_height;

	m_buffer = new Uint8[m_bufferSize];

	if( srcBuffer != IMP_NULL )
		memcpy(m_buffer, srcBuffer, m_bufferSize);
	else
		memset(m_buffer, 0, m_bufferSize);
}

//--------------------------------------------------------------
void ImageData::destroy()
{
    if(m_buffer != IMP_NULL)
    {
        delete [] m_buffer;
		
		m_buffer = IMP_NULL;
		m_width = 0;
		m_height = 0;
		m_bpp = 0;
		m_channels = 0;
		m_format = PixelFormat_Unknown;

		m_rawSize = 0;
		m_bufferSize = 0;
    }
}

//--------------------------------------------------------------
void ImageData::resize(Int32 w, Int32 h, Int32 xrel, Int32 yrel)
{ 
}

//--------------------------------------------------------------
Pixel ImageData::getPixel(Uint32 x, Uint32 y) const
{
	Pixel result;
	
	if(x >= m_width || x<0 || y>= m_height || y<0)
		return result;
	
	Uint32 start = (y*m_rawSize) + (x*m_bpp/8);

	if(m_format == PixelFormat_RGB8)
	{
		Uint8 pixel[3];
		memcpy(pixel, &(m_buffer[start]), 3);
		result.r = pixel[0];
		result.g = pixel[1];
		result.b = pixel[2];
		result.a = 255;
	}
	else if(m_format == PixelFormat_BGR8)
	{
		Uint8 pixel[3];
		memcpy(pixel, &(m_buffer[start]), 3);
		result.b = pixel[0];
		result.g = pixel[1];
		result.r = pixel[2];
		result.a = 255;
	}
	else if(m_format == PixelFormat_RGBA8)
	{
		Uint8 pixel[4];
		memcpy(pixel, &(m_buffer[start]), 4);
		result.r = pixel[0];
		result.g = pixel[1];
		result.b = pixel[2];
		result.a = pixel[3];
	}
	else if(m_format == PixelFormat_BGRA8)
	{
		Uint8 pixel[4];
		memcpy(pixel, &(m_buffer[start]), 4);
		result.b = pixel[0];
		result.g = pixel[1];
		result.r = pixel[2];
		result.a = pixel[3];
	}
	else if(m_format == PixelFormat_RG16)
	{
		Uint16 pixel[2];
		memcpy(pixel, &(m_buffer[start]), 4);
		result.r = pixel[0];
		result.g = pixel[1];
		result.b = 0;
		result.a = 1;
	}
	else if(m_format == PixelFormat_R8)
	{
		Uint8 pixel[1];
		memcpy(pixel, &(m_buffer[start]), 1);
		result.r = pixel[0];
		result.g = 0;
		result.b = 0;
		result.a = 1;
	}
	else if(m_format == PixelFormat_R16)
	{
		Uint16 pixel[1];
		memcpy(pixel, &(m_buffer[start]), 2);
		result.r = pixel[0];
		result.g = 0;
		result.b = 0;
		result.a = 1;
	}
	else
	{
		result.r = 0;
		result.g = 0;
		result.b = 0;
		result.a = 1;
	}

	return result;
}

//--------------------------------------------------------------
void ImageData::setPixel(Uint32 x, Uint32 y, Pixel pixel)
{
	if(x >= m_width || x<0 || y>= m_height || y<0)
		return;
	
	Uint32 start = (y*m_rawSize) + (x*m_bpp/8);

	if(start > m_bufferSize)
		std::cout << "error : start pas bon\n";

	if(m_format == PixelFormat_RGB8)
	{
		Uint8 pxbuf[3];
		pxbuf[0] = pixel.r;
		pxbuf[1] = pixel.g;
		pxbuf[2] = pixel.b;
		memcpy(&(m_buffer[start]), pxbuf, 3);
	}
	else if(m_format == PixelFormat_BGR8)
	{
		Uint8 pxbuf[3];
		pxbuf[0] = pixel.b;
		pxbuf[1] = pixel.g;
		pxbuf[2] = pixel.r;
		memcpy(&(m_buffer[start]), pxbuf, 3);
	}
	else if(m_format == PixelFormat_RGBA8)
	{
		Uint8 pxbuf[4];
		pxbuf[0] = pixel.r;
		pxbuf[1] = pixel.g;
		pxbuf[2] = pixel.b;
		pxbuf[3] = pixel.a;
		memcpy(&(m_buffer[start]), pxbuf, 4);
	}
	else if(m_format == PixelFormat_BGRA8)
	{
		Uint8 pxbuf[4];
		pxbuf[0] = pixel.b;
		pxbuf[1] = pixel.g;
		pxbuf[2] = pixel.r;
		pxbuf[3] = pixel.a;
		memcpy(&(m_buffer[start]), pxbuf, 4);
	}
	else if(m_format == PixelFormat_RG16)
	{
		Uint16 pxbuf[2];
		pxbuf[0] = pixel.r;
		pxbuf[1] = pixel.g;
		memcpy(&(m_buffer[start]), pxbuf, 4);
	}
	else if(m_format == PixelFormat_R8)
	{
		Uint8 pxbuf[1];
		pxbuf[0] = pixel.b;
		memcpy(&(m_buffer[start]), pxbuf, 1);
	}
	else if(m_format == PixelFormat_R16)
	{
		Uint16 pxbuf[1];
		pxbuf[0] = pixel.b;
		memcpy(&(m_buffer[start]), pxbuf, 2);
	}
}

//--------------------------------------------------------------
Pixel ImageData::getRepeatPixel(Uint32 x, Uint32 y) const
{
    if(x < 0.0)
        x = getWidth()+x;
    else if(x>=getWidth())
        x -= getWidth();
    
    if(y < 0.0)
        y = getHeight()+y;
    else if(y>=getHeight())
        y-= getHeight();
    
    return getPixel(x,y);
}

//--------------------------------------------------------------
void ImageData::setRepeatPixel(Uint32 x, Uint32 y, Pixel pixel)
{
    if(x < 0.0)
        x = getWidth()+x;
    else if(x>=getWidth())
        x -= getWidth();
    
    if(y < 0.0)
        y = getHeight()+y;
    else if(y>=getHeight())
        y-= getHeight();
    
    setPixel(x,y, pixel);
}

/*
//--------------------------------------------------------------
void ImageData::getPixels(Uint32 x, Uint32 y, Uint32 w, Uint32 h, Pixel* data) const
{
    Uint32 index = 0;

	for(Uint32 j=0; j<h; ++j)
	for(Uint32 i=0; i<w; ++i)
	{
		data[index] = getPixel(x+i,y+j);
		++index;
	}
}

//--------------------------------------------------------------
void ImageData::setPixels(Uint32 x, Uint32 y, Uint32 w, Uint32 h, const Pixel* data)
{
	Uint32 index = 0;

	for(Uint32 j=0; j<h; ++j)
	for(Uint32 i=0; i<w; ++i)
	{
	    setPixel(x+i, y+j, data[index]);
		++index;
	}
}
*/
//--------------------------------------------------------------
void ImageData::clone(const ImageData& other)
{	
    if(m_buffer == IMP_NULL
       || (m_width != other.m_width)
       || (m_height != other.m_height)
       || (m_format != other.m_format)
	   || (m_bpp != other.m_bpp))
    {
		destroy();
        create(other.m_width, other.m_height, other.m_format);
    }

    memcpy(m_buffer, other.m_buffer, m_bufferSize);
}

//--------------------------------------------------------------
void ImageData::draw(const ImageData& srcData, const Rect& srcRect, const Rect& dstRect)
{
	if(m_buffer == NULL)
        return;

    if(m_bpp != srcData.m_bpp)
    	return;

    if(m_format != srcData.m_format)
    	return;

    for(Uint32 i=0; i<srcRect.h; ++i)
    {
    	Uint32 srcRaw = (srcRect.y+i)*srcData.m_rawSize;
    	Uint32 srcStart = srcRaw + (srcData.m_bpp/8 * srcRect.x);

    	Uint32 dstRaw = (dstRect.y+i)*m_rawSize;
    	Uint32 dstStart = dstRaw + (m_bpp/8 * dstRect.x);

    	Uint32 length = srcRect.w * m_bpp/8;

    	memcpy(&(m_buffer[dstStart]), &(srcData.m_buffer[srcStart]), length);
    }
}

//--------------------------------------------------------------
void ImageData::convert(const ImageData& srcData, PixelFormat targetFormat)
{
	create(srcData.m_width, srcData.m_height, targetFormat);
	for(Uint32 i = 0; i<m_width; ++i)
	{
		for(Uint32 j = 0; j<m_height; ++j)
		{
			setPixel( i, j, srcData.getPixel(i,j) );
		}
	}
}

//--------------------------------------------------------------
void ImageData::fill(const Pixel& color)
{
	std::cout << "fill : size=" << m_bufferSize << "; chanl=" << m_channels << "; format=" << m_format << "\n";
	std::cout << "color=" << color.r << " " << color.g << " " << color.b << "\n";
	for(Uint32 i=0; i<m_bufferSize; i+=m_channels)
	{
		if(m_format == PixelFormat_RGB8)
		{
			Uint8 pixel[3];
			pixel[0] = color.r;
			pixel[1] = color.g;
			pixel[2] = color.b;
			memcpy(&(m_buffer[i]), pixel, 3);
		}
		else if(m_format == PixelFormat_BGR8)
		{
			Uint8 pixel[3];
			pixel[0] = color.b;
			pixel[1] = color.g;
			pixel[2] = color.r;
			memcpy(&(m_buffer[i]), pixel, 3);
		}
		else if(m_format == PixelFormat_RGBA8)
		{
			Uint8 pixel[4];
			pixel[0] = color.r;
			pixel[1] = color.g;
			pixel[2] = color.b;
			pixel[3] = color.a;
			memcpy(&(m_buffer[i]), pixel, 4);
		}
		else
		{
			std::cout << "impError : pixel format not supported by the fill() function.\n";
		}
	}	
}
/*
Pixel ImageData::convertIntoPixel(const Uint8* pixelBuff, PixelFormat format)
{
    Pixel pixel;
    Uint16 ui16;

    switch(format)
    {
        case PixelFormat_RGBA8:
            pixel.red = static_cast<Uint32>(pixelBuff[0]);
            pixel.green = static_cast<Uint32>(pixelBuff[1]);
            pixel.blue = static_cast<Uint32>(pixelBuff[2]);
            pixel.alpha = static_cast<Uint32>(pixelBuff[3]);
            break;
        case PixelFormat_RGB8:
            pixel.red = static_cast<Uint32>(pixelBuff[0]);
            pixel.green = static_cast<Uint32>(pixelBuff[1]);
            pixel.blue = static_cast<Uint32>(pixelBuff[2]);
            pixel.alpha = 255;
            break;
        case PixelFormat_RG16:
            memcpy(&ui16, pixelBuff, sizeof(Uint16));
            pixel.red = static_cast<Uint32>(ui16);
            memcpy(&ui16, pixelBuff+sizeof(Uint16), sizeof(Uint16));
            pixel.green = static_cast<Uint32>(ui16);
            pixel.blue = 0;
            pixel.alpha = 0;
            break;
        case PixelFormat_R16:
            memcpy(&ui16, pixelBuff, sizeof(Uint16));
            pixel.red = static_cast<Uint32>(ui16);
            pixel.green = 0;
            pixel.blue = 0;
            pixel.alpha = 0;
            break;
        default:
        break;
    }

    return pixel;
}

void ImageData::fillBuffFromPixel(Pixel pixel, Uint8* outBuff, PixelFormat format)
{
    Uint16 ui16;

    switch(format)
    {
        case PixelFormat_RGBA8:
            outBuff[0] = static_cast<Uint8>(pixel.red);
            outBuff[1] = static_cast<Uint8>(pixel.green);
            outBuff[2] = static_cast<Uint8>(pixel.blue);
            outBuff[3] = static_cast<Uint8>(pixel.alpha);
            break;
        case PixelFormat_RGB8:
            outBuff[0] = static_cast<Uint8>(pixel.red);
            outBuff[1] = static_cast<Uint8>(pixel.green);
            outBuff[2] = static_cast<Uint8>(pixel.blue);
            break;
        case PixelFormat_RG16:
            ui16 = static_cast<Uint16>(pixel.red);
            memcpy(outBuff, &ui16, sizeof(Uint16));
            ui16 = static_cast<Uint16>(pixel.green);
            memcpy(outBuff+sizeof(Uint16), &ui16, sizeof(Uint16));
            pixel.blue = 0;
            pixel.alpha = 0;
            break;
        case PixelFormat_R16:
            ui16 = static_cast<Uint16>(pixel.red);
            memcpy(outBuff, &ui16, sizeof(Uint16));
            pixel.green = 0;
            pixel.blue = 0;
            pixel.alpha = 0;
            break;
        default:
        break;
    }
}
*/
IMPGEARS_END
