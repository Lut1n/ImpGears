#ifndef IMP_LIGHT_H
#define IMP_LIGHT_H

#include <SceneGraph/Node.h>

IMPGEARS_BEGIN

class IMP_API LightNode : public Node
{
public:
	
	Meta_Class(LightNode)
	
	LightNode();
	virtual ~LightNode();

private:

	Vec3 _color;
	float _power;
};

IMPGEARS_END

#endif // IMP_LIGHT_H
