#include <Geometry/PolygonScissor.h>
#include <cmath>
#include <map>


IMPGEARS_BEGIN

static const int INSIDE = 0;
static const int OUTSIDE = 1;
static const int COMEIN = 2;
static const int COMEOUT = 3;


//--------------------------------------------------------------
bool subsplitAA(Edge& e1, Edge& e2, std::vector<imp::Vector3>& regPoint, std::vector<double>& regDist)
{
    imp::Vector3 ipoint;
    if(e1.intersection(e2,ipoint))
    {
        double dirp1 = (e1._p1 - ipoint).dotProduct(e2._n);
        
        regPoint.push_back( ipoint );
        regDist.push_back( dirp1 );
        
        return true;
    }
    return false;
}
    
//--------------------------------------------------------------
Edge cutA(Edge& e1, const imp::Vector3& point, double dist)
{
    Edge result;
    if(dist > 0.0)
    {
        result._p1 = e1._p1;
    }
    else
    {
        result._p1 = e1._p2;
    }
    
    result._p2 = point;
    result._n = e1._n;
    
    return result;
}

//--------------------------------------------------------------
void subsplitA(Edge& e1, Polygon& solid2, Polygon& result)
{
    bool inter = false;
    std::vector<imp::Vector3> regPoint;
    std::vector<double> regDist;
    
    
    for(unsigned int j=0; j<solid2.size(); j++)
    {
        if( subsplitAA(e1, solid2[j], regPoint, regDist) )
        {
            inter = true;
        }
    }
        
    if(!inter)
    {
        // pb : when point is clipped in no surface
        if(solid2.outside(e1)) result.push_back( e1 );
    }
    else
    {
        for(unsigned int j=0; j<regPoint.size(); ++j)
        {
            result.push_back( cutA(e1, regPoint[j], regDist[j]) );
        }
    }
}

    
//--------------------------------------------------------------
bool subsplitBB(Edge& e2, Edge& e1, std::map<double, imp::Vector3>& regPoint, std::map<double, int>& regState)
{
    imp::Vector3 ipoint;
    if(e1.intersection(e2,ipoint))
    {
        double dirp1 = (e2._p1 - ipoint).dotProduct(e1._n);
        
        double key = (ipoint - e2._p1).getNorm();
        
        if(dirp1 >= 0.0)
        {
            regState[key] = COMEIN;
            
        }
        else
        {
            regState[key] = COMEOUT;
        }
        regPoint[key] = ipoint;
        
        return true;
    }
    return false;
}

//--------------------------------------------------------------
void subsplitB(Edge& e2, Polygon& solid1, Polygon& result)
{
    bool inter = false;
    std::map<double, imp::Vector3> regPoint;
    std::map<double, int> regState;
    
    double firstKey = 0.0;
    double lastKey = (e2._p2 - e2._p1).getNorm();
    
    regState[firstKey] = solid1.distance(e2._p1) > 0.0 ? OUTSIDE : INSIDE;
    regPoint[firstKey] = e2._p1;
    
    regState[lastKey] = solid1.distance(e2._p2) > 0.0 ? OUTSIDE : INSIDE;
    regPoint[lastKey] = e2._p2;
    
    for(unsigned int i=0; i<solid1.size(); i++)
    {
        if(subsplitBB(e2, solid1[i], regPoint, regState))
        {
            inter = true;
        }
    }
        
    if(!inter)
    {
        if(solid1.inside(e2))
        {
            Edge seg2d(e2);
            seg2d._n = imp::Vector3(0.0,0.0,0.0) - e2._n;
            result.push_back( seg2d );
        }
    }
    else
    {
        std::map<double, int>::iterator it=regState.begin();
        for(; it!=regState.end();it++)
        {
            double key = it->first;
            std::map<double, int>::iterator it2 = it; it2++;
            if(it2 == regState.end()) continue;
            
            int state = regState[key];            
            int state2 = regState[ it2->first ];
    
            if(state == OUTSIDE || state == COMEOUT) continue;
            if(state2 != INSIDE && state2 != COMEOUT) continue;
            
            result.push_back( Edge(regPoint[key], regPoint[it2->first], imp::Vector3(0.0,0.0,0.0) - e2._n) );
        }
    }
}



//--------------------------------------------------------------
void splitA(Polygon& solid1, Polygon& solid2, Polygon& result)
{
	for(unsigned int i=0; i<solid1.size(); i++)
    {
		subsplitA(solid1[i],solid2,result);
    }
}


//--------------------------------------------------------------
void splitB(Polygon& solid1, Polygon& solid2, Polygon& result)
{
    for(unsigned int j=0; j<solid2.size(); j++)
    {
		subsplitB(solid2[j],solid1,result);
    }
}

//--------------------------------------------------------------
Polygon PolygonScissor::substract(Polygon& solid1, Polygon& solid2)
{
    
	Polygon result;
    
    splitA(solid1,solid2,result);
	
    splitB(solid1,solid2,result);
    
    return result;
}


IMPGEARS_END