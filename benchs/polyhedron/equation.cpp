#include "equation.h"

void processBar(float p)
{
		std::cout << '\r';
		std::cout << "[";
		for(int i=0;i<p*100;++i) std::cout << "x";
		for(int i=p*100;i<100;++i) std::cout << ".";
		std::cout << "] " << (int)(p*100) << "%";
}

std::string asstring(const Vec3& v)
{
	std::stringstream ss;
	ss << "[" << v[0] << ";" << v[1] << ";" << v[2] << "]";
	return ss.str();
}

//--------------------------------------------------------------
float sign(float f) {return f>0.0?1.0:-1.0;}

//--------------------------------------------------------------
void GeoStruct::set(Vec3 p1) {buf={p1};}
void GeoStruct::set(Vec3 p1, Vec3 p2){buf={p1,p2};}
void GeoStruct::set(Vec3 p1, Vec3 p2, Vec3 p3){buf={p1,p2,p3};}
int GeoStruct::size() const {return buf.size();}
Vec3& GeoStruct::operator[](int i){return buf[i];}
Vec3 GeoStruct::operator[](int i) const {return buf[i];}
void GeoStruct::clear(){buf.clear();}
void GeoStruct::add(Vec3 p){buf.push_back(p);}
Vec3 GeoStruct::normal() const { Vec3 d=buf[1]-buf[0]; Vec3 n=d.cross(buf[2]-buf[0]); n.normalize(); return n; }

//--------------------------------------------------------------
bool rayTriangle( const GeoStruct& seg, const GeoStruct& tri, Vec3& iPoint, bool strict)
{
	Vec3 p12 = tri[1]-tri[0];
	Vec3 p13 = tri[2]-tri[0];
	Vec3 n = p12.cross(p13); n.normalize();
	
	float d1 = n.dot(seg[0]-tri[0]);
	float d2 = n.dot(seg[1]-tri[0]);
	
	bool sideTest = sign(d1)!=sign(d2) && (d1!=0.0 && d2!=0.0);
	if(!strict) sideTest = d1==0.0 || d2==0.0 || sign(d1)!=sign(d2);
	
	// check sides
	if (sideTest)
	{
		float l = std::abs(d1) + std::abs(d2);
		float t = l>0.0 ? (std::abs(d1) / l) : 0.0;
		iPoint = mix(seg[0],seg[1],t);
		
		// demi-plane
		Vec3 dp1 = n.cross( tri[1]-tri[0] );
		Vec3 dp2 = n.cross( tri[2]-tri[1] );
		Vec3 dp3 = n.cross( tri[0]-tri[2] );
        
		float idist1 = dp1.dot(iPoint-tri[0]);
		float idist2 = dp2.dot(iPoint-tri[1]);
		float idist3 = dp3.dot(iPoint-tri[2]);
		
        if(strict)
			return idist1>0.0 && idist2>0.0 && idist3>0.0;
		else
			return idist1>=0.0 && idist2>=0.0 && idist3>=0.0;
	}
	return false;
}

//--------------------------------------------------------------
void segTriExtra(GeoStruct& tri, Edge& seg)
{
	Vec3 n = tri.normal();
	
	Vec3 p1 = tri[0]-seg._p1;
	Vec3 p2 = tri[1]-seg._p1;
	Vec3 p3 = tri[2]-seg._p1;
	Vec3 tan = seg._p2 - seg._p1; tan.normalize();
	Vec3 bitan = n.cross(tan); bitan.normalize();
	
	
	float dists[3];
	dists[0] = bitan.dot(p1);
	dists[1] = bitan.dot(p2);
	dists[2] = bitan.dot(p3);
	
	// first has to be solo on its side
	if( sign(dists[0])==sign(dists[1]) )
	{
		float t=dists[0]; dists[0]=dists[2]; dists[2]=t;
		Vec3 vt=tri[0]; tri[0]=tri[2]; tri[2]=vt;
	}
	if( sign(dists[0])==sign(dists[2]) )
	{
		float t=dists[0]; dists[0]=dists[1]; dists[1]=t;
		Vec3 vt=tri[0]; tri[0]=tri[1]; tri[1]=vt;
	}
	// if( sign(dists[1])==sign(dists[2]) ) // already ok

	float da = std::abs(dists[0]) + std::abs(dists[1]);
	float db = std::abs(dists[0]) + std::abs(dists[2]);
	
	float ta = da>0.0 ? (std::abs(dists[0]) / da) : 0.0;
	float tb = db>0.0 ? (std::abs(dists[0]) / db) : 0.0;
	
	seg._p1 = mix(tri[0],tri[1],ta);
	seg._p2 = mix(tri[0],tri[2],tb);
}

