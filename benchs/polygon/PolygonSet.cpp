#include "PolygonSet.h"


PolygonSet::PolygonSet(const Cycle& basic)
{
	addCycle(basic,false);
}

PolygonSet::PolygonSet(const CycleSet& bound)
{
	for(const auto& cy:bound)addCycle(cy,false);
}

PolygonSet::PolygonSet(const CycleSet& bound, const CycleSet& hole)
{
	for(const auto& cy:bound)addCycle(cy,false);
	for(const auto& cy:hole)addCycle(cy,true);
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
	std::cout << "break hole " << exti << "; " << inti << std::endl;
	Cycle ce = _cycles[exti].boundary();
	Cycle ci = _cycles[inti].boundary();
	_cycles.clear();

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
	Cycle cpy = cy;
	if(hole && cy.windingNumber() > 0)cpy.reverse();
	if(!hole && cy.windingNumber() < 0)cpy.reverse();
	_cycles.push_back(cpy);
}

void PolygonSet::addCycles(const CycleSet& other)
{
	for(const auto& cy : other)_cycles.push_back(cy);
}

PolygonSet PolygonSet::operator-(const PolygonSet& other) const
{
	PolygonSet result;
	for(auto& cy1:_cycles)
		for(auto& cy2:other._cycles)
	{
		PolygonSet local = applyOp(cy1,cy2, OpType_NOT);
		result.addCycles(local._cycles);
	}
	return result;
}

PolygonSet PolygonSet::operator+(const PolygonSet& other) const
{
	PolygonSet result;
	for(auto& cy1:_cycles)
		for(auto& cy2:other._cycles)
	{
		PolygonSet local = applyOp(cy1,cy2, OpType_OR);
		result.addCycles(local._cycles);
	}
	return result;
}

PolygonSet PolygonSet::operator*(const PolygonSet& other) const
{
	PolygonSet result;
	for(auto& cy1:_cycles)
		for(auto& cy2:other._cycles)
	{
		PolygonSet local = applyOp(cy1,cy2, OpType_AND);
		result.addCycles(local._cycles);
	}
	return result;
}

bool isEnter(const Vec3& p,const Cycle& src,const Cycle& other)
{
	Vec3 testP = src.next(p);
	if(other.composes(testP))testP=(p+testP)*0.5;
	return other.contains(testP);
}

Cycle pathFinder(const Cycle& cycleA, const Cycle& cycleB, const Vec3& start, bool aOut, bool reverseB)
{
	Cycle result;
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
	
	return result;
}



void findAllPathFrom(const Cycle& cycle1, const Cycle& cycle2, const Intersection::Cache& cache, PolygonSet::OpType op, PolygonSet::CycleSet& cycles)
{
	bool aOut = op==PolygonSet::OpType_OR || op==PolygonSet::OpType_NOT;
	bool reverseB = op==PolygonSet::OpType_NOT;
	Vertices iPoints = Intersection::getVertices(cache);
	for(auto node : iPoints)
	{
		if(PolygonSet::contains(cycles, node))continue;
		Cycle result = pathFinder(cycle1,cycle2,node,aOut,reverseB);
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
	findAllPathFrom(simplA,simplB,computedInter,opType,cycles);
	
	if(opType==OpType_OR)
		findAllPathFrom(simplB,simplA,computedInter,opType,cycles);
	
	result.addCycles(cycles);
	
	return result;
}

void PolygonSet::resolveHoles()
{
	// only one hole. todo multiple holes
	int i=0,i2=0;
	for(auto& cy1 : _cycles)
	{
		i++;
		if(cy1.windingNumber() > 0)continue;
		
		for(auto& cy2 : _cycles)
		{
			i2++;
			if(cy2.windingNumber() < 0)continue;
			std::cout << "test " << (i-1) << " in " << (i2-1) << std::endl; 
			if(cy2.contains(cy1))
			{
				breakHoles(i2-1,i-1);
				return;
			}
		}
	}
}




