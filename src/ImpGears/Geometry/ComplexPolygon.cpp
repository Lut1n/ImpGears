#include <Geometry/ComplexPolygon.h>
#include <Geometry/Polygon.h>

#include <map>

IMPGEARS_BEGIN

//--------------------------------------------------------------
ComplexPolygon::ComplexPolygon(const Path& basic)
{
	addPath(basic,Orientation_POS);
}

//--------------------------------------------------------------
ComplexPolygon::ComplexPolygon(const PathSet& bound)
{
	for(const auto& cy:bound)addPath(cy,Orientation_POS);
}

//--------------------------------------------------------------
ComplexPolygon::ComplexPolygon(const PathSet& bound, const PathSet& hole)
{
	for(const auto& cy:bound)addPath(cy,Orientation_POS);
	for(const auto& cy:hole)addPath(cy,Orientation_NEG);
}

//--------------------------------------------------------------
ComplexPolygon::ComplexPolygon()
{
}

//--------------------------------------------------------------
void ComplexPolygon::operator+=(const Vec3& mv)
{
	for(auto& _path : _cycles)
	{
		int N = _path.count();
		for(int i=0;i<N;++i) _path.vertices[i] += mv;
	}
}

//--------------------------------------------------------------
void ComplexPolygon::operator-=(const Vec3& mv)
{
	for(auto& _path : _cycles)
	{
		int N = _path.count();
		for(int i=0;i<N;++i) _path.vertices[i] -= mv;
	}
}

//--------------------------------------------------------------
void ComplexPolygon::operator*=(const Vec3& mv)
{
	for(auto& _path : _cycles)
	{
		int N = _path.count();
		for(int i=0;i<N;++i) _path.vertices[i] *= mv;
	}
}

//--------------------------------------------------------------
void ComplexPolygon::operator/=(const Vec3& mv)
{
	for(auto& _path : _cycles)
	{
		int N = _path.count();
		for(int i=0;i<N;++i) _path.vertices[i] /= mv;
	}
}

//--------------------------------------------------------------
void ComplexPolygon::rotation(float rad)
{
	for(auto& _path : _cycles)
	{
		int N = _path.count();
		for(int i=0;i<N;++i) _path.vertices[i] *= imp::Matrix3::rotationZ(rad);
	}
}

//--------------------------------------------------------------
bool ComplexPolygon::contains(const PathSet& cycles, const Vec3& v)
{
	for(auto& cy : cycles)if(cy.composes(v))return true;
	return false;
}

//--------------------------------------------------------------
bool find2Link(const Path& cy1, const Path& cy2, Edge& link1, Edge& link2, const std::vector<Path>& set)
{
	bool link1Done = false;
	for(auto v1:cy1.vertices)
		for(auto v2:cy2.vertices)
		{
			Edge lnk(v1,v2);
			// if(Intersection::isCrossing(cy1,lnk,true))continue;
			// if(Intersection::isCrossing(cy2,lnk,true))continue;
			bool cont = false;
			for(const auto& cset : set)cont = cont || Intersection::isCrossing(cset,lnk,true);
			if(cont)continue;
			
			if(!link1Done)
			{
				link1=lnk;
				link1Done=true;
			}
			else
			{
				if(link1.connectedTo(lnk))continue;
				if(Intersection(link1,lnk).compute())continue;
				link2=lnk; return true;
			}
	}
	return false;
}

//--------------------------------------------------------------
void ComplexPolygon::addPath(const Path& cy, Orientation ori)
{
	Path scy = cy.simplify();
	
	if(ori != Orientation_NONE)
	{
		Orientation currOri = scy.windingNumber()>0?Orientation_POS:Orientation_NEG;
		if(currOri != ori)scy.reverse();
	}
	_cycles.push_back(scy);
}

//--------------------------------------------------------------
void ComplexPolygon::addPaths(const PathSet& other)
{
	for(const auto& cy : other)_cycles.push_back(cy);
}



std::vector<Path> findHoles(const ComplexPolygon& cp, const Path& path)
{
	Polygon ppath(path);
	std::vector<Path> res;
	if(ppath.windingNumber() < 0)return res;
	
	for(auto& c:cp._cycles)
	{
		Polygon p(c);
		if(p.windingNumber() > 0)continue;
		if(ppath.inside(p))res.push_back(c);
	}
	
	return res;
}

