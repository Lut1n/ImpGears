#include "SimplePolygon.h"

#include <map>
#include <cstdlib>

SimplePolygon::SimplePolygon()
{
}

SimplePolygon::SimplePolygon(const Path& path)
{
	_path = path;
}

SimplePolygon::~SimplePolygon()
{
}

void SimplePolygon::operator+=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] += mv;
}

void SimplePolygon::operator-=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] -= mv;
}

void SimplePolygon::operator*=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] *= mv;
}

void SimplePolygon::operator/=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] /= mv;
}

void SimplePolygon::rotation(float rad)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] *= imp::Matrix3::rotationZ(rad);
}

Vec3 SimplePolygon::tan(int i) const
{
	Vec3 dir = _path.vertex(i)-_path.vertex(i-1); dir.normalize();
	return dir;
}

Edge SimplePolygon::edge(int i) const
{
	return Edge(_path.vertex(i-1),_path.vertex(i));
}

bool SimplePolygon::contains(const Vec3& v) const
{
	// Vec3(std::rand(),std::rand(),0.0);
	
	int cpt = 0;
	
	Vec3 ext = leftExtremity() - Vec3(10.0,10.0,0.0); //Vec3((double)std::rand()/RAND_MAX,(double)std::rand()/RAND_MAX,0.0)*10.0;
	Edge ray(ext,v);
	for(int i=0;i<_path.count();++i)
	{
		Intersection inter(edge(i), ray);
		if(inter.compute())++cpt;
	}
	
	return (cpt%2)>0;
}

bool SimplePolygon::contains(const SimplePolygon& c) const
{
	for(auto v: c._path.vertices)if(contains(v)==false)return false;
	return true;
}

Vec3 SimplePolygon::gravity() const
{
	Vec3 total;
	for(auto v:_path.vertices)total+=v;
	return total/_path.count();
}

Vec3 SimplePolygon::findNextByAngle(const Edge& curr, const Vec3& tangent, bool maxi) const
{
	std::vector<Vec3> cnx = _path.getConnexes(curr.p2);
	std::map<float,Vec3> found;
	for(auto v : cnx)
	{
		if(v == curr.p1)continue;
		float angle = Vec3(v-curr.p2).angleFrom(tangent);
		found[angle] = v;
	}
	
	Vec3 best = found.begin()->second;
	if(maxi) best = found.rbegin()->second;
	
	return best;
}

SimplePolygon SimplePolygon::boundary() const
{
	Path result;
	Vec3 first = leftExtremity();
	
	Edge currEdge(first,first);
	Vec3 tan = imp::Vec3::X;
	do
	{
		result.addVertex(currEdge.p2);
		Vec3 next = findNextByAngle(currEdge, tan, true);
		currEdge = Edge(currEdge.p2,next);
		tan = currEdge.p2 - currEdge.p1;
	}
	while(currEdge.p2 != first);
	
	return SimplePolygon(result);
}


SimplePolygon SimplePolygon::simplify() const
{
	Path cpy = _path;
	Intersection::selfResolve(cpy);
	return SimplePolygon(cpy).boundary();
}

Vec3 SimplePolygon::leftExtremity() const
{
	Vec3 res = _path.vertices[0];
	for(auto v:_path.vertices)if(v[0]<res[0])res=v;
	return res;
}

SimplePolygon SimplePolygon::extractTriangle()
{
	std::vector<int> deg = _path.degrees();
	std::map<float,int> found;
	for(int i=0;i<_path.count();++i)
	{
		int i1 = _path.cycleIndex(i-2);
		int i2 = _path.cycleIndex(i-1);
		int i3 = _path.cycleIndex(i);
		bool bridge = deg[i2]>2;
		// bool bridgeA = deg[i]>2;
		// bool bridgeB = deg[i3]>2;
		if(bridge)continue;// || (bridgeA && bridgeB)) continue;
		
		Edge edge(_path.vertex(i1),_path.vertex(i3));
		if(Intersection::isCrossing(_path,edge))continue;
		
		Vec3 ref = tan(i2);
		float na = tan(i3).angleFrom(ref);
		found[na]=i3;
	}
	
	int best = found.begin()->second;
	
	std::vector<Vec3> tri={_path.vertex(best),_path.vertex(best-1),_path.vertex(best-2)};
	
	if(_path.areConnected(tri[0],tri[1],tri[2])) _path.erase({best,best-1,best-2});
	else _path.erase({best-1});
	
	return SimplePolygon(tri);
}

std::vector<SimplePolygon> SimplePolygon::triangulate() const
{
	SimplePolygon cpy = *this;
	std::vector<SimplePolygon> res;
	while(cpy._path.count() != 0)
	{
		SimplePolygon extracted;
		if(cpy._path.count()==3){extracted=cpy; cpy=SimplePolygon();}
		else extracted = cpy.extractTriangle();
		res.push_back(extracted);
	}
	return res;
}

int SimplePolygon::windingNumber() const
{
	float rad = 0.0;
	for(int i=0;i<_path.count();++i)rad += tan(i).angleFrom(tan(i-1));
	return rad/(2*3.141592);
}

void SimplePolygon::reverse()
{
	Path cpy = _path;
	_path = Path();
	for(auto it=cpy.vertices.rbegin();it!=cpy.vertices.rend();it++)_path.vertices.push_back(*it);
}
