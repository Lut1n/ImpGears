#include <SceneGraph/Material.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
ImageSampler::Ptr Material::s_default_baseColor;
ImageSampler::Ptr Material::s_default_emissive;
ImageSampler::Ptr Material::s_default_normalmap;
ImageSampler::Ptr Material::s_default_reflectivity;

//--------------------------------------------------------------
Material::Material()
    : _shininess(32.0)
    , _color(1.0)
{
    if(s_default_baseColor == nullptr)
    {
        s_default_baseColor = ImageSampler::create(8,8,4,Vec4(1.0));
        s_default_emissive = ImageSampler::create(8,8,4,Vec4(0.0,0.0,0.0,1.0));
        s_default_normalmap = ImageSampler::create(8,8,3,Vec3(0.5,0.5,1.0));
        s_default_reflectivity = ImageSampler::create(8,8,3,Vec3(0.0));
    }

    _baseColor = s_default_baseColor;
    _emissive = s_default_emissive;
    _normalmap = s_default_normalmap;
    _reflectivity = s_default_reflectivity;
}

//--------------------------------------------------------------
Material::Material(const Vec4& color, float shininess)
    : _shininess(shininess)
    , _color(color)
{
    if(s_default_baseColor == nullptr)
    {
        s_default_baseColor = ImageSampler::create(8,8,4,Vec4(1.0));
        s_default_emissive = ImageSampler::create(8,8,4,Vec4(0.0,0.0,0.0,1.0));
        s_default_normalmap = ImageSampler::create(8,8,3,Vec3(0.5,0.5,1.0));
        s_default_reflectivity = ImageSampler::create(8,8,3,Vec3(0.0));
    }

    _baseColor = s_default_baseColor;
    _emissive = s_default_emissive;
    _normalmap = s_default_normalmap;
    _reflectivity = s_default_reflectivity;
}

//--------------------------------------------------------------
Material::Material(ImageSampler::Ptr& baseColor,
                   ImageSampler::Ptr& emissive,
                   ImageSampler::Ptr& normalmap,
                   ImageSampler::Ptr& reflectivity,
                   float shininess)
    : _shininess(shininess)
    , _color(1.0)
{
    _baseColor = baseColor;
    _emissive = emissive;
    _normalmap = normalmap;
    _reflectivity = reflectivity;
}

//--------------------------------------------------------------
Material::~Material()
{
}

IMPGEARS_END
