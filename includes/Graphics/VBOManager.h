/*
Graphics/VBOManager.h

Define a manager for using VBOs. VBOManager keeps id and size of VBO created. It's useful for detecting unfree video memory.
*/

#ifndef VBOMANAGER_H
#define VBOMANAGER_H

#include "../Core/impBase.h"

#define VBO_MAX 500

IMPGEARS_BEGIN

struct IMP_API VBO_Info
{
    imp::Uint32 videoID;
    imp::Uint32 size;
};

/// \brief Defines The game VBO Manager.
class IMP_API VBOManager
{
private:

    VBOManager();

    static VBOManager* instance;

    imp::Uint32 memoryUsed;

    VBO_Info vboInfos[VBO_MAX];
    imp::Uint32 nbVbo;

public:


    enum UsageMode
    {
        UsageMode_Static = 0,
        UsageMode_Dynamic
    };

    virtual ~VBOManager();

    static VBOManager* getInstance();

    imp::Uint32 request(imp::Uint32 _size, UsageMode _usage);
    void release(imp::Uint32 _id);

    void resize(imp::Uint32 _id, imp::Uint32 _size, UsageMode _usage);

    imp::Uint32 getVideoID(imp::Uint32 _index);
    imp::Uint32 findVideoID(imp::Uint32 _id);

    imp::Uint32 getMemoryUsed() const{return memoryUsed;}
};

IMPGEARS_END

#endif // VBOMANAGER_H
