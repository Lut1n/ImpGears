#include "BufferObject.h"
#include "GlError.h"

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
	enableVertexArray(0);
	GL_CHECKERROR("[impErr] BufferObject::draw - vertex pointer");

	///texture coord
	int texcoordOffset = _verticesCount * (3 * sizeof(float) );
	enableTexCoordArray(texcoordOffset);

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
void BufferObject::setVertices(const float* buffer, int size)
{
	int vertexSize = (sizeof(float) * 3.0);
	_verticesCount = size / vertexSize;

	setData((const void*)buffer, size, 0);
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
	
	// gpu load
	int vertSize = vertex.size()*sizeof(float);
	int texSize = texcoords.size()*sizeof(float);
	request(vertSize+texSize);
	setVertices(vertex.data(), vertSize);
	if(texSize > 0) setData(texcoords.data(), texSize, vertSize);
}

/*
void Mesh::updateVBO(bool clearLocalData)
{
    int vboSize = (m_vertexBufferSize+m_texCoordBufferSize+m_normalBufferSize)*sizeof(float);

    if(getVBOID() == 0)
    {
        requestVBO( vboSize );
    }
    else if(getVBOSize() != vboSize)
    {
        resizeVBO(vboSize);
    }

    m_vertexOffset = 0;
    m_texCoordOffset = m_vertexBufferSize*sizeof(float);
    m_normalOffset = m_texCoordOffset + m_texCoordBufferSize*sizeof(float);

    setData(m_vertexBuffer, m_vertexBufferSize*sizeof(float), m_vertexOffset);
    setData(m_texCoordBuffer, m_texCoordBufferSize*sizeof(float), m_texCoordOffset);
    setData(m_normalBuffer, m_normalBufferSize*sizeof(float), m_normalOffset);

    if(clearLocalData)
    {
        clearVertexBuffer();
        clearTexCoordBuffer();
        clearNormalBuffer();
    }
}
*/

//--------------------------------------------------------------
void BufferObject::enableVertexArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)oft);
}

//--------------------------------------------------------------
void BufferObject::enableNormalArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer(GL_FLOAT, 0, (GLvoid*)oft);
}

//--------------------------------------------------------------
void BufferObject::enableTexCoordArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)oft);
}

//--------------------------------------------------------------
void BufferObject::enableColorArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)oft);
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
