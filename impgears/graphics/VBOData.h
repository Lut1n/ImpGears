/*
graphics/VBOData.h

Define an interface for using VBO. VBOData keeps memory size used after request.
*/

#ifndef VBODATA_H
#define VBODATA_H

#include "VBOManager.h"

/// \brief Defines a vbo data.
class VBOData
{
private:

    imp::Uint32 vboID;
    imp::Uint32 vboSize;

public:
    VBOData();

    ~VBOData();

    void requestVBO(imp::Uint32 _size);

    void releaseVBO();

    void resizeVBO(imp::Uint32 _size);

    void setData(void* _buffer, imp::Uint32 _size, imp::Uint32 _vboOffset);

    void setData(void* _buffer, imp::Uint32 _size);

    imp::Uint32 getVBOID();
    imp::Uint32 getVBOSize();
};

#endif // VBODATA_H
