#ifndef IMP_RENDERTOSAMPLER_NODE_H
#define IMP_RENDERTOSAMPLER_NODE_H

#include <Graphics/Sampler.h>
#include <SceneGraph/Node.h>
#include <SceneGraph/Graph.h>

IMPGEARS_BEGIN

class IMP_API RenderToSamplerNode : public Node
{
public:

    Meta_Class(RenderToSamplerNode)

    RenderToSamplerNode();
    virtual ~RenderToSamplerNode();

    CubeMapSampler::Ptr cubemap() {return _cubemap;}
    void setCubeMap(CubeMapSampler::Ptr cm){_cubemap=cm;}
    
    Graph::Ptr scene() {return _scene;}
    void setScene(Graph::Ptr s){_scene=s;}
    
    bool ready() const {return _rendered;}
    void makeReady() {_rendered=true;}
    

protected:

    CubeMapSampler::Ptr _cubemap;
    Graph::Ptr _scene;
    bool _rendered;

    
};

IMPGEARS_END

#endif // IMP_RENDERTOSAMPLER_NODE_H
