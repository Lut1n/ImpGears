#include "equation.h"
#include <iostream>  

std::ostream& operator<<(std::ostream& os, const imp::Vector3& v)  
{  
    os << "[" << v.getX() << ";" << v.getY() << ";" << v.getZ() << "]";  
    return os;  
}  

imp::Vector3 mix(const imp::Vector3& a, const imp::Vector3& b, float f)
{
    return a + (b-a) * f;
}

void removeDuplicated(std::vector<imp::Vector3>& buff, float tolerance = 0.0)
{
    unsigned int i=0;
    
    while(i < buff.size())
    {
        bool found = false;
        for(unsigned int j=i+1; j<buff.size(); ++j)
        {
            imp::Vector3 diff = buff[i] - buff[j];
            bool eq = diff.getSqNorm() <= tolerance*tolerance;
            if(found || eq)
            {
                found = true;
                if(j < buff.size() - 1)
                buff[j] = buff[j+1];
            }
        }
        
        if(found)
        {
            buff.resize(buff.size()-1);
        }
        
        ++i;
    }
}


void removeDuplicated(std::vector<imp::Vector3>& buff, std::vector<int>& sidebuff, float tolerance = 0.0)
{
    unsigned int i=0;
    
    while(i < buff.size())
    {
        bool found = false;
        for(unsigned int j=i+1; j<buff.size(); ++j)
        {
            imp::Vector3 diff = buff[i] - buff[j];
            bool eq = diff.getSqNorm() <= tolerance*tolerance;
            if(found || eq)
            {
                found = true;
                if(j < buff.size() - 1)
                buff[j] = buff[j+1];
                sidebuff[j] = sidebuff[j+1];
            }
        }
        
        if(found)
        {
            buff.resize(buff.size()-1);
            sidebuff.resize(sidebuff.size()-1);
        }
        
        ++i;
    }
}

//--------------------------------------------------------------
Sphere::Sphere()
	: r(0.0)
{}


//--------------------------------------------------------------
Sphere::Sphere(const imp::Vector3& c, double r)
	: c(c)
	, r(r)
{}


//--------------------------------------------------------------
Triangle::Triangle()
{}

//--------------------------------------------------------------
Triangle::Triangle(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3)
	: p1(p1)
	, p2(p2)
	, p3(p3)
{}

//--------------------------------------------------------------
void Segment::operator=(const Segment& other)
{
	p1 = other.p1;
	p2 = other.p2;
}

//--------------------------------------------------------------
bool contains(const Sphere& sphere, const imp::Vector3& point)
{
	return (point - sphere.c).getNorm() < sphere.r;
}

//--------------------------------------------------------------
bool contains(const Sphere& sphere, const Triangle& tri)
{
	return contains(sphere, tri.p1) && contains(sphere, tri.p2) && contains(sphere, tri.p3);
}

//--------------------------------------------------------------
bool isOut(const Sphere& sphere, const Triangle& tri)
{
	return !contains(sphere, tri.p1) && !contains(sphere, tri.p2) && !contains(sphere, tri.p3);
}

//--------------------------------------------------------------
bool contains(const Sphere& sphere, const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3)
{
	return contains(sphere, p1) && contains(sphere, p2) && contains(sphere, p3);
}

//--------------------------------------------------------------
bool isOut(const Sphere& sphere, const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3)
{
	return !contains(sphere, p1) && !contains(sphere, p2) && !contains(sphere, p3);
}

//--------------------------------------------------------------
bool isOut(const Sphere& sphere, const imp::Vector3& point)
{
	return !contains(sphere, point);
}

//--------------------------------------------------------------
void segToLine(const Segment& seg, Line& line)
{
	line.p0 = seg.p1;
	line.dir = seg.p2 - seg.p1;
	line.dir.normalize();
}

