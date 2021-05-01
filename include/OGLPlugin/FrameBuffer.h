#ifndef IMP_FRAMEBUFFER_H
#define IMP_FRAMEBUFFER_H

#include <ImpGears/Core/Object.h>

#include <OGLPlugin/Texture.h>
#include <OGLPlugin/CubeMap.h>
#include <OGLPlugin/Export.h>

IMPGEARS_BEGIN

class IG_BKND_GL_API FrameBuffer : public Object
{
public:

    Meta_Class(FrameBuffer)

    FrameBuffer();
    virtual ~FrameBuffer();

    void build(int width, int height, int textureCount = 1, bool depthBuffer = false);
    void build(const std::vector<Texture::Ptr>& textures, bool depthBuffer = false);
    void build(const CubeMap::Ptr& cubemap, int faceID, bool depthBuffer = false);

    void destroy();

    bool hasDepthBuffer() const{return _hasDepthBuffer;}
    bool useCubeMap() const {return _useCubeMap;}

    Texture::Ptr getTexture(int n);
    CubeMap::Ptr getCubeMap();
    int getFaceID();

    void bind();
    void unbind();

    void setClearColors(const std::vector<Vec4>& clearColors);

    static void s_clearBuffers();
    static FrameBuffer* s_bound;

    static std::uint32_t s_count() { return _s_count; }
    
    int getVideoID() const {return _id;}
    int count() const { return _colorTextures.size(); }
    
    int width() const {return _width;}
    int height() const {return _height;}

protected:

    int _id;
    int _rbo;
    int _textureCount;
    int _faceID;
    bool _hasDepthBuffer;
    bool _useCubeMap;
    int _width;
    int _height;

    static std::uint32_t _s_count;

    std::vector<Texture::Ptr> _colorTextures;
    std::vector<unsigned int> _drawBuffers;
    std::vector<Vec4> _clearColors;
    CubeMap::Ptr _cubemap;
};

IMPGEARS_END

#endif // IMP_FRAMEBUFFER_H