//--------------------------------------------------------------
ComplexPolygon ComplexPolygon::operator-(const ComplexPolygon& other) const
{
	ComplexPolygon result;
	for(auto& cy1:_cycles)
	{
		Polygon tp(cy1);
		if(tp.windingNumber() < 0)continue;
		PathSet setA = findHoles(*this,cy1);
		setA.push_back(cy1);
			
		for(auto& cy2:other._cycles)
		{
			Polygon tp2(cy2);
			if(tp2.windingNumber() < 0)continue;
			PathSet setB = findHoles(other,cy2);
			setB.push_back(cy2);
			
			ComplexPolygon local = applyOp2(setA, setB, OpType_A_NOT_B);
			result.addPaths(local._cycles);
		}
	}
	return result;
}

//--------------------------------------------------------------
ComplexPolygon ComplexPolygon::operator+(const ComplexPolygon& other) const
{
	ComplexPolygon result;
	for(auto& cy1:_cycles)
	{
		Polygon tp(cy1);
		if(tp.windingNumber() < 0)continue;
		PathSet setA = findHoles(*this,cy1);
		setA.push_back(cy1);
			
		for(auto& cy2:other._cycles)
		{
			Polygon tp2(cy2);
			if(tp2.windingNumber() < 0)continue;
			PathSet setB = findHoles(other,cy2);
			setB.push_back(cy2);
			
			ComplexPolygon local = applyOp2(setA, setB, OpType_OR);
			result.addPaths(local._cycles);
		}
	}
	return result;
}

//--------------------------------------------------------------
ComplexPolygon ComplexPolygon::operator*(const ComplexPolygon& other) const
{
	ComplexPolygon result;
	for(auto& cy1:_cycles)
	{
		Polygon tp(cy1);
		if(tp.windingNumber() < 0)continue;
		PathSet setA = findHoles(*this,cy1);
		setA.push_back(cy1);
			
		for(auto& cy2:other._cycles)
		{
			Polygon tp2(cy2);
			if(tp2.windingNumber() < 0)continue;
			PathSet setB = findHoles(other,cy2);
			setB.push_back(cy2);
			
			ComplexPolygon local = applyOp2(setA, setB, OpType_AND);
			result.addPaths(local._cycles);
		}
	}
	return result;
}

//--------------------------------------------------------------
bool isEnter(const Vec3& p,const Path& src,const Path& other)
{
	Vec3 testP = src.next(p);
	if(other.composes(testP))testP=(p+testP)*0.5;
	Polygon opoly(other);
	return opoly.inside(testP);
}

//--------------------------------------------------------------
Path pathFinder(const Path& cycleA, const Path& cycleB, const Vec3& start, bool aOut, bool reverseB)
{
	Path result;
	bool src = false;
	
	Vec3 curr = start;
	do
	{
		result.addVertex(curr);
		if(cycleA.composes(curr) && cycleB.composes(curr))
		{
			bool _p1EnterP2 = isEnter(curr,cycleA,cycleB);
			src=(_p1EnterP2 != aOut);
		}
		if(src)curr = cycleA.next(curr);
		else if(reverseB)curr = cycleB.previous(curr);
		else curr = cycleB.next(curr);
	}
	while(curr != start);
	
	return Path(result);
}

//--------------------------------------------------------------
void findAllPathFrom(const Path& cycle1, const Path& cycle2, const Intersection::Cache& cache, ComplexPolygon::OpType op, ComplexPolygon::PathSet& cycles)
{
	bool aOut = op==ComplexPolygon::OpType_OR || op==ComplexPolygon::OpType_A_NOT_B;
	bool reverseB = op==ComplexPolygon::OpType_A_NOT_B;
	std::vector<Vec3> iPoints = Intersection::getVertices(cache);
	for(auto node : iPoints)
	{
		if(ComplexPolygon::contains(cycles, node))continue;
		Path result = pathFinder(cycle1,cycle2,node,aOut,reverseB);
		cycles.push_back(result);
	}
}

