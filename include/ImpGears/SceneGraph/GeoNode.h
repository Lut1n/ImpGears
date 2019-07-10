#ifndef IMP_GEONODE_H
#define IMP_GEONODE_H

#include <Core/Object.h>
#include <Geometry/Polyhedron.h>
#include <Geometry/Geometry.h>
#include <SceneGraph/Node.h>
#include <SceneGraph/Material.h>
#include <Graphics/Uniform.h>

IMPGEARS_BEGIN

class IMP_API GeoNode : public Node
{
    public:
		
	Meta_Class(GeoNode)
	
	Geometry::Ptr _geo;
	Material::Ptr _material;
	int _gBuffer;
	bool _wireframe;
	bool _loaded;
	
	GeoNode(const Polyhedron& buf, bool wireframe = false);
	GeoNode(const Geometry::Ptr& geo, bool wireframe = false);
	
	void setReflexion(ReflexionModel::Ptr reflexion);
	void setMaterial(const Material::Ptr material);
	
	virtual ~GeoNode();
	
	virtual void update();
	
};

IMPGEARS_END

#endif // IMP_GEONODE_H
