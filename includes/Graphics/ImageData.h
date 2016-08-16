#ifndef IMP_IMAGEDATA_H
#define IMP_IMAGEDATA_H

#include "Core/impBase.h"

IMPGEARS_BEGIN

/// \struct Pixel - Defines a RGBA pixel.
union IMP_API Pixel
{
	struct { imp::Uint32 red, green, blue, alpha; };
	struct { imp::Uint32 x, y, z, w; };
	struct { imp::Uint32 r, g, b, a; };
};

enum IMP_API PixelFormat
{
    PixelFormat_Unknown = 0,
    PixelFormat_RGBA8,
    PixelFormat_BGRA8,
    PixelFormat_RGB8,
    PixelFormat_BGR8,
    PixelFormat_RG16,
	PixelFormat_R8,
    PixelFormat_R16
};

struct Rect
{
	Uint32 x;
	Uint32 y;
	Uint32 w;
	Uint32 h;
};

/*
enum IMP_API PixelType
{
    PixelType_Unknown = 0,
    PixelType_Uint8,
    PixelType_Uint16
};
*/

class IMP_API ImageData
{
    public:

	ImageData();
	virtual ~ImageData();

	void create(Uint32 w, Uint32 h, Uint32 bpp, PixelFormat format, Uint8* srcBuffer = IMP_NULL);
	void create(Uint32 w, Uint32 h, PixelFormat format, Uint8* srcBuffer = IMP_NULL);
	void destroy();

	const Uint8* getBuffer() const{return m_buffer; }

	void clone(const ImageData& other);
	void resize(Int32 w, Int32 h, Int32 xrel=0, Int32 yrel=0);
	void draw(const ImageData& srcData, const Rect& srcRect, const Rect& dstRect);

	void fill(const Pixel& color);


	Pixel getPixel(Uint32 x, Uint32 y) const;
	void setPixel(Uint32 x, Uint32 y, Pixel pixel);

	Uint32 getWidth() const{return m_width;}
	Uint32 getHeight() const{return m_height;}
	Uint32 getBpp() const{return m_bpp;}
	PixelFormat getFormat()const{return m_format;}
	// PixelType getType()const{return m_type;}

	Uint8* getData() {return m_buffer;}
	Uint32 getDataSize() {return m_bufferSize;}
	Uint32 getDataRawSize() {return m_rawSize;}

	// static Pixel convertIntoPixel(const Uint8* pixelBuff, PixelFormat format);
	// static void fillBuffFromPixel(Pixel pixel, Uint8* outBuff, PixelFormat format);

    protected:
    private:

        Uint8* m_buffer;
        Uint32 m_bufferSize;
		
		Uint32 m_rawSize;

        Uint32 m_width, m_height;
        Uint32 m_bpp;
        Uint32 m_channels;
        PixelFormat m_format;
        // PixelType m_type;
};

IMPGEARS_END

#endif // IMP_IMAGEDATA_H
