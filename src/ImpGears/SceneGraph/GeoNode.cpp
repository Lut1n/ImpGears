#include <SceneGraph/GeoNode.h>
// #include <SceneGraph/BasicShader.h>
#include <SceneGraph/GraphRenderer.h>


IMPGEARS_BEGIN

//--------------------------------------------------------------
GeoNode::GeoNode(const Polyhedron& buf, bool wireframe)
{
	_loaded = false;
	_geo.setPrimitive(Geometry::Primitive_Triangles);
	buf.getTriangles(_geo._vertices);
	_wireframe = wireframe;
	
	_shader = LightModel::create();
	u_color = Uniform::create("u_color", Uniform::Type_3f);
	_color = Vec3(0.0,0.0,1.0);
	u_color->set(_color);
	getState()->setUniform(u_color);
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
GeoNode::GeoNode(const Geometry& geo, bool wireframe)
{
	_loaded = false;
	_geo = geo;
	_wireframe = wireframe;
	
	_shader = LightModel::create();
	u_color = Uniform::create("u_color", Uniform::Type_3f);
	_color = Vec3(0.0,0.0,1.0);
	u_color->set(_color);
	getState()->setUniform(u_color);
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
void GeoNode::setColor(const Vec3& color)
{
	_color = color;
	u_color->set(_color);
	getState()->setUniform(u_color);
}

//--------------------------------------------------------------
void GeoNode::setShader(LightModel::Ptr shader)
{
	_shader = shader;
	// _shader->addUniform(u_color);
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
GeoNode::~GeoNode()
{}

//--------------------------------------------------------------
void GeoNode::update()
{ 
	if(!_loaded)
	{
		if(_geo.getPrimitive()==Geometry::Primitive_Triangles)
		{
			if(_wireframe)
			{
				_geo = Geometry::intoLineBuf( _geo );
				_geo.setPrimitive(Geometry::Primitive_Lines);
			}
			else
			{
				Geometry::intoCCW(_geo);
			}
		}
		_loaded = true;
	}
}

//--------------------------------------------------------------
void GeoNode::render()
{ 
	if(GraphRenderer::s_interface != nullptr )
	{
		if(_gBuffer == nullptr)
			_gBuffer = GraphRenderer::s_interface->load(&_geo);
		if(_gBuffer != nullptr)
		GraphRenderer::s_interface->draw(_gBuffer);
	}
}

IMPGEARS_END
