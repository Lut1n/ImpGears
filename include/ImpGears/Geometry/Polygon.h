#ifndef IMP_POLYGON_H
#define IMP_POLYGON_H

#include <Core/Object.h>
#include <Core/Math.h>
#include <Core/Vector3.h>

#include <vector>

IMPGEARS_BEGIN

class IMP_API Edge : public Object
{
	public:
	
    imp::Vector3 _p1;
    imp::Vector3 _p2;
    imp::Vector3 _n;
	
	Meta_Class(Edge)
	
	Edge(){}
	
	Edge(const Edge& other)
		: _p1(other._p1)
		, _p2(other._p2)
		, _n(other._n)
	{}
	
	Edge(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& n)
		: _p1(p1)
		, _p2(p2)
		, _n(n)
	{}
	
	void operator=(const Edge& other)
	{
		_p1 = other._p1;
		_p2 = other._p2;
		_n = other._n;
	}
    
    bool operator==(const Edge& other) const
    {
        return _p1==other._p1 && _p2==other._p2;
    }
	
	bool intersection(const Edge& other, imp::Vector3& ipoint) const;
};

	
typedef std::vector<Edge> EdgeBuffer;

class IMP_API Polygon : public Object
{
	public:
	
	EdgeBuffer	_edges;
	
	mutable imp::Vector3 _boundsA;
	mutable imp::Vector3 _boundsB;
	
	Meta_Class(Polygon)
	
	Polygon(){}
	
	virtual ~Polygon(){}
	
	void computeBounds() const;
	
	bool inside(const imp::Vector3& point) const;
	
	bool inside(const Edge& edge) const;
	bool outside(const Edge& edge) const;
	
	float distance(const imp::Vector3& point) const;
	
	bool intersection(const Edge& edge, imp::Vector3& ipoint) const;
	
	void push_back(const Edge& edge)
	{
		_edges.push_back(edge);
	}
	
	unsigned int size() const
	{
		return _edges.size();
	}
	
	Edge& operator[](unsigned int i)
	{
		return _edges[i];
	}
	
	const Edge& operator[](unsigned int i) const
	{
		return _edges[i];
	}
	
};

IMPGEARS_END


#endif // IMP_POLYGON_H

