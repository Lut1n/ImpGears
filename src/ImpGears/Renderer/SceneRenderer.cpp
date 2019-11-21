#include <Renderer/SceneRenderer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SceneRenderer::SceneRenderer()
{
    _visitor = RenderVisitor::create();
    _direct = true;
}

//--------------------------------------------------------------
SceneRenderer::~SceneRenderer()
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
void SceneRenderer::setTarget(Image::Ptr& target, int id)
{
    if(id >= (int)_targets.size()) _targets.resize(id+1);
    _targets[id] = target;
}

//---------------------------------------------------------------
Image::Ptr SceneRenderer::getTarget(bool dlFromGPU, int id)
{
    if(id <0 || id>=(int)_targets.size()) return nullptr;

    Image::Ptr target = _targets[id];
    if(dlFromGPU) { /* dl image here */ }

    return target;
}

IMPGEARS_END