//--------------------------------------------------------------
Vec3 getExPt(const BufV3& ph)
{
	Vec3 e = ph[0];
	for(auto p:ph)
	{
		if(e[0] > p[0])e[0]=p[0] - 10.3875452;
		if(e[1] > p[1])e[1]=p[1] - 10.7656347;
		if(e[2] > p[2])e[2]=p[2] - 10.0143432;
	}
	return e;
}

//--------------------------------------------------------------
bool contains(const BufV3& ph, const Vec3& p)
{
	Vec3 e = getExPt(ph);
	GeoStruct tri;
	GeoStruct seg; seg.set(e,p);
	int cpt = 0;
	for(int i=0;i<(int)ph.size();i+=3)
	{
		tri.set(ph[i+0],ph[i+1],ph[i+2]);
		Vec3 ipoint;
		if( rayTriangle(seg,tri,ipoint,false)) cpt++;
	}
	return cpt%2 != 0;
}

//--------------------------------------------------------------
bool rayPolyhedron(const GeoStruct& seg, const BufV3& ph, Vec3& ipt)
{
	bool ctn1 = contains(ph,seg[0]);
	bool ctn2 = contains(ph,seg[1]);
	if(ctn1 == ctn2) return false;
	
	GeoStruct tri;
	for(int i=0;i<(int)ph.size();i+=3)
	{
		tri.set(ph[i+0],ph[i+1],ph[i+2]);
		if( rayTriangle(seg,tri,ipt)) return true;
	}
	return false;
}

//--------------------------------------------------------------
bool iTriangle2(const GeoStruct& t1, const GeoStruct& t2, BufV3& pts)
{
	int is = pts.size();
	
	GeoStruct seg[3],seg2[3];
	seg[0].set(t1[0],t1[1]);
	seg[1].set(t1[1],t1[2]);
	seg[2].set(t1[0],t1[2]);
	
	seg2[0].set(t2[0],t2[1]);
	seg2[1].set(t2[1],t2[2]);
	seg2[2].set(t2[0],t2[2]);

	Vec3 ip;
	for(int i=0;i<3;++i)
	{
		if(rayTriangle(seg[i],t2,ip,false)) pts.push_back(ip);
		if(rayTriangle(seg2[i],t1,ip,false)) pts.push_back(ip);
	}
		
	return ((int)pts.size() > is);
}

//--------------------------------------------------------------
void iPolyhedron2(const BufV3& ph1, const BufV3& ph2, BufV3& phOut, bool invRes)
{
	std::vector<bool> ph2i(ph2.size(),false);
	for(int i=0;i<(int)ph1.size();i+=3)
	{
		bool r0 = false;
		GeoStruct tri1; tri1.set(ph1[i+0],ph1[i+1],ph1[i+2]);
		for(int j=0;j<(int)ph2.size();j+=3)
		{
			GeoStruct tri2; tri2.set(ph2[j+0],ph2[j+1],ph2[j+2]);
			BufV3 o;
			bool r = iTriangle2(tri1,tri2,o);
			if( invRes != r )
			{
				r0 = true;
				ph2i[j] = true;
				ph2i[j+1] = true;
				ph2i[j+2] = true;
			}
		}
		
		if(r0)
		{
			phOut.push_back(tri1[0]);
			phOut.push_back(tri1[1]);
			phOut.push_back(tri1[2]);
		}
	}
	
	for(int i=0;i<(int)ph2i.size();++i) if(ph2i[i]) phOut.push_back(ph2[i]);
}

//--------------------------------------------------------------
void selfIntersect2(std::vector<Edge>& edges)
{
	Intersection::Cache precomputed;
	std::vector<Edge> cpy = edges;
	edges.clear();
	
	int N = cpy.size();
	for(int i=0;i<N;++i)
	{
		Edge e1 = cpy[i];
		BufV3 toInsert;
		
		for(int j=0;j<N;++j)
		{
			if(i==j)continue;
			Edge e2 = cpy[j];
			if(e1._p1==e2._p1 || e1._p1==e2._p2 || e1._p2==e2._p1 || e1._p2==e2._p2)continue;
			
			Intersection inter(e1,e2);
			auto found = std::find(precomputed.begin(),precomputed.end(),inter);
			bool needInsert = false;
			if(found != precomputed.end()){inter.ipoint=found->ipoint;needInsert=true;}
			else if(inter.compute()){precomputed.push_back(inter);needInsert=true;}
			
			if(needInsert)toInsert.push_back(inter.ipoint);
		}
		
		// sort and insert by distance to e1._p1
		Vec3 ref = e1._p1;
		auto cmp=[ref](Vec3& v1,Vec3& v2){return (v1-ref).length()<(v2-ref).length();};
		std::sort(toInsert.begin(),toInsert.end(),cmp);
		
		Vec3 last = e1._p1;
		for(auto v : toInsert) { edges.push_back( Edge(last,v) ); last=v; }
		edges.push_back( Edge(last,e1._p2) );
	}
}

