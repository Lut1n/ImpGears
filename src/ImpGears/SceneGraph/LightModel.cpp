#include <SceneGraph/LightModel.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
LightModel::LightModel(Model model)
	: _model(model)
{
}

//--------------------------------------------------------------
LightModel::~LightModel()
{
}

//--------------------------------------------------------------
void LightModel::setModel(Model model)
{
	_model = model;
}

//--------------------------------------------------------------
LightModel::Model LightModel::getModel() const
{
	return _model;
}

IMPGEARS_END
