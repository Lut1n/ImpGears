#include <OGLPlugin/BufferObject.h>
#include <OGLPlugin/GlError.h>

#include <cstdio>

IMPGEARS_BEGIN

#define VBO_MAX 100000

std::uint32_t BufferObject::s_memoryUsed = 0;
std::uint32_t BufferObject::s_vboCount = 0;

//--------------------------------------------------------------
BufferObject::BufferObject()
{
	_id = 0;
	_size = 0;
	_verticesCount = 0;
	_primitive = Primitive_Lines;
	_vertIndex = -1;
	_texIndex = -1;
	_colIndex = -1;
	_norIndex = -1;
}

//--------------------------------------------------------------
BufferObject::~BufferObject()
{
	if(_id != 0) release();
}

//--------------------------------------------------------------
void BufferObject::draw()
{
	bind();

	///vertex
	enableVertexArray();
	enableTexCoordArray();
	enableNormalArray();
	enableColorArray();
	GL_CHECKERROR("[impErr] BufferObject::draw - vertex pointer");

	unsigned int glPrimitive = GL_LINES;
	switch(_primitive)
	{
	case Primitive_Points:
		glPrimitive = GL_POINTS;
		break;
	case Primitive_Lines:
		glPrimitive = GL_LINES;
		break;
	case Primitive_Triangles:
		glPrimitive = GL_TRIANGLES;
		break;
	case Primitive_Quads:
		glPrimitive = GL_QUADS;
		break;
	default:
		glPrimitive = GL_LINES;
		break;
	};

	glDrawArrays(glPrimitive, 0, _verticesCount);

	unbind();
}

//--------------------------------------------------------------
void BufferObject::request(int size, UsageMode usage)
{
	if(s_vboCount >= VBO_MAX)
	{
		std::cout << "impError : Maximum number of VBO has been reached (" << VBO_MAX << ")." << std::endl;
		return;
	}

	GLuint id;
	glGenBuffers(1, &id);
	GL_CHECKERROR("request VBO");
	_id = (std::uint32_t)id;
	_usage = usage;

	resize(size);
}

//--------------------------------------------------------------
void BufferObject::release()
{;
	glDeleteBuffers(1, (GLuint*)&_id  );
	GL_CHECKERROR("delete VBO");
	_id = 0;
	_size = 0;
}

//--------------------------------------------------------------
void BufferObject::resize(int size)
{
	if(_id == 0)
	{
		std::cout << "[impError] try resize vbo id=" << _id << std::endl;
		return;
	}

	GLint glUsage = _usage == UsageMode_Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_id );
	GL_CHECKERROR("VBO resize");
	glBufferData(GL_ARRAY_BUFFER, size, 0, glUsage);
	GL_CHECKERROR("VBO set data");
	
	_size = size;
}

//--------------------------------------------------------------
void BufferObject::setData(const void* buffer, int size, int vboOffset)
{
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_id);
	glBufferSubData(GL_ARRAY_BUFFER, vboOffset, size, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void BufferObject::load(const Geometry& geometry)
{
	// primitive selection
	if(geometry.getPrimitive()==Geometry::Primitive_Triangles)
		setPrimitive(BufferObject::Primitive_Triangles);
	else
		setPrimitive(BufferObject::Primitive_Lines);
	
	// uniformize
	std::vector<float> vertex;
	geometry.fillBuffer( vertex );
	
	std::vector<float> texcoords;
	for(auto v2 : geometry._texCoords) { texcoords.push_back(v2[0]); texcoords.push_back(v2[1]); }
	
	std::vector<float> normals;
	for(auto v3 : geometry._normals) { normals.push_back(v3[0]); normals.push_back(v3[1]); normals.push_back(v3[2]); }
	
	std::vector<float> colors;
	for(auto v4 : geometry._colors)
	{
		colors.push_back(v4[0]);
		colors.push_back(v4[1]);
		colors.push_back(v4[2]);
		colors.push_back(v4[3]);
	}
	
	int vertSize = vertex.size()*sizeof(float);
	int texSize = texcoords.size()*sizeof(float);
	int norSize = normals.size()*sizeof(float);
	int colSize = colors.size()*sizeof(float);
	
	_vertIndex = 0;
	_texIndex = _vertIndex+vertSize;
	_norIndex = _texIndex+texSize;
	_colIndex = _norIndex+norSize;
	_verticesCount = geometry.size();
	
	// gpu load
	request(vertSize+texSize+norSize+colSize);
	if(vertSize > 0) setData(vertex.data(), vertSize, _vertIndex);
	if(texSize > 0) setData(texcoords.data(), texSize, _texIndex);
	if(norSize > 0) setData(normals.data(), norSize, _norIndex);
	if(colSize > 0) setData(colors.data(), colSize, _colIndex);
}

//--------------------------------------------------------------
void BufferObject::enableVertexArray()
{
	if(_vertIndex > -1)
	{
		std::uint64_t oft = _vertIndex;
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)oft);
	}
}

//--------------------------------------------------------------
void BufferObject::enableNormalArray()
{
	if(_norIndex > -1)
	{
		std::uint64_t oft = _norIndex;
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer(GL_FLOAT, 0, (GLvoid*)oft);
	}
}

//--------------------------------------------------------------
void BufferObject::enableTexCoordArray()
{
	if(_texIndex > -1)
	{
		std::uint64_t oft = _texIndex;
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)oft);
	}
}

//--------------------------------------------------------------
void BufferObject::enableColorArray()
{
	if(_colIndex > -1)
	{
		std::uint64_t oft = _colIndex;
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(4, GL_FLOAT, 0, (GLvoid*)oft);
	}
}

//--------------------------------------------------------------
int BufferObject::id() const
{
	return _id;
}

//--------------------------------------------------------------
int BufferObject::size() const
{
	return _size;
}

//--------------------------------------------------------------
void BufferObject::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, _id);
}

//--------------------------------------------------------------
void BufferObject::unbind()
{
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, _id);
}

IMPGEARS_END
