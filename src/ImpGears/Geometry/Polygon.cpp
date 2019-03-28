#include <Geometry/Polygon.h>
#include <Core/Matrix3.h>

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
	addPath(path,PathType_Shape);
}

//--------------------------------------------------------------
Polygon::Polygon(const PathSet& shapes)
{
	for(const auto& shp:shapes)addPath(shp,PathType_Shape);
}

//--------------------------------------------------------------
Polygon::Polygon(const PathSet& shapes, const PathSet& holes)
{
	for(const auto& shp:shapes)addPath(shp,PathType_Shape);
	for(const auto& shp:holes)addPath(shp,PathType_Hole);
}

//--------------------------------------------------------------
Polygon::~Polygon()
{
}

//--------------------------------------------------------------
void Polygon::operator+=(const Vec3& vec)
{
	for(auto& p : _paths)
		for(int i=0;i<p.count();++i) p.vertices[i] += vec;
}

//--------------------------------------------------------------
void Polygon::operator-=(const Vec3& vec)
{
	for(auto& p : _paths)
		for(int i=0;i<p.count();++i) p.vertices[i] -= vec;
}

//--------------------------------------------------------------
void Polygon::operator*=(const Vec3& vec)
{
	for(auto& p : _paths)
		for(int i=0;i<p.count();++i) p.vertices[i] *= vec;
}

//--------------------------------------------------------------
void Polygon::operator/=(const Vec3& vec)
{
	for(auto& p : _paths)
		for(int i=0;i<p.count();++i) p.vertices[i] /= vec;
}

//--------------------------------------------------------------
void Polygon::rotation(float rad)
{
	for(auto& p : _paths)
		for(int i=0;i<p.count();++i) p.vertices[i] *= imp::Matrix3::rotationZ(rad);
}

//--------------------------------------------------------------
bool Polygon::inside(const Vec3& v) const
{
	bool insideOneShape = false;
	for(auto p : _paths)
	{
		if(!p.inside(v)) continue;
		if(p.windingNumber()>0) insideOneShape = true;
		else return false;
	}
	
	return insideOneShape;
}

//--------------------------------------------------------------
bool Polygon::inside(const Path& path) const
{
	bool insideOneShape = false;
	for(auto p : _paths)
	{
		if(!p.inside(path)) continue;
		if(p.windingNumber()>0) insideOneShape = true;
		else return false;
	}
	
	return insideOneShape;
}

//--------------------------------------------------------------
bool Polygon::inside(const Polygon& poly) const
{
	for(auto p : poly._paths) if(!inside(p)) return false;
	return true;
}

//--------------------------------------------------------------
bool Polygon::contains(const PathSet& cycles, const Vec3& v)
{
	for(auto& cy : cycles)if(cy.composes(v))return true;
	return false;
}

//--------------------------------------------------------------
void Polygon::addPath(const Path& cy, PathType ptype)
{
	Path scy = cy.simplify();
	
	if(ptype != PathType_Unchange)
	{
		PathType currType = scy.windingNumber()>0?PathType_Shape:PathType_Hole;
		if(currType != ptype)scy.reverse();
	}
	_paths.push_back(scy);
}

//--------------------------------------------------------------
void Polygon::addPaths(const PathSet& other)
{
	for(const auto& cy : other)_paths.push_back(cy);
}

//--------------------------------------------------------------
Polygon Polygon::operator-(const Polygon& other) const
{
	return applyOp2(*this, other, OpType_A_NOT_B);
}

//--------------------------------------------------------------
Polygon Polygon::operator+(const Polygon& other) const
{
	return applyOp2(*this, other, OpType_OR);
}

//--------------------------------------------------------------
Polygon Polygon::operator*(const Polygon& other) const
{
	return applyOp2(*this, other, OpType_AND);
}

//--------------------------------------------------------------
void analyticResolution2(const Path& pathA, const Polygon& setB, Polygon::OpType opType, Polygon& result)
{	
	using Vec2i = Vec<2,int>;
	bool aInsideB = setB.inside(pathA);

	Vec2i res;
	if(opType == Polygon::OpType_OR)
		res = aInsideB ? Vec2i(0,0) : Vec2i(1,-1);
	
	if(opType == Polygon::OpType_A_NOT_B)
		res = aInsideB ? Vec2i(0,0) : Vec2i(1,-1);
	
	if(opType == Polygon::OpType_B_NOT_A)
		res = aInsideB ? Vec2i(-1,1) : Vec2i(0,0);
	
	if(opType == Polygon::OpType_AND)
		res = aInsideB ? Vec2i(1,-1) : Vec2i(0,0);
	
	int rInd = pathA.windingNumber()>0 ? 0 : 1;
	
	if(res[rInd] != 0)
		result.addPath(pathA,res[rInd]>0?Polygon::PathType_Shape:Polygon::PathType_Hole);
}

