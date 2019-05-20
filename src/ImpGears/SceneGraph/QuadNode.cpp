#include <SceneGraph/QuadNode.h>

#include <SceneGraph/SceneGraph.h>

IMPGEARS_BEGIN

QuadNode::QuadNode()
{
	// quad generation
	Vec3 p1 = -Vec3::X-Vec3::Y;
	Vec3 p2 = -Vec3::X+Vec3::Y;
	Vec3 p3 = Vec3::X-Vec3::Y;
	Vec3 p4 = Vec3::X+Vec3::Y;
	_geo = Geometry::quad(p1,p2,p3,p4); Geometry::intoCCW( _geo );
	Geometry::TexCoordBuf texCoords;
	
	// texture coords generation
	for(auto v : _geo._vertices) {texCoords.push_back(v);}
	_geo.setTexCoords(texCoords);
}

QuadNode::~QuadNode()
{
	// TODO release _gData
}


void QuadNode::render()
{
	if(GraphRenderer::s_interface != nullptr)
	{
		if(_gData == nullptr) _gData = GraphRenderer::s_interface->load(&_geo);
		GraphRenderer::s_interface->draw(_gData);
	}
}

IMPGEARS_END
