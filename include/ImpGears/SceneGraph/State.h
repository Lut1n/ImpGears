#ifndef IMP_STATE_H
#define IMP_STATE_H

#include <Core/Object.h>
#include <Core/Vec3.h>
#include <Core/Matrix4.h>
#include <Core/Vec4.h>
#include <Graphics/Uniform.h>
#include <SceneGraph/ReflexionModel.h>
#include <SceneGraph/RenderPass.h>

#include <map>

IMPGEARS_BEGIN

class IMP_API State : public Object
{
public:

    enum CloneOpt
    {
        CloneOpt_None,
        CloneOpt_Duplicate,
        CloneOpt_DuplicateChanged,
        CloneOpt_OverrideRef,
        CloneOpt_OverrideChangedRef
    };

    enum FaceCullingMode
    {
        FaceCullingMode_None = 0,
        FaceCullingMode_Back,
        FaceCullingMode_Front
    };

    enum BlendMode
    {
        BlendMode_None = 0,
        BlendMode_SrcAlphaBased
    };

    Meta_Class(State)

    State();
    State(const State& other);
    virtual ~State();

    const State& operator=(const State& other);

    void clone(const State::Ptr& other, CloneOpt opt = CloneOpt_Duplicate);

    void setFaceCullingMode(FaceCullingMode faceCullingMode){_faceCullingMode = faceCullingMode;_faceCullingChanged=true;}
    FaceCullingMode getFaceCullingMode() const{return _faceCullingMode;}

    void setBlendMode(BlendMode blendMode){_blendMode = blendMode; _blendModeChanged=true;}
    BlendMode getBlendMode() const{return _blendMode;}

    void setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue);
    void setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue);

    void setProjectionMatrix(const Matrix4& projection){_projection = projection; _projectionChanged=true;}
    const Matrix4& getProjectionMatrix() const {return _projection;}

    void setDepthTest(bool depthTest);
    bool getDepthTest() const {return _depthTest;}

    void setTransparent(bool transparent);
    bool getTransparent() const {return _transparent;}

    void setViewport(float x, float y, float width, float height);
    void setViewport(const Vec4& viewport);
    const Vec4 getViewport() const {return _viewport;}

    void setLineWidth(float lw);
    float getLineWidth() const {return _lineWidth;}

    void setReflexion(ReflexionModel::Ptr reflexion);
    void setRenderPass(RenderPass::Ptr renderPass);

    void cloneUniforms(const std::map<std::string,Uniform::Ptr>& uniforms,
                       const std::map<std::string,bool>& changedFlags,
                       CloneOpt opt = CloneOpt_Duplicate);

    void clearUniforms();
    void setUniform(Uniform::Ptr& uniform);

    void setUniform(const std::string& name, int i1);
    void setUniform(const std::string& name, float f1);
    void setUniform(const std::string& name, const Vec2& f2);
    void setUniform(const std::string& name, const Vec3& f3);
    void setUniform(const std::string& name, const Vec4& f4);
    void setUniform(const std::string& name, const Matrix3& mat3);
    void setUniform(const std::string& name, const Matrix4& mat4);
    void setUniform(const std::string& name, const ImageSampler::Ptr& sampler, int index);
    void setUniform(const std::string& name, const CubeMapSampler::Ptr& sampler, int index);
    const std::map<std::string,Uniform::Ptr>& getUniforms() const {return _uniforms;}
    std::map<std::string,Uniform::Ptr>& getUniforms() {return _uniforms;}


    ReflexionModel::Ptr getReflexion() { return _reflexion; }

    RenderPass::Ptr getRenderPass() { return _renderPass; }

protected:
private:

    Matrix4 _projection;
    Vec4 _viewport;
    FaceCullingMode _faceCullingMode;
    BlendMode _blendMode;
    float _lineWidth;
    bool _depthTest;
    bool _transparent;

    ReflexionModel::Ptr _reflexion;
    RenderPass::Ptr _renderPass;

    std::map<std::string,Uniform::Ptr> _uniforms;
    std::map<std::string,bool> _uniformChangedFlags;

    bool _projectionChanged;
    bool _viewportChanged;
    bool _faceCullingChanged;
    bool _blendModeChanged;
    bool _lineWidthChanged;
    bool _depthTestChanged;
    bool _transparentChanged;
    bool _reflexionChanged;
    bool _renderPassChanged;
    bool _uniformsChanged;
};

IMPGEARS_END

#endif // IMP_STATE_H
