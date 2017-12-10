#include <Geometry/PolygonScissor.h>
#include <cmath>
#include <map>


IMPGEARS_BEGIN

//--------------------------------------------------------------
static const int INSIDE = 0;
static const int OUTSIDE = 1;
static const int COMEIN = 2;
static const int COMEOUT = 3;
    
//--------------------------------------------------------------
bool checkIntersection(Edge& e1, Polygon& poly, std::map<double, imp::Vector3>& regPoint, std::map<double, int>& regState)
{
	bool result = false;
	
    for(unsigned int i=0; i<poly.size(); i++)
    {
		imp::Vector3 ipoint;
		if(e1.intersection(poly[i],ipoint))
		{
			double dirp1 = (e1._p1 - ipoint).dotProduct(poly[i]._n);
			
			double key = (ipoint - e1._p1).getNorm();
			
			if(dirp1 >= 0.0)
			{
				regState[key] = COMEIN;
				
			}
			else
			{
				regState[key] = COMEOUT;
			}
			regPoint[key] = ipoint;
			
			result = true;
		}
	}
	
	return result;
}

//--------------------------------------------------------------
void splitEdge(Edge& edge, Polygon& poly, Polygon& result, bool edgeMinusPoly)
{
	static const imp::Vector3 zeroVec(0.0,0.0,0.0);
	
    std::map<double, imp::Vector3> regPoint;
    std::map<double, int> regState;
    
    double firstKey = 0.0;
    double lastKey = (edge._p2 - edge._p1).getNorm();
    
    regState[firstKey] = poly.distance(edge._p1) > 0.0 ? OUTSIDE : INSIDE;
    regPoint[firstKey] = edge._p1;
    
    regState[lastKey] = poly.distance(edge._p2) > 0.0 ? OUTSIDE : INSIDE;
    regPoint[lastKey] = edge._p2;
        
    if(checkIntersection(edge, poly, regPoint, regState))
    {
        std::map<double, int>::iterator it=regState.begin();
        for(; it!=regState.end();it++)
        {
            double key = it->first;
            std::map<double, int>::iterator it2 = it; it2++;
            if(it2 == regState.end()) continue;
            
            int state = regState[key];            
            int state2 = regState[ it2->first ];
    
			imp::Vector3 normal = edge._n;
	
			if(edgeMinusPoly)
			{
				if(state == INSIDE || state == COMEIN) continue;
				if(state2 != OUTSIDE && state2 != COMEIN) continue;
			}
			else
			{
				if(state == OUTSIDE || state == COMEOUT) continue;
				if(state2 != INSIDE && state2 != COMEOUT) continue;
				normal = zeroVec - edge._n;
			}
            
            result.push_back( Edge(regPoint[key], regPoint[it2->first], normal) );
        }
    }
	else
    {
		if(edgeMinusPoly && poly.outside(edge))
		{
			result.push_back( edge );
		}
        if(!edgeMinusPoly && poly.inside(edge))
        {
            Edge seg2d(edge);
            seg2d._n = zeroVec - edge._n;
            result.push_back( seg2d );
        }
    }
}

//--------------------------------------------------------------
void inversePolygon(Polygon& poly)
{
	static const imp::Vector3 zeroVec(0.0,0.0,0.0);
	
	for(unsigned int i=0; i<poly.size(); i++)
    {
		poly[i]._n = zeroVec - poly[i]._n;
    }
}

//--------------------------------------------------------------
Polygon PolygonScissor::substract(Polygon& solid1, Polygon& solid2)
{
	Polygon result;
	for(unsigned int i=0; i<solid1.size(); i++)
    {
		splitEdge(solid1[i],solid2,result, true);
    }
    for(unsigned int i=0; i<solid2.size(); i++)
    {
		splitEdge(solid2[i],solid1,result, false);
    }
    
    return result;
}

//--------------------------------------------------------------
Polygon PolygonScissor::merge(Polygon& solid1, Polygon& solid2)
{
	Polygon result;
	for(unsigned int i=0; i<solid1.size(); i++)
    {
		splitEdge(solid1[i],solid2,result, true);
    }
    for(unsigned int i=0; i<solid2.size(); i++)
    {
		splitEdge(solid2[i],solid1,result, true);
    }
    
    return result;
}

//--------------------------------------------------------------
Polygon PolygonScissor::intersect(Polygon& solid1, Polygon& solid2)
{
	Polygon result;
	for(unsigned int i=0; i<solid1.size(); i++)
    {
		splitEdge(solid1[i],solid2,result, false);
    }
    for(unsigned int i=0; i<solid2.size(); i++)
    {
		splitEdge(solid2[i],solid1,result, false);
    }
	
    inversePolygon(result);
	
    return result;
}


IMPGEARS_END