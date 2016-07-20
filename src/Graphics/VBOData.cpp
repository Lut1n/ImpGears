#include "Graphics/VBOData.h"
#include "Graphics/GLcommon.h"

#include <cstdio>

IMPGEARS_BEGIN

//--------------------------------------------------------------
VBOData::VBOData()
{
    vboID = 0;
    vboSize = 0;
}

//--------------------------------------------------------------
VBOData::~VBOData()
{
    if(vboID != 0)
        releaseVBO();
}

//--------------------------------------------------------------
void VBOData::requestVBO(imp::Uint32 _size, VBOManager::UsageMode _usage)
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
void VBOData::resizeVBO(imp::Uint32 _size)
{
    VBOManager::getInstance()->resize(vboID, _size, usage);
}

//--------------------------------------------------------------
void VBOData::setData(const void* _buffer, imp::Uint32 _size, imp::Uint32 _vboOffset)
{
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)vboID);
    glBufferSubData(GL_ARRAY_BUFFER, _vboOffset, _size, _buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void VBOData::setData(const void* _buffer, imp::Uint32 _size)
{
    setData(_buffer, _size, 0);
}

//--------------------------------------------------------------
void VBOData::enableVertexArray(imp::Uint64 _offset)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)_offset);
}

//--------------------------------------------------------------
void VBOData::enableNormalArray(imp::Uint64 _offset)
{
    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer(GL_FLOAT, 0, (GLvoid*)(_offset));
}

//--------------------------------------------------------------
void VBOData::enableTexCoordArray(imp::Uint64 _offset)
{
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)(_offset));
}

//--------------------------------------------------------------
void VBOData::enableColorArray(imp::Uint64 _offset)
{
    glEnableClientState( GL_COLOR_ARRAY );
    glColorPointer(4, GL_FLOAT, 0, (GLvoid*)(_offset));
}

//--------------------------------------------------------------
imp::Uint32 VBOData::getVBOID() const
{
    return vboID;
}

//--------------------------------------------------------------
imp::Uint32 VBOData::getVBOSize() const
{
    return vboSize;
}

//--------------------------------------------------------------
imp::Uint32 VBOData::getBoundVbo()
{
    GLint id;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &id);

    return static_cast<imp::Uint32>(id);
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
