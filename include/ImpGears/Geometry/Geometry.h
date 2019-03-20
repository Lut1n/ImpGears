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
	
	using BufType = std::vector<Vec3>;
	
	enum Primitive
	{
		Primitive_Lines,
		Primitive_Triangles
	};
	
	Primitive _prim;
	BufType _vertices;
	// std::vector<Vec3> _normals;
	//std::vector<unsigned int> _indices;
	
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
	
	void fillBuffer(std::vector<float>& buffer);
	
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
	
	static void intoCCW( Geometry& buf );

	static Geometry intoLineBuf(const Geometry& buf);
	
	static Geometry sphere(int sub, float size);

	static Geometry extrude(const Path& base, float len, float ratioTop=1.0, int sub=0);

	static Geometry cylinder(int sub, float len, float radius);
	
	static Geometry cone(int sub, float len, float radius1, float radius2=0.0);
	
	static Geometry createQuad(unsigned int subdivisionCount, const imp::Vec3& xvec, const imp::Vec3& yvec, const imp::Vec3& zvec, float size = 1.f);
	
	static Geometry createCube(unsigned int subdivisionCount);
	
	static Geometry generateTriangle(unsigned int subdivisionCount, const imp::Vec3& p1, const imp::Vec3& p2, const imp::Vec3& p3);
	
	static Geometry createTetrahedron(unsigned int subdivisionCount);
	
	static Geometry createPyramid(unsigned int baseDivision, unsigned int subdivisionCount);
};


IMPGEARS_END

#endif
