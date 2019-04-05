/*
Graphics/VBOData.h

Define an interface for ustd::sing VBO. VBOData keeps memory size used after request.
*/

#ifndef VBODATA_H
#define VBODATA_H

#include <SceneGraph/VBOManager.h>
#include <Geometry/Geometry.h>

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

int _id;
int _size;
VBOManager::UsageMode _usage;
Primitive _primitive;
int _verticesCount;

public:

	Meta_Class(VBOData)

	VBOData();

	~VBOData();

	void drawVBO();

	void requestVBO(int _size, VBOManager::UsageMode _usage = VBOManager::UsageMode_Static);

	void releaseVBO();

	void resizeVBO(int _size);

	void setVertices(const float* buffer, int size);

	void setData(const void* _buffer, int _size, int _vboOffset);

	void setData(const void* _buffer, int _size);
	
	void loadGeometry(const Geometry& geometry);

	void setPrimitive(Primitive prim) {_primitive = prim;}
	Primitive getPrimitive() const {return _primitive;}

	void enableVertexArray(int _offset);
	void enableNormalArray(int _offset);
	void enableTexCoordArray(int _offset);
	void enableColorArray(int _offset);

	int getVBOID() const;
	int getVBOSize() const;

	static int getBoundVbo();
	static void bindVBO(const VBOData& _vboData);
	static void unbindVBO();
};

IMPGEARS_END

#endif // VBODATA_H
