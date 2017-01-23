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

public:

	enum Primitive
	{
		Primitive_Points = 0,
		Primitive_Lines,
		Primitive_Triangles,
		Primitive_Quads,
	};

private:

    imp::Uint32 vboID;
    imp::Uint32 vboSize;
    VBOManager::UsageMode usage;
	Primitive _primitive;
	imp::Uint32 _verticesCount;

public:

    VBOData();

    ~VBOData();

	void drawVBO();

    void requestVBO(imp::Uint32 _size, VBOManager::UsageMode _usage = VBOManager::UsageMode_Static);

    void releaseVBO();

    void resizeVBO(imp::Uint32 _size);

	void setVertices(const float* buffer, imp::Uint32 size);

    void setData(const void* _buffer, imp::Uint32 _size, imp::Uint32 _vboOffset);

    void setData(const void* _buffer, imp::Uint32 _size);

	void setPrimitive(Primitive prim) {_primitive = prim;}
	Primitive getPrimitive() const {return _primitive;}

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
