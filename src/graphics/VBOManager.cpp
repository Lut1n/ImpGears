#include "Graphics/VBOManager.h"
#include "Graphics/GLcommon.h"

#include <stdio.h>

IMPGEARS_BEGIN

VBOManager* VBOManager::instance = IMP_NULL;

//--------------------------------------------------------------
VBOManager* VBOManager::getInstance()
{
    if(VBOManager::instance == IMP_NULL)
        new VBOManager();

    return VBOManager::instance;
}

//--------------------------------------------------------------
VBOManager::VBOManager():
    memoryUsed(0),
    nbVbo(0)
{
    VBOManager::instance = this;
}

//--------------------------------------------------------------
VBOManager::~VBOManager()
{
    if( memoryUsed != 0 )
        fprintf(stderr, "impError : GPU memory unfree : %d bytes (%d vbo)\n", memoryUsed, nbVbo);

    for(imp::Uint32 index = 0; index<nbVbo; ++index)
        release(vboInfos[index].videoID);

    VBOManager::instance = IMP_NULL;
}

//--------------------------------------------------------------
imp::Uint32 VBOManager::request(imp::Uint32 _size, UsageMode _usage)
{
    if(nbVbo >= VBO_MAX)
        return 0;

    GLuint id;
    glGenBuffers(1, &id);

    VBO_Info info;
    info.videoID = static_cast<imp::Uint32>(id);
    info.size = 0;

    vboInfos[nbVbo++] = info;

    resize(info.videoID, _size, _usage);

    return info.videoID;
}

//--------------------------------------------------------------
void VBOManager::release(imp::Uint32 _id)
{
    if(_id == 0)
        return;

    imp::Uint32 index = findVideoID(_id);

    memoryUsed -= vboInfos[index].size;

    for(imp::Uint32 i = index+1; i<nbVbo; ++i)
        vboInfos[i-1] = vboInfos[i];

    --nbVbo;

    GLuint id[1];
    id[0] = (GLuint)_id;

    glDeleteBuffers(1, id);
}

//--------------------------------------------------------------
void VBOManager::resize(imp::Uint32 _id, imp::Uint32 _size, UsageMode _usage)
{
    if(_id == 0)
        return;

    GLuint id[1];
    id[0] = (GLuint)_id;

    GLint glUsage = _usage == UsageMode_Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    glBindBuffer(GL_ARRAY_BUFFER, id[0]);
    glBufferData(GL_ARRAY_BUFFER, _size, 0, glUsage);


    imp::Uint32 index = findVideoID(_id);
    memoryUsed -= vboInfos[index].size;
    vboInfos[index].size = _size;
    memoryUsed += vboInfos[index].size;
}

//--------------------------------------------------------------
imp::Uint32 VBOManager::getVideoID(imp::Uint32 _index)
{
    return vboInfos[_index].videoID;
}

//--------------------------------------------------------------
imp::Uint32 VBOManager::findVideoID(imp::Uint32 _id)
{
    for(imp::Uint32 index = 0; index<nbVbo; ++index)
        if(vboInfos[index].videoID == _id)
            return index;

    return 0;
}

IMPGEARS_END
