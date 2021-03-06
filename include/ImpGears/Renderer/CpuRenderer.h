#ifndef IMP_CPU_RENDERER_H
#define IMP_CPU_RENDERER_H

#include <ImpGears/Renderer/GeometryRenderer.h>
#include <ImpGears/Renderer/SceneRenderer.h>
#include <ImpGears/Renderer/RenderTarget.h>

#include <ImpGears/SceneGraph/ClearNode.h>
#include <ImpGears/SceneGraph/GeoNode.h>

IMPGEARS_BEGIN

class IMP_API CpuRenderer : public SceneRenderer
{
public:

    Meta_Class(CpuRenderer)

    CpuRenderer();
    virtual ~CpuRenderer();
    
    virtual void initialize();

    virtual void render(const Graph::Ptr& scene);

protected:

    // RenderTarget::Ptr _renderTargets;
    GeometryRenderer _geoRenderer;

    void applyState(const State::Ptr& state);
    void applyClear(ClearNode* clearNode);
    void drawGeometry(GeoNode* geoNode);
};

IMPGEARS_END

#endif // IMP_CPU_RENDERER_H
