#ifndef BUFFERDATA_H
#define BUFFERDATA_H

#include <Core/Object.h>
#include <Geometry/Geometry.h>


IMPGEARS_BEGIN

class IMP_API BufferObject : public Object
{
public:
	
	Meta_Class(BufferObject)

	enum Primitive
	{
		Primitive_Points = 0,
		Primitive_Lines,
		Primitive_Triangles,
		Primitive_Quads,
	};
	
	enum UsageMode
	{
		UsageMode_Static = 0,
		UsageMode_Dynamic
	};

	BufferObject();

	~BufferObject();

	void draw();

	void request(int _size, UsageMode _usage = UsageMode_Static);

	void release();

	void resize(int _size);

	void setVertices(const float* buffer, int size);

	void setData(const void* _buffer, int _size, int _vboOffset = 0);
	
	void load(const Geometry& geometry);

	void setPrimitive(Primitive prim) {_primitive = prim;}
	Primitive getPrimitive() const {return _primitive;}

	void enableVertexArray(int _offset);
	void enableNormalArray(int _offset);
	void enableTexCoordArray(int _offset);
	void enableColorArray(int _offset);

	int id() const;
	int size() const;
	
	void bind();
	void unbind();
	

private:

	int _id;
	int _size;
	UsageMode _usage;
	Primitive _primitive;
	int _verticesCount;
	
    static std::uint32_t s_memoryUsed;
    static std::uint32_t s_vboCount;

};

IMPGEARS_END

#endif // BUFFERDATA_H
