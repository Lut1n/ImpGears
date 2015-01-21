#include "VBOData.h"

#include "GLcommon.h"

#include <cstdio>

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
void VBOData::requestVBO(imp::Uint32 _size)
{
    vboID = VBOManager::getInstance()->request(_size);
    vboSize = _size;
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
    VBOManager::getInstance()->resize(vboID, _size);
}

//--------------------------------------------------------------
void VBOData::setData(void* _buffer, imp::Uint32 _size, imp::Uint32 _vboOffset)
{
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)vboID);
    glBufferSubData(GL_ARRAY_BUFFER, _vboOffset, _size, _buffer );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void VBOData::setData(void* _buffer, imp::Uint32 _size)
{
    setData(_buffer, _size, 0);
}

//--------------------------------------------------------------
imp::Uint32 VBOData::getVBOID()
{
    return vboID;
}

//--------------------------------------------------------------
imp::Uint32 VBOData::getVBOSize()
{
    return vboSize;
}
