#ifndef IMP_GEONODE_H
#define IMP_GEONODE_H

#include <Core/Object.h>
#include <Geometry/Polyhedron.h>
#include <Geometry/Geometry.h>
#include <SceneGraph/SceneNode.h>
#include <SceneGraph/Shader.h>
#include <SceneGraph/VBOData.h>

IMPGEARS_BEGIN

class IMP_API GeoNode : public SceneNode
{
    public:
		
	Meta_Class(GeoNode)
	
	Geometry _geo;
	VBOData _gBuffer;
	bool _wireframe;
	bool _loaded;
	Vec3 _color;
	Shader::Ptr _shader;
	Uniform::Ptr u_color;
	
	GeoNode(const Polyhedron& buf, bool wireframe = false);
	
	GeoNode(const Geometry& geo, bool wireframe = false);
	
	void setColor(const Vec3& color);
	
	void setShader(Shader::Ptr shader);
	
	virtual ~GeoNode();
	virtual void render();
};

IMPGEARS_END

#endif // IMP_GEONODE_H