//--------------------------------------------------------------
int findComposite(const Vec3& curr, const Polygon& set)
{
	int i=0;
	for(auto c : set._paths) { if(c.composes(curr))return i; i++; }
	return -1;
}

//--------------------------------------------------------------
bool isEnter2(const Vec3& p,const Path& src,const Path& other)
{
	Vec3 testP = src.next(p);
	if(other.composes(testP))testP=(p+testP)*0.5;
	
	bool goInsidePoly = other.inside(testP);
	bool targetIsPlain = other.windingNumber() > 0;
	
	return goInsidePoly == targetIsPlain;
}

//--------------------------------------------------------------
Path pathFinder2(const Polygon& cycleA, const Polygon& cycleB, 
				 std::vector<Vec3>& iPoints, const Vec3& start, bool aOut, bool reverseB)
{
	Path result;
	bool src = false;
	
	Vec3 curr = start;
	int itA = findComposite(start,cycleA);
	int itB = findComposite(start,cycleB);
	do
	{
		result.addVertex(curr);
		auto it = std::find(iPoints.begin(),iPoints.end(),curr);
		if(it != iPoints.end())
		{
			iPoints.erase(it);
			itA = findComposite(curr,cycleA);
			itB = findComposite(curr,cycleB);
			bool _p1EnterP2 = isEnter2(curr,cycleA._paths[itA],cycleB._paths[itB]);
			src=(_p1EnterP2 != aOut);
		}
		if(src)curr = cycleA._paths[itA].next(curr);
		else if(reverseB)curr = cycleB._paths[itB].previous(curr);
		else curr = cycleB._paths[itB].next(curr);
	}
	while(curr != start);
	
	return Path(result);
}

//--------------------------------------------------------------
void findAllPathFrom2(const Polygon& poly1, const Polygon& poly2, const Intersection::Cache& cache, Polygon::OpType op, PathSet& cycles)
{
	bool aOut = op==Polygon::OpType_OR || op==Polygon::OpType_A_NOT_B;
	bool reverseB = op==Polygon::OpType_A_NOT_B;
	std::vector<Vec3> iPoints = Intersection::getVertices(cache);
	
	while(iPoints.size() > 0)
	{
		Vec3 node = iPoints[0];
		Path result = pathFinder2(poly1,poly2,iPoints,node,aOut,reverseB);
		cycles.push_back(result);
	}
}

//--------------------------------------------------------------
Polygon Polygon::applyOp2(const Polygon& setA, const Polygon& setB, OpType opType)
{
	Polygon result;
	
	Polygon cpyA = setA;
	Polygon cpyB = setB;
	Intersection::Cache cache;
	
	std::vector<bool> analyticNeedA(cpyA._paths.size(),true);
	std::vector<bool> analyticNeedB(cpyB._paths.size(),true);
	for(int iA=0;iA<(int)cpyA._paths.size();++iA)
		for(int iB=0;iB<(int)cpyB._paths.size();++iB)
	{
		bool xAB = Intersection::resolve2(cpyA._paths[iA],cpyB._paths[iB],cache);
		if(xAB){analyticNeedA[iA]=false;analyticNeedB[iB]=false;}
	}
	
	for(int i=0;i<(int)analyticNeedA.size();++i)
	{
		if(analyticNeedA[i]){analyticResolution2(cpyA._paths[i],setB,opType,result);}
	}
	OpType op = opType; if(op == OpType_A_NOT_B)op=OpType_B_NOT_A;
	for(int i=0;i<(int)analyticNeedB.size();++i)
	{
		if(analyticNeedB[i]){analyticResolution2(cpyB._paths[i],setA,op,result);}
	}
	
	if(cache.size() > 0)
	{
		PathSet cycles;
		findAllPathFrom2(cpyA,cpyB,cache,opType,cycles);
		result.addPaths(cycles);
	}
	
	return result;
}

IMPGEARS_END

