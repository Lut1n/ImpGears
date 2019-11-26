#ifndef IMP_GEOMETRY_RENDERER_H
#define IMP_GEOMETRY_RENDERER_H


#include <Core/Matrix4.h>
#include <Geometry/Geometry.h>
#include <Graphics/FloatImage.h>
#include <Graphics/Image.h>
#include <Graphics/Rasterizer.h>

IMPGEARS_BEGIN

class IMP_API GeometryRenderer : public Object
{
public:

    Meta_Class(GeometryRenderer)

    struct VertCallback : public VertexOperation
    {
        Meta_Class(VertCallback)

         const UniformMap* _uniforms;
        Varyings* _varyings;

        virtual void apply(const Vec3& vertex, Attributes& att) = 0;
        void exec(const Vec3& vert, Attributes& att, const UniformMap& uniforms, Varyings& varyings)
        {
            _uniforms = &uniforms;
            _varyings = &varyings;
            apply(vert,att);
        }
    };

    enum Cull
    {
        Cull_None,
        Cull_Back,
        Cull_Front
    };

    GeometryRenderer();
    virtual ~GeometryRenderer();

    void init();
    void clearTargets();
    void render(const Geometry& geo);

    void setTarget(int id, Image::Ptr& target, Vec4 clearValue = Vec4(0.0));
    void setClearColor(int targetId, const Vec4& clearValue);

    Image::Ptr getTarget(int id);
    const Image::Ptr getTarget(int id) const;
    const Vec4& getClearColor(int id);
    int getTargetCount() const;

    void setProj(const Matrix4& proj);
    void setView(const Matrix4& view);
    void setModel(const Matrix4& model);
    void setViewport(const Vec4& viewport);
    void setCullMode(Cull mode);
    void setUniforms(const UniformMap& cu);
    void setUniform(const Uniform::Ptr& uniform);
    void setDefaultVertCallback();
    void setDefaultFragCallback();
    void setVertCallback(const VertCallback::Ptr& callback);
    void setFragCallback(const FragCallback::Ptr& callback);

    void enableDepthTest(bool b);

    const Matrix4& getProj() const;
    const Matrix4& getView() const;
    const Matrix4& getModel() const;
    const Vec4& getViewport() const;

protected:

    std::map<int,Image::Ptr> _targets;
    std::map<int,Vec4> _clearColors;
    UniformMap _uniforms;

    VertCallback::Ptr _vertCallback;
    FragCallback::Ptr _fragCallback;

    Rasterizer _rasterizer;

    Matrix4 _proj, _view, _model;
    Vec4 _viewport;
    Cull _cull;

    FloatImage::Ptr _depthBuffer;
    bool _depthTestEnabled;
};

IMPGEARS_END

#endif // IMP_GEOMETRY_RENDERER_H
