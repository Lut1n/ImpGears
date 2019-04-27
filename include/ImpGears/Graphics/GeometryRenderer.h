#include <Core/Matrix4.h>
#include <Geometry/Geometry.h>
#include <Graphics/Image.h>
#include <Graphics/Rasterizer.h>

IMPGEARS_BEGIN

class IMP_API GeometryRenderer : public Object
{
public:
	
	Meta_Class(GeometryRenderer)
	
	struct VertCallback : public Object
	{
		Meta_Class(VertCallback)
		virtual void exec(const Vec3& vert, const Vec3& col, const Vec3& normal, const Vec3& tex, const CnstUniforms& cu, Uniforms& uniforms, const GeometryRenderer* renderer) = 0;
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
	
	void setProj(const Matrix4& proj);
	void setView(const Matrix4& view);
	void setModel(const Matrix4& model);
	void setViewport(const Vec4& viewport);
	void setCullMode(Cull mode);
	void setUniforms(const CnstUniforms& cu);
	void setDefaultVertCallback();
	void setDefaultFragCallback();
	void setVertCallback(const VertCallback::Ptr& callback);
	void setFragCallback(const FragCallback::Ptr& callback);
	
	const Matrix4& getProj() const;
	const Matrix4& getView() const;
	const Matrix4& getModel() const;
	const Vec4& getViewport() const;
	
protected:

	std::vector<Image::Ptr> _targets;
	std::vector<Vec4> _clearColors;
	CnstUniforms _uniforms;
	
	VertCallback::Ptr _vertCallback;
	FragCallback::Ptr _fragCallback;
	
	Rasterizer _rasterizer;
	
	// TODO : put model, view, projection into uniforms
	// Uniforms _uniforms;
	
	Matrix4 _proj, _view, _model;
	Vec4 _viewport;
	Cull _cull;
};

IMPGEARS_END
