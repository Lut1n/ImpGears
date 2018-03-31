#include <Geometry/Polygon.h>

#include <map>
#include <vector>
#include <cmath>
#include <iostream>

IMPGEARS_BEGIN

/*

bool operator==(const imp::Vec3& v1, const imp::Vec3& v2)
{
    return v1.x() == v2.x() && v1.y() == v2.y() && v1.z() == v2.z();
}*/

std::ostream& operator<<(std::ostream& os, const imp::Vec3& v)
{  
    os << "[" << v.x() << ";" << v.y() << ";" << v.z() << "]";  
    return os;  
}  

imp::Vec3 mix(const imp::Vec3& a, const imp::Vec3& b, float f)
{
    return a + (b-a) * f;
}

//--------------------------------------------------------------
bool Edge::intersection(const Edge& other, imp::Vec3& ipoint) const
{
	const Edge& s1 = *this;
	const Edge& s2 = other;
	
	imp::Vec3 p12 = s1._p2 - s1._p1;
    imp::Vec3 a = s2._p1 - s1._p1;
    imp::Vec3 b = s2._p2 - s1._p1;
    
    imp::Vec3 tan = p12;
    tan.normalize();
    
    imp::Vec3 bitan = imp::Vec3(0.0,0.0,1.0).cross( tan );
	bitan.normalize();
    
    float da = a.dot(bitan);
    float db = b.dot(bitan);
    if( da * db <= 0.0 ) // one on each side
    {
        float t = da / (da - db);
        ipoint = mix(a, b, t);
        
        float di = ipoint.dot(tan);
        if(di > 0.0 && di < p12.length())
        {
			ipoint = s1._p1 + ipoint;
            return true;
        }
    }
    
    return false;
}

void Polygon::computeBounds() const
{
	if(_edges.size() > 0)
	{
		_boundsA = _edges[0]._p1;
		_boundsB = _edges[0]._p1;
	}
	
	for(unsigned int i=0; i<_edges.size(); i++)
    {
		if( _edges[i]._p1.x() < _boundsA.x()
			|| _edges[i]._p1.y() < _boundsA.y()  )
		{
			_boundsA = _edges[i]._p1;
		}
		if( _edges[i]._p2.x() < _boundsA.x()
			|| _edges[i]._p2.y() < _boundsA.y()  )
		{
			_boundsA = _edges[i]._p2;
		}
		if( _edges[i]._p1.x() > _boundsB.x()
			|| _edges[i]._p1.y() > _boundsB.y()  )
		{
			_boundsB = _edges[i]._p1;
		}
		if( _edges[i]._p2.x() > _boundsB.x()
			|| _edges[i]._p2.y() > _boundsB.y()  )
		{
			_boundsB = _edges[i]._p2;
		}
	}
}


//--------------------------------------------------------------
bool Polygon::inside(const imp::Vec3& p) const
{	
    computeBounds();
	imp::Vec3 offPolygonSet = (_boundsB - _boundsA);
	offPolygonSet.normalize();
	imp::Vec3 target = _boundsB + offPolygonSet;
	
	Edge fakeSeg;
	fakeSeg._p1 = p;
	fakeSeg._p2 = target;
	
	std::map<double, imp::Vec3> _distNormals;
	
	for(unsigned int i=0; i<_edges.size(); i++)
    {
		imp::Vec3 ipoint;
		if( fakeSeg.intersection(_edges[i], ipoint) )
		{
			_distNormals[(ipoint - p).length()] = _edges[i]._n;
		}
	}
	
	std::map<double, imp::Vec3>::iterator it = _distNormals.begin();
	if(it != _distNormals.end())
	{
		imp::Vec3 n = it->second;
		imp::Vec3 dir = target-p;
		if(dir.dot(n) >= 0.0)
		{
			return true;
		}
	}
	
	return false;
}


//--------------------------------------------------------------
float Polygon::distance(const imp::Vec3& p) const
{
	float res = 1e20;
	float ares = res;
	bool bench_test = false;
	
	for(unsigned int i=0; i<_edges.size(); i++)
    {
		imp::Vec3 pp1 = (p - _edges[i]._p1);
		imp::Vec3 p12 = (_edges[i]._p2 - _edges[i]._p1);
		imp::Vec3 tan = p12;
		tan.normalize();
		
		float projp = pp1.dot(tan);
		
		if(projp <= 0.0 || projp > p12.length())
			continue;
		
		float local = (p - _edges[i]._p1).dot(_edges[i]._n);
		float alocal = std::abs(local);
		
		if(alocal < ares)
		{
			res = local;
			ares = alocal;
			bench_test = true;
		}
	}
	
	if(!bench_test)
	{
		return 1.0;
	}
	
	return res;
}

//--------------------------------------------------------------
bool Polygon::outside(const Edge& seg) const
{
	bool d1 = inside(seg._p1);
	bool d2 = inside(seg._p2);
	
	return !d1 && !d2;
}


//--------------------------------------------------------------
bool Polygon::inside(const Edge& seg) const
{
	bool d1 = inside(seg._p1);
	bool d2 = inside(seg._p2);
	
	return d1 && d2;
}

//--------------------------------------------------------------
bool Polygon::intersection(const Edge& edge, imp::Vec3& ipoint) const
{
	for(unsigned int i=0; i<_edges.size(); i++)
    {
		// exclude case of edge vertex == polygon vertex
        if( (_edges[i]._p1 == edge._p1)
			|| (_edges[i]._p2 == edge._p1)
			|| (_edges[i]._p1 == edge._p2)
			|| (_edges[i]._p2 == edge._p2) ) 
        {
            continue;
        }
        
		if( edge.intersection(_edges[i], ipoint) )
		{
			// intersect with at least one edge of the polygon
			return true;
		}
	}
	
	return false;
}


IMPGEARS_END