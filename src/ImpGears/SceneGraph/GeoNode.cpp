#include <SceneGraph/GeoNode.h>
#include <SceneGraph/BasicShader.h>


IMPGEARS_BEGIN

//--------------------------------------------------------------
GeoNode::GeoNode(const Polyhedron& buf, bool wireframe)
{
	_loaded = false;
	_geo.setPrimitive(Geometry::Primitive_Triangles);
	buf.getTriangles(_geo._vertices);
	_wireframe = wireframe;
	
	_shader = BasicShader::create();
	u_color = Uniform::create("u_color", Uniform::Type_3f);
	_color = Vec3(0.0,0.0,1.0);
	u_color->set(&_color);
	_shader->addUniform(u_color);
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
GeoNode::GeoNode(const Geometry& geo, bool wireframe)
{
	_loaded = false;
	_geo = geo;
	_wireframe = wireframe;
	
	_shader = BasicShader::create();
	u_color = Uniform::create("u_color", Uniform::Type_3f);
	_color = Vec3(0.0,0.0,1.0);
	u_color->set(&_color);
	_shader->addUniform(u_color);
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
void GeoNode::setColor(const Vec3& color)
{
	_color = color;
	u_color->set(&_color);
}

//--------------------------------------------------------------
void GeoNode::setShader(Shader::Ptr shader)
{
	_shader = shader;
	_shader->addUniform(u_color);
	getState()->setShader(_shader);
}

//--------------------------------------------------------------
GeoNode::~GeoNode()
{}

//--------------------------------------------------------------
void GeoNode::render()
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
		
		if(_geo.getPrimitive()==Geometry::Primitive_Triangles)
			_gBuffer.setPrimitive(VBOData::Primitive_Triangles);
		else
			_gBuffer.setPrimitive(VBOData::Primitive_Lines);
		
		std::vector<float> floatBuffer;
		_geo.fillBuffer( floatBuffer );
		std::uint32_t size = floatBuffer.size()*sizeof(float);
		_gBuffer.requestVBO(size);
		_gBuffer.setVertices(floatBuffer.data(), size);
		_loaded = true;
	}
	
	_gBuffer.drawVBO();
}

IMPGEARS_END
