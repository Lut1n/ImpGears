#ifndef IMP_LIGHT_MODEL_H
#define IMP_LIGHT_MODEL_H

#include <Graphics/GeometryRenderer.h>
#include <SceneGraph/RenderPlugin.h>

IMPGEARS_BEGIN

struct IMP_API LightModel : public Object
{
	Meta_Class(LightModel);
	
	enum Texturing
	{
		Texturing_PlainColor = 0,
		Texturing_Samplers,
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
	FragCallback::Ptr _fragCb_texturing;
	FragCallback::Ptr _fragCb_lighting;
	FragCallback::Ptr _fragCb_mrt;
	
	std::string _vertCode;
	std::string _fragCode_texturing;
	std::string _fragCode_lighting;
	std::string _fragCode_mrt;
	
	RenderPlugin::Data::Ptr _d;
};

IMPGEARS_END

#endif // IMP_LIGHT_MODEL_H
