#ifndef IMP_RENDERTARGET_H
#define IMP_RENDERTARGET_H

#include <Core/Object.h>
#include <Graphics/Sampler.h>

IMPGEARS_BEGIN

class IMP_API RenderTarget : public Object
{
public:
	
	Meta_Class(RenderTarget);
	
	RenderTarget();
	virtual ~RenderTarget();

	void create(int w, int h, int count = 1, bool hasDepth = false);
	void create(const std::vector<ImageSampler::Ptr>& textures, bool hasDepth = false);

	void destroy();

	bool hasDepth() const;

	ImageSampler::Ptr get(int n);
	
	int width() const;
	int height() const;
	int count() const;
	
	void change();
	void update();
	
protected:
	
	std::vector<ImageSampler::Ptr> _targets;
	bool _hasDepthBuffer;
	bool _hasChanged;
};

IMPGEARS_END

#endif // IMP_RENDERTARGET_H
