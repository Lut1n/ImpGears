#ifndef IMP_GEONODE_H
#define IMP_GEONODE_H

#include <Core/Object.h>
#include <Geometry/Polyhedron.h>
#include <Geometry/Geometry.h>
#include <SceneGraph/Node.h>
#include <SceneGraph/Material.h>
#include <Renderer/Uniform.h>
#include <Renderer/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API GeoNode : public Node
{
    public:
		
	Meta_Class(GeoNode)
	
	Geometry _geo;
	Material::Ptr _material;
	RenderPlugin::Data::Ptr _gBuffer;
	bool _wireframe;
	bool _loaded;
	Vec3 _color;
	ReflexionModel::Ptr _shader;
	Uniform::Ptr u_color;
	
	GeoNode(const Polyhedron& buf, bool wireframe = false);
	
	GeoNode(const Geometry& geo, bool wireframe = false);
	
	void setColor(const Vec3& color);
	
	void setShader(ReflexionModel::Ptr shader);
	void setMaterial(const Material::Ptr material);
	
	virtual ~GeoNode();
	
	virtual void update();
	virtual void render();
};

IMPGEARS_END

#endif // IMP_GEONODE_H
