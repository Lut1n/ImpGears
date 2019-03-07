#include "Triangulator.h"

IMPGEARS_BEGIN

std::string asstring(const Vec3& v)
{
	std::stringstream ss;
	ss << "[" << v[0] << ";" << v[1] << ";" << v[2] << "]";
	return ss.str();
}

//--------------------------------------------------------------
struct Triplet
{
	Vec3 p1,p2,p3;
	Triplet(){}
	Triplet(Vec3 p1,Vec3 p2,Vec3 p3) : p1(p1),p2(p2),p3(p3){}
	bool contains(Vec3 p) const	{return p==p1||p==p2||p==p3;}
	bool operator==(const Triplet& other) const	{return contains(other.p1) && contains(other.p2) && contains(other.p3);}
	bool connectedTo(const Triplet& other) const
	{
		bool test1 = contains(other.p1) && contains(other.p2);
		bool test2 = contains(other.p1) && contains(other.p3);
		bool test3 = contains(other.p2) && contains(other.p3);
		
		return test1 || test2 || test3;
	}
	
	bool flipIfNeed(Triplet& other)
	{
		int c=0, o=0;
		Vec3 common[2], opposite[2];
		if(contains(other.p1)) common[c++]=other.p1;else opposite[o++]=other.p1;
		if(contains(other.p2)) common[c++]=other.p2;else opposite[o++]=other.p2;
		if(contains(other.p3)) common[c++]=other.p3;else opposite[o++]=other.p3;
		
		if(!other.contains(p1)) opposite[o++]=p1;
		if(!other.contains(p2)) opposite[o++]=p2;
		if(!other.contains(p3)) opposite[o++]=p3;
		
		Vec3 tav1 = common[0] - opposite[0];
		Vec3 tav2 = common[1] - opposite[0];
		Vec3 tbv1 = common[0] - opposite[1];
		Vec3 tbv2 = common[1] - opposite[1];
		
		float alpha = std::abs( tav1.angleFrom(tav2) );
		float beta = std::abs( tbv1.angleFrom(tbv2) );
		
		float TT = alpha + beta;
		const static float EPSILON = 0.01;
		if(TT > 3.141592 + EPSILON)
		{
			// Flip diagonals
			p1 = opposite[0];
			p2 = opposite[1];
			p3 = common[0];
			other.p1 = opposite[0];
			other.p2 = opposite[1];
			other.p3 = common[1];
			return true;
		}
		return false;
	}
};

//--------------------------------------------------------------
using TriBuf = std::vector<Triplet>;
using TriIt = TriBuf::iterator;

//--------------------------------------------------------------
std::vector<int> findAdjacents(TriBuf& buf, int t)
{
	std::vector<int> res;
	for(int i=0;i<(int)buf.size();++i)
		if( i!=t && buf[i].connectedTo(buf[t])) res.push_back(i);
	return res;
}

//--------------------------------------------------------------
void delaunay(TriBuf& buf)
{
	std::vector<bool> flags(buf.size(),true);
	auto flg_it = flags.begin();
	
	while( flg_it != flags.end() )
	{
		int t1 = (flg_it-flags.begin());
		int t2 = -1;
		std::vector<int> adj_t1 = findAdjacents(buf,t1);
		
		for(auto it:adj_t1)
			if( buf[t1].flipIfNeed(buf[it]) ) { t2=it; break; }
			
		if(t2 != -1)
		{
			std::vector<int> adj_t1 = findAdjacents(buf,t1);
			std::vector<int> adj_t2 = findAdjacents(buf,t2);
			for(auto it:adj_t1) flags[it]=true;
			for(auto it:adj_t2) flags[it]=true;
			
			flags[t2] = false;
		}
		
		flags[t1] = false;
		
		flg_it = std::find(flags.begin(),flags.end(),true);
	}
}