//--------------------------------------------------------------
ComplexPolygon ComplexPolygon::applyOp(const Path& cycleA, const Path& cycleB, OpType opType)
{
	ComplexPolygon result;
	Path simplA = cycleA;//.simplify();
	Path simplB = cycleB;//.simplify();
	Intersection::Cache computedInter;
	bool hasIntersection = Intersection::resolve2(simplA,simplB,computedInter);
	
	if(!hasIntersection)
	{
		Polygon polyA(simplA);
		Polygon polyB(simplB);
		
		 if(polyA.inside(polyB))
		 {
			 if(opType!=OpType_AND)result.addPath(simplA,Orientation_POS);
			 else result.addPath(simplB,Orientation_POS);
			 if(opType==OpType_A_NOT_B) result.addPath(simplB,Orientation_NEG);
		 }
		 else if(polyB.inside(polyA))
		 {
			 if(opType!=OpType_AND) result.addPath(simplB,Orientation_POS);
			 else result.addPath(simplA,Orientation_POS);
		 }
		 else
		 {
			 if(opType!=OpType_AND)result.addPath(simplA,Orientation_POS);
			 if(opType==OpType_OR)result.addPath(simplB,Orientation_POS);
		 }
		 return result;
	}
	
	PathSet cycles;
	findAllPathFrom(simplA,simplB,computedInter,opType,cycles);
	
	if(opType==OpType_OR)
		findAllPathFrom(simplB,simplA,computedInter,opType,cycles);
	
	result.addPaths(cycles);
	
	return result;
}

bool isInside(const std::vector<Path>& set, const Path& toTest)
{
	Polygon p_test(toTest);
	
	for(auto p : set)
	{
		Polygon poly(p);
		bool isBody = poly.windingNumber()>0;
		bool inside = poly.inside(p_test);
		if(isBody != inside)return false;
	}
	
	return true;
}

void analyticResolution2(const Path& cycleA, const std::vector<Path>& setB, ComplexPolygon::OpType opType, ComplexPolygon& result)
{
	Polygon pa(cycleA); bool aIsHole = pa.windingNumber() < 0;
	bool aInsideB = isInside(setB,cycleA);
	
	int addA = 0, addhA = 0;
	if(opType == ComplexPolygon::OpType_OR)
	{
		if(aInsideB)
		{
			addA = 0;
			addhA = 0;
		}
		else
		{
			addA = 1;
			addhA = -1;
		}
	}
	if(opType == ComplexPolygon::OpType_A_NOT_B)
	{
		if(aInsideB)
		{
			addA = 0;
			addhA = 0;
		}
		else
		{
			addA = 1;
			addhA = -1;
		}
	}
	if(opType == ComplexPolygon::OpType_B_NOT_A)
	{
		if(aInsideB)
		{
			addA = -1;
			addhA = 1;
		}
		else
		{
			addA = 0;
			addhA = 0;
		}
	}
	if(opType == ComplexPolygon::OpType_AND)
	{
		if(aInsideB)
		{
			addA = 1;
			addhA = -1;
		}
		else
		{
			addA = 0;
			addhA = 0;
		}
	}
	
	if(aIsHole && addhA!=0)result.addPath(cycleA,addhA>0?ComplexPolygon::Orientation_POS:ComplexPolygon::Orientation_NEG);
	else if(!aIsHole && addA!=0)result.addPath(cycleA,addA>0?ComplexPolygon::Orientation_POS:ComplexPolygon::Orientation_NEG);
}

bool tryAnalyticResolution(Path& bodyA,std::vector<Path>& setB, Intersection::Cache& cache, ComplexPolygon::OpType op, ComplexPolygon& result)
{
	bool iTest = false;
	for(auto h:setB)iTest = Intersection::resolve(bodyA,h,cache) || iTest;
	
	if(!iTest) analyticResolution2(bodyA,setB,op,result);
	
	return !iTest;
}

int findComposite(const Vec3& curr, const std::vector<Path>& cycles)
{
	int i=0;
	for(auto c : cycles)
	{
		if(c.composes(curr))return i;
		i++;
	}
	return -1;
}

//--------------------------------------------------------------
bool isEnter2(const Vec3& p,const Path& src,const Path& other)
{
	Vec3 testP = src.next(p);
	if(other.composes(testP))testP=(p+testP)*0.5;
	Polygon opoly(other);
	
	bool goInsidePoly = opoly.inside(testP);
	bool targetIsPlain = opoly.windingNumber() > 0;
	
	return goInsidePoly == targetIsPlain;
}

