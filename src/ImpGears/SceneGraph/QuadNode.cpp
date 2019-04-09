#include <SceneGraph/QuadNode.h>
#include <Geometry/Geometry.h>

#include <SceneGraph/GraphRenderer.h>

IMPGEARS_BEGIN

QuadNode::QuadNode()
{
	// quad generation
	Vec3 p1 = -Vec3::X-Vec3::Y;
	Vec3 p2 = -Vec3::X+Vec3::Y;
	Vec3 p3 = Vec3::X-Vec3::Y;
	Vec3 p4 = Vec3::X+Vec3::Y;
	Geometry geo = Geometry::quad(p1,p2,p3,p4); Geometry::intoCCW( geo );
	Geometry::TexCoordBuf texCoords;
	
	// texture coords generation
	for(auto v : geo._vertices) {texCoords.push_back(v);}
	geo.setTexCoords(texCoords);

	_gData = GraphRenderer::s_interface->load(&geo);
	// _gData.loadGeometry(geo);
}

QuadNode::~QuadNode()
{
	// _gData.releaseVBO();
}


void QuadNode::render()
{
	GraphRenderer::s_interface->draw(_gData);
	// _gData.drawVBO();
}

IMPGEARS_END