//--------------------------------------------------------------
Polygon edges2Triangles(const std::vector<Edge>& edges)
{
	std::vector<Triplet> buf;
	
	for(auto e1:edges)
	{
		for(auto e2:edges)
		{
			if(e1 == e2)continue;
			if(!e1.connectedTo(e2)) continue;
			
			Edge e3;
			if(e1._p1==e2._p1) e3 = Edge(e1._p2,e2._p2);
			else if(e1._p1==e2._p2) e3 = Edge(e1._p2,e2._p1);
			else if(e1._p2==e2._p1) e3 = Edge(e1._p1,e2._p2);
			else if(e1._p2==e2._p2) e3 = Edge(e1._p1,e2._p1);
			
			bool fnd = std::find(edges.begin(),edges.end(),e3) != edges.end();
			if(!fnd)continue;
			
			Triplet t; t.p1=e1._p1;t.p2=e1._p2;
			if(e2._p1==e1._p1 || e2._p1==e1._p2)t.p3=e2._p2;
			else t.p3=e2._p1;
			
			if(std::find(buf.begin(),buf.end(),t)==buf.end())buf.push_back(t);
		}
	}
	/*
	std::stringstream ss;
	ss << "triangles :" << std::endl;
	for(auto t : buf)
		ss << asstring(t.p1) << "; " << asstring(t.p2) << "; " << asstring(t.p3) << std::endl;
	Triangulation::s_lastlog = ss.str();

	delaunay(buf);
	*/
	// Polygon res;
	PathSet res;
	for(auto t:buf)
	{
		Path p;p.addVertex(t.p1);p.addVertex(t.p2);p.addVertex(t.p3);
		// res.addPath(p);
		res.push_back(p);
	}
	Polygon poly; poly.addPaths(res);
	
	return poly;
}

//--------------------------------------------------------------
/*bool inside(const std::vector<Vec3>& tri, const Vec3& pt)
{
	Vec3 p12 = tri[1]-tri[0];
	Vec3 p13 = tri[2]-tri[0];
	Vec3 n = p12.cross(p13); n.normalize();
	
	Vec3 dp1 = n.cross( tri[1]-tri[0] );
	Vec3 dp2 = n.cross( tri[2]-tri[1] );
	Vec3 dp3 = n.cross( tri[0]-tri[2] );
	
	float idist1 = dp1.dot(pt-tri[0]);
	float idist2 = dp2.dot(pt-tri[1]);
	float idist3 = dp3.dot(pt-tri[2]);
	return idist1>0.0 && idist2>0.0 && idist3>0.0;
}*/

//--------------------------------------------------------------
std::vector<Edge> points2Edges(const std::vector<Vec3>& pts, Vec3 n,const std::vector<Edge> constrained)
{
	std::vector<Edge> edges = constrained;
	std::vector<Vec3> points = pts;
	
	for(int i=0;i<(int)points.size();++i)
		for(int j=0;j<(int)points.size();++j)
	{
		if(i==j) continue;
		
		Edge e(points[i], points[j]);

		// check if edge is not already selected
		auto found = std::find(edges.begin(),edges.end(),e);
		if(found != edges.end())continue;
		
		// check if edge is not intersecting with other edges
		bool xPoly = false;
		for(auto e2:edges)
		{
			if(e.connectedTo(e2))continue;
			if(Intersection(e,e2).compute()){xPoly=true;break;}
		}
		if(xPoly)continue;
		
		edges.push_back(e);
	}
	
	return edges;
}

//--------------------------------------------------------------
Polygon Triangulation::triangulate(const Polygon& poly)
{
	return poly;
}

std::string Triangulation::s_lastlog;

//--------------------------------------------------------------
std::vector<Vec3> Triangulation::triangulate(const std::vector<Vec3>& buf, Vec3 n,const std::vector<Edge> constrained)
{
	// std::stringstream log;
	
	std::vector<Edge> edges = points2Edges(buf,n, constrained);
	// for(auto e : constrained)edges.push_back(e);
	// log << "computed edges : " << std::endl;
	// for(auto e : edges) log << asstring(e._p1) << "; " << asstring(e._p2) << std::endl;
	Polygon respoly = edges2Triangles(edges);
	std::vector<Vec3> res;
	for(auto p : respoly._paths)
	{
		for(auto v : p.vertices) res.push_back(v);
	}
	
	// s_lastlog = log.str();
	
	return res;
}

IMPGEARS_END
