/*
Graphics/VBOData.h

Define an interface for using VBO. VBOData keeps memory size used after request.
*/

#ifndef VBODATA_H
#define VBODATA_H

#include "VBOManager.h"

IMPGEARS_BEGIN

/// \brief Defines a vbo data.
class IMP_API VBOData
{
private:

    imp::Uint32 vboID;
    imp::Uint32 vboSize;
    VBOManager::UsageMode usage;

public:
    VBOData();

    ~VBOData();

    void requestVBO(imp::Uint32 _size, VBOManager::UsageMode _usage = VBOManager::UsageMode_Static);

    void releaseVBO();

    void resizeVBO(imp::Uint32 _size);

    void setData(const void* _buffer, imp::Uint32 _size, imp::Uint32 _vboOffset);

    void setData(const void* _buffer, imp::Uint32 _size);

    void enableVertexArray(imp::Uint64 _offset);
    void enableNormalArray(imp::Uint64 _offset);
    void enableTexCoordArray(imp::Uint64 _offset);
    void enableColorArray(imp::Uint64 _offset);

    imp::Uint32 getVBOID() const;
    imp::Uint32 getVBOSize() const;

    static imp::Uint32 getBoundVbo();
    static void bindVBO(const VBOData& _vboData);
    static void unbindVBO();
};

IMPGEARS_END

#endif // VBODATA_H
