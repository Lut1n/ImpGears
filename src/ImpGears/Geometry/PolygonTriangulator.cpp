#include <Geometry/PolygonTriangulator.h>
#include <cmath>

IMPGEARS_BEGIN

//--------------------------------------------------------------
int findNextSegment(const Edge& curr, const Polygon& poly, const EdgeBuffer& alreadyTaken, const imp::Vec3& pTaken)
{
	for(unsigned int i=0; i<poly.size(); ++i)
	{
        bool taken = false;
        for(unsigned int j=0; j<alreadyTaken.size(); ++j)
        {
            if(alreadyTaken[j] == poly[i])
            {
                taken = true;
                break;
            }
        }
        if(taken || poly[i]._p1 == pTaken || poly[i]._p2 == pTaken )
			continue;

        if (poly[i]._p1 == curr._p1 || poly[i]._p1 == curr._p2 || poly[i]._p2 == curr._p1 || poly[i]._p2 == curr._p2)
        {
            return i;
        }
	}
    return -1;
}

//--------------------------------------------------------------
imp::Vec3 findOppositePoint(const imp::Vec3& p0, const Polygon& poly)
{
	imp::Vec3 unused;
			
    std::vector<imp::Vec3> adjacents;
    for(unsigned int i=0; i<poly.size(); ++i)
	{
        if(poly[i]._p1 == p0)
        {
            adjacents.push_back(poly[i]._p2);
        }
        if(poly[i]._p2 == p0)
        {
            adjacents.push_back(poly[i]._p1);
        }
    }
    adjacents.push_back(p0);
    
	for(unsigned int i=0; i<poly.size(); ++i)
	{
        // exclude adjacent
        bool p1adj = false;
        bool p2adj = false;
        for(unsigned int j=0; j<adjacents.size(); ++j)
        {
            if(poly[i]._p1 == adjacents[j])
            {
                p1adj = true;
            }
            if(poly[i]._p2 == adjacents[j])
            {
                p2adj = true;
            }
        }
        
        if(!p1adj)
        {
            Edge seg;
            seg._p1 = p0;
            seg._p2 = poly[i]._p1;
            if( false == poly.intersection(seg,unused) && poly.inside( (seg._p1+seg._p2)*0.5) )
            {
                return seg._p2;
            }
        }
        if(!p2adj)
        {
            Edge seg;
            seg._p1 = p0;
            seg._p2 = poly[i]._p2;
            if( false == poly.intersection(seg, unused) && poly.inside( (seg._p1+seg._p2)*0.5) )
            {
                return seg._p2;
            }
        }
	}
    return imp::Vec3(0.0,0.0,0.0);
}

//--------------------------------------------------------------
int findSegment(const imp::Vec3& point, const Polygon& poly, const imp::Vec3& dirv)
{
	for(unsigned int i=0; i<poly.size(); ++i)
	{
        if(poly[i]._p1==point && (poly[i]._p2-point).dot(dirv) > 0.0)
        {
			return i;
        }
        if(poly[i]._p2==point && (poly[i]._p1-point).dot(dirv) > 0.0)
        {
			return i;
        }
	}
    return -1;
}

//--------------------------------------------------------------
Polygon constructPolygon(const Polygon& src, const imp::Vec3& begin, const imp::Vec3& end, const imp::Vec3& n)
{
    imp::Vec3 b2e = end - begin;
    imp::Vec3 dirv = imp::Vec3(0.0,0.0,1.0).cross(b2e);
    dirv.normalize();
    
    Polygon res;
    int segBegin = -1;
	
	segBegin = findSegment(begin, src, n);
	if(segBegin == -1)
	{
		return res;
	}
    res.push_back(src[segBegin]);
    while( !(res._edges.back()._p2 == end) && !(res._edges.back()._p1 == end) )
    {
        int next = findNextSegment(res._edges.back(), src, res._edges, begin);
        if(next == -1) break;
        res.push_back(src[next]);
		 
    }
    return res;
}

//--------------------------------------------------------------
bool processPolygon(std::vector<Polygon>& polygons, unsigned int index)
{
    const Polygon& poly = polygons[index];
    
    if(poly.size() > 3)
    {
        imp::Vec3 firstPoint = poly[0]._p1;
        imp::Vec3 opposite = findOppositePoint(firstPoint, poly);
		if(opposite == imp::Vec3(0.0,0.0,0.0))
		{
			firstPoint = poly[0]._p2;
			opposite = findOppositePoint(firstPoint, poly);
		}
        
        Edge segScissor;
        segScissor._p1 = firstPoint;
        segScissor._p2 = opposite;
        segScissor._n = imp::Vec3(0.0,0.0,0.0);
		
		imp::Vec3 n = opposite - firstPoint;
		n = imp::Vec3(0.0,0.0,1.0).cross(n);
		n.normalize();
        
        // construct 2 sub path
        // polygons.
        Polygon subpoly1 = constructPolygon(poly, segScissor._p1, segScissor._p2, n);
        Polygon subpoly2 = constructPolygon(poly, segScissor._p1, segScissor._p2, imp::Vec3(0.0,0.0,0.0)-n);
		
        segScissor._n = imp::Vec3(0.0,0.0,0.0)-n;
        subpoly1.push_back(segScissor);
        segScissor._n = n;
        subpoly2.push_back(segScissor);
        
        std::vector<Polygon>::iterator it=polygons.begin();
        for(unsigned int i=0; it!=polygons.end(); it++, ++i)
        {
            if(i == index)
            {
                polygons.erase(it);
                break;
            }
        }
        polygons.push_back(subpoly1);
        polygons.push_back(subpoly2);
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------
void PolygonTriangulator::triangulate(Polygon& shape, std::vector<Polygon>& out)
{
    out.clear();
	out.push_back(shape);
    
	bool test = true;
	while(test)
	{
        test = false;
        for(unsigned int i=0; i<out.size(); ++i)
        {
            if( true == processPolygon(out, i))
            {
                test = true;
                break;
            }
        }
	}
}

IMPGEARS_END