//--------------------------------------------------------------
bool iGeoPolyh(const GeoStruct& geo, const BufV3& ph2, std::vector<Edge>& eout)
{
	bool res = false;
	
	for(int j=0;j<(int)ph2.size();j+=3)
	{
		GeoStruct geo2; geo2.set(ph2[j+0],ph2[j+1],ph2[j+2]);
		BufV3 o;
		bool r = iTriangle2(geo,geo2,o);
		// if(o.size() > 2)r = false;
		res = res || r;
		if(r)eout.push_back( Edge(o[0],o[1]) );
	}
	
	return res;
}

std::vector<GeoStruct> cut(const GeoStruct& poly, Edge cutter)
{
	std::vector<GeoStruct> res;
	GeoStruct cpy = poly;
	segTriExtra(cpy, cutter);
	
	GeoStruct geo;
	geo.add(cpy[0]);
	geo.add(cutter._p1);
	geo.add(cutter._p2);
	
	GeoStruct geoA;
	geoA.add(cutter._p1);
	geoA.add(cutter._p2);
	geoA.add(cpy[1]);
	
	GeoStruct geoB;
	geoB.add(cutter._p2);
	geoB.add(cpy[1]);
	geoB.add(cpy[2]);
	
	res.push_back(geo);
	res.push_back(geoA);
	res.push_back(geoB);
	
	return res;
}

std::vector<GeoStruct> multicut(const GeoStruct& poly, std::vector<Edge> cutters)
{
	using GeoBuf = std::vector<GeoStruct>;
	
	GeoBuf res; res.push_back(poly);
	
	// cut poly into triangles using straight lines
	for(int i=0;i<(int)cutters.size();++i)
	{
		GeoBuf swapped;
		for(int j=0;j<(int)res.size();++j)
		{
			GeoBuf r = cut(res[j],cutters[i]);
			for(auto g : r)swapped.push_back(g);
		}
		res = swapped;
	}
	
	return res;
}

void reduce(BufV3& buf, int i, float f)
{
	Vec3 mid = buf[i+0] + buf[i+1] + buf[i+2]; mid *= (1.0/3.0);
	
	for(int j=0;j<3;++j)
	{
		Vec3 d = buf[i+j] - mid;
		d *= f;
		buf[i+j] = mid + d;
	}
}

//--------------------------------------------------------------
void diffPolyhedron2(const BufV3& ph1, const BufV3& ph2, BufV3& phOut, bool inverse)
{
	processBar( 0.0 );
	
	for(int i=0;i<(int)ph1.size();i+=3)
	{
		GeoStruct geo; geo.set(ph1[i+0],ph1[i+1],ph1[i+2]);
		Vec3 n = geo.normal();
		
		BufV3 pout;
		std::vector<Edge> eout;
		if( iGeoPolyh(geo, ph2, eout) )
		{
			std::vector<GeoStruct> buf3v = multicut(geo, eout);
			
			for(auto g : buf3v)
			{
				Vec3 mid = g[0] + g[1] + g[2]; mid *= (1.0/3.0);
				if(inverse == contains(ph2, mid))
				{
					phOut.push_back(g[0]);
					phOut.push_back(g[1]);
					phOut.push_back(g[2]);
				}
			}
			
		}
		else
		{
			Vec3 mid = geo[0] + geo[1] + geo[2]; mid *= (1.0/3.0);
			if(inverse == contains(ph2, mid))
			{
				phOut.push_back(geo[0]);
				phOut.push_back(geo[1]);
				phOut.push_back(geo[2]);
			}
		}
		processBar( (float)i/(float)ph1.size() );
	}
	
	processBar( 1.0 );
	std::cout << std::endl;
}

void reducepolyh(BufV3& ph1, float f)
{
	for(int i=0;i<(int)ph1.size();i+=3) reduce(ph1,i,f);
}
