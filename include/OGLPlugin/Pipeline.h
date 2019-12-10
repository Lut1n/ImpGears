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

    FrameOperation();
    virtual ~FrameOperation();

    Graph::Ptr buildQuadGraph(const std::string& debug_name,
                              const std::string& glsl_code,
                              Vec4 viewport = Vec4(0.0,0.0,1024.0,1024.0)) const;

    void setInput(ImageSampler::Ptr sampler, int id);
    void setOutput(ImageSampler::Ptr sampler, int id);
    ImageSampler::Ptr getInput(int id);
    ImageSampler::Ptr getOutput(int id);
    void setEnvCubeMap(CubeMapSampler::Ptr& cubemap);
    void setShaCubeMap(CubeMapSampler::Ptr& cubemap);
    void setCamera(const Camera* camera);
    void setLight(const LightNode* light);

    virtual void setup() = 0;
    virtual void apply(GlRenderer* renderer) = 0;

protected:
    FrameBuf _input;
    FrameBuf _output;

    CubeMapSampler::Ptr _shadows;
    CubeMapSampler::Ptr _environment;
    const Camera* _camera;
    const LightNode* _light;
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

    void setOperation(const FrameOperation::Ptr& op, int index);
    void bind(int dstOpId, int srcOpId, int dstInputId, int srcOuputId = 0);

    ImageSampler::Ptr getOutputFrame( int opIndex, int outputIndex );

    void setupOperations();
    void buildDependencies();
    void deduceOrder();

    void prepare(const Camera* camera,
                 const LightNode* light,
                 CubeMapSampler::Ptr& shadowCubemap,
                 CubeMapSampler::Ptr& envCubemap);
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
