#ifndef IMP_SCENE_RENDERER_H
#define IMP_SCENE_RENDERER_H

#include <Core/Object.h>

#include <Renderer/RenderVisitor.h>
#include <Renderer/RenderTarget.h>
#include <Graphics/Image.h>

#include <SceneGraph/Graph.h>

IMPGEARS_BEGIN

class IMP_API SceneRenderer : public Object
{
public:

    Meta_Class(SceneRenderer)

    enum Feature
    {
        Feature_Bloom = 0,
        Feature_Shadow,
        Feature_Environment,
        Feature_SSAO,

        Feature_Count
    };

    enum RenderFrame
    {
        RenderFrame_Default,
        RenderFrame_Color,
        RenderFrame_ShadowMap,
        RenderFrame_Environment,
        RenderFrame_Lighting,
        RenderFrame_Depth,
        RenderFrame_Emissive,
        RenderFrame_Bloom,
        RenderFrame_Normals,
        RenderFrame_Reflectivity,
    };

    SceneRenderer();
    virtual ~SceneRenderer();

    virtual void render(const Graph::Ptr& scene) = 0;

    void setDirect(bool direct) {_direct = direct;}
    bool isDirect() const {return _direct;}

    void setTargets(RenderTarget::Ptr targets);
    // void setTarget(Image::Ptr& target, int id = 0);
    virtual Image::Ptr getTarget(bool dlFromGPU = false, int id = 0);

    LightNode* closest(Node* node, const std::vector<LightNode*>& ls);

    void enableFeature(Feature id, bool enable);
    bool isFeatureEnabled(Feature id) const;

    void setOutputFrame(RenderFrame rf){ _renderFrame = rf; }
    RenderFrame getOutputFrame() const { return _renderFrame; }

protected:

    bool _direct;
    bool _enabledFeatures[Feature_Count];
    // std::vector<Image::Ptr> _targets;
    RenderTarget::Ptr _targets;
    RenderVisitor::Ptr _visitor;

    RenderFrame _renderFrame;
};

IMPGEARS_END

#endif // IMP_SCENE_RENDERER_H
