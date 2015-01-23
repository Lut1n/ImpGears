/*
graphics/VBOManager.h

Define a manager for using VBOs. VBOManager keeps id and size of VBO created. It's useful for detecting unfree video memory.
*/

#ifndef VBOMANAGER_H
#define VBOMANAGER_H

#include "../base/impBase.hpp"

#define VBO_MAX 500

struct VBO_Info
{
    imp::Uint32 videoID;
    imp::Uint32 size;
};

/// \brief Defines The game VBO Manager.
class VBOManager
{
private:

    VBOManager();

    static VBOManager* instance;

    imp::Uint32 memoryUsed;

    VBO_Info vboInfos[VBO_MAX];
    imp::Uint32 nbVbo;

public:

    virtual ~VBOManager();

    static VBOManager* getInstance();

    imp::Uint32 request(imp::Uint32 _size);
    void release(imp::Uint32 _id);

    void resize(imp::Uint32 _id, imp::Uint32 _size);

    imp::Uint32 getVideoID(imp::Uint32 _index);
    imp::Uint32 findVideoID(imp::Uint32 _id);

    imp::Uint32 getMemoryUsed() const{return memoryUsed;}
};

#endif // VBOMANAGER_H
