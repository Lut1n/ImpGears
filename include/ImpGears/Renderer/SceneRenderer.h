#ifndef IMP_SCENE_RENDERER_H
#define IMP_SCENE_RENDERER_H

#include <Core/Object.h>

#include <Renderer/RenderVisitor.h>
#include <Graphics/Image.h>

#include <SceneGraph/Graph.h>

IMPGEARS_BEGIN

class IMP_API SceneRenderer : public Object
{
public:

    Meta_Class(SceneRenderer)

    SceneRenderer();
    virtual ~SceneRenderer();

    virtual void render(const Graph::Ptr& scene) = 0;

    void setDirect(bool direct) {_direct = direct;}
    bool isDirect() const {return _direct;}

    void setTarget(Image::Ptr& target, int id = 0);
    virtual Image::Ptr getTarget(bool dlFromGPU = false, int id = 0);

    LightNode* closest(Node* node, const std::vector<LightNode*>& ls);

protected:

    bool _direct;
    std::vector<Image::Ptr> _targets;
    RenderVisitor::Ptr _visitor;
};

IMPGEARS_END

#endif // IMP_SCENE_RENDERER_H
