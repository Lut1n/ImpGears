#ifndef IMP_RENDERTARGET_H
#define IMP_RENDERTARGET_H

#include <Core/Object.h>

#include "Texture.h"

IMPGEARS_BEGIN

class IMP_API FrameBuffer : public Object
{
public:

	Meta_Class(FrameBuffer)

	FrameBuffer();
	virtual ~FrameBuffer();

	void create(int width, int height, int textureCount = 1, bool depthBuffer = false);

	void destroy();

	bool hasDepthBuffer() const{return _hasDepthBuffer;}

	Texture::Ptr getTexture(int n);

	void bind();
	void unbind();

protected:

	int _id;
	int _rbo;
	int _textureCount;
	bool _hasDepthBuffer;
	
	std::vector<Texture::Ptr> _colorTextures;
};

IMPGEARS_END

#endif // IMP_RENDERTARGET_H
