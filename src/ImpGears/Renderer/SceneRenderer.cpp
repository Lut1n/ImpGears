#include <Renderer/SceneRenderer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneRenderer::SceneRenderer()
{
    _visitor = RenderVisitor::create();
    _direct = true;
    _renderFrame = RenderFrame_Default;
    _outputViewport = Vec4(0.0,0.0,512.0,512.0);
    _shadowResolution = 256;
    _environmentResolution = 128;
    _shadowSamples = 16;
    _ssaoSamples = 16;
    _lightpower = 200.0;
    _ambient = 0.1;
    _msaa = false;

    for(auto& f : _enabledFeatures) f=false;
}

//--------------------------------------------------------------
SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::initialize()
{
}

//---------------------------------------------------------------
LightNode* SceneRenderer::closest(Node* node, const std::vector<LightNode*>& ls)
{
    Matrix4 m = node->getModelMatrix();
    Vec3 wPos(m(3,0),m(3,1),m(3,2));

    LightNode* res = nullptr;
    float dist = -1;

    for(auto l : ls)
    {
        float d = (l->_worldPosition - wPos).length2();
        if(dist == -1 || d<dist) { res=l;dist=d; }
    }

    return res;
}

//---------------------------------------------------------------
void SceneRenderer::setTargets(RenderTarget::Ptr targets)
{
    _targets = targets;
}

//---------------------------------------------------------------
// void SceneRenderer::setTarget(Image::Ptr& target, int id)
// {
//     if(id >= (int)_targets.size()) _targets.resize(id+1);
//     _targets[id] = target;
// }

//---------------------------------------------------------------
Image::Ptr SceneRenderer::getTarget(bool dlFromGPU, int id)
{
    if(id <0 || id>=(int)_targets->count()) return nullptr;

    Image::Ptr target = _targets->get(id)->getSource();
    if(dlFromGPU) { /* dl image here */ }

    return target;
}

//---------------------------------------------------------------
void SceneRenderer::enableFeature(Feature id, bool enable)
{
    _enabledFeatures[id] = enable;
}

//---------------------------------------------------------------
bool SceneRenderer::isFeatureEnabled(Feature id) const
{
    return _enabledFeatures[id];
}

IMPGEARS_END
