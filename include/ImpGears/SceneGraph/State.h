#ifndef IMP_STATE_H
#define IMP_STATE_H

#include <Core/Object.h>
#include <Core/Vec3.h>
#include <Core/Matrix4.h>
#include <Core/Vec4.h>
#include <SceneGraph/Uniform.h>
#include <SceneGraph/Target.h>

#include <map>

IMPGEARS_BEGIN

class IMP_API State : public Object
{
public:
	
	enum CloneOpt
	{
		CloneOpt_None,
		CloneOpt_All,
		CloneOpt_IfChanged
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
	
	void clone(const State::Ptr& other, CloneOpt opt = CloneOpt_All);

	void setFaceCullingMode(FaceCullingMode faceCullingMode){_faceCullingMode = faceCullingMode;_faceCullingChanged=true;}
	FaceCullingMode getFaceCullingMode() const{return _faceCullingMode;}

	void setBlendMode(BlendMode blendMode){_blendMode = blendMode; _blendModeChanged=true;}
	BlendMode getBlendMode() const{return _blendMode;}
	
	void apply() const;

	void setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue);
	void setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue);

	void setProjectionMatrix(const Matrix4& projection){_projection = projection; _projectionChanged=true;}
	const Matrix4& getProjectionMatrix() const {return _projection;}
	
	void setDepthTest(bool depthTest);
	
	void setViewport(float x, float y, float width, float height);
	void setViewport(const Vec4& viewport);
	
	void setLineWidth(float lw);
	
	void setTarget(Target::Ptr target);
	void setShader(ShaderDsc::Ptr shader);
	void setUniforms(const std::map<std::string,Uniform::Ptr>& uniforms);
	void clearUniforms();
	void setUniform(const Uniform::Ptr& uniform);
	
	Target::Ptr getTarget() { return _target; }
	ShaderDsc::Ptr getShader() { return _shader; }

protected:
private:

	Matrix4 _projection;
	Vec4 _viewport;
	FaceCullingMode _faceCullingMode;
	BlendMode _blendMode;
	float _lineWidth;
	bool _depthTest;
	
	Target::Ptr _target;
	ShaderDsc::Ptr _shader;
	std::map<std::string,Uniform::Ptr> _uniforms;
	
	bool _projectionChanged;
	bool _viewportChanged;
	bool _faceCullingChanged;
	bool _blendModeChanged;
	bool _lineWidthChanged;
	bool _depthTestChanged;
	bool _targetChanged;
	bool _shaderChanged;
	bool _uniformsChanged;
};

IMPGEARS_END

#endif // IMP_STATE_H
