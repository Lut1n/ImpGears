#include <SceneGraph/LightModel.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
LightModel::LightModel(Lighting l, Texturing t, MRT mrt)
	: _lighting(l)
	, _texturing(t)
	, _mrt(mrt)
{
}

//--------------------------------------------------------------
LightModel::~LightModel()
{
}

//--------------------------------------------------------------
void LightModel::setTexturing(Texturing t)
{
	_texturing = t;
}

//--------------------------------------------------------------
LightModel::Texturing LightModel::getTexturing() const
{
	return _texturing;
}

//--------------------------------------------------------------
void LightModel::setLighting(Lighting l)
{
	_lighting = l;
}

//--------------------------------------------------------------
LightModel::Lighting LightModel::getLighting() const
{
	return _lighting;
}

//--------------------------------------------------------------
void LightModel::setMRT(MRT mrt)
{
	_mrt = mrt;
}

//--------------------------------------------------------------
LightModel::MRT LightModel::getMRT() const
{
	return _mrt;
}

IMPGEARS_END
