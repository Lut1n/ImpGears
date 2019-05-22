#include <SceneGraph/QuadNode.h>

#include <Renderer/SceneRenderer.h>

IMPGEARS_BEGIN

QuadNode::QuadNode()
{
	// quad generation
	Vec3 p1 = -Vec3::X-Vec3::Y;
	Vec3 p2 = -Vec3::X+Vec3::Y;
	Vec3 p3 = Vec3::X-Vec3::Y;
	Vec3 p4 = Vec3::X+Vec3::Y;
	_geo = Geometry::quad(p1,p2,p3,p4);
	
	_geo.generateColors(Vec3(1.0));
	
	// texture coords generation
	// and normals generation
	Geometry::TexCoordBuf texCoords;
	Geometry::BufType normals;
	for(auto v : _geo._vertices)
	{
		texCoords.push_back(v*0.5+0.5);
		normals.push_back(Vec3(0.0,0.0,1.0));
	}
	_geo.setTexCoords(texCoords);
	_geo.setNormals(normals);
	
	Uniform::Ptr u_color = Uniform::create("u_color",Uniform::Type_3f);
	u_color->set(Vec3(1.0));
	getState()->setUniform(u_color);
	
	// Geometry::intoCCW( _geo );
}

QuadNode::~QuadNode()
{
	// TODO release _gData
}


void QuadNode::render()
{
	if(SceneRenderer::s_interface != nullptr)
	{
		if(_gData == nullptr) _gData = SceneRenderer::s_interface->load(&_geo);
		SceneRenderer::s_interface->draw(_gData);
	}
}

IMPGEARS_END
