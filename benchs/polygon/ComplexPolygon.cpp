#include "ComplexPolygon.h"
#include "SimplePolygon.h"

#include <map>

ComplexPolygon::ComplexPolygon(const Path& basic)
{
	addPath(basic,Orientation_POS);
}

ComplexPolygon::ComplexPolygon(const PathSet& bound)
{
	for(const auto& cy:bound)addPath(cy,Orientation_POS);
}

ComplexPolygon::ComplexPolygon(const PathSet& bound, const PathSet& hole)
{
	for(const auto& cy:bound)addPath(cy,Orientation_POS);
	for(const auto& cy:hole)addPath(cy,Orientation_NEG);
}

ComplexPolygon::ComplexPolygon()
{
}

bool ComplexPolygon::contains(const PathSet& cycles, const Vec3& v)
{
	for(auto& cy : cycles)if(cy.composes(v))return true;
	return false;
}

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

void ComplexPolygon::addPaths(const PathSet& other)
{
	for(const auto& cy : other)_cycles.push_back(cy);
}

ComplexPolygon ComplexPolygon::operator-(const ComplexPolygon& other) const
{
	ComplexPolygon result;
	for(auto& cy1:_cycles)
		for(auto& cy2:other._cycles)
	{
		ComplexPolygon local = applyOp(cy1,cy2, OpType_NOT);
		result.addPaths(local._cycles);
	}
	return result;
}

ComplexPolygon ComplexPolygon::operator+(const ComplexPolygon& other) const
{
	ComplexPolygon result;
	for(auto& cy1:_cycles)
		for(auto& cy2:other._cycles)
	{
		ComplexPolygon local = applyOp(cy1,cy2, OpType_OR);
		result.addPaths(local._cycles);
	}
	return result;
}

ComplexPolygon ComplexPolygon::operator*(const ComplexPolygon& other) const
{
	ComplexPolygon result;
	for(auto& cy1:_cycles)
		for(auto& cy2:other._cycles)
	{
		ComplexPolygon local = applyOp(cy1,cy2, OpType_AND);
		result.addPaths(local._cycles);
	}
	return result;
}

bool isEnter(const Vec3& p,const Path& src,const Path& other)
{
	Vec3 testP = src.next(p);
	if(other.composes(testP))testP=(p+testP)*0.5;
	SimplePolygon opoly(other);
	return opoly.contains(testP);
}

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
			bool p1EnterP2 = isEnter(curr,cycleA,cycleB);
			src=(p1EnterP2 != aOut);
		}
		if(src)curr = cycleA.next(curr);
		else if(reverseB)curr = cycleB.previous(curr);
		else curr = cycleB.next(curr);
	}
	while(curr != start);
	
	return Path(result);
}



void findAllPathFrom(const Path& cycle1, const Path& cycle2, const Intersection::Cache& cache, ComplexPolygon::OpType op, ComplexPolygon::PathSet& cycles)
{
	bool aOut = op==ComplexPolygon::OpType_OR || op==ComplexPolygon::OpType_NOT;
	bool reverseB = op==ComplexPolygon::OpType_NOT;
	Vertices iPoints = Intersection::getVertices(cache);
	for(auto node : iPoints)
	{
		if(ComplexPolygon::contains(cycles, node))continue;
		Path result = pathFinder(cycle1,cycle2,node,aOut,reverseB);
		cycles.push_back(result);
	}
}

ComplexPolygon ComplexPolygon::applyOp(const Path& cycleA, const Path& cycleB, OpType opType)
{
	ComplexPolygon result;
	Path simplA = cycleA;//.simplify();
	Path simplB = cycleB;//.simplify();
	Intersection::Cache computedInter;
	bool hasIntersection = Intersection::resolve2(simplA,simplB,computedInter);
	
	if(!hasIntersection)
	{
		SimplePolygon polyA(simplA);
		SimplePolygon polyB(simplB);
		
		 if(polyA.contains(polyB))
		 {
			 if(opType!=OpType_AND)result.addPath(simplA,Orientation_POS);
			 else result.addPath(simplB,Orientation_POS);
			 if(opType==OpType_NOT) result.addPath(simplB,Orientation_NEG);
		 }
		 else if(polyB.contains(polyA))
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


ComplexPolygon::PathSet ComplexPolygon::breakHoles(const Path& body, const Path& hole, const PathSet& set)
{
	PathSet result;
	// find 1 exterior node and 1 interior
	Edge link1,link2;
	find2Link(body, hole, link1, link2,set);

	int ce_p1 = body.index(link1.p1);
	int ci_p1 = hole.index(link1.p2);
	int ce_p2 = body.index(link2.p1);
	int ci_p2 = hole.index(link2.p2);
	
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
		
		bodies[i-1].push_back(*cy1);
		
		for(auto cy2=_cycles.begin();cy2!=_cycles.end();cy2++)
		{
			if(cy2->windingNumber()>0)continue; // keep only hole
			SimplePolygon spcy1(*cy1);
			if(spcy1.contains(*cy2))holes[i-1].push_back(*cy2);
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
			for(;partIt!=bodyparts.end();partIt++)if(SimplePolygon(*partIt).contains(hole))break;
			
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




