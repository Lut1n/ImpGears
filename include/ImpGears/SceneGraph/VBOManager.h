/*
Graphics/VBOManager.h

Define a manager for using VBOs. VBOManager keeps id and size of VBO created. It's useful for detecting unfree video memory.
*/

#ifndef VBOMANAGER_H
#define VBOMANAGER_H

#include <Core/Object.h>

#define VBO_MAX 100000

IMPGEARS_BEGIN

struct IMP_API VBO_Info
{
    std::uint32_t videoID;
    std::uint32_t size;
};

/// \brief Defines The game VBO Manager.
class IMP_API VBOManager
{
private:

    VBOManager();

    static VBOManager* instance;

    std::uint32_t memoryUsed;

    VBO_Info vboInfos[VBO_MAX];
    std::uint32_t nbVbo;

public:


    enum UsageMode
    {
        UsageMode_Static = 0,
        UsageMode_Dynamic
    };

    virtual ~VBOManager();

    static VBOManager* getInstance();

    std::uint32_t request(std::uint32_t _size, UsageMode _usage);
    void release(std::uint32_t _id);

    void resize(std::uint32_t _id, std::uint32_t _size, UsageMode _usage);

    std::uint32_t getVideoID(std::uint32_t _index);
    std::uint32_t findVideoID(std::uint32_t _id);

    std::uint32_t getMemoryUsed() const{return memoryUsed;}
};

IMPGEARS_END

#endif // VBOMANAGER_H
