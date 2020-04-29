#include <ImpGears/SceneGraph/RenderPass.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
RenderPass::RenderPass(PassFlag rp, PostFXFlag fx)
    : _passFlags(rp)
    , _postFXFlags(fx)
{

}

//--------------------------------------------------------------
RenderPass::~RenderPass()
{

}

//--------------------------------------------------------------
void RenderPass::setPassFlags(PassFlag rp)
{
    _passFlags = rp;
}

//--------------------------------------------------------------
RenderPass::PassFlag RenderPass::getPassFlags() const
{
    return _passFlags;
}

//--------------------------------------------------------------
void RenderPass::setPostFXFlags(PostFXFlag fx)
{
    _postFXFlags = fx;
}

//--------------------------------------------------------------
RenderPass::PostFXFlag RenderPass::getPostFXFlags() const
{
    return _postFXFlags;
}

//--------------------------------------------------------------
void RenderPass::enablePass( PassFlag rp )
{
    _passFlags = static_cast<PassFlag>(_passFlags | rp);
}

//--------------------------------------------------------------
void RenderPass::disablePass( PassFlag rp )
{
    _passFlags = static_cast<PassFlag>(_passFlags ^ rp);
}

//--------------------------------------------------------------
void RenderPass::enableFX( PostFXFlag fx )
{
    _postFXFlags = static_cast<PostFXFlag>(_postFXFlags | fx);
}

//--------------------------------------------------------------
void RenderPass::disableFX( PostFXFlag fx )
{
    _postFXFlags = static_cast<PostFXFlag>(_postFXFlags ^ fx);
}

//--------------------------------------------------------------
bool RenderPass::isPassEnabled(PassFlag rp) const
{
    return (_passFlags & rp) == rp;
}

//--------------------------------------------------------------
bool RenderPass::isFXEnabled(PostFXFlag fx) const
{
    return (_postFXFlags & fx) == fx;
}

IMPGEARS_END