//--------------------------------------------------------------
void triangleToPlane(const Triangle& tri, Plane& plane)
{
    imp::Vector3 p12 = tri.p2-tri.p1;
    imp::Vector3 p13 = tri.p3-tri.p1;
    p12.normalize();
    p13.normalize();
    
	plane.n = p12.crossProduct(p13);
	plane.n.normalize();
	plane.p0 = tri.p1;
	
}

//--------------------------------------------------------------
void triangleToSeg3(const Triangle& tri, Segment& s1, Segment& s2, Segment& s3)
{
	s1.p1 = tri.p1;
	s1.p2 = tri.p2;
	
	s2.p1 = tri.p2;
	s2.p2 = tri.p3;
	
	s3.p1 = tri.p3;
	s3.p2 = tri.p1;
}


//--------------------------------------------------------------
bool iLine2(const Line& l1, const Line& l2, imp::Vector3& iPoint)
{
	// p = p1 + t*v1
	// p = p2 + t2*v2
	// n(p-p2) dot v2 == 1 
	
	// n( p1+t*v1 - p2 ) dot v2 == 1
	// p_2 = (t * v1 + p1 - p2)
	// n(p_2) dot v2 == 1
	// p_2 dot v2 == |p_2|
	// (p_2 dot v2)^2 == p_2 dot p_2
	
	return false;
}

//--------------------------------------------------------------
bool iSegPlane( const Segment& seg, const Plane& plane, imp::Vector3& iPoint)
{
	// std::cout << "test iSegPlane" << std::endl;
	const float EPSILON = 1e-9;

	float d1 = plane.n.dotProduct(seg.p1 - plane.p0);
	float d2 = plane.n.dotProduct(seg.p2 - plane.p0);
	

	// same side
	if (d1*d2 > EPSILON)return false;

	// std::cout << "d1 = " << d1 <<"; d2 = " << d2 << std::endl;
	// not on line
	if(std::abs(d1) > EPSILON && std::abs(d2) > EPSILON )
	{
		float t = std::abs(d1) / ( std::abs(d1) + std::abs(d2) );
		iPoint = mix(seg.p1, seg.p2, t);

		// std::cout << "result iSegPlane : yes" << std::endl;
		return true;
	}
	else
	{
		// std::cout << "d1 = " << abs(d1) <<"; d2 = " << abs(d2) << std::endl;
		// std::cout << "result iSegPlane : no" << std::endl;
		return false;
	}
}

//--------------------------------------------------------------
bool iSegTriangle( const Segment& seg, const Triangle& tri, imp::Vector3& iv, IntersectionInfo& iinfo)
{
	const float EPSILON = 1e-9;
    
    Plane plane;
	triangleToPlane(tri,plane);
	imp::Vector3 iPoint;

    iinfo.side1 = false;
    iinfo.side2 = false;
    iinfo.side3 = false;
    
	if( iSegPlane(seg,plane,iPoint) )
	{   
		// demi-plane
		imp::Vector3 dp1 = plane.n.crossProduct( tri.p2 - tri.p1 );
		imp::Vector3 dp2 = plane.n.crossProduct( tri.p3 - tri.p2 );
		imp::Vector3 dp3 = plane.n.crossProduct( tri.p1 - tri.p3 );
        
        dp1.normalize();
        dp2.normalize();
        dp3.normalize();
        
        float idist1 = dp1.dotProduct(iPoint-tri.p1);
        float idist2 = dp2.dotProduct(iPoint-tri.p2);
        float idist3 = dp3.dotProduct(iPoint-tri.p3);
		
        
		if( idist1 > 0.0  &&  idist2 > 0.0 && idist3 > 0.0)
		{
			// temporary
			iv = iPoint;
			// std::cout << "result iSegTriangle : yes " << std::endl;
            
            iinfo.side1 = (idist1 <= EPSILON);
            iinfo.side2 = (idist2 <= EPSILON);
            iinfo.side3 = (idist3 <= EPSILON);
            
			return true;
		}
		/* else if( dp1.dotProduct(iPoint-tri.p1) < -EPSILON  &&  dp2.dotProduct(iPoint-tri.p2) < -EPSILON && dp3.dotProduct(iPoint-tri.p3) < -EPSILON)
        {
			// temporary
			iseg.p1 = iPoint;
			iseg.p2 = iPoint;
			// std::cout << "result iSegTriangle : yes " << std::endl;
			return true;
        }*/
		else
		{
			// std::cout << "result iSegTriangle : no " << std::endl;
			return false;
		}
	}
	else
	{
		// std::cout << "result iSegTriangle : no" << std::endl;
		return false;	
	}
}

