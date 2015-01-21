#ifndef IMP_TEXTURE_H
#define IMP_TEXTURE_H

#include "../base/impBase.hpp"

IMPGEARS_BEGIN

struct Pixel
{
	imp::Uint8 red;
	imp::Uint8 green;
	imp::Uint8 blue;
	imp::Uint8 alpha;
};

class Texture
{
    public:

        enum MemoryMode
        {
            MemoryMode_ramAndVideo = 0,
            MemoryMode_videoMemOnly,
            MemoryMode_ramOnly
        };

        enum Format
        {
            Format_RGBA = 0,
            Format_RGB,
            Format_Depth16 /// /!\ Conflicting with set/get/copy Pixels
        };

        Texture();

        virtual ~Texture();

        void loadFromMemory(char* data, Uint32 width, Uint32 height, Format format = Format_RGBA, MemoryMode memoryMode = MemoryMode_ramAndVideo);

		void create(Uint32 width, Uint32 height, Format format = Format_RGBA, MemoryMode memoryMode = MemoryMode_ramAndVideo);
		void destroy();

		void updateGlTex();

        void bind() const;
        void unbind() const;

		Pixel getPixel(Uint32 _x, Uint32 _y) const;
		void setPixel(Uint32 _x, Uint32 _y, Pixel _pixel);

		void getPixels(Uint32 _x, Uint32 _y, Uint32 _w, Uint32 _h, Pixel* _data) const;
		void setPixels(Uint32 _x, Uint32 _y, Uint32 _w, Uint32 _h, const Pixel* _data);

		void lockPixels();
		void unlockPixels();

		Uint32 getVideoID() const{return m_videoID;}

		Uint32 getWidth() const{return m_width;}
		Uint32 getHeight() const{return m_height;}
		Format getFormat()const{return m_format;}
		MemoryMode getMemoryMode() const{return m_memoryMode;}

		void draw2D(Uint32 x, Uint32 y, float alpha) const;
		void draw2D(Uint32 x, Uint32 y, Uint32 x2, Uint32 y2, float alpha) const;

		void copyPixelsFrom(const Texture* _src,
									Uint32 _srcx, Uint32 _srcy, Uint32 _srcw, Uint32 _srch,
									Uint32 _dstx, Uint32 _dsty);

        bool isSmooth() const{return m_isSmooth;}
        void setSmooth(bool smooth){m_isSmooth = smooth;}

        bool isRepeated() const{return m_isRepeated;}
        void setRepeated(bool repeated){m_isRepeated = repeated;}

        bool hasMimap() const{return m_hasMipmap;}
        void setMipmap(bool mipmap, Uint32 maxLevel = 1000){m_hasMipmap = mipmap; m_mipmapMaxLevel = maxLevel;}

    protected:
    private:

        char* m_data;
        Uint32 m_width, m_height;
        Format m_format;
        MemoryMode m_memoryMode;
        Uint32 m_videoID;

		bool m_pixelsLocked;

		bool m_isSmooth;
		bool m_isRepeated;
		bool m_hasMipmap;

		Uint32 m_mipmapMaxLevel;
};


IMPGEARS_END

#endif // IMP_TEXTURE_H
