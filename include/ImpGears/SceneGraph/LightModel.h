#ifndef IMP_LIGHT_MODEL_H
#define IMP_LIGHT_MODEL_H

#include <Graphics/GeometryRenderer.h>
#include <SceneGraph/RenderPlugin.h>

IMPGEARS_BEGIN

struct IMP_API LightModel : public Object
{
	Meta_Class(LightModel);
	
	enum Model
	{
		Model_PlainColor = 0,
		Model_Phong_NoTex,
		Model_Phong,
		Model_Phong_Emissive,
		Model_Customized,
	};
	
	LightModel(Model model = Model_PlainColor);
	virtual ~LightModel();
	
	void setModel(Model model);
	Model getModel() const;
	
	Model _model;
	
	GeometryRenderer::VertCallback::Ptr vertCb;
	FragCallback::Ptr fragCb;
	
	std::string vertCode;
	std::string fragCode;
	
	RenderPlugin::Data::Ptr _d;
};

IMPGEARS_END

#endif // IMP_LIGHT_MODEL_H