//--------------------------------------------------------------
bool iTriangle2(const Triangle& tri1, const Triangle& tri2)
{
	Segment t1s1,  t1s2,  t1s3,  t2s1,  t2s2,  t2s3;
	triangleToSeg3(tri1,t1s1,t1s2,t1s3);
	triangleToSeg3(tri2,t2s1,t2s2,t2s3);
	
	imp::Vector3 p;
    IntersectionInfo iinfo;
    
	return (iSegTriangle(t1s1, tri2, p, iinfo)
        || iSegTriangle(t1s2, tri2, p, iinfo)
        || iSegTriangle(t1s3, tri2, p, iinfo)
        || iSegTriangle(t2s1, tri1, p, iinfo)
        || iSegTriangle(t2s2, tri1, p, iinfo)
        || iSegTriangle(t2s3, tri1, p, iinfo));
}

//--------------------------------------------------------------
bool iTriangle2(const Triangle& tri1, const Triangle& tri2, Segment& iseg)
{
    std::cout << "begin iTriangle2 test" << std::endl;
	Segment t1s1,  t1s2,  t1s3,  t2s1,  t2s2,  t2s3;
	triangleToSeg3(tri1,t1s1,t1s2,t1s3);
	triangleToSeg3(tri2,t2s1,t2s2,t2s3);
	
	std::vector<imp::Vector3> iPoints, iPoints2;
	
	imp::Vector3 p;
    IntersectionInfo curr1, curr2;
    IntersectionInfo iinfo;
    
    iinfo.side1 = false;
    iinfo.side2 = false;
    iinfo.side3 = false;
    
    int iCount = 0, iSelected = 0;
    
	if(iSegTriangle(t1s1, tri2, p, iinfo))
	{
        iCount ++;
        if( !(iinfo.side1 && curr1.side1) && !(iinfo.side2 && curr1.side2) && !(iinfo.side3 && curr1.side3) )
        {
            iSelected++;
            iPoints.push_back(p);
            if(iinfo.side1) curr1.side1 = true;
            if(iinfo.side2) curr1.side2 = true;
            if(iinfo.side3) curr1.side3 = true;
        }
	}
	if(iSegTriangle(t1s2, tri2, p, iinfo))
	{
        iCount ++;
        if( !(iinfo.side1 && curr1.side1) && !(iinfo.side2 && curr1.side2) && !(iinfo.side3 && curr1.side3) )
        {
            iSelected++;
            iPoints.push_back(p);
            if(iinfo.side1) curr1.side1 = true;
            if(iinfo.side2) curr1.side2 = true;
            if(iinfo.side3) curr1.side3 = true;
        }
	}
	if(iSegTriangle(t1s3, tri2, p, iinfo))
	{
        iCount ++;
        if( !(iinfo.side1 && curr1.side1) && !(iinfo.side2 && curr1.side2) && !(iinfo.side3 && curr1.side3) )
        {
            iSelected++;
            iPoints.push_back(p);
            if(iinfo.side1) curr1.side1 = true;
            if(iinfo.side2) curr1.side2 = true;
            if(iinfo.side3) curr1.side3 = true;
        }
	}
    
    std::cout << "tri1 - isegtri count = " << iCount << "; selected = " << iSelected << std::endl;
    
    if(iPoints.size() == 3)
    {
        // plane intersection
        return false;
    }
    else if(iPoints.size() == 2)
	{   
		std::cout << "just need one triangle test : ok" << std::endl;
		iseg.p1 = iPoints[0];
		iseg.p2 = iPoints[1];
        
		return true;
	}
    
    iinfo.side1 = false;
    iinfo.side2 = false;
    iinfo.side3 = false;
    
    iCount = 0; iSelected = 0;
    
	if(iSegTriangle(t2s1, tri1, p, iinfo))
	{
        iCount++;
        if( !(iinfo.side1 && curr2.side1) && !(iinfo.side2 && curr2.side2) && !(iinfo.side3 && curr2.side3) )
        {
            iSelected++;
            iPoints.push_back(p);
            iPoints2.push_back(p);
            if(iinfo.side1) curr2.side1 = true;
            if(iinfo.side2) curr2.side2 = true;
            if(iinfo.side3) curr2.side3 = true;
        }
	}
	if(iSegTriangle(t2s2, tri1, p, iinfo))
	{
        iCount++;
        if( !(iinfo.side1 && curr2.side1) && !(iinfo.side2 && curr2.side2) && !(iinfo.side3 && curr2.side3) )
        {
            iSelected++;
            iPoints.push_back(p);
            iPoints2.push_back(p);
            if(iinfo.side1) curr2.side1 = true;
            if(iinfo.side2) curr2.side2 = true;
            if(iinfo.side3) curr2.side3 = true;
        }
	}
	if(iSegTriangle(t2s3, tri1, p, iinfo))
	{
        iCount++;
        if( !(iinfo.side1 && curr2.side1) && !(iinfo.side2 && curr2.side2) && !(iinfo.side3 && curr2.side3) )
        {
            iSelected++;
            iPoints.push_back(p);
            iPoints2.push_back(p);
            if(iinfo.side1) curr2.side1 = true;
            if(iinfo.side2) curr2.side2 = true;
            if(iinfo.side3) curr2.side3 = true;
        }
	}
    
    std::cout << "tri1 - isegtri count = " << iCount << "; selected = " << iSelected << std::endl;
    
    if(iPoints2.size() == 3)
    {
        // plane intersection
        return false;
    }
    else if(iPoints2.size() == 2)
    {
		std::cout << "just need one triangle test (2) : ok" << std::endl;
		iseg.p1 = iPoints2[0];
		iseg.p2 = iPoints2[1];
        
		return true;
    }
	
	removeDuplicated(iPoints, 1e-9);
	
	if(iPoints.size() == 2)
	{   
		std::cout << "one intersect in each triangle : ok" << std::endl;
		iseg.p1 = iPoints[0];
		iseg.p2 = iPoints[1];
        
		return true;
	}
	else if(iPoints.size() == 1)
	{
		// std::cout << "result iTrangle2 : no" << std::endl;
		return false;
	}
	else if(iPoints.size() > 0)
    {
        std::cout << "too much iPoints - count = " << iPoints.size() << std::endl;
		iseg.p1 = iPoints[0];
		iseg.p2 = iPoints[1];
        return true;
    }
	else
	{
		// std::cout << "result iTrangle2 : no" << std::endl;
		return false;
	}
}

