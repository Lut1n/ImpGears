#ifndef IMP_GEOMETRY_BUILDER_H
#define IMP_GEOMETRY_BUILDER_H

#include <ImpGears/Geometry/Geometry.h>
#include <ImpGears/Geometry/Export.h>

IMPGEARS_BEGIN

class IG_GEO_API GeometryBuilder : public Object
{
public:

    Meta_Class(GeometryBuilder)

    GeometryBuilder();
    virtual ~GeometryBuilder();

    static Geometry::Ptr createCapsule(int sub = 16, float r = 1.0, float dlen = 1.0);

    static Geometry::Ptr createCrystal(int sub = 16, float r = 1.0, float dlen = 1.0);

    static Geometry::Ptr createCylinder(int sub = 16, float r = 1.0, float dlen = 1.0);

    static Geometry::Ptr createTorus(int sub = 16, float mr = 1.0, float r = 0.5);

    static Geometry::Ptr createCube(float s = 1.0);

    static Geometry::Ptr createSphere(int sub = 16, float r = 0.5);

    static void cylindricNormalInterpo(Geometry::Ptr& geo);

protected:
};

IMPGEARS_END

#endif