//--------------------------------------------------------------
Path pathFinder2(const std::vector<Path>& cycleA, const std::vector<Path>& cycleB, 
				 std::vector<Vec3>& iPoints, const Vec3& start, bool aOut, bool reverseB)
{
	Path result;
	bool src = false;
	
	Vec3 curr = start;
	int itA = findComposite(start,cycleA);
	int itB = findComposite(start,cycleB);
	std::cout << "findCommposite start : " << itA << " " << itB << std::endl;
	do
	{
		result.addVertex(curr);
		auto it = std::find(iPoints.begin(),iPoints.end(),curr);
		if(it != iPoints.end())
		{
			// std::cout << "      ---- iPoints reached" << std::endl;
			iPoints.erase(it);
			int nitA = findComposite(curr,cycleA);
			int nitB = findComposite(curr,cycleB);
			if(nitA < 0 || nitB < 0)
			{
				std::cout << "findCommposite failed : " << itA << " " << itB << std::endl;
			}
			else
			{
				itA = nitA;
				itB = nitB;
			}
			bool _p1EnterP2 = isEnter2(curr,cycleA[itA],cycleB[itB]);
			src=(_p1EnterP2 != aOut);
		}
		if(src)curr = cycleA[itA].next(curr);
		else if(reverseB)curr = cycleB[itB].previous(curr);
		else curr = cycleB[itB].next(curr);
	}
	while(curr != start);
	
	return Path(result);
}

//--------------------------------------------------------------
void findAllPathFrom2(const std::vector<Path>& cycle1, const std::vector<Path>& cycle2, const Intersection::Cache& cache, ComplexPolygon::OpType op, ComplexPolygon::PathSet& cycles)
{
	bool aOut = op==ComplexPolygon::OpType_OR || op==ComplexPolygon::OpType_A_NOT_B;
	bool reverseB = op==ComplexPolygon::OpType_A_NOT_B;
	std::vector<Vec3> iPoints = Intersection::getVertices(cache);
	std::cout << "BEGIN ---- find All" << std::endl;
	
	
	/*std::cout << " TEST COMPOSITE " << std::endl;
	for(auto c : iPoints)
	{
		int itA = findComposite(c,cycle1);
		int itB = findComposite(c,cycle2);
			std::cout << "dump iPoint : " << c[0]<<";"<<c[1]<<";"<<c[2]<< " ___ ipoint " << itA << " " << itB << std::endl;
	}*/
	
	while(iPoints.size() > 0)
	// for(auto node : iPoints)
	{
		
		std::cout << "      ---- find All" << std::endl;
		Vec3 node = iPoints[0];
		// std::cout << "      ---- find All" << std::endl;
		// if(ComplexPolygon::contains(cycles, node))continue;
		Path result = pathFinder2(cycle1,cycle2,iPoints,node,aOut,reverseB);
		cycles.push_back(result);
	}
}


//--------------------------------------------------------------
ComplexPolygon ComplexPolygon::applyOp2(const PathSet& setA, const PathSet& setB, OpType opType)
{
	ComplexPolygon result;
	
	PathSet cpyA = setA;
	PathSet cpyB = setB;
	Intersection::Cache cache;
	
	// if(!tryAnalyticResolution(ibodyA,cycleB,holesB, cache, opType, result)) iInterA.push_back(ibodyA);
	// for(auto h:holesA)
	std::vector<bool> analyticNeed(cpyB.size(),true);
	for(auto it=cpyA.begin();it!=cpyA.end();it++)
	{
		bool iTest = false;
		
		int i=0;
		for(auto it2=cpyB.begin();it2!=cpyB.end();it2++)
		{
			// if(!tryAnalyticResolution(*it,cycleB,holesB, cache, opType, result)) iInterA.push_back(h);
			iTest = Intersection::resolve2(*it,*it2,cache) || iTest;
			if(iTest)analyticNeed[i]=false;
			i++;
		}
		
		if(!iTest) analyticResolution2(*it,setB,opType,result);
	}
	
	for(int i=0;i<(int)analyticNeed.size();++i)
	{
		OpType op = opType; if(op == OpType_A_NOT_B)op=OpType_B_NOT_A;
		if(analyticNeed[i])analyticResolution2(cpyB[i],setA,op,result);
	}
	
	/*if(!tryAnalyticResolution(ibodyB,cycleA,holesA, cache, op, result)) iInterB.push_back(ibodyB);
	for(auto h:holesB)
		if(!tryAnalyticResolution(h,cycleA,holesA, cache, op, result)) iInterB.push_back(h);
	*/
	
	if(cache.size() > 0)
	{
		PathSet cycles;
		
		findAllPathFrom2(cpyA,cpyB,cache,opType,cycles);
	
		//if(opType==OpType_OR) findAllPathFrom2(iHoleB,iHoleA,computedInter,opType,cycles)
	
		result.addPaths(cycles);
	}
	
	return result;
}

