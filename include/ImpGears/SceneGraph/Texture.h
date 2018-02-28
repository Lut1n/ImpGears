#ifndef IMP_TEXTURE_H
#define IMP_TEXTURE_H

#include <Core/Object.h>
#include <SceneGraph/ImageData.h>

#include <string>

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

        Texture(const std::string& name = "");

        virtual ~Texture();

        /// \brief Loads the texture from memory.
        /// \param data - A pointer to the data buffer.
        /// \param width - The width of the texture.
        /// \param height - The height of the texture.
        /// \param format - The pixel format.
        /// \param memoryMode - The memory mode.
        void loadFromMemory(char* data, std::uint32_t width, std::uint32_t height, PixelFormat format = PixelFormat_RGBA8);

        /// \brief Loads the texture from pixel buffer.
        /// \param data - A pointer to the data buffer.
        /// \param memoryMode - The memory mode.
        void loadFromImageData(const ImageData* data);

        /// \brief Creates a texture.
        /// \param width - The width of the texture.
        /// \param height - The height of the texture.
        /// \param format - The pixel format.
        /// \param memoryMode - The memory mode.
		void create(std::uint32_t width, std::uint32_t height, PixelFormat format = PixelFormat_RGBA8);

		/// \brief Destroy a texture
		void destroy();

        void getImageData(ImageData* data) const;

        void notifyTextureRendering(){m_videoMemLastModified = true;}
		void synchronize();

        void bind() const;
        void unbind() const;

		std::uint32_t getVideoID() const{return m_videoID;}

        bool isSmooth() const{return m_isSmooth;}
        void setSmooth(bool smooth){m_isSmooth = smooth; updateVideoParams();}

        bool isRepeated() const{return m_isRepeated;}
        void setRepeated(bool repeated){m_isRepeated = repeated; updateVideoParams();}

        bool hasMimap() const{return m_hasMipmap;}
        void setMipmap(bool mipmap, std::uint32_t maxLevel = 1000){m_hasMipmap = mipmap; m_mipmapMaxLevel = maxLevel; updateVideoParams();}

        void setMemorySyncMode(MemorySyncMode mode){m_syncMode = mode;}

        std::uint32_t getWidth() const {return m_data.getWidth();}
        std::uint32_t getHeight() const {return m_data.getHeight();}

    protected:
    private:

		void updateVideoMemory();
        void updateVideoParams();
		void updateLocalMemory();

        ImageData m_data;

        std::uint32_t m_videoID;

        MemorySyncMode m_syncMode;
        bool m_videoMemLastModified;

		bool m_isSmooth;
		bool m_isRepeated;
		bool m_hasMipmap;

		std::uint32_t m_mipmapMaxLevel;
		
		std::string m_name;
};


IMPGEARS_END

#endif // IMP_TEXTURE_H
