#include "Cycle.h"

#include <map>

std::string asstring(const Vec3& v) {std::stringstream s; s << "["<<v[0]<<";"<<v[1]<<";"<<v[2]<<"]"; return s.str();}


Cycle::Cycle()
{
}

Cycle::Cycle(const Vertices& vert)
{
	vertices = vert;
}

Cycle::~Cycle()
{
}

float sign(float a)
{
	return a<0.0?-1.0:1.0;
}

void Cycle::operator+=(Vec3 mv)
{
	int N = vertices.size();
	for(int i=0;i<N;++i) vertices[i] += mv;
}

void Cycle::operator-=(Vec3 mv)
{
	int N = vertices.size();
	for(int i=0;i<N;++i) vertices[i] -= mv;
}

void Cycle::operator*=(Vec3 mv)
{
	int N = vertices.size();
	for(int i=0;i<N;++i) vertices[i] *= mv;
}

void Cycle::operator/=(Vec3 mv)
{
	int N = vertices.size();
	for(int i=0;i<N;++i) vertices[i] /= mv;
}

void Cycle::rotation(float rad)
{
	int N = vertices.size();
	for(int i=0;i<N;++i) vertices[i] *= imp::Matrix3::rotationZ(rad);
}

void Cycle::addVertex(Vec3 vt)
{
	vertices.push_back(vt);
}

void Cycle::addVertices(Vertices toInsert)
{
	for(auto v:toInsert)addVertex(v);
}

void Cycle::addFrom(const Cycle& other, int a, int b, int dir)
{
	dir = dir>0?1:-1;
	for(int i=a; i!=b; i+=dir)
	{
		i=other.cycleIndex(i); if(i==b)break;
		addVertex(other.vertex(i));
	}
	addVertex(other.vertex(b));
}

int Cycle::cycleIndex(int i) const
{
	int N = count();
	while(i>=N)i-=N;
	while(i<0)i+=N;
	return i;
}

Vec3& Cycle::vertex(int i)
{
	return vertices[cycleIndex(i)];
}

const Vec3& Cycle::vertex(int i) const
{
	return vertices[cycleIndex(i)];
}

int Cycle::index(const Vec3& v) const
{
	auto it = std::find(vertices.begin(),vertices.end(),v);
	return it-vertices.begin();
}

Vec3 Cycle::tan(int i) const
{
	Vec3 dir = vertex(i)-vertex(i-1); dir.normalize();
	return dir;
}

Edge Cycle::edge(int i) const
{
	return Edge(vertex(i-1),vertex(i));
}

int Cycle::count() const
{
	return vertices.size();
}

void Cycle::dump() const
{
	std::cout << "dump polygon :" << std::endl;
	for(auto vert : vertices)std::cout << asstring(vert) << std::endl;
}

bool Cycle::composes(const Vec3& v) const
{
	return std::find(vertices.begin(),vertices.end(),v) != vertices.end();
}

bool Cycle::contains(const Vec3& v) const
{
	int cpt = 0;
	
	Vec3 ext = leftExtremity() - Vec3(10.0,10.0,0.0);
	Edge ray(ext,v);
	for(int i=0;i<count();++i)
	{
		Intersection inter(edge(i), ray);
		if(inter.compute())++cpt;
	}
	
	return (cpt%2)>0;
}

bool Cycle::contains(const Cycle& c) const
{
	for(auto v: c.vertices)if(contains(v)==false)return false;
	return true;
}

Vertices Cycle::getConnexes(const Vec3& v) const
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

int Cycle::getEdgeCnt(const Vec3& v) const
{
	Vertices vert = getConnexes(v);
	return vert.size();
}

Vec3 Cycle::previous(const Vec3& v) const
{
	return vertex(index(v)-1);
}

Vec3 Cycle::next(const Vec3& v) const
{
	return vertex(index(v)+1);
}

Vec3 Cycle::gravity() const
{
	Vec3 total;
	for(auto v:vertices)total+=v;
	return total/count();
}