//--------------------------------------------------------------
void soloFirst(Triangle& tri1, const Triangle& tri2)
{
	Plane plane;
	triangleToPlane(tri2, plane);
	
	bool side1 = (tri1.p1-tri2.p1).dotProduct(plane.n) > 0.0;
	bool side2 = (tri1.p2-tri2.p1).dotProduct(plane.n) > 0.0;
	bool side3 = (tri1.p3-tri2.p1).dotProduct(plane.n) > 0.0;
	
	if(side2!=side1 && side2!=side3)
	{
		Triangle t(tri1.p2,tri1.p3,tri1.p1);
		tri1.p1 = t.p1;
		tri1.p2 = t.p2;
		tri1.p3 = t.p3;
	}
	else if(side3!=side1 && side3!=side2)
	{
		Triangle t(tri1.p3,tri1.p1,tri1.p2);
		tri1.p1 = t.p1;
		tri1.p2 = t.p2;
		tri1.p3 = t.p3;
	}
	else if(side1 == side2 && side2 == side3)
    {
        std::cout << "equal side" << std::endl;
    }
}

//--------------------------------------------------------------
void soloFirst(Triangle& tri1, const Segment& seg)
{
    Plane plane;
	triangleToPlane(tri1, plane);
    
    imp::Vector3 tan = seg.p2 - seg.p1;
    tan.normalize();
    
    imp::Vector3 bitan = plane.n.crossProduct( tan );
    bitan.normalize();
	
	bool side1 = (tri1.p1-seg.p1).dotProduct(bitan) > 0.0;
	bool side2 = (tri1.p2-seg.p1).dotProduct(bitan) > 0.0;
	bool side3 = (tri1.p3-seg.p1).dotProduct(bitan) > 0.0;
	
	if(side2!=side1 && side2!=side3)
	{
		Triangle t(tri1.p2,tri1.p3,tri1.p1);
		tri1.p1 = t.p1;
		tri1.p2 = t.p2;
		tri1.p3 = t.p3;
	}
	else if(side3!=side1 && side3!=side2)
	{
		Triangle t(tri1.p3,tri1.p1,tri1.p2);
		tri1.p1 = t.p1;
		tri1.p2 = t.p2;
		tri1.p3 = t.p3;
	}
	else if(side1 == side2 && side2 == side3)
    {
        std::cout << "equal side" << std::endl;
    }
}

