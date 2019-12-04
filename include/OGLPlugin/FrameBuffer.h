#ifndef IMP_FRAMEBUFFER_H
#define IMP_FRAMEBUFFER_H

#include <Core/Object.h>

#include <OGLPlugin/Texture.h>
#include <OGLPlugin/CubeMap.h>

IMPGEARS_BEGIN

class IMP_API FrameBuffer : public Object
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

protected:

    int _id;
    int _rbo;
    int _textureCount;
    int _faceID;
    bool _hasDepthBuffer;
    bool _useCubeMap;

    std::vector<Texture::Ptr> _colorTextures;
    std::vector<unsigned int> _drawBuffers;
    std::vector<Vec4> _clearColors;
    CubeMap::Ptr _cubemap;
};

IMPGEARS_END

#endif // IMP_FRAMEBUFFER_H
