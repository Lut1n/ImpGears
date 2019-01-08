#include "PolygonSet.h"


PolygonSet::PolygonSet(const Cycle& basic)
{
	addCycle(basic,false);
}

PolygonSet::PolygonSet(const CycleSet& bound)
{
	_exterior = bound;
}

PolygonSet::PolygonSet(const CycleSet& bound, const CycleSet& hole)
{
	_exterior = bound;
	_interior = hole;
}

PolygonSet::PolygonSet()
{
}

bool PolygonSet::contains(const CycleSet& cycles, const Vec3& v)
{
	for(auto& cy : cycles)if(cy.composes(v))return true;
	return false;
}

bool find2Link(Cycle& cy1, Cycle& cy2, Edge& link1, Edge& link2)
{
	bool link1Done = false;
	for(auto v1:cy1.vertices)
		for(auto v2:cy2.vertices)
		{
			Edge lnk(v1,v2);
			if(Intersection::isCrossing(cy1,lnk,true))continue;
			if(Intersection::isCrossing(cy2,lnk,true))continue;
			
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

void PolygonSet::breakHoles(int exti, int inti)
{
	Cycle ce = _exterior[exti].boundary();
	Cycle ci = _interior[inti].boundary();
	_exterior.clear();
	_interior.clear();

	// find 1 exterior node and 1 interior
	Edge link1,link2;
	find2Link(ce, ci, link1, link2);

	int ce_p1 = ce.index(link1.p1);
	int ci_p1 = ci.index(link1.p2);
	int ce_p2 = ce.index(link2.p1);
	int ci_p2 = ci.index(link2.p2);
	
	Cycle res;
	res.addFrom(ce,ce_p1,ce_p2,1);
	res.addFrom(ci,ci_p2,ci_p1,-1);
	addCycle(res,false);
	
	res=Cycle();
	res.addFrom(ce,ce_p2,ce_p1,1);
	res.addFrom(ci,ci_p1,ci_p2,-1);
	addCycle(res,false);
}

void PolygonSet::addCycle(const Cycle& cy, bool hole)
{
	if(hole)_interior.push_back(cy);
	else _exterior.push_back(cy);
}

Cycle cycleFinder(const Cycle& icyA, const Cycle& icyB, const Intersection::Cache& precomputed, const Edge& startEdge, bool op)
{
	Cycle result;
	bool src = icyA.composes(startEdge.p2);
	
	Edge currEdge = startEdge;
	Vec3 tan = currEdge.p2 - currEdge.p1;
	do
	{
		result.addVertex(currEdge.p2);
		if(currEdge.p2 == startEdge.p1)break;
		if(Intersection::contains(precomputed,currEdge.p2))src=!src;
		Vec3 next;
		if(src) next = icyA.findNextByAngle(currEdge,tan,op);
		else next =  icyB.findNextByAngle(currEdge,tan,op);
		currEdge = Edge(currEdge.p2,next);
		tan = currEdge.p2 - currEdge.p1;
	}
	while(currEdge.p2 != startEdge.p2);
	
	return result;
}

void findAllCycleFrom(const Cycle& cycle1, const Cycle& cycle2, const Intersection::Cache& cache, bool op, PolygonSet::CycleSet& cycles)
{
	for(int i=0;i<cycle1.count();++i)
	{
		Edge e = cycle1.edge(i);
		if(Intersection::contains(cache,e.p2) || cycle2.contains(e.p2))continue;
		if(PolygonSet::contains(cycles, e.p2))continue;
		Cycle result = cycleFinder(cycle1,cycle2,cache,e,op);
		cycles.push_back(result);
	}
}

PolygonSet PolygonSet::applyOp(const Cycle& cycleA, const Cycle& cycleB, OpType opType)
{
	PolygonSet result;
	Cycle simplA = cycleA.simplify();
	Cycle simplB = cycleB.simplify();
	Intersection::Cache computedInter;
	bool hasIntersection = Intersection::resolve2(simplA,simplB,computedInter);
	
	if(!hasIntersection)
	{
		 if(simplA.contains(simplB))
		 {
			 if(opType!=OpType_AND)result.addCycle(simplA,false);
			 if(opType==OpType_NOT) result.addCycle(simplB,true);
		 }
		 else if(simplB.contains(simplA))
		 {
			 if(opType!=OpType_AND) result.addCycle(simplB,false);
		 }
		 else
		 {
			 if(opType!=OpType_AND)result.addCycle(simplA,false);
			 if(opType==OpType_OR)result.addCycle(simplB,false);
		 }
		 return result;
	}
	
	CycleSet cycles;
	findAllCycleFrom(simplA,simplB,computedInter,opType==PolygonSet::OpType_OR,cycles);
	
	if(opType==OpType_OR)
	{
		findAllCycleFrom(simplB,simplA,computedInter,true,cycles);
		
		for(auto& cy1:cycles)
		{
			bool hole = false;
			for(auto& cy2:cycles)
			{
				if(cy1.vertex(0)==cy2.vertex(0)) continue;
				if(cy2.contains(cy1)){hole=true;break;}
			}
			result.addCycle(cy1,hole);
		}
	}
	else
	{
		result = PolygonSet(cycles);
	}
	
	return result;
}

PolygonSet PolygonSet::operator-(const PolygonSet& other) const
{
	PolygonSet result;
	for(auto& cy1:_exterior)
		for(auto& cy2:other._exterior)
	{
		PolygonSet local = applyOp(cy1,cy2, OpType_NOT);
		for(auto& cy:local._exterior)result.addCycle(cy,false);
		for(auto& cy:local._interior)result.addCycle(cy,true);
	}
	return result;
}

PolygonSet PolygonSet::operator+(const PolygonSet& other) const
{
	PolygonSet result;
	for(auto& cy1:_exterior)
		for(auto& cy2:other._exterior)
	{
		PolygonSet local = applyOp(cy1,cy2, OpType_OR);
		for(auto& cy:local._exterior)result.addCycle(cy,false);
		for(auto& cy:local._interior)result.addCycle(cy,true);
	}
	return result;
}

PolygonSet PolygonSet::operator*(const PolygonSet& other) const
{
	PolygonSet result;
	/*for(auto& cy1:_exterior)
		for(auto& cy2:other._exterior)
	{
		PolygonSet local = applyOp(cy1,cy2, OpType_AND);
		for(auto& cy:local._exterior)result.addCycle(cy,false);
		for(auto& cy:local._interior)result.addCycle(cy,true);
	}*/
	return result;
}