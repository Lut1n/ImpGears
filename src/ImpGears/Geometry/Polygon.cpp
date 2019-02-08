#include <Geometry/Polygon.h>

#include <map>
#include <cstdlib>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Polygon::Polygon()
{
}

//--------------------------------------------------------------
Polygon::Polygon(const Path& path)
{
	_path = path;
}

//--------------------------------------------------------------
Polygon::~Polygon()
{
}

//--------------------------------------------------------------
void Polygon::operator+=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] += mv;
}

//--------------------------------------------------------------
void Polygon::operator-=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] -= mv;
}

//--------------------------------------------------------------
void Polygon::operator*=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] *= mv;
}

//--------------------------------------------------------------
void Polygon::operator/=(const Vec3& mv)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] /= mv;
}

//--------------------------------------------------------------
void Polygon::rotation(float rad)
{
	int N = _path.count();
	for(int i=0;i<N;++i) _path.vertices[i] *= imp::Matrix3::rotationZ(rad);
}

//--------------------------------------------------------------
Vec3 Polygon::tan(int i) const
{
	Vec3 dir = _path.vertex(i)-_path.vertex(i-1); dir.normalize();
	return dir;
}

//--------------------------------------------------------------
Edge Polygon::edge(int i) const
{
	return Edge(_path.vertex(i-1),_path.vertex(i));
}

//--------------------------------------------------------------
bool Polygon::inside(const Vec3& v) const
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

//--------------------------------------------------------------
bool Polygon::inside(const Polygon& c) const
{
	for(auto v: c._path.vertices)if(inside(v)==false)return false;
	return true;
}

//--------------------------------------------------------------
Vec3 Polygon::gravity() const
{
	Vec3 total;
	for(auto v:_path.vertices)total+=v;
	return total/_path.count();
}

//--------------------------------------------------------------
Vec3 Polygon::findNextByAngle(const Edge& curr, const Vec3& tangent, bool maxi) const
{
	std::vector<Vec3> cnx = _path.getConnexes(curr._p2);
	std::map<float,Vec3> found;
	for(auto v : cnx)
	{
		if(v == curr._p1)continue;
		float angle = Vec3(v-curr._p2).angleFrom(tangent);
		found[angle] = v;
	}
	
	Vec3 best = found.begin()->second;
	if(maxi) best = found.rbegin()->second;
	
	return best;
}

//--------------------------------------------------------------
Polygon Polygon::boundary() const
{
	Path result;
	Vec3 first = leftExtremity();
	
	Edge currEdge(first,first);
	Vec3 tan = imp::Vec3::X;
	do
	{
		result.addVertex(currEdge._p2);
		Vec3 next = findNextByAngle(currEdge, tan, true);
		currEdge = Edge(currEdge._p2,next);
		tan = currEdge._p2 - currEdge._p1;
	}
	while(currEdge._p2 != first);
	
	return Polygon(result);
}

//--------------------------------------------------------------
Polygon Polygon::simplify() const
{
	Path cpy = _path;
	Intersection::selfResolve(cpy);
	return Polygon(cpy).boundary();
}

//--------------------------------------------------------------
Vec3 Polygon::leftExtremity() const
{
	Vec3 res = _path.vertices[0];
	for(auto v:_path.vertices)if(v[0]<res[0])res=v;
	return res;
}

//--------------------------------------------------------------
Polygon Polygon::extractTriangle()
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
	
	return Polygon(tri);
}

//--------------------------------------------------------------
std::vector<Polygon> Polygon::triangulate() const
{
	Polygon cpy = *this;
	std::vector<Polygon> res;
	while(cpy._path.count() != 0)
	{
		Polygon extracted;
		if(cpy._path.count()==3){extracted=cpy; cpy=Polygon();}
		else extracted = cpy.extractTriangle();
		res.push_back(extracted);
	}
	return res;
}

//--------------------------------------------------------------
int Polygon::windingNumber() const
{
	float rad = 0.0;
	for(int i=0;i<_path.count();++i)rad += tan(i).angleFrom(tan(i-1));
	return rad/(2*3.141592);
}

//--------------------------------------------------------------
void Polygon::reverse()
{
	Path cpy = _path;
	_path = Path();
	for(auto it=cpy.vertices.rbegin();it!=cpy.vertices.rend();it++)_path.vertices.push_back(*it);
}

IMPGEARS_END
