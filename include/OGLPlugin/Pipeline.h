#ifndef IMP_GL_PIPELINE_H
#define IMP_GL_PIPELINE_H

#include <Renderer/RenderTarget.h>
#include <Plugins/RenderPlugin.h>
#include <SceneGraph/ClearNode.h>
#include <SceneGraph/GeoNode.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/LightNode.h>

IMPGEARS_BEGIN

class GlRenderer;

class IMP_API FrameOperation
{
public:

    Meta_Class(FrameOperation)

    using FrameBuf = std::vector<ImageSampler::Ptr>;

    FrameOperation(/*SceneRenderer::RenderFrame renderFrame*/);
    virtual ~FrameOperation();

    void setup(FrameBuf& input, FrameBuf& output, bool needCamera, bool needLight, bool needCubeMap);
    virtual void setInput(ImageSampler::Ptr sampler, int id = 0);
    virtual void setOutput(ImageSampler::Ptr sampler, int id = 0);

    Graph::Ptr buildQuadGraph(const std::string& debug_name,
                              const std::string& glsl_code,
                              Vec4 viewport = Vec4(0.0,0.0,1024.0,1024.0)) const;

    virtual void setCubeMap(CubeMapSampler::Ptr cubemap) = 0;
    virtual void setCamera(Camera::Ptr& cameraPos) = 0;
    virtual void setLight(LightNode::Ptr& lightPos) = 0;

    virtual void apply(GlRenderer* renderer) = 0;

protected:

    bool _needCamera;
    bool _needLight;
    bool _needCubeMap;

    FrameBuf _input;
    FrameBuf _output;
};

class IMP_API Pipeline
{
public:

    struct Binding
    {
        Binding();
        Binding(int opA, int opB, int opA_output, int opB_input);

        bool operator==(const Binding& that) const;

        int _opA;
        int _opB;

        int _opA_output;
        int _opB_input;
    };

    using FrameBuf = std::vector<ImageSampler::Ptr>;

    Meta_Class(Pipeline)

    Pipeline(GlRenderer* renderer);
    virtual ~Pipeline();

    void setOperation(FrameOperation::Ptr& op, int index);
    void setBinding(Binding& binding);

    void setupOperations();
    void buildDependencies();
    void deduceOrder();

    void prepare();
    void run();

protected:

    GlRenderer* _renderer;
    std::vector<FrameOperation::Ptr> _operations;
    std::vector< std::vector<bool> > _dependencies;
    std::vector<int> _orderedOp;
    std::vector<Binding> _bindings;
    bool _dirty;
    FrameBuf _frames;
};

IMPGEARS_END

#endif // IMP_GL_RENDERER_H
