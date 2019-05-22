#include <SceneGraph/GeoNode.h>
#include <Renderer/SceneRenderer.h>


IMPGEARS_BEGIN

//--------------------------------------------------------------
GeoNode::GeoNode(const Polyhedron& buf, bool wireframe)
{
	_loaded = false;
	_geo.setPrimitive(Geometry::Primitive_Triangles);
	buf.getTriangles(_geo._vertices);
	_wireframe = wireframe;
	
	_shader = ReflexionModel::create();
	_material = Material::create();
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
GeoNode::GeoNode(const Geometry& geo, bool wireframe)
{
	_loaded = false;
	_geo = geo;
	_wireframe = wireframe;
	
	_shader = ReflexionModel::create();
	_material = Material::create();
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
void GeoNode::setShader(ReflexionModel::Ptr shader)
{
	_shader = shader;
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
void GeoNode::setMaterial(const Material::Ptr material)
{
	_material = material;
}

//--------------------------------------------------------------
GeoNode::~GeoNode()
{
}

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
	if(SceneRenderer::s_interface != nullptr )
	{
		if(_gBuffer == nullptr)
			_gBuffer = SceneRenderer::s_interface->load(&_geo);
		if(_gBuffer != nullptr)
		SceneRenderer::s_interface->draw(_gBuffer);
	}
}

IMPGEARS_END
