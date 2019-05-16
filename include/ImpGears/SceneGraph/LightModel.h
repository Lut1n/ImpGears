#ifndef IMP_LIGHT_MODEL_H
#define IMP_LIGHT_MODEL_H

#include <Graphics/GeometryRenderer.h>
#include <SceneGraph/RenderPlugin.h>

IMPGEARS_BEGIN

struct IMP_API LightModel : public Object
{
	Meta_Class(LightModel);
	
	struct TexturingCallback
	{
		Meta_Class(TexturingCallback)
		
		virtual Vec3 textureColor(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings) = 0;
		virtual Vec3 textureNormal(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings) = 0;
		virtual Vec3 textureEmissive(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings) = 0;
	};
	
	struct LightingCallback
	{
		Meta_Class(LightingCallback)
		
		virtual void applyLighting(const UniformMap& uniforms, Varyings& varyings, TexturingCallback::Ptr& texturing, std::vector<Vec3>& outColor) = 0;
	};
	
	struct MRTCallback
	{
		Meta_Class(MRTCallback)
		
		virtual void applyMRT(ImageBuf& targets, const Vec3& pt, const std::vector<Vec3>& outColor) = 0;
	};
	
	struct AbstractFrag : public FragCallback
	{
		
		Meta_Class(AbstractFrag)
		virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings);
		
		TexturingCallback::Ptr _texturing;
		LightingCallback::Ptr _lighting;
		MRTCallback::Ptr _mrt;
	};
	
	enum Texturing
	{
		Texturing_PlainColor = 0,
		Texturing_Samplers_CN,
		Texturing_Samplers_CNE,
		Texturing_Customized,
	};
	
	enum Lighting
	{
		Lighting_None = 0,
		Lighting_Phong,
		Lighting_Customized,
	};
	
	enum MRT
	{
		MRT_1_Col = 0,
		MRT_2_Col_Emi,
	};
	
	LightModel(Lighting l = Lighting_None, Texturing t = Texturing_PlainColor, MRT mrt = MRT_1_Col);
	virtual ~LightModel();
	
	void setTexturing(Texturing t);
	Texturing getTexturing() const;
	
	void setLighting(Lighting l);
	Lighting getLighting() const;
	
	void setMRT(MRT mrt);
	MRT getMRT() const;
	
	Lighting _lighting;
	Texturing _texturing;
	MRT _mrt;
	
	GeometryRenderer::VertCallback::Ptr _vertCb;
	TexturingCallback::Ptr _texturingCb;
	LightingCallback::Ptr _lightingCb;
	MRTCallback::Ptr _mrtCb;
	
	std::string _vertCode;
	std::string _fragCode_texturing;
	std::string _fragCode_lighting;
	std::string _fragCode_mrt;
	
	RenderPlugin::Data::Ptr _d;
};

IMPGEARS_END

#endif // IMP_LIGHT_MODEL_H
