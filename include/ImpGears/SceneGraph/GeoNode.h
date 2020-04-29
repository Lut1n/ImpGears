#ifndef IMP_GEONODE_H
#define IMP_GEONODE_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Geometry/Polyhedron.h>
#include <ImpGears/Geometry/Geometry.h>
#include <ImpGears/SceneGraph/Node.h>
#include <ImpGears/SceneGraph/Material.h>
#include <ImpGears/Graphics/Uniform.h>

IMPGEARS_BEGIN

class IMP_API GeoNode : public Node
{
public:

    Meta_Class(GeoNode)

    Geometry::Ptr _geo;
    Material::Ptr _material;
    bool _wireframe;

    GeoNode(const Polyhedron& buf, bool wireframe = false);
    GeoNode(const Geometry::Ptr& geo, bool wireframe = false);

    void setGeometry(Geometry::Ptr& geo, bool wireframe = false);
    void setReflexion(ReflexionModel::Ptr reflexion);
    void setRenderPass(RenderPass::Ptr renderPass);
    void setMaterial(const Material::Ptr material);

    virtual ~GeoNode();

};

IMPGEARS_END

#endif // IMP_GEONODE_H
