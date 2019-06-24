#include <SceneGraph/QuadNode.h>

IMPGEARS_BEGIN

QuadNode::QuadNode()
	: GeoNode(nullptr,false)
{
	_loaded = true;
	
	// quad generation
	Vec3 p1(-1,-1,0);
	Vec3 p2(-1,+1,0);
	Vec3 p3(+1,-1,0);
	Vec3 p4(+1,+1,0);
	_geo = Geometry::create();
	*_geo = Geometry::quad(p1,p2,p3,p4);
	
	// texture coords generation
	// and normals generation
	Geometry::TexCoordBuf texCoords;
	Geometry::BufType normals;
	for(auto v : _geo->_vertices)
	{
		texCoords.push_back(v*0.5+0.5);
		normals.push_back(Vec3(0.0,0.0,1.0));
	}
	_geo->setTexCoords(texCoords);
	_geo->setNormals(normals);
	_geo->generateColors(Vec3(1.0));
}

QuadNode::~QuadNode()
{
}


void QuadNode::update()
{
	// do nothing
}

IMPGEARS_END
