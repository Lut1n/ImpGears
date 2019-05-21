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
	virtual ~Material();

private:

	ImageSampler::Ptr _baseColor;
	float _shininess;
};

IMPGEARS_END

#endif // IMP_MATERIAL_H
