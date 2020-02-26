#ifndef IMP_GL_PIPELINE_H
#define IMP_GL_PIPELINE_H

#include <Renderer/RenderTarget.h>
#include <Plugins/RenderPlugin.h>
#include <SceneGraph/ClearNode.h>
#include <SceneGraph/GeoNode.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/LightNode.h>
#include <OGLPlugin/RenderToCubeMap.h>

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

    virtual void setup(const Vec4& vp) = 0;
    virtual void apply(GlRenderer* renderer, bool skip = false) = 0;

    void clearOutput();

protected:
    FrameBuf _input;
    FrameBuf _output;

    CubeMapSampler::Ptr _shadows;
    CubeMapSampler::Ptr _environment;
    const Camera* _camera;
    const LightNode* _light;

    static std::string s_glsl_copy;
    static std::string s_glsl_fill;
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

    FrameOperation::Ptr getOperation(int index)
    {
        if(index>=0 && index<(int)_operations.size())
            return _operations[index];
        return nullptr;
    }

    void setActive(int opIndex, bool activate);

    void bindExternal(int dstOpId, const RenderTarget::Ptr& rt, int dstInputId, int srcOuputId = 0);
    void bind(int dstOpId, int srcOpId, int dstInputId, int srcOuputId = 0);
    void unbind(int dstOpId, int dstInputId);

    ImageSampler::Ptr getOutputFrame( int opIndex, int outputIndex );

    void setupOperations(const Vec4& vp);
    void buildDependencies();
    void deduceOrder();

    void prepare(const Camera* camera,
                 const LightNode* light,
                 CubeMapSampler::Ptr& shadowCubemap,
                 CubeMapSampler::Ptr& envCubemap);
    void run(int targetOpIndex);

    void setViewport(const Vec4& vp);

protected:

    GlRenderer* _renderer;
    std::vector<FrameOperation::Ptr> _operations;
    std::vector< std::vector<bool> > _dependencies;
    std::vector< bool > _activated;
    std::vector<int> _orderedOp;
    std::vector<Binding> _bindings;
    bool _dirty;
    FrameBuf _frames;
    Vec4 _viewport;
};

IMPGEARS_END

#endif // IMP_GL_RENDERER_H
