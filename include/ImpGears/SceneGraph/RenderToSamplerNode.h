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
    
    ImageSampler::Ptr texture() {return _texture;}
    void setTexture(ImageSampler::Ptr tx){_texture=tx;}
    
    Graph::Ptr scene() {return _scene;}
    void setScene(Graph::Ptr s){_scene=s;}
    
    bool ready() const {return _rendered;}
    void makeReady() {_rendered=true;}
    void makeDirty() {_rendered=false;}
    

protected:

    CubeMapSampler::Ptr _cubemap;
    ImageSampler::Ptr _texture;
    
    Graph::Ptr _scene;
    bool _rendered;

    
};

IMPGEARS_END

#endif // IMP_RENDERTOSAMPLER_NODE_H