//--------------------------------------------------------------
void extendsSegInTriangle(const Triangle& tri, Segment& seg)
{
	Plane plane;
	triangleToPlane(tri,plane);
    
    imp::Vector3 p1s1 = seg.p1 - tri.p1;
    imp::Vector3 p1s2 = seg.p2 - tri.p1;
    p1s1.normalize();
    p1s2.normalize();
    
    imp::Vector3 s12Dir = p1s1.crossProduct(p1s2);
    s12Dir.normalize();
    /*
    if(s12Dir.dotProduct(plane.n) < 0.0)
    {
        imp::Vector3 tp = seg.p1;
        seg.p1 = seg.p2;
        seg.p2 = tp;
    }*/
	
	imp::Vector3 s1p1 = tri.p1 -seg.p1;
	imp::Vector3 s1p2 = tri.p2 -seg.p1;
	imp::Vector3 s1p3 = tri.p3 -seg.p1;
    
	imp::Vector3 tan = seg.p2-seg.p1;
	tan.normalize();
	imp::Vector3 bitan = plane.n.crossProduct( tan );
	bitan.normalize();
    
	float d1 = std::abs(bitan.dotProduct(s1p1));
	float d2 = std::abs(bitan.dotProduct(s1p2));
	float d3 = std::abs(bitan.dotProduct(s1p3));	
    
    
    float t2 = d1 / (d1 + d2);
    //t2 = t2<0.0?0.0:(t2<1.0?1.0:t2);
    seg.p1 = mix(tri.p1, tri.p2, t2);
    
    float t3 = d1 / (d1 + d3);
    //t3 = t3<0.0?0.0:(t3>1.0?1.0:t3);
    seg.p2 = mix(tri.p1, tri.p3, t3);
    
    
	if(tan.dotProduct(s1p2) > tan.dotProduct(s1p3))
	{
        imp::Vector3 t = seg.p1;
        seg.p1 = seg.p2;
        seg.p2 = t;
	}
}

//--------------------------------------------------------------
void VertexBuffer::remove(int index, int count)
{
    for(unsigned int i=index; i<_data.size()-count; ++i)
    {
        _data[i] = _data[i+count];
    }
    
    _data.resize(_data.size()-count);
}

