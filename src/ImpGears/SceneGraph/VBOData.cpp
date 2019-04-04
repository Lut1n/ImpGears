#include <SceneGraph/VBOData.h>
#include <SceneGraph/OpenGL.h>

#include <cstdio>

IMPGEARS_BEGIN

//--------------------------------------------------------------
VBOData::VBOData()
{
    vboID = 0;
    vboSize = 0;
	_verticesCount = 0;
	_primitive = Primitive_Lines;
}

//--------------------------------------------------------------
VBOData::~VBOData()
{
    if(vboID != 0)
        releaseVBO();
}

//--------------------------------------------------------------
void VBOData::drawVBO()
{
    bindVBO(*this);

    ///vertex
    enableVertexArray(0);
    GL_CHECKERROR("[impErr] VBOData::drawVBO - vertex pointer");

    ///texture coord
	std::uint32_t texcoordOffset = _verticesCount * (3 * sizeof(float) );
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
void VBOData::requestVBO(std::uint32_t _size, VBOManager::UsageMode _usage)
{
    vboID = VBOManager::getInstance()->request(_size, _usage);
    vboSize = _size;
    usage = _usage;
}

//--------------------------------------------------------------
void VBOData::releaseVBO()
{
    if(vboID != 0)
    {
        VBOManager::getInstance()->release(vboID);
        vboID = 0;
        vboSize = 0;
    }
}

//--------------------------------------------------------------
void VBOData::resizeVBO(std::uint32_t _size)
{
    VBOManager::getInstance()->resize(vboID, _size, usage);
}

//--------------------------------------------------------------
void VBOData::setVertices(const float* buffer, std::uint32_t size)
{
	std::uint32_t vertexSize = (sizeof(float) * 3.0);
	_verticesCount = size / vertexSize;

	setData((const void*)buffer, size, 0);
}

//--------------------------------------------------------------
void VBOData::setData(const void* _buffer, std::uint32_t _size, std::uint32_t _vboOffset)
{
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)vboID);
    glBufferSubData(GL_ARRAY_BUFFER, _vboOffset, _size, _buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void VBOData::setData(const void* _buffer, std::uint32_t _size)
{
    setData(_buffer, _size, 0);
}

//--------------------------------------------------------------
void VBOData::enableVertexArray(std::uint64_t _offset)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_offset);
}

//--------------------------------------------------------------
void VBOData::enableNormalArray(std::uint64_t _offset)
{
    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer(GL_FLOAT, 0, (GLvoid*)(_offset));
}

//--------------------------------------------------------------
void VBOData::enableTexCoordArray(std::uint64_t _offset)
{
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(_offset));
}

//--------------------------------------------------------------
void VBOData::enableColorArray(std::uint64_t _offset)
{
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(_offset));
}

//--------------------------------------------------------------
std::uint32_t VBOData::getVBOID() const
{
    return vboID;
}

//--------------------------------------------------------------
std::uint32_t VBOData::getVBOSize() const
{
    return vboSize;
}

//--------------------------------------------------------------
std::uint32_t VBOData::getBoundVbo()
{
    GLint id;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &id);

    return static_cast<std::uint32_t>(id);
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
