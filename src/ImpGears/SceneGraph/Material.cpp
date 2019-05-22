#include <SceneGraph/Material.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Material::Material()
	: _shininess(4.0)
	, _color(1.0)
{
}

//--------------------------------------------------------------
Material::Material(const Vec3& color, float shininess)
	: _shininess(shininess)
	, _color(color)
{
}

//--------------------------------------------------------------
Material::~Material()
{
}

IMPGEARS_END
