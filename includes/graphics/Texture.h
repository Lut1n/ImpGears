#ifndef IMP_TEXTURE_H
#define IMP_TEXTURE_H

#include "Core/impBase.h"
#include "Graphics/PixelData.h"

IMPGEARS_BEGIN

/// \brief Defines a texture.
class IMP_API Texture
{
    public:

        enum MemorySyncMode
        {
            MemorySyncMode_None = 0,
            MemorySyncMode_VideoOnLocal,
            MemorySyncMode_LocalOnVideo,
            MemorySyncMode_Bidirectional
        };

        Texture();

        virtual ~Texture();

        /// \brief Loads the texture from memory.
        /// \param data - A pointer to the data buffer.
        /// \param width - The width of the texture.
        /// \param height - The height of the texture.
        /// \param format - The pixel format.
        /// \param memoryMode - The memory mode.
        void loadFromMemory(char* data, Uint32 width, Uint32 height, PixelFormat format = PixelFormat_RGBA8);

        /// \brief Loads the texture from pixel buffer.
        /// \param data - A pointer to the data buffer.
        /// \param memoryMode - The memory mode.
        void loadFromPixelData(const PixelData* data);

        /// \brief Creates a texture.
        /// \param width - The width of the texture.
        /// \param height - The height of the texture.
        /// \param format - The pixel format.
        /// \param memoryMode - The memory mode.
		void create(Uint32 width, Uint32 height, PixelFormat format = PixelFormat_RGBA8);

		/// \brief Destroy a texture
		void destroy();

        void getPixelData(PixelData* data) const;

        void notifyTextureRendering(){m_videoMemLastModified = true;}
		void synchronize();

        void bind() const;
        void unbind() const;

		Uint32 getVideoID() const{return m_videoID;}

        bool isSmooth() const{return m_isSmooth;}
        void setSmooth(bool smooth){m_isSmooth = smooth; updateVideoParams();}

        bool isRepeated() const{return m_isRepeated;}
        void setRepeated(bool repeated){m_isRepeated = repeated; updateVideoParams();}

        bool hasMimap() const{return m_hasMipmap;}
        void setMipmap(bool mipmap, Uint32 maxLevel = 1000){m_hasMipmap = mipmap; m_mipmapMaxLevel = maxLevel; updateVideoParams();}

        void setMemorySyncMode(MemorySyncMode mode){m_syncMode = mode;}

        Uint32 getWidth() const {return m_data.getWidth();}
        Uint32 getHeight() const {return m_data.getHeight();}

    protected:
    private:

		void updateVideoMemory();
        void updateVideoParams();
		void updateLocalMemory();

        PixelData m_data;

        Uint32 m_videoID;

        MemorySyncMode m_syncMode;
        bool m_videoMemLastModified;

		bool m_isSmooth;
		bool m_isRepeated;
		bool m_hasMipmap;

		Uint32 m_mipmapMaxLevel;
};


IMPGEARS_END

#endif // IMP_TEXTURE_H