//--------------------------------------------------------------
void VertexBuffer::remove(const Triangle& triangle)
{
	remove(triangle.p1, triangle.p2, triangle.p3);
}

//--------------------------------------------------------------
void VertexBuffer::remove(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3)
{
	bool found = false;
	unsigned int index = 0;
    for(unsigned int i=0; i<_data.size(); i+=3)
    {
        if(_data[i+0]==p1 && _data[i+1]==p2 && _data[i+2]==p3)
        {
            found = true;
            index = i;
			break;
        }
    }
    
	if(found)
    {
        remove(index,3);
    }
    else
    {
        std::cout << "remove failed" << std::endl;
    }
}

//--------------------------------------------------------------
void VertexBuffer::push(const Triangle& triangle)
{
	push(triangle.p1,triangle.p2,triangle.p3);
}

//--------------------------------------------------------------
void VertexBuffer::push(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3)
{
    push(p1);
    push(p2);
    push(p3);
}

//--------------------------------------------------------------
void VertexBuffer::push(const imp::Vector3& point)
{
    _data.push_back(point);
}

//--------------------------------------------------------------
VertexBuffer intersect(Sphere& desc1, VertexBuffer& solid1, Sphere& desc2, VertexBuffer& solid2)
{
	VertexBuffer result;
	
	for(unsigned int i=0; i<solid1._data.size(); i+=3)
	{
        if(contains(desc2, solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]))
        {
			result.push(solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]);
        }
	}
    for(unsigned int i=0; i<solid2._data.size(); i+=3)
    {
        if(contains(desc1, solid2._data[i+0],solid2._data[i+1],solid2._data[i+2]))
        {
            result.push(solid2._data[i+0],solid2._data[i+1],solid2._data[i+2]);
        }
    }
	
	return result;
}

//--------------------------------------------------------------
VertexBuffer substract(Sphere& desc1, VertexBuffer& solid1, Sphere& desc2, VertexBuffer& solid2)
{
	VertexBuffer result, inter1, inter2;
	
    std::cout << "start substract" << std::endl;
    
    std::cout << "split solid1" << std::endl;
	/*for(unsigned int i=0; i<solid1._data.size(); i+=3)
	{
        
        if(isOut(desc2, solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]))
        {
            //result.push(solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]);
        }
        else if(!contains(desc2, solid1._data[i+0],solid1._data[i+1],solid1._data[i+2])) // need to test here triangle intersection
        {
            inter1.push(solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]);
        }
	}
    std::cout << "split solid2" << std::endl;
    for(unsigned int i=0; i<solid2._data.size(); i+=3)
    {
        if(contains(desc1, solid2._data[i+0],solid2._data[i+1],solid2._data[i+2]))
        {
            //result.push(solid2._data[i+0],solid2._data[i+2],solid2._data[i+1]); // inversed
        }
        else if(!isOut(desc1, solid2._data[i+0],solid2._data[i+1],solid2._data[i+2]))  // need to test here triangle intersection
        {
            inter2.push(solid2._data[i+0],solid2._data[i+2],solid2._data[i+1]);
        }
    }*/
    
	for(unsigned int i=0; i<solid1._data.size(); i+=3)
    {
        /*if(solid1._data[i+0].getX() > 0.0 && solid1._data[i+0].getY() > 0.0 && solid1._data[i+0].getZ() > 0.0
        && solid1._data[i+1].getX() > 0.0 && solid1._data[i+1].getY() > 0.0 && solid1._data[i+1].getZ() > 0.0
        && solid1._data[i+2].getX() > 0.0 && solid1._data[i+2].getY() > 0.0 && solid1._data[i+2].getZ() > 0.0)*/
        {
            std::vector<int> already;
            Triangle tri1(solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]);
            
            for(unsigned int j=0; j<solid2._data.size(); j+=3)
            {            
                /*if(solid2._data[j+0].getX() > 0.0 && solid2._data[j+0].getY() > 0.0 && solid2._data[j+0].getZ() > 0.0
                && solid2._data[j+1].getX() > 0.0 && solid2._data[j+1].getY() > 0.0 && solid2._data[j+1].getZ() > 0.0
                && solid2._data[j+2].getX() > 0.0 && solid2._data[j+2].getY() > 0.0 && solid2._data[j+2].getZ() > 0.0)*/
                {        
                    if( contains(already, j) )
                    {
                        continue;
                    }
                    
                    Triangle tri2(solid2._data[j+0],solid2._data[j+1],solid2._data[j+2]);
                    if(iTriangle2(tri1, tri2))
                    {
                        already.push_back(j); // continue on second loop
                        inter1.push(tri1);
                        inter2.push(tri2);
                        break; // continue on first loop
                    }
                }
            }
        }
    }
    
    scissors(inter1, inter2);
	
    for(unsigned int i=0; i<inter1._data.size(); i+=3)
	{
		/*imp::Vector3 middle = (inter1._data[i+0] + inter1._data[i+1] + inter1._data[i+2]) / 3.0;
        if(isOut(desc2, middle))*/
        
        int score = 0;
        for(unsigned int s=0; s<3; ++s)
            if(isOut(desc2, inter1._data[i+s])) score++;
        
        //if(score >= 1)
        if(true)
        {
            result.push(inter1._data[i+0], inter1._data[i+1], inter1._data[i+2]);
        }
	}
    for(unsigned int i=0; i<inter2._data.size(); i+=3)
    {
		/*imp::Vector3 middle = (inter2._data[i+0] + inter2._data[i+1] + inter2._data[i+2]) / 3.0;
        if(contains(desc1, middle))*/
        
        int score = 0;
        for(unsigned int s=0; s<3; ++s)
            if(isOut(desc1, inter2._data[i+s])) score++;
        
        //if(score >= 1)
        if(true)
        {
            result.push(inter2._data[i+0], inter2._data[i+2], inter2._data[i+1]); // inversed
        }
    }
    
	return result;
}

