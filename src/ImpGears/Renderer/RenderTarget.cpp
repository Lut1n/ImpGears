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
void RenderTarget::build(int w, int h, int count, bool hasDepth, bool msaa)
{
    _useFaceSampler = false;
    _targets.resize(count);
    _clearColors.resize( count, Vec4(0.0) );
    for(int i=0;i<count;++i)
    {
        _targets[i] = ImageSampler::create();
        _targets[i]->setInternalSrc(w,h,4);
        _targets[i]->setMSAA(msaa);
    }

    _hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void RenderTarget::build(const std::vector<ImageSampler::Ptr>& textures, bool hasDepth)
{
    _useFaceSampler = false;
    _targets = textures;
    _clearColors.resize( textures.size(), Vec4(0.0) );
    _hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void RenderTarget::build(const FaceSampler& face, bool hasDepth)
{
    _faceTarget = face;
    _clearColors.resize( 1, Vec4(0.0) );
    _useFaceSampler = true;
    _hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void RenderTarget::setClearColors(const std::vector<Vec4>& clearColors)
{
    _clearColors = clearColors;
}

//--------------------------------------------------------------
void RenderTarget::setClearColor(int index, const Vec4& clearColor)
{
    _clearColors[index] = clearColor;
}

//--------------------------------------------------------------
const std::vector<Vec4>& RenderTarget::getClearColors() const
{
    return _clearColors;
}

//--------------------------------------------------------------
const Vec4& RenderTarget::getClearColor(int index) const
{
    return _clearColors[index];
}

//--------------------------------------------------------------
void RenderTarget::clearTargets()
{
    if(_useFaceSampler)
    {
        CubeMapSampler::Ptr sampler = _faceTarget.first;
        int faceID = _faceTarget.second;
        sampler->getSource()[faceID]->fill(_clearColors[0]*255.0);
    }
    else
    {
        for(int i=0;i<count();++i)
            _targets[i]->getSource()->fill(_clearColors[i]*255.0);
    }
}

//--------------------------------------------------------------
void RenderTarget::destroy()
{
    _targets.clear();
    _hasDepthBuffer = false;
}

//--------------------------------------------------------------
ImageSampler::Ptr RenderTarget::get(int index)
{
    update();
    return _targets[index];
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
    if(_useFaceSampler)
    {
        CubeMapSampler::Ptr sampler = _faceTarget.first;
        if(sampler) sampler->_needUpdate = true;
    }
    else
    {
        for(int i=0;i<count();++i)
        {
            if(_targets[i]) _targets[i]->_needUpdate = true;
        }
    }
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
