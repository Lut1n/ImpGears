#include "Path.h"

#include <map>
#include <cstdlib>

std::string asstring(const Vec3& v) {std::stringstream s; s << "["<<v[0]<<";"<<v[1]<<";"<<v[2]<<"]"; return s.str();}


Path::Path()
{
}

Path::Path(const BufType& vert)
{
	vertices = vert;
}

Path::~Path()
{
}

float sign(float a)
{
	return a<0.0?-1.0:1.0;
}

void Path::addVertex(Vec3 vt)
{
	vertices.push_back(vt);
}

void Path::addVertices(const BufType& toInsert)
{
	for(auto v:toInsert)addVertex(v);
}

void Path::addFrom(const Path& cycle)
{
	addVertices(cycle.vertices);
}

Path Path::subPath(int from, int to, bool reverse) const
{
	Path res;
	int inc = reverse?-1:1;
	for(int i=from; i!=to; i+=inc)
	{
		i=cycleIndex(i);if(i==to)break;
		res.addVertex(vertex(i));
	}
	res.addVertex(vertex(to));
	return res;
}

int Path::cycleIndex(int i) const
{
	int N = count();
	while(i>=N)i-=N;
	while(i<0)i+=N;
	return i;
}

Vec3& Path::vertex(int i)
{
	return vertices[cycleIndex(i)];
}

const Vec3& Path::vertex(int i) const
{
	return vertices[cycleIndex(i)];
}

int Path::index(const Vec3& v) const
{
	auto it = std::find(vertices.begin(),vertices.end(),v);
	return it-vertices.begin();
}

Vec3 Path::tan(int i) const
{
	Vec3 dir = vertex(i)-vertex(i-1); dir.normalize();
	return dir;
}

Edge Path::edge(int i) const
{
	return Edge(vertex(i-1),vertex(i));
}

int Path::count() const
{
	return vertices.size();
}

void Path::dump() const
{
	std::cout << "dump polygon :" << std::endl;
	for(auto vert : vertices)std::cout << asstring(vert) << std::endl;
}

bool Path::composes(const Vec3& v) const
{
	return std::find(vertices.begin(),vertices.end(),v) != vertices.end();
}

Path::BufType Path::getConnexes(const Vec3& v) const
{
	std::vector<Vec3> res;
	for(int i=0;i<count();++i)
	{
		Vec3 a=vertex(i-2), b=vertex(i-1), c=vertex(i);
		if(b != v) continue;
		if(std::find(res.begin(),res.end(),a)==res.end())res.push_back(a);
		if(std::find(res.begin(),res.end(),c)==res.end())res.push_back(c);
	}
	
	return res;
}

int Path::getEdgeCnt(const Vec3& v) const
{
	BufType vert = getConnexes(v);
	return vert.size();
}

Vec3 Path::previous(const Vec3& v) const
{
	return vertex(index(v)-1);
}

Vec3 Path::next(const Vec3& v) const
{
	return vertex(index(v)+1);
}

Vec3 Path::gravity() const
{
	Vec3 total;
	for(auto v:vertices)total+=v;
	return total/count();
}

std::vector<int> Path::degrees() const
{
	std::vector<int> res;
	for(auto v:vertices) res.push_back(getConnexes(v).size());
	return res;
}

void Path::erase(std::vector<int> toErase)
{
	for(auto& i : toErase)i=cycleIndex(i);
	std::sort(toErase.begin(),toErase.end(),std::greater<int>());
	for(auto e : toErase)vertices.erase(vertices.begin()+e);
}

Vec3 Path::findNextByAngle(const Edge& curr, const Vec3& tangent, bool maxi) const
{
	std::vector<Vec3> cnx = getConnexes(curr.p2);
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

Path Path::boundary() const
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
	
	return result;
}


Path Path::simplify() const
{
	Path cpy = *this;
	Intersection::selfResolve(cpy);
	return cpy.boundary();
}

Vec3 Path::leftExtremity() const
{
	Vec3 res = vertices[0];
	for(auto v:vertices)if(v[0]<res[0])res=v;
	return res;
}

bool Path::areConnected(const Vec3& v1, const Vec3& v2) const
{
	std::vector<Vec3> co=getConnexes(v1);
	return std::find(co.begin(),co.end(),v2)!=co.end();
}

bool Path::areConnected(const Vec3& v1, const Vec3& v2, const Vec3& v3) const
{
	std::vector<Vec3> co1=getConnexes(v1);
	std::vector<Vec3> co2=getConnexes(v2);
	
	bool test1 = std::find(co1.begin(),co1.end(),v2)!=co1.end();
	bool test2 = std::find(co1.begin(),co1.end(),v3)!=co1.end();
	bool test3 = std::find(co2.begin(),co2.end(),v3)!=co2.end();
	
	return test1 && test2 && test3;
}

Path Path::extractTriangle()
{
	std::vector<int> deg = degrees();
	std::map<float,int> found;
	for(int i=0;i<count();++i)
	{
		int i1 = cycleIndex(i-2);
		int i2 = cycleIndex(i-1);
		int i3 = cycleIndex(i);
		bool bridge = deg[i2]>2;
		// bool bridgeA = deg[i]>2;
		// bool bridgeB = deg[i3]>2;
		if(bridge)continue;// || (bridgeA && bridgeB)) continue;
		
		Edge edge(vertex(i1),vertex(i3));
		if(Intersection::isCrossing(*this,edge))continue;
		
		Vec3 ref = tan(i2);
		float na = tan(i3).angleFrom(ref);
		found[na]=i3;
	}
	
	int best = found.begin()->second;
	
	std::vector<Vec3> tri={vertex(best),vertex(best-1),vertex(best-2)};
	
	if(areConnected(tri[0],tri[1],tri[2])) erase({best,best-1,best-2});
	else erase({best-1});
	
	return Path(tri);
}

