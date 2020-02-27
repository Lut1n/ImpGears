#ifndef IMP_INSTANCED_GEOMETRY_H
#define IMP_INSTANCED_GEOMETRY_H

#include <Geometry/Geometry.h>
#include <Core/Matrix4.h>

IMPGEARS_BEGIN

class IMP_API InstancedGeometry : public Geometry
{
public:

    Meta_Class(InstancedGeometry)

    InstancedGeometry();

    void clone(Geometry::Ptr geometry);
    
    void setTransforms(const std::vector<Matrix4>& buf);
    void setTransformAt(int index, const Matrix4& mat);
    
    const std::vector<Matrix4>& getTransforms() const;
    
    bool hasChanged() const;
    void changesDone();
    
protected:

    std::vector<Matrix4> _transforms;
    bool _transformsChanged;
};


IMPGEARS_END

#endif // IMP_INSTANCED_GEOMETRY_H
