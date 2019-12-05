#ifndef IMP_MATERIAL_H
#define IMP_MATERIAL_H

#include <SceneGraph/Node.h>
#include <Graphics/Sampler.h>

IMPGEARS_BEGIN

class IMP_API Material : public Object
{
public:

    Meta_Class(Material)

    Material();
    Material(const Vec3& color, float shininess);
    Material(ImageSampler::Ptr& baseColor, ImageSampler::Ptr& emissive, ImageSampler::Ptr& normalmap, float shininess = 4.0);
    virtual ~Material();

    ImageSampler::Ptr _baseColor;
    ImageSampler::Ptr _normalmap;
    ImageSampler::Ptr _emissive;

    float _shininess;
    Vec3 _color;

    static ImageSampler::Ptr s_default_baseColor;
    static ImageSampler::Ptr s_default_emissive;
    static ImageSampler::Ptr s_default_normalmap;
};

IMPGEARS_END

#endif // IMP_MATERIAL_H
