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

	void setData(const void* _buffer, int _size, int _vboOffset = 0);
	
	void load(const Geometry& geometry);

	void setPrimitive(Primitive prim) {_primitive = prim;}
	Primitive getPrimitive() const {return _primitive;}

	void enableVertexArray();
	void enableNormalArray();
	void enableTexCoordArray();
	void enableColorArray();

	int id() const;
	int size() const;
	
	void bind();
	void unbind();

        static std::uint32_t s_count() { return _s_count; }

private:

	int _id;
	int _size;
	UsageMode _usage;
	Primitive _primitive;
	int _verticesCount;
	
        static std::uint32_t s_memoryUsed;
        static std::uint32_t s_vboCount;
        static std::uint32_t _s_count;
	
	int _vertIndex;
	int _texIndex;
	int _colIndex;
	int _norIndex;

};

IMPGEARS_END

#endif // BUFFERDATA_H
