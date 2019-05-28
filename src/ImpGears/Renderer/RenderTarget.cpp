#include <Core/Object.h>
#include <Renderer/RenderTarget.h>

#include <Renderer/SceneRenderer.h>

IMPGEARS_BEGIN


//--------------------------------------------------------------
RenderTarget::RenderTarget()
{
	_hasChanged = false;
	_hasDepthBuffer = false;
	_d = -1;
}

//--------------------------------------------------------------
RenderTarget::~RenderTarget()
{
	
}

//--------------------------------------------------------------
void RenderTarget::create(int w, int h, int count, bool hasDepth)
{
	_targets.resize(count);
	for(int i=0;i<count;++i)
	{
		_targets[i] = ImageSampler::create();
		_targets[i]->setInternalSrc(w,h,4);
	}
	
	_hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void RenderTarget::create(const std::vector<ImageSampler::Ptr>& textures, bool hasDepth)
{
	_targets = textures;;
	_hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void RenderTarget::destroy()
{
	_targets.clear();
	_hasDepthBuffer = false;
}

//--------------------------------------------------------------
ImageSampler::Ptr RenderTarget::get(int n)
{
	update();
	return _targets[n];
}

//--------------------------------------------------------------
int RenderTarget::width() const
{
	int w = 0;
	if(_targets.size() > 0) w = _targets[0]->getSource()->width();
	return w;
}

//--------------------------------------------------------------
int RenderTarget::height() const
{
	int h = 0;
	if(_targets.size() > 0) h = _targets[0]->getSource()->height();
	return h;
}

//--------------------------------------------------------------
int RenderTarget::count() const
{
	return _targets.size();
}

//--------------------------------------------------------------
bool RenderTarget::hasDepth() const
{
	return _hasDepthBuffer;
}

//--------------------------------------------------------------
void RenderTarget::change()
{
	_hasChanged = true;
}

//--------------------------------------------------------------
void RenderTarget::update()
{
	if(SceneRenderer::s_interface != nullptr
		&& _hasChanged)
	{
		for(int i=0;i<(int)_targets.size();++i)
		{
			ImageSampler::Ptr& img = _targets[i];
			SceneRenderer::s_interface->bringBack(img.get());
		}
		_hasChanged = false;
	}
	
}


IMPGEARS_END