//--------------------------------------------------------------
VertexBuffer merge(Sphere& desc1, VertexBuffer& solid1, Sphere& desc2, VertexBuffer& solid2)
{
	VertexBuffer result;
	
	for(unsigned int i=0; i<solid1._data.size(); i+=3)
	{
        if(isOut(desc2, solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]))
        {
			result.push(solid1._data[i+0],solid1._data[i+1],solid1._data[i+2]);
        }
	}
    for(unsigned int i=0; i<solid2._data.size(); i+=3)
    {
        if(isOut(desc1, solid2._data[i+0],solid2._data[i+1],solid2._data[i+2]))
        {
			result.push(solid2._data[i+0],solid2._data[i+1],solid2._data[i+2]);
        }
    }
	
	return result;
}

//--------------------------------------------------------------
bool contains(std::vector<int>& buff, int v)
{
    for(unsigned int i=0; i<buff.size();++i)
    {
        if(buff[i] == v)
            return true;
    }
    return false;
}

//--------------------------------------------------------------
void scissors(VertexBuffer& buffer1, VertexBuffer& buffer2)
{
    std::cout << "start scissor" << std::endl;
    bool hasIntersection = true;
      
    unsigned int maxIt = 2;
    while(hasIntersection)//  && maxIt > 0)
    //for(unsigned int x=0; x<1; ++x)
    {
        maxIt--;
        VertexBuffer toAdd1, toAdd2, toRem1, toRem2;
        std::vector<int> alreadyIntersected2;
     
        hasIntersection = false;
        int iCount = 0;
      
        // scanning and looking for intersections
        for(unsigned int i=0;i<buffer1._data.size();i+=3)
        {
            for(unsigned int j=0;j<buffer2._data.size();j+=3)
            {
                if(contains(alreadyIntersected2, j))
                    continue;
                
                Triangle tri1(buffer1._data[i+0],buffer1._data[i+1],buffer1._data[i+2]);
                Triangle tri2(buffer2._data[j+0],buffer2._data[j+1],buffer2._data[j+2]);
                Segment iSeg;
                
				// triangle intersect test
                if(iTriangle2(tri1,tri2,iSeg))
                {	
                    Triangle tta;
                    
					soloFirst(tri1,iSeg);
					soloFirst(tri2,iSeg);
                    
                    Plane plane1, plane2;
                    triangleToPlane(tri1,plane1);
                    triangleToPlane(tri2,plane2);
					
					// here, need to extend segment to triangle border
					Segment segOnTri1 = iSeg;
					extendsSegInTriangle(tri1, segOnTri1);
                    // split triangle 1
                    toRem1.push(buffer1._data[i+0],buffer1._data[i+1],buffer1._data[i+2]);
                    
                    tta = normalizeTriangle(tri1.p1, segOnTri1.p1, segOnTri1.p2, plane1.n);
                    //if(maxIt > 0)
                    toAdd1.push(tta);
                    tta = normalizeTriangle(segOnTri1.p1, tri1.p2, segOnTri1.p2, plane1.n);
                    //if(maxIt > 0)
                    toAdd1.push(tta);
                    tta = normalizeTriangle(segOnTri1.p1, tri1.p2, tri1.p3, plane1.n);
                    //if(maxIt > 0)
                    toAdd1.push(tta );
					
					// here, need to extend segment to triangle border
					Segment segOnTri2 = iSeg;
					extendsSegInTriangle(tri2, segOnTri2);
                    // split triangle 2
                    toRem2.push(buffer2._data[j+0],buffer2._data[j+1],buffer2._data[j+2]);
                    
                    tta = normalizeTriangle(tri2.p1, segOnTri2.p1, segOnTri2.p2, plane2.n);
                    // if(maxIt > 0)
                    toAdd2.push(tta);
                    tta = normalizeTriangle(segOnTri2.p1, tri2.p2, segOnTri2.p2, plane2.n);
                    // if(maxIt > 0)
                    toAdd2.push(tta);
                    tta = normalizeTriangle(segOnTri2.p1, tri2.p2, tri2.p3, plane2.n);
                    //if(maxIt > 0)
                    toAdd2.push(tta);
					
                    hasIntersection = true;
                    iCount++;
                    alreadyIntersected2.push_back(j);
                    break;
                }
            }
        }
        
        std::cout << "scissor iteration : detected " << iCount << " intersections" << std::endl;
        
        // do transactions
        for(unsigned int i=0; i<toRem1._data.size(); i+=3)
        {
            buffer1.remove(toRem1._data[i+0], toRem1._data[i+1], toRem1._data[i+2]);
        }
        for(unsigned int i=0; i<toRem2._data.size(); i+=3)
        {
            buffer2.remove(toRem2._data[i+0], toRem2._data[i+1], toRem2._data[i+2]);
        }
        for(unsigned int i=0; i<toAdd1._data.size(); i+=3)
        {
            buffer1.push(toAdd1._data[i],toAdd1._data[i+1],toAdd1._data[i+2]);
        }
        for(unsigned int i=0; i<toAdd2._data.size(); i+=3)
        {
            buffer2.push(toAdd2._data[i],toAdd2._data[i+1],toAdd2._data[i+2]);
        }
    }
}


Triangle normalizeTriangle(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3, const imp::Vector3& n)
{
    imp::Vector3 p1p2 = p2-p1;
    imp::Vector3 p1p3 = p3-p1;
    
    Triangle tri;
    tri.p1 = p1;
    tri.p2 = p2;
    tri.p3 = p3;
    
    if( p1p2.crossProduct(p1p3).dotProduct(n) < 0.0)
    {
        tri.p1 = p1;
        tri.p2 = p3;
        tri.p3 = p2;
    }
    
    return tri;
}


