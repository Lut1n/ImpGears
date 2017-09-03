#ifndef IMP_PROCEDURAL_GEOMETRY_H
#define IMP_PROCEDURAL_GEOMETRY_H

#include <Core/impBase.h>

#include <Core/Vector3.h>
#include <Core/Math.h>

#include <vector>

IMPGEARS_BEGIN

class IMP_API SignalFunctor
{
	public:
	
	virtual double apply(double x, double y, double z) = 0;
};


class IMP_API Geometry
{
	public:
	
	std::vector<imp::Vector3> _vertices;
	// std::vector<imp::Vector3> _normals;
	//std::vector<unsigned int> _indices;
	
	Geometry();
	
	Geometry(const Geometry& other);
	
	void operator=(const Geometry& other);
	
	void operator+=(const Geometry& other);
	
	Geometry operator+(const Geometry& other);
	
	void origin(const imp::Vector3& origin);
	
	void scale(const imp::Vector3& vec);
	
	void sphericalNormalization(float factor);
	
	void rotationX(float a);
	
	void rotationY(float a);
	
	void rotationZ(float a);
	
	void optimize();
	
	void fillBuffer(std::vector<float>& buffer);
	
	void noiseBump(unsigned int octaveCount, double persistence, double freq, float force = 1.0);
	
	void bump(SignalFunctor* functor, float force = 1.0);
	
	static Geometry createQuad(unsigned int subdivisionCount, const imp::Vector3& xvec, const imp::Vector3& yvec, const imp::Vector3& zvec, float size = 1.f);
	
	static Geometry createCube(unsigned int subdivisionCount);
	
	static Geometry generateTriangle(unsigned int subdivisionCount, const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3);
	
	static Geometry createTetrahedron(unsigned int subdivisionCount);
	
	static Geometry createPyramid(unsigned int baseDivision, unsigned int subdivisionCount);
};


IMPGEARS_END

#endif