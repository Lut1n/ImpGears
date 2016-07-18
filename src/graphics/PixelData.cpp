#include "Graphics/PixelData.h"

#include <cstdlib>
#include <cstring>

IMPGEARS_BEGIN

//--------------------------------------------------------------
PixelData::PixelData():
    m_buffer(IMP_NULL),
    m_width(0),
    m_height(0),
    m_bpp(32),
    m_channels(4),
    m_format(PixelFormat_Unknown),
    m_type(PixelType_Unknown)
{
}

//--------------------------------------------------------------
PixelData::~PixelData()
{
    destroy();
}

//--------------------------------------------------------------
void PixelData::loadFromMemory(const void* data, Uint32 width, Uint32 height, PixelFormat format)
{
    create(width, height, format);
    memcpy(m_buffer, data, m_bufferSize);
}

//--------------------------------------------------------------
void PixelData::create(Uint32 width, Uint32 height, PixelFormat format)
{
    destroy();

    m_width = width;
    m_height = height;
    m_format = format;

    m_type = PixelType_Uint8;
    m_bpp = 32;
    m_channels = 4;

    switch(format)
    {
        case PixelFormat_RGB8:
            m_type = PixelType_Uint8;
            m_bpp = 24;
            m_channels = 3;
            break;
        case PixelFormat_RGBA8:
            m_type = PixelType_Uint8;
            m_bpp = 32;
            m_channels = 4;
            break;
        case PixelFormat_RG16:
            m_type = PixelType_Uint16;
            m_bpp = 32;
            m_channels = 2;
            break;
        case PixelFormat_R16:
            m_type = PixelType_Uint16;
            m_bpp = 16;
            m_channels = 1;
            break;
        default:
            break;
    }

    m_bufferSize = width*height*(m_bpp/8);

    m_buffer = new Uint8[m_bufferSize];
    memset(m_buffer, 0, m_bufferSize);
}

//--------------------------------------------------------------
void PixelData::destroy()
{
    if(m_buffer != IMP_NULL)
    {
        free(m_buffer);
        m_buffer = IMP_NULL;
    }
}

//--------------------------------------------------------------
Pixel PixelData::getPixel(Uint32 x, Uint32 y) const
{
    const Uint32 pixelSize = m_bpp/8;

    const Uint64 offset = (y*m_width+x)*pixelSize;

    return convertIntoPixel(m_buffer+offset, m_format);
}

//--------------------------------------------------------------
void PixelData::setPixel(Uint32 x, Uint32 y, Pixel pixel)
{
    const Uint32 pixelSize = m_bpp/8;

    const Uint64 offset = (y*m_width+x)*pixelSize;

    fillBuffFromPixel(pixel, m_buffer+offset, m_format);
}

//--------------------------------------------------------------
void PixelData::getPixels(Uint32 x, Uint32 y, Uint32 w, Uint32 h, Pixel* data) const
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
void PixelData::setPixels(Uint32 x, Uint32 y, Uint32 w, Uint32 h, const Pixel* data)
{
	Uint32 index = 0;

	for(Uint32 j=0; j<h; ++j)
	for(Uint32 i=0; i<w; ++i)
	{
	    setPixel(x+i, y+j, data[index]);
		++index;
	}
}

//--------------------------------------------------------------
void PixelData::copy(const PixelData* data)
{
    if(m_buffer == IMP_NULL
       || (m_width != data->m_width)
       || (m_height != data->m_height)
       || (m_format != data->m_format))
    {
        create(data->getWidth(), data->getHeight(), data->getFormat());
    }

    memcpy(m_buffer, data->m_buffer, m_bufferSize);
}

//--------------------------------------------------------------
void PixelData::copyPixels(const PixelData* dataSrc,
                            Uint32 xSrc, Uint32 ySrc, Uint32 wSrc, Uint32 hSrc,
                            Uint32 xDst, Uint32 yDst)
{
	Pixel buffer[wSrc*hSrc];

	dataSrc->getPixels(xSrc, ySrc, wSrc, hSrc, buffer);
	setPixels(xDst, yDst, wSrc, hSrc, buffer);
}

Pixel PixelData::convertIntoPixel(const Uint8* pixelBuff, PixelFormat format)
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

void PixelData::fillBuffFromPixel(Pixel pixel, Uint8* outBuff, PixelFormat format)
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

IMPGEARS_END
