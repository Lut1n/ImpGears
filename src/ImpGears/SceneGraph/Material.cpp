#include <SceneGraph/Material.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
ImageSampler::Ptr Material::s_default_baseColor;
ImageSampler::Ptr Material::s_default_emissive;
ImageSampler::Ptr Material::s_default_normalmap;

//--------------------------------------------------------------
Material::Material()
    : _shininess(4.0)
    , _color(1.0)
{
    if(s_default_baseColor == nullptr)
    {
        s_default_baseColor = ImageSampler::create(8,8,4,Vec4(1.0));
        s_default_emissive = ImageSampler::create(8,8,4,Vec4(0.0,0.0,0.0,1.0));
        s_default_normalmap = ImageSampler::create(8,8,3,Vec3(0.5,0.5,1.0));
    }

    _baseColor = s_default_baseColor;
    _emissive = s_default_emissive;
    _normalmap = s_default_normalmap;
}

//--------------------------------------------------------------
Material::Material(const Vec3& color, float shininess)
    : _shininess(shininess)
    , _color(color)
{
    if(s_default_baseColor == nullptr)
    {
        s_default_baseColor = ImageSampler::create(8,8,4,Vec4(1.0));
        s_default_emissive = ImageSampler::create(8,8,4,Vec4(0.0,0.0,0.0,1.0));
        s_default_normalmap = ImageSampler::create(8,8,3,Vec3(0.5,0.5,1.0));
    }

    _baseColor = s_default_baseColor;
    _emissive = s_default_emissive;
    _normalmap = s_default_normalmap;
}

//--------------------------------------------------------------
Material::Material(ImageSampler::Ptr& baseColor, ImageSampler::Ptr& emissive, ImageSampler::Ptr& normalmap, float shininess)
    : _shininess(shininess)
    , _color(1.0)
{
    _baseColor = baseColor;
    _emissive = emissive;
    _normalmap = normalmap;
}

//--------------------------------------------------------------
Material::~Material()
{
}

IMPGEARS_END
