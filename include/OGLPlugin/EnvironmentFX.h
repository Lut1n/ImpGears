#ifndef IMP_ENVIRONMENT_FX_H
#define IMP_ENVIRONMENT_FX_H

#include <Core/Object.h>

#include <SceneGraph/Graph.h>
#include <SceneGraph/ReflexionModel.h>
#include <Renderer/RenderTarget.h>
#include <Graphics/Sampler.h>
#include <SceneGraph/GeoNode.h>

#include <OGLPlugin/RenderToCubeMap.h>


IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API EnvironmentFX : public Object
{
public:

    Meta_Class(EnvironmentFX)

    EnvironmentFX();
    virtual ~EnvironmentFX();

    void setup();

    Graph::Ptr begin(GlRenderer* renderer, const Graph::Ptr& scene);

    void end(GlRenderer* renderer, const Graph::Ptr& scene);

protected:

    RenderToCubeMap::Ptr cubemapRenderer;
    Image::Ptr source;
    CubeMapSampler::Ptr sampler;
    ReflexionModel::Ptr _shader;
    GeoNode::Ptr node;
};

IMPGEARS_END

#endif // IMP_ENVIRONMENT_FX_H
