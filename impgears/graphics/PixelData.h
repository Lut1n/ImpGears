#ifndef IMP_PIXELDATA_H
#define IMP_PIXELDATA_H

#include "base/impBase.hpp"

IMPGEARS_BEGIN

/// \struct Pixel - Defines a RGBA pixel.
struct Pixel
{
	imp::Uint32 red;
	imp::Uint32 green;
	imp::Uint32 blue;
	imp::Uint32 alpha;
};

enum PixelFormat
{
    PixelFormat_Unknown = 0,
    PixelFormat_RGBA8,
    PixelFormat_RGB8,
    PixelFormat_RG16,
    PixelFormat_R16
};

enum PixelType
{
    PixelType_Unknown = 0,
    PixelType_Uint8,
    PixelType_Uint16
};

class PixelData
{
    public:

        PixelData();
        virtual ~PixelData();

        void loadFromMemory(const void* data, Uint32 width, Uint32 height, PixelFormat format = PixelFormat_RGBA8);
		void create(Uint32 width, Uint32 height, PixelFormat format = PixelFormat_RGBA8);
		void destroy();

		Pixel getPixel(Uint32 x, Uint32 y) const;
		void setPixel(Uint32 x, Uint32 y, Pixel pixel);

		void getPixels(Uint32 x, Uint32 y, Uint32 w, Uint32 h, Pixel* data) const;
		void setPixels(Uint32 x, Uint32 y, Uint32 w, Uint32 h, const Pixel* data);

		Uint32 getWidth() const{return m_width;}
		Uint32 getHeight() const{return m_height;}
		Uint32 getBpp() const{return m_bpp;}
		PixelFormat getFormat()const{return m_format;}
		PixelType getType()const{return m_type;}

		void copy(const PixelData* data);

		void copyPixels(const PixelData* dataSrc,
                        Uint32 xSrc, Uint32 ySrc, Uint32 wSrc, Uint32 hSrc,
                        Uint32 xDst, Uint32 yDst);

        const Uint8* getBuffer() const{return m_buffer;}

        static Pixel convertIntoPixel(const Uint8* pixelBuff, PixelFormat format);
        static void fillBuffFromPixel(Pixel pixel, Uint8* outBuff, PixelFormat format);

    protected:
    private:

        Uint8* m_buffer;
        Uint32 m_bufferSize;

        Uint32 m_width, m_height;
        Uint32 m_bpp;
        Uint32 m_channels;
        PixelFormat m_format;
        PixelType m_type;
};

IMPGEARS_END

#endif // IMP_PIXELDATA_H