std::vector<Path> Path::triangulate() const
{
	Path cpy = *this;
	std::vector<Path> res;
	while(cpy.count() != 0)
	{
		Path extracted;
		if(cpy.count()==3){extracted=cpy; cpy=Path();}
		else extracted = cpy.extractTriangle();
		res.push_back(extracted);
	}
	return res;
}

Edge::Edge(){}
Edge::Edge(Vec3 p1,Vec3 p2):p1(p1),p2(p2){}

bool Edge::operator==(const Edge& other)
{
	return (other.p1==p1 && other.p2==p2) || (other.p1==p2 && other.p2==p1);
}

bool Edge::connectedTo(const Edge& other)
{
	return other.p1==p1 || other.p1==p2 || other.p2==p1 || other.p2==p2;
}

Intersection::Intersection(){}
Intersection::Intersection(Edge e1, Edge e2)
{
	edge[0]=e1;
	edge[1]=e2;
}
bool Intersection::operator==(Intersection other)
{
	return (edge[0]==other.edge[0] && edge[1]==other.edge[1]) || (edge[0]==other.edge[1] && edge[1]==other.edge[0]) ;
}

bool Intersection::compute()
{
	imp::Vec3 p12 = edge[0].p2 - edge[0].p1;
	imp::Vec3 a = edge[1].p1 - edge[0].p1;
	imp::Vec3 b = edge[1].p2 - edge[0].p1;
	
	imp::Vec3 tan = p12;
	tan.normalize();
	
	imp::Vec3 bitan = imp::Vec3::Z.cross( tan );
	bitan.normalize();
	
	float da = a.dot(bitan);
	float db = b.dot(bitan);
	if( sign(da) != sign(db) ) // one on each side
	{
		float t = da / (da - db);
		ipoint = imp::mix(a, b, t);
		
		float di = ipoint.dot(tan);
		if(di > 0.0 && di < p12.length())
		{
			ipoint = edge[0].p1 + ipoint;
			return true;
		}
	}
	
	return false;
}

bool Intersection::resolve(Path& target, const Path& other, Cache& precomputed)
{
	Path targetCpy = target;
	target = Path();
	
	for(int i=0;i<targetCpy.count();++i)
	{
		Edge e1 = targetCpy.edge(i);
		BufType toInsert;
		
		for(int j=0;j<other.count();++j)
		{
			Edge e2 = other.edge(j);	
			if(e1.p1==e2.p1 || e1.p1==e2.p2 || e1.p2==e2.p1 || e1.p2==e2.p2)continue;
			
			Intersection inter(e1,e2);
			auto found = std::find(precomputed.begin(),precomputed.end(),inter);
			bool needInsert = false;
			if(found != precomputed.end()){inter.ipoint=found->ipoint;needInsert=true;}
			else if(inter.compute()){precomputed.push_back(inter);needInsert=true;}
			
			if(needInsert)toInsert.push_back(inter.ipoint);
		}
		
		// sort and insert by distance to e1.p1
		Vec3 ref = e1.p1;
		auto cmp=[ref](Vec3& v1,Vec3& v2){return (v1-ref).length()<(v2-ref).length();};
		std::sort(toInsert.begin(),toInsert.end(),cmp);
		target.addVertices(toInsert);
		target.addVertex(e1.p2);
	}
	
	return target.count() > targetCpy.count();
}

bool Intersection::resolve2(Path& cy1, Path& cy2, Cache& precomputed)
{
	Path cpy = cy1;
	
	bool r1 = resolve(cy1,cy2,precomputed);
	bool r2 = resolve(cy2,cpy,precomputed);
	
	return r1 || r2;
}

bool Intersection::selfResolve(Path& cy, Cache& precomputed)
{
	Path cpy = cy;
	bool res = resolve(cy,cpy,precomputed);
	return res;
}

bool Intersection::resolve(Path& target, const Path& other)
{
	Cache tmp;
	return resolve(target,other,tmp);
}

bool Intersection::resolve2(Path& cy1, Path& cy2)
{
	Cache tmp;
	return resolve2(cy1,cy2,tmp);
}

bool Intersection::selfResolve(Path& cy)
{
	Cache tmp;
	return selfResolve(cy,tmp);
}

bool Intersection::isCrossing(const Path& target, const Edge& e, bool excludeNode)
{
	for(int i=0;i<target.count();++i)
	{
		Edge e1 = target.edge(i);
		if(excludeNode && (e1.p1==e.p1 || e1.p2==e.p1 || e1.p1==e.p2 || e1.p2==e.p2)) continue;
		
		Intersection inter(e1,e);
		if(inter.compute())return true;
	}
	return false;
}

bool Intersection::contains(const Cache& cache, const Vec3& v)
{
	for(const auto& i:cache)if(i.ipoint==v)return true;
	return false;
}

int Path::windingNumber() const
{
	float rad = 0.0;
	for(int i=0;i<count();++i)rad += tan(i).angleFrom(tan(i-1));
	return rad/(2*3.141592);
}

void Path::reverse()
{
	BufType cpy = vertices;
	vertices.clear();
	for(auto it=cpy.rbegin();it!=cpy.rend();it++)vertices.push_back(*it);
}

Intersection::BufType Intersection::getVertices(const Cache& cache)
{
	BufType vert;
	for(const auto& i:cache) vert.push_back(i.ipoint);
	return vert;
}