std::vector<int> Cycle::degrees() const
{
	std::vector<int> res;
	for(auto v:vertices) res.push_back(getConnexes(v).size());
	return res;
}

void Cycle::erase(std::vector<int> toErase)
{
	for(auto& i : toErase)i=cycleIndex(i);
	std::sort(toErase.begin(),toErase.end(),std::greater<int>());
	for(auto e : toErase)vertices.erase(vertices.begin()+e);
}

Vec3 Cycle::findNextByAngle(const Edge& curr, const Vec3& tangent, bool maxi) const
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

Cycle Cycle::boundary() const
{
	Cycle result;
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


Cycle Cycle::simplify() const
{
	Cycle cpy = *this;
	Intersection::selfResolve(cpy);
	return cpy.boundary();
}

Vec3 Cycle::leftExtremity() const
{
	Vec3 res = vertices[0];
	for(auto v:vertices)if(v[0]<res[0])res=v;
	return res;
}

bool Cycle::areConnected(const Vec3& v1, const Vec3& v2) const
{
	std::vector<Vec3> co=getConnexes(v1);
	return std::find(co.begin(),co.end(),v2)!=co.end();
}

bool Cycle::areConnected(const Vec3& v1, const Vec3& v2, const Vec3& v3) const
{
	std::vector<Vec3> co1=getConnexes(v1);
	std::vector<Vec3> co2=getConnexes(v2);
	
	bool test1 = std::find(co1.begin(),co1.end(),v2)!=co1.end();
	bool test2 = std::find(co1.begin(),co1.end(),v3)!=co1.end();
	bool test3 = std::find(co2.begin(),co2.end(),v3)!=co2.end();
	
	return test1 && test2 && test3;
}

Cycle Cycle::extractTriangle()
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
	
	return Cycle(tri);
}

std::vector<Cycle> Cycle::triangulate() const
{
	Cycle cpy = *this;
	std::vector<Cycle> res;
	while(cpy.count() != 0)
	{
		Cycle extracted;
		if(cpy.count()==3){extracted=cpy; cpy=Cycle();}
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

bool Intersection::resolve(Cycle& target, const Cycle& other, Cache& precomputed)
{
	Cycle targetCpy = target;
	target = Cycle();
	
	for(int i=0;i<targetCpy.count();++i)
	{
		Edge e1 = targetCpy.edge(i);
		Vertices toInsert;
		
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

bool Intersection::resolve2(Cycle& cy1, Cycle& cy2, Cache& precomputed)
{
	Cycle cpy = cy1;
	
	bool r1 = resolve(cy1,cy2,precomputed);
	bool r2 = resolve(cy2,cpy,precomputed);
	
	return r1 || r2;
}

bool Intersection::selfResolve(Cycle& cy, Cache& precomputed)
{
	Cycle cpy = cy;
	bool res = resolve(cy,cpy,precomputed);
	return res;
}

bool Intersection::resolve(Cycle& target, const Cycle& other)
{
	Cache tmp;
	return resolve(target,other,tmp);
}

bool Intersection::resolve2(Cycle& cy1, Cycle& cy2)
{
	Cache tmp;
	return resolve2(cy1,cy2,tmp);
}

bool Intersection::selfResolve(Cycle& cy)
{
	Cache tmp;
	return selfResolve(cy,tmp);
}

bool Intersection::isCrossing(const Cycle& target, const Edge& e, bool excludeNode)
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

int Cycle::windingNumber() const
{
	float rad = 0.0;
	for(int i=0;i<count();++i)rad += tan(i).angleFrom(tan(i-1));
	return rad/(2*3.141592);
}

void Cycle::reverse()
{
	Vertices cpy = vertices;
	vertices.clear();
	for(auto it=cpy.rbegin();it!=cpy.rend();it++)vertices.push_back(*it);
}

Vertices Intersection::getVertices(const Cache& cache)
{
	Vertices vert;
	for(const auto& i:cache) vert.push_back(i.ipoint);
	return vert;
}