//--------------------------------------------------------------
ComplexPolygon::PathSet ComplexPolygon::breakHoles(const Path& body, const Path& hole, const PathSet& set)
{
	PathSet result;
	// find 1 exterior node and 1 interior
	Edge link1,link2;
	find2Link(body, hole, link1, link2,set);

	int ce_p1 = body.index(link1._p1);
	int ci_p1 = hole.index(link1._p2);
	int ce_p2 = body.index(link2._p1);
	int ci_p2 = hole.index(link2._p2);
	
	Path respath;
	respath.addFrom(body.subPath(ce_p1,ce_p2));
	respath.addFrom(hole.subPath(ci_p2,ci_p1));
	Path res(respath);
	if(res.windingNumber()<0)res.reverse();
	result.push_back(res);
	// addPath(res,Orientation_POS);
	
	respath = Path();
	respath.addFrom(body.subPath(ce_p2,ce_p1));
	respath.addFrom(hole.subPath(ci_p1,ci_p2));
	res=Path(respath);
	if(res.windingNumber()<0)res.reverse();
	result.push_back(res);
	// addPath(res,Orientation_POS);
	
	return result;
}

//--------------------------------------------------------------
ComplexPolygon ComplexPolygon::resolveHoles() const
{
	using PathMap = std::map<int,PathSet>;
	
	ComplexPolygon result;
	PathMap bodies,holes;

	int i=0;
	for(auto cy1=_cycles.begin();cy1!=_cycles.end();cy1++)
	{
		i++;
		if(cy1->windingNumber()<0)continue; // exclude hole
		
		Polygon spcy1(*cy1);
		
		PathSet subBodies;
		for(auto sb : _cycles)
		{
			if(sb.windingNumber()<0)continue; // exclude hole
			if(spcy1.inside(sb))subBodies.push_back(sb);
		}
		
		bodies[i-1].push_back(*cy1);
		
		for(auto cy2=_cycles.begin();cy2!=_cycles.end();cy2++)
		{
			if(cy2->windingNumber()>0)continue; // keep only hole
			
			bool subHole = false;
			for(auto sb : subBodies)
			{
				Polygon sbpoly(sb);
				if(sbpoly.inside(*cy2)){subHole=true;break;}
			}
			   
			if(!subHole && spcy1.inside(*cy2))holes[i-1].push_back(*cy2);
		}
	}
	
	
	// add all polygon that is not hole and does not contains hole
	// and for each hole, add the broken two parts of the body around it
	for(auto it : bodies)
	{
		PathSet bodyparts = it.second;
		PathSet reg_holes = holes[it.first];
		for(auto hole : reg_holes)
		{
			auto partIt = bodyparts.begin();
			for(;partIt!=bodyparts.end();partIt++)if(Polygon(*partIt).inside(hole))break;
			
			PathSet toTest = _cycles;
			for(auto b:bodyparts)toTest.push_back(b);
			PathSet set = breakHoles(*partIt, hole,toTest);
			std::cout << "process body " << it.first << " break bodypart " << (partIt-bodyparts.begin()) << " res = " << set.size() << std::endl;
			bodyparts.erase(partIt);
			for(auto& c:set)bodyparts.push_back(c);
			
			// result.addPath(bodyparts[0],Orientation_POS);
			// for(auto bp : bodyparts)result.addPath(bp,Orientation_POS);
			// return result;
		}
		
		// if(reg_holes.size() == 0)
		for(auto bp : bodyparts)result.addPath(bp,Orientation_POS);
	}
	
	return result;
}

IMPGEARS_END
