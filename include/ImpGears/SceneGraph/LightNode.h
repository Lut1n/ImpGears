#ifndef IMP_LIGHT_H
#define IMP_LIGHT_H

#include <ImpGears/SceneGraph/Node.h>

IMPGEARS_BEGIN

class IMP_API LightNode : public Node
{
public:

    Meta_Class(LightNode)

    LightNode();
    LightNode(const Vec3& color, float power);
    virtual ~LightNode();

    Vec3 _worldPosition;
    Vec3 _color;
    float _power;
};

IMPGEARS_END

#endif // IMP_LIGHT_H
