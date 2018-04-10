#ifndef IMP_IMAGEDATA_H
#define IMP_IMAGEDATA_H

#include <Core/Object.h>
#include <Core/Vec4.h>

IMPGEARS_BEGIN

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
	std::uint32_t x;
	std::uint32_t y;
	std::uint32_t w;
	std::uint32_t h;
};

class IMP_API ImageData : public Object
{
    public:
	
	Meta_Class(ImageData)

	ImageData();
	virtual ~ImageData();

	void build(std::uint32_t w, std::uint32_t h, std::uint32_t bpp, PixelFormat format, std::uint8_t* srcBuffer = nullptr);
	void build(std::uint32_t w, std::uint32_t h, PixelFormat format, std::uint8_t* srcBuffer = nullptr);
	void destroy();

	const std::uint8_t* getBuffer() const{return m_buffer; }

	void clone(const ImageData& other);
	void resize(std::int32_t w, std::int32_t h, std::int32_t xrel=0, std::int32_t yrel=0);
	void rescale(std::int32_t w, std::int32_t h);
	void draw(const ImageData& srcData, const Rect& srcRect, const Rect& dstRect);
	
	void convert(const ImageData& srcData, PixelFormat targetFormat);

	void fill(const Vec4& color);


	Vec4 getPixel(std::uint32_t x, std::uint32_t y) const;
	void setPixel(std::uint32_t x, std::uint32_t y, const Vec4& pixel);

	Vec4 getRepeatPixel(std::uint32_t x, std::uint32_t y) const;
	void setRepeatPixel(std::uint32_t x, std::uint32_t y, const Vec4& pixel);

	std::uint32_t getWidth() const{return m_width;}
	std::uint32_t getHeight() const{return m_height;}
	std::uint32_t getBpp() const{return m_bpp;}
	PixelFormat getFormat()const{return m_format;}
	// PixelType getType()const{return m_type;}

	std::uint8_t* getData() {return m_buffer;}
	std::uint32_t getDataSize() {return m_bufferSize;}
	std::uint32_t getDataRawSize() {return m_rawSize;}

	// static Pixel convertIntoPixel(const std::uint8_t* pixelBuff, PixelFormat format);
	// static void fillBuffFromPixel(Pixel pixel, std::uint8_t* outBuff, PixelFormat format);

    protected:
    private:

        std::uint8_t* m_buffer;
        std::uint32_t m_bufferSize;
		
		std::uint32_t m_rawSize;

        std::uint32_t m_width, m_height;
        std::uint32_t m_bpp;
        std::uint32_t m_channels;
        PixelFormat m_format;
};

IMPGEARS_END

#endif // IMP_IMAGEDATA_H
