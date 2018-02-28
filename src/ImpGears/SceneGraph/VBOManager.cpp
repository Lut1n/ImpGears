#include <SceneGraph/VBOManager.h>
#include <SceneGraph/OpenGL.h>

#include <iostream>

IMPGEARS_BEGIN

VBOManager* VBOManager::instance = nullptr;

//--------------------------------------------------------------
VBOManager* VBOManager::getInstance()
{
    if(VBOManager::instance == nullptr)
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
		std::cout << "impError : GPU memory unfree : " << memoryUsed << " bytes (" << nbVbo << " vbo)" << std::endl;

    for(std::uint32_t index = 0; index<nbVbo; ++index)
        release(vboInfos[index].videoID);

    VBOManager::instance = nullptr;
}

//--------------------------------------------------------------
std::uint32_t VBOManager::request(std::uint32_t _size, UsageMode _usage)
{
    if(nbVbo >= VBO_MAX)
    {
		std::cout << "impError : Maximum number of VBO has been reached (" << VBO_MAX << ").\n" << std::endl;
		return 0;
	}

    GLuint id;
    glGenBuffers(1, &id);
    GL_CHECKERROR("request VBO");

    VBO_Info info;
    info.videoID = static_cast<std::uint32_t>(id);
    info.size = 0;

    vboInfos[nbVbo++] = info;

    resize(info.videoID, _size, _usage);

    return info.videoID;
}

//--------------------------------------------------------------
void VBOManager::release(std::uint32_t _id)
{
    if(_id == 0)
        return;

    std::uint32_t index = findVideoID(_id);

    memoryUsed -= vboInfos[index].size;

    for(std::uint32_t i = index+1; i<nbVbo; ++i)
        vboInfos[i-1] = vboInfos[i];

    --nbVbo;

    GLuint id[1];
    id[0] = (GLuint)_id;

    glDeleteBuffers(1, id);
    GL_CHECKERROR("delete VBO");
}

//--------------------------------------------------------------
void VBOManager::resize(std::uint32_t _id, std::uint32_t _size, UsageMode _usage)
{
    if(_id == 0)
        return;

    GLuint id[1];
    id[0] = (GLuint)_id;

    GLint glUsage = _usage == UsageMode_Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    glBindBuffer(GL_ARRAY_BUFFER, id[0]);
    GL_CHECKERROR("VBO resize");
    glBufferData(GL_ARRAY_BUFFER, _size, 0, glUsage);
    GL_CHECKERROR("VBO set data");

    std::uint32_t index = findVideoID(_id);
    memoryUsed -= vboInfos[index].size;
    vboInfos[index].size = _size;
    memoryUsed += vboInfos[index].size;
}

//--------------------------------------------------------------
std::uint32_t VBOManager::getVideoID(std::uint32_t _index)
{
    return vboInfos[_index].videoID;
}

//--------------------------------------------------------------
std::uint32_t VBOManager::findVideoID(std::uint32_t _id)
{
    for(std::uint32_t index = 0; index<nbVbo; ++index)
        if(vboInfos[index].videoID == _id)
            return index;

    return 0;
}

IMPGEARS_END
