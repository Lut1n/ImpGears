#include <SceneGraph/VBOData.h>
#include <SceneGraph/OpenGL.h>

#include <cstdio>

IMPGEARS_BEGIN

//--------------------------------------------------------------
VBOData::VBOData()
{
	_id = 0;
	_size = 0;
	_verticesCount = 0;
	_primitive = Primitive_Lines;
}

//--------------------------------------------------------------
VBOData::~VBOData()
{
	if(_id != 0) releaseVBO();
}

//--------------------------------------------------------------
void VBOData::drawVBO()
{
	bindVBO(*this);

	///vertex
	enableVertexArray(0);
	GL_CHECKERROR("[impErr] VBOData::drawVBO - vertex pointer");

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

	unbindVBO();
}

//--------------------------------------------------------------
void VBOData::requestVBO(int size, VBOManager::UsageMode usage)
{
	_id = VBOManager::getInstance()->request(size, usage);
	_size = size;
	_usage = usage;
}

//--------------------------------------------------------------
void VBOData::releaseVBO()
{
	if(_id != 0)
	{
		VBOManager::getInstance()->release(_id);
		_id = 0;
		_size = 0;
	}
}

//--------------------------------------------------------------
void VBOData::resizeVBO(int size)
{
	VBOManager::getInstance()->resize(_id, size, _usage);
}

//--------------------------------------------------------------
void VBOData::setVertices(const float* buffer, int size)
{
	int vertexSize = (sizeof(float) * 3.0);
	_verticesCount = size / vertexSize;

	setData((const void*)buffer, size, 0);
}

//--------------------------------------------------------------
void VBOData::setData(const void* buffer, int size, int vboOffset)
{
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_id);
	glBufferSubData(GL_ARRAY_BUFFER, vboOffset, size, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void VBOData::setData(const void* buffer, int size)
{
	setData(buffer, size, 0);
}

//--------------------------------------------------------------
void VBOData::loadGeometry(const Geometry& geometry)
{
	// primitive selection
	if(geometry.getPrimitive()==Geometry::Primitive_Triangles)
		setPrimitive(VBOData::Primitive_Triangles);
	else
		setPrimitive(VBOData::Primitive_Lines);
	
	// uniformize
	std::vector<float> vertex;
	geometry.fillBuffer( vertex );
	
	std::vector<float> texcoords;
	for(auto v2 : geometry._texCoords) { texcoords.push_back(v2[0]); texcoords.push_back(v2[1]); }
	
	// gpu load
	int vertSize = vertex.size()*sizeof(float);
	int texSize = texcoords.size()*sizeof(float);
	requestVBO(vertSize+texSize);
	setVertices(vertex.data(), vertSize);
	if(texSize > 0) setData(texcoords.data(), texSize, vertSize);
}

//--------------------------------------------------------------
void VBOData::enableVertexArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)oft);
}

//--------------------------------------------------------------
void VBOData::enableNormalArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer(GL_FLOAT, 0, (GLvoid*)oft);
}

//--------------------------------------------------------------
void VBOData::enableTexCoordArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)oft);
}

//--------------------------------------------------------------
void VBOData::enableColorArray(int offset)
{
	std::uint64_t oft = offset;
	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer(4, GL_FLOAT, 0, (GLvoid*)oft);
}

//--------------------------------------------------------------
int VBOData::getVBOID() const
{
	return _id;
}

//--------------------------------------------------------------
int VBOData::getVBOSize() const
{
	return _size;
}

//--------------------------------------------------------------
int VBOData::getBoundVbo()
{
	GLint id;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &id);

	return (int)id;
}

//--------------------------------------------------------------
void VBOData::bindVBO(const VBOData& _vboData)
{
	glBindBuffer(GL_ARRAY_BUFFER, _vboData.getVBOID());
}

//--------------------------------------------------------------
void VBOData::unbindVBO()
{
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IMPGEARS_END
