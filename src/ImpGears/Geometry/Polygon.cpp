#include <Geometry/Polygon.h>

#include <map>
#include <vector>
#include <cmath>
#include <iostream>

IMPGEARS_BEGIN

/*

bool operator==(const imp::Vector3& v1, const imp::Vector3& v2)
{
    return v1.getX() == v2.getX() && v1.getY() == v2.getY() && v1.getZ() == v2.getZ();
}*/

std::ostream& operator<<(std::ostream& os, const imp::Vector3& v)
{  
    os << "[" << v.getX() << ";" << v.getY() << ";" << v.getZ() << "]";  
    return os;  
}  

imp::Vector3 mix(const imp::Vector3& a, const imp::Vector3& b, float f)
{
    return a + (b-a) * f;
}

//--------------------------------------------------------------
bool Edge::intersection(const Edge& other, imp::Vector3& ipoint) const
{
	const Edge& s1 = *this;
	const Edge& s2 = other;
	
	imp::Vector3 p12 = s1._p2 - s1._p1;
    imp::Vector3 a = s2._p1 - s1._p1;
    imp::Vector3 b = s2._p2 - s1._p1;
    
    imp::Vector3 tan = p12;
    tan.normalize();
    
    imp::Vector3 bitan = imp::Vector3(0.0,0.0,1.0).crossProduct( tan );
	bitan.normalize();
    
    float da = a.dotProduct(bitan);
    float db = b.dotProduct(bitan);
    if( da * db <= 0.0 ) // one on each side
    {
        float t = da / (da - db);
        ipoint = mix(a, b, t);
        
        float di = ipoint.dotProduct(tan);
        if(di > 0.0 && di < p12.getNorm())
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
		if( _edges[i]._p1.getX() < _boundsA.getX()
			|| _edges[i]._p1.getY() < _boundsA.getY()  )
		{
			_boundsA = _edges[i]._p1;
		}
		if( _edges[i]._p2.getX() < _boundsA.getX()
			|| _edges[i]._p2.getY() < _boundsA.getY()  )
		{
			_boundsA = _edges[i]._p2;
		}
		if( _edges[i]._p1.getX() > _boundsB.getX()
			|| _edges[i]._p1.getY() > _boundsB.getY()  )
		{
			_boundsB = _edges[i]._p1;
		}
		if( _edges[i]._p2.getX() > _boundsB.getX()
			|| _edges[i]._p2.getY() > _boundsB.getY()  )
		{
			_boundsB = _edges[i]._p2;
		}
	}
}


//--------------------------------------------------------------
bool Polygon::inside(const imp::Vector3& p) const
{	
    computeBounds();
	imp::Vector3 offPolygonSet = (_boundsB - _boundsA);
	offPolygonSet.normalize();
	imp::Vector3 target = _boundsB + offPolygonSet;
	
	Edge fakeSeg;
	fakeSeg._p1 = p;
	fakeSeg._p2 = target;
	
	std::map<double, imp::Vector3> _distNormals;
	
	for(unsigned int i=0; i<_edges.size(); i++)
    {
		imp::Vector3 ipoint;
		if( fakeSeg.intersection(_edges[i], ipoint) )
		{
			_distNormals[(ipoint - p).getNorm()] = _edges[i]._n;
		}
	}
	
	std::map<double, imp::Vector3>::iterator it = _distNormals.begin();
	if(it != _distNormals.end())
	{
		imp::Vector3 n = it->second;
		imp::Vector3 dir = target-p;
		if(dir.dotProduct(n) >= 0.0)
		{
			return true;
		}
	}
	
	return false;
}


//--------------------------------------------------------------
float Polygon::distance(const imp::Vector3& p) const
{
	float res = 1e20;
	float ares = res;
	bool bench_test = false;
	
	for(unsigned int i=0; i<_edges.size(); i++)
    {
		imp::Vector3 pp1 = (p - _edges[i]._p1);
		imp::Vector3 p12 = (_edges[i]._p2 - _edges[i]._p1);
		imp::Vector3 tan = p12;
		tan.normalize();
		
		float projp = pp1.dotProduct(tan);
		
		if(projp <= 0.0 || projp > p12.getNorm())
			continue;
		
		float local = (p - _edges[i]._p1).dotProduct(_edges[i]._n);
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
bool Polygon::intersection(const Edge& edge, imp::Vector3& ipoint) const
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