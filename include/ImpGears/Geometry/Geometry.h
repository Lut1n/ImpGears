#ifndef IMP_PROCEDURAL_GEOMETRY_H
#define IMP_PROCEDURAL_GEOMETRY_H

#include <Core/Object.h>

#include <Core/Vec3.h>
#include <Core/Math.h>

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
	
	std::vector<imp::Vec3> _vertices;
	// std::vector<imp::Vec3> _normals;
	//std::vector<unsigned int> _indices;
	
	Meta_Class(Geometry)
	
	Geometry();
	
	Geometry(const Geometry& other);
	
	void operator=(const Geometry& other);
	
	void operator+=(const Geometry& other);
	
	Geometry operator+(const Geometry& other);
	
	void origin(const imp::Vec3& origin);
	
	void scale(const imp::Vec3& vec);
	
	void sphericalNormalization(float factor);
	
	void rotX(float a);
	
	void rotY(float a);
	
	void rotZ(float a);
	
	void optimize();
	
	void fillBuffer(std::vector<float>& buffer);
	
	void noiseBump(unsigned int octaveCount, double persistence, double freq, float force = 1.0);
	
	void bump(SignalFunctor* functor, float force = 1.0);
	
	static Geometry createQuad(unsigned int subdivisionCount, const imp::Vec3& xvec, const imp::Vec3& yvec, const imp::Vec3& zvec, float size = 1.f);
	
	static Geometry createCube(unsigned int subdivisionCount);
	
	static Geometry generateTriangle(unsigned int subdivisionCount, const imp::Vec3& p1, const imp::Vec3& p2, const imp::Vec3& p3);
	
	static Geometry createTetrahedron(unsigned int subdivisionCount);
	
	static Geometry createPyramid(unsigned int baseDivision, unsigned int subdivisionCount);
};


IMPGEARS_END

#endif