#include <Core/Object.h>
#include <Renderer/RenderTarget.h>

IMPGEARS_BEGIN


//--------------------------------------------------------------
RenderTarget::RenderTarget()
{
    _hasChanged = false;
    _hasDepthBuffer = false;
    _useFaceSampler = false;
}

//--------------------------------------------------------------
RenderTarget::~RenderTarget()
{

}

//--------------------------------------------------------------
void RenderTarget::build(int w, int h, int count, bool hasDepth)
{
    _useFaceSampler = false;
    _targets.resize(count);
    for(int i=0;i<count;++i)
    {
        _targets[i] = ImageSampler::create();
        _targets[i]->setInternalSrc(w,h,4);
    }

    _hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void RenderTarget::build(const std::vector<ImageSampler::Ptr>& textures, bool hasDepth)
{
    _useFaceSampler = false;
    _targets = textures;
    _hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void RenderTarget::build(const FaceSampler& face, bool hasDepth)
{
    _faceTarget = face;
    _useFaceSampler = true;
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
RenderTarget::FaceSampler RenderTarget::getFace()
{
    return _faceTarget;
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
bool RenderTarget::useFaceSampler() const
{
    return _useFaceSampler;
}

//--------------------------------------------------------------
void RenderTarget::change()
{
    _hasChanged = true;
}

//--------------------------------------------------------------
void RenderTarget::update()
{
    /*if(SceneRenderer::s_interface != nullptr
        && _hasChanged)
    {
        for(int i=0;i<(int)_targets.size();++i)
        {
            ImageSampler::Ptr& img = _targets[i];
            SceneRenderer::s_interface->bringBack(img);
        }
        _hasChanged = false;
    }*/

}


IMPGEARS_END
