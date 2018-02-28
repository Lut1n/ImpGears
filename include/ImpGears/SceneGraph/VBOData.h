/*
Graphics/VBOData.h

Define an interface for using VBO. VBOData keeps memory size used after request.
*/

#ifndef VBODATA_H
#define VBODATA_H

#include "VBOManager.h"

IMPGEARS_BEGIN

/// \brief Defines a vbo data.
class IMP_API VBOData : public Object
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

    std::uint32_t vboID;
    std::uint32_t vboSize;
    VBOManager::UsageMode usage;
	Primitive _primitive;
	std::uint32_t _verticesCount;

public:

	Meta_Class(VBOData)

    VBOData();

    ~VBOData();

	void drawVBO();

    void requestVBO(std::uint32_t _size, VBOManager::UsageMode _usage = VBOManager::UsageMode_Static);

    void releaseVBO();

    void resizeVBO(std::uint32_t _size);

	void setVertices(const float* buffer, std::uint32_t size);

    void setData(const void* _buffer, std::uint32_t _size, std::uint32_t _vboOffset);

    void setData(const void* _buffer, std::uint32_t _size);

	void setPrimitive(Primitive prim) {_primitive = prim;}
	Primitive getPrimitive() const {return _primitive;}

    void enableVertexArray(std::uint64_t _offset);
    void enableNormalArray(std::uint64_t _offset);
    void enableTexCoordArray(std::uint64_t _offset);
    void enableColorArray(std::uint64_t _offset);

    std::uint32_t getVBOID() const;
    std::uint32_t getVBOSize() const;

    static std::uint32_t getBoundVbo();
    static void bindVBO(const VBOData& _vboData);
    static void unbindVBO();
};

IMPGEARS_END

#endif // VBODATA_H
