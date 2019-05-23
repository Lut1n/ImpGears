#include <SceneGraph/ReflexionModel.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
void ReflexionModel::AbstractFrag::exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings)
{
	std::vector<Vec3> outColor(10);
	if( _lighting )_lighting->applyLighting( uniforms,varyings, _texturing, outColor );
	if( _mrt ) _mrt->applyMRT( targets, pt, outColor );
}

//--------------------------------------------------------------
ReflexionModel::ReflexionModel(Lighting l, Texturing t, MRT mrt)
	: _lighting(l)
	, _texturing(t)
	, _mrt(mrt)
{
	_d = -1;
}

//--------------------------------------------------------------
ReflexionModel::~ReflexionModel()
{
}

//--------------------------------------------------------------
void ReflexionModel::setTexturing(Texturing t)
{
	_texturing = t;
}

//--------------------------------------------------------------
ReflexionModel::Texturing ReflexionModel::getTexturing() const
{
	return _texturing;
}

//--------------------------------------------------------------
void ReflexionModel::setLighting(Lighting l)
{
	_lighting = l;
}

//--------------------------------------------------------------
ReflexionModel::Lighting ReflexionModel::getLighting() const
{
	return _lighting;
}

//--------------------------------------------------------------
void ReflexionModel::setMRT(MRT mrt)
{
	_mrt = mrt;
}

//--------------------------------------------------------------
ReflexionModel::MRT ReflexionModel::getMRT() const
{
	return _mrt;
}

IMPGEARS_END
