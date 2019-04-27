#ifndef IMP_PROCEDURAL_GEOMETRY_H
#define IMP_PROCEDURAL_GEOMETRY_H

#include <Core/Object.h>
#include <Core/Vec3.h>
#include <Core/Math.h>

#include <Geometry/Path.h>

#include <vector>

IMPGEARS_BEGIN

class IMP_API SignalFunctor
{
	public:
	
	virtual double apply(double x, double y, double z) = 0;
};


class IMP_API Geometry : public Object
{
	public:
	
	using TexCoord = Vec<2,float>;
	using BufType = std::vector<Vec3>;
	using TexCoordBuf = std::vector<TexCoord>;
	
	enum Primitive
	{
		Primitive_Lines,
		Primitive_Triangles
	};
	
	Primitive _prim;
	BufType _vertices;
	BufType _colors;
	BufType _normals;
	TexCoordBuf _texCoords;
	
	bool _hasTexCoords;
	bool _hasColors;
	bool _hasNormals;
	
	Meta_Class(Geometry)
	
	Geometry();
	
	Geometry(const Geometry& other);
	
	void operator=(const Geometry& other);
	
	void operator+=(const Geometry& other);
	
	Geometry operator+(const Geometry& other);
	
	void origin(const Vec3& origin);
	
	void scale(const Vec3& vec);
	
	void sphericalNormalization(float factor);
	
	void rotX(float a);
	
	void rotY(float a);
	
	void rotZ(float a);
	
	void optimize();
	
	void fillBuffer(std::vector<float>& buffer) const;
	
	void noiseBump(unsigned int octaveCount, double persistence, double freq, float force = 1.0);
	
	void bump(SignalFunctor* functor, float force = 1.0);
	
	void setPrimitive(Primitive p);
	Primitive getPrimitive() const;
	
	void operator+=(const Vec3& v);
	void operator-=(const Vec3& v);
	void operator*=(const Vec3& v);
	void operator*=(float f);
	
	int size() const;
	Vec3& at(int i);
	const Vec3& at(int i) const;
	Vec3& operator[](int i);
	const Vec3& operator[](int i) const;
	
	void add(const Vec3& v);
	void add(const BufType& buf);
	
	void reduceTriangles(float f);
	
	Geometry subdivise(int count);
	
	void setTexCoords(const TexCoordBuf& coords);
	void setVertices(const BufType& vertices);
	void setColors(const BufType& colors);
	void setNormals(const BufType& normals);
	
	static void intoCCW( Geometry& buf );

	static Geometry intoLineBuf(const Geometry& buf);
	
	static Geometry sphere(int sub, float size);

	static Geometry extrude(const Path& base, float len, float ratioTop=1.0);

	static Geometry cylinder(int sub, float len, float radius);
	
	static Geometry cone(int sub, float len, float radius1, float radius2=0.0);
	
	static Geometry quad(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p4);
	
	static Geometry cube();
	
	static Geometry tetrahedron();
};


IMPGEARS_END

#endif
