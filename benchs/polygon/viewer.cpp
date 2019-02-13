#include <SFML/Graphics.hpp>

#include <Geometry/Polygon.h>

#include <Graphics/Rasterizer.h>
#include <Graphics/Image.h>
#include <Core/Perlin.h>

using Vec3 = imp::Vec3;
using Vec4 = imp::Vec4;
using Edge = imp::Edge;
using Path = imp::Path;
using CPO = imp::Polygon;


CPO generateSaw();
CPO circle(float radius, int sub=10);
CPO generateButterfly(float radius, float angle);
CPO generateAnvil(float radius, float angle);
void smaller(CPO& poly,float scale=0.8);
void drawPlain(imp::Rasterizer& rast, Path& poly);
void draw(imp::Rasterizer& rast, const Path& poly);
void render(imp::Rasterizer& rast, const Path& poly, Vec4 color, bool wireframe=false, float sml=1.0);
void render(imp::Rasterizer& rast, const CPO& poly, Vec4 color, bool wireframe=false, float sml=1.0);
CPO triangulate(const std::vector<imp::Edge>& edges,const CPO& poly);
CPO triangulatePolygon(const CPO& poly);
void draw(imp::Rasterizer& rast, const std::vector<imp::Edge>& edges);

bool msg_log = false;
imp::Rasterizer rast; 
void display(CPO& poly, int l, int t, int mode = 0, int col = 0)
{
	imp::Vec4 color(255,255,255,255);
	switch(col)
	{
		case 0: color = imp::Vec4(255,0,0,255); break;
		case 1: color = imp::Vec4(0,255,0,255); break;
		case 2: color = imp::Vec4(0,0,255,255); break;
		case 3: color = imp::Vec4(0,255,255,255); break;
		case 4: color = imp::Vec4(255,0,255,255); break;
		case 5: color = imp::Vec4(255,255,0,255); break;
		default : color = imp::Vec4(255,255,255,255); break;
	};
	
	Vec3 center(((float)l+0.5)*1000/5,((float)t+0.5)*600/4,0);
	CPO toDisplay = poly; toDisplay += center;
	render(rast,toDisplay,color,mode);
}


std::vector<imp::Edge> triangulate(const CPO& poly);

void display2(CPO& poly, int l, int t, int mode = 0, int col = 0)
{
	imp::Vec4 color(255,255,255,255);
	switch(col)
	{
		case 0: color = imp::Vec4(255,0,0,255); break;
		case 1: color = imp::Vec4(0,255,0,255); break;
		case 2: color = imp::Vec4(0,0,255,255); break;
		case 3: color = imp::Vec4(0,255,255,255); break;
		case 4: color = imp::Vec4(255,0,255,255); break;
		case 5: color = imp::Vec4(255,255,0,255); break;
		default : color = imp::Vec4(255,255,255,255); break;
	};
	
	rast.setColor(color);
	Vec3 center(((float)l+0.5)*1000/5,((float)t+0.5)*600/4,0);
	CPO toDisplay = poly; toDisplay += center;
	
	render(rast,toDisplay,color,mode);
}


CPO glyph(Vec3 center1, float d1, Vec3 center2, float d2)
{
	CPO c1 = circle(d1,4); c1.rotation(3.14/4);
	CPO c2 = circle(d2,4); c2.rotation(3.14/4);
	c1 += Vec3(center1);
	c2 += Vec3(center2);
	return c1 - c2;
}


void gridAnalytic(std::vector<CPO>& polys)
{
	CPO cp1,cp2;
	Vec3 center(500,300,0);
	
	CPO prim = circle(50,4) - circle(40,4);
	CPO prim2 = circle(50,4);
	CPO prim3 = circle(5,4); prim3 -= Vec3(40,0,0);
	CPO part1 = prim;
	CPO part2 = prim; part2 *= Vec3(0.5,0.5,0.5);
	CPO part3 = prim2 - prim3;
	
	cp1 = part1; cp2 = part2;
	// cp1 -= Vec3(30,0,0);  cp2 += Vec3(30,0,0);
	CPO ttover_p1 = cp1;
	CPO ttover_p2 = cp2;
	CPO ttover_add = cp1 + cp2;
	CPO ttover_sub = cp1 - cp2;
	CPO ttover_int = cp1 * cp2;
	CPO ttover_sub2 = cp2 - cp1;
	
	cp1 = part1; cp2 = part2;
	cp1 -= Vec3(40,0,0);  cp2 += Vec3(50,0,0);
	CPO bover_p1 = cp1;
	CPO bover_p2 = cp2;
	CPO bover_add = cp1 + cp2;
	CPO bover_sub = cp1 - cp2;
	CPO bover_int = cp1 * cp2;
	CPO bover_sub2 = cp2 - cp1;
	
	cp1 = part3; cp2 = part2;
	// cp1 += Vec3(0,0,0);  cp2 += Vec3(18,0,0);
	CPO hover_p1 = cp1;
	CPO hover_p2 = cp2;
	CPO hover_add = cp1 + cp2;
	CPO hover_sub = cp1 - cp2;
	CPO hover_int = cp1 * cp2;
	CPO hover_sub2 = cp2 - cp1;
	
	polys.push_back(ttover_p1);
	polys.push_back(ttover_p2);
	polys.push_back(ttover_add);
	polys.push_back(ttover_sub);
	polys.push_back(ttover_int);
	polys.push_back(ttover_sub2);
	
	polys.push_back(bover_p1);
	polys.push_back(bover_p2);
	polys.push_back(bover_add);
	polys.push_back(bover_sub);
	polys.push_back(bover_int);
	polys.push_back(bover_sub2);
	
	polys.push_back(hover_p1);
	polys.push_back(hover_p2);
	polys.push_back(hover_add);
	polys.push_back(hover_sub);
	polys.push_back(hover_int);
	polys.push_back(hover_sub2);
}

void gridIntersection(std::vector<CPO>& polys)
{
	CPO cp1,cp2;
	Vec3 center(400,300,0);
	
	CPO prim = circle(70,4) - circle(50,4);
	CPO part1 = prim;
	CPO part2 = prim; part2 *= Vec3(0.5,0.5,0.5);
	
	cp1 = part1; cp2 = part2;
	cp1 -= Vec3(30,0,0);  cp2 += Vec3(32,0,0);
	CPO ttover_p1 = cp1;
	CPO ttover_p2 = cp2;
	CPO ttover_add = cp1 + cp2;
	CPO ttover_sub = cp1 - cp2;
	CPO ttover_int = cp1 * cp2;
	CPO ttover_sub2 = cp2 - cp1;
	
	cp1 = part1; cp2 = part2;
	cp1 -= Vec3(35,0,0);  cp2 += Vec3(62,0,0);
	CPO bover_p1 = cp1;
	CPO bover_p2 = cp2;
	CPO bover_add = cp1 + cp2;
	CPO bover_sub = cp1 - cp2;
	CPO bover_int = cp1 * cp2;
	CPO bover_sub2 = cp2 - cp1;
	
	cp1 = part1; cp2 = part2;
	cp1 += Vec3(0,0,0);  cp2 += Vec3(18,0,0);
	CPO hover_p1 = cp1;
	CPO hover_p2 = cp2;
	CPO hover_add = cp1 + cp2;
	CPO hover_sub = cp1 - cp2;
	CPO hover_int = cp1 * cp2;
	CPO hover_sub2 = cp2 - cp1;
	
	polys.push_back(ttover_p1);
	polys.push_back(ttover_p2);
	polys.push_back(ttover_add);
	polys.push_back(ttover_sub);
	polys.push_back(ttover_int);
	polys.push_back(ttover_sub2);
	
	polys.push_back(bover_p1);
	polys.push_back(bover_p2);
	polys.push_back(bover_add);
	polys.push_back(bover_sub);
	polys.push_back(bover_int);
	polys.push_back(bover_sub2);
	
	polys.push_back(hover_p1);
	polys.push_back(hover_p2);
	polys.push_back(hover_add);
	polys.push_back(hover_sub);
	polys.push_back(hover_int);
	polys.push_back(hover_sub2);
}


// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int width=1000,height=600;
	
	sf::RenderWindow window(sf::VideoMode(width, height), "CPO bench", sf::Style::Default, sf::ContextSettings(24));
	sf::Texture texture;
	texture.create(width, height);
	sf::Sprite sprite;
	sprite.setTexture(texture);
	// sprite.setPosition(0,600);
	// sprite.setScale(1,-1);
	
	imp::Image::Ptr image = imp::Image::create(width,height,4);
    imp::Vec4 bgcol(50,128,200,255);
	image->fill(bgcol);
	rast.setTarget(image);
	
	float div = 1000/5;
	float div2 = 600/4;
	Vec3 t1(div,0,0), t2(div*2,0,0),t3(div*3,0,0), t4(div*4,0,0);
	Vec3 b1(div,600,0),b2(div*2,600,0),b3(div*3,600,0), b4(div*4,600,0);
	Vec3 l1(0,div2,0),l2(0,div2*2,0),l3(0,div2*3,0);
	Vec3 r1(1000,div2,0),r2(1000,div2*2,0),r3(1000,div2*3,0);
	rast.line(t1, b1);
	rast.line(t2, b2);
	rast.line(t3, b3);
	rast.line(t4, b4);
	rast.line(l1, r1);
	rast.line(l2, r2);
	rast.line(l3, r3);
	
	msg_log = false;
	
	CPO glyph_union = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(0.0,-7.0,0.0), 10);
	CPO glyph_diff = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(-5.0,5.0,0.0), 15);
	CPO glyph_inter = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(0.0,7.0,0.0), 10);
	CPO glyph_diff2 = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(5.0,5.0,0.0), 15);
	display2(glyph_union, 1, 0);
	display2(glyph_diff, 2, 0);
	display2(glyph_inter, 3, 0);
	display(glyph_diff2, 4, 0,0);
	
	std::vector<CPO> polys;
	gridAnalytic(polys);
	// gridIntersection(polys);
	
	display2(polys[0],0,1,true); display2(polys[1],0,1,true);
	display2(polys[2], 1,1,0,1);	// crash
	display2(polys[3], 2,1,0,1);
	display2(polys[4], 3,1,0,1);
	display2(polys[5], 4,1,0,1);
	
	display2(polys[6],0,2,true); display2(polys[7],0,2,true);
	display2(polys[8], 1,2,0,1);
	display2(polys[9], 2,2,0,1);
	display2(polys[10], 3,2,0,1);
	display2(polys[11], 4,2,0,1);
	
	display2(polys[12],0,3,true); display2(polys[13],0,3,true);
	display2(polys[14], 1,3,0,1);
	display2(polys[15], 2,3,0,1);	// crash
	display2(polys[16], 3,3,0,1);
	display2(polys[17], 4,3,0,1);
	
	texture.update(image->asGrid()->data());
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if(event.type==sf::Event::Closed)window.close();
		}
		if(!window.isOpen())break;
		
		window.clear();
		window.draw(sprite);
		window.display();
	}
   
    return 0;
}

CPO generateSaw()
{
	Path cy;
	
	cy.addVertex(Vec3(-10.0,10.0,0.0));
	cy.addVertex(Vec3(-10.0,-10.0,0.0));
	cy.addVertex(Vec3(5.0,-10.0,0.0));
	cy.addVertex(Vec3(20.0,-10.0,0.0));
	cy.addVertex(Vec3(5.0,-8.0,0.0));
	cy.addVertex(Vec3(-5.0,-5.0,0.0));
	cy.addVertex(Vec3(20.0,0.0,0.0));
	cy.addVertex(Vec3(-5.0,5.0,0.0));
	cy.addVertex(Vec3(20.0,10.0,0.0));
	
	return CPO(cy);
}

CPO circle(float radius, int sub)
{
	Path polygon;
	for(int i=0;i<sub;++i)
	{
		float r = (float)i * 3.14*2.0 / sub;
		imp::Vec3 p = Vec3(std::cos(r),std::sin(r),0) * radius;
		polygon.addVertex(p);
	}
	
	std::vector<Path> buf; buf.push_back(polygon);
	
	return CPO(buf);
}

CPO generateButterfly(float radius, float angle)
{
	Path polygon;
	float r = angle * 3.14 / 180.0;
	float c = std::cos(r);
	float s = std::sin(r);
	polygon.addVertex(Vec3(-c*0.5,s,0) * radius);
	polygon.addVertex(Vec3(-c*0.5,-s,0) * radius);
	polygon.addVertex(Vec3(c,s*2,0) * radius);
	polygon.addVertex(Vec3(c*1.5,-s,0) * radius);
	polygon.addVertex(Vec3(c*1.5,s,0) * radius);
	polygon.addVertex(Vec3(c,-s*2,0) * radius);
	return CPO(polygon);
}

void drawArrow(imp::Rasterizer& rast, Vec3 p1, Vec3 p2)
{
	rast.line(p1,p2);
	Vec3 n = p1-p2; n.normalize();
	Vec3 bt = imp::Vec3::Z.cross(n); bt.normalize();
	
	Vec3 a1 = p2 + n*5.0 - bt*2.0;
	Vec3 a2 = p2 + n*5.0 + bt*2.0;
	rast.line(p2,a1);
	rast.line(p2,a2);
}

void drawPlain(imp::Rasterizer& rast, Path& poly)
{
	for(int i=0;i<poly.count();++i)
		rast.triangle(poly.vertex(i-2),poly.vertex(i-1),poly.vertex(i));
}


void draw(imp::Rasterizer& rast, const std::vector<imp::Edge>& edges)
{
	for(auto e : edges)
	{
		rast.line(e._p1,e._p2);
		// drawArrow(rast,poly.vertex(i-1),poly.vertex(i));
		rast.square(e._p1,3);
	}
}

void draw(imp::Rasterizer& rast, const Path& poly)
{
	for(int i=0;i<poly.count();++i)
	{
		rast.line(poly.vertex(i-1),poly.vertex(i));
		drawArrow(rast,poly.vertex(i-1),poly.vertex(i));
		rast.square(poly.vertex(i),3);
	}
}

void smaller(Path& poly,float scale)
{
	Vec3 center = poly.gravity();
	// Path path = poly._path;
	for(auto& v : poly.vertices)v=center+(v-center)*scale;
	// poly = Polygon(path);
}

CPO generateAnvil(float radius, float angle)
{
	Path result;
	
	result.addVertex(Vec3(0.0,0.0,0.0));
	result.addVertex(Vec3(0.5,-1.0,0.0)*radius);
	result.addVertex(Vec3(-0.7,0.3,0.0)*radius);
	result.addVertex(Vec3(0.7,0.3,0.0)*radius);
	result.addVertex(Vec3(-0.5,-1.0,0.0)*radius);
	
	return CPO(result);
}

void render(imp::Rasterizer& rast, const CPO& input, Vec4 color, bool wireframe, float sml)
{
	// CPO poly = input.resolveHoles();
	
	// for(auto cy : input._paths)
	{
		Vec4 col = color;
		// if(cy.windingNumber() < 0)col = Vec4(255,255,255,510)-color;
		
		rast.setColor(col);
		std::vector<imp::Edge> edges = triangulate(input);
		CPO geometries = triangulate(edges,input);
		// CPO geometries = triangulatePolygon(cy);	// get triangles to render
		for(auto geo : geometries._paths) {smaller(geo,sml);if(wireframe)draw(rast,geo); else drawPlain(rast,geo);}
	}
}




struct Triplet
{
	Vec3 p1,p2,p3;
	
	Triplet(){}
	
	Triplet(Vec3 p1,Vec3 p2, Vec3 p3)
		: p1(p1)
		, p2(p2)
		, p3(p3)
	{}
	
	bool contains(Vec3 p) const
	{
		return p==p1 || p==p2 || p==p3;
	}
	
	bool operator==(const Triplet& other) const
	{
		return contains(other.p1) && contains(other.p2) && contains(other.p3);
	}
};

CPO triangulate(const std::vector<imp::Edge>& edges,const CPO& poly)
{
	std::vector<Triplet> buf;
	
	for(auto e1:edges)
	{
		for(auto e2:edges)
		{
			if(e1 == e2)continue;
			if(!e1.connectedTo(e2)) continue;
			
			imp::Edge e3;
			if(e1._p1==e2._p1) e3 = imp::Edge(e1._p2,e2._p2);
			else if(e1._p1==e2._p2) e3 = imp::Edge(e1._p2,e2._p1);
			else if(e1._p2==e2._p1) e3 = imp::Edge(e1._p1,e2._p2);
			else if(e1._p2==e2._p2) e3 = imp::Edge(e1._p1,e2._p1);
			else {std::cout << "prout" <<std::endl;}
			
			bool fnd = std::find(edges.begin(),edges.end(),e3) != edges.end();
			if(!fnd)continue;
			
			Triplet t; t.p1=e1._p1;t.p2=e1._p2;
			if(e2._p1==e1._p1 || e2._p1==e1._p2)t.p3=e2._p2;
			else t.p3=e2._p1;
			
			imp::Vec3 c = (t.p1 + t.p2 + t.p3) * (1.0/3.0);
			if(!poly.inside(c))continue;
			
			if(std::find(buf.begin(),buf.end(),t)==buf.end())buf.push_back(t);
		}
	}
	
	CPO res;
	for(auto t:buf)
	{
		Path p;p.addVertex(t.p1);p.addVertex(t.p2);p.addVertex(t.p3);
		res.addPath(p);
	}
	return res;
}


void msg(std::string s)
{
	if(msg_log)
		std::cout << s << std::endl;
}

std::vector<imp::Edge> triangulate(const CPO& poly)
{
	std::vector<imp::Edge> edges;
	std::vector<imp::Vec3> points;
	
	// conversion
	for(auto p:poly._paths)
	{
		for(int i=0; i<p.count();++i)
		{
			edges.push_back(p.edge(i));
			points.push_back(p.vertex(i));
		}
	}
	
	for(int i=0;i<(int)points.size();++i)
		for(int j=0;j<(int)points.size();++j)
	{
		if(i==j) {msg("jump edge : is point"); continue;}
		
		Edge e(points[i], points[j]);

		// check edges already selected
		auto found = std::find(edges.begin(),edges.end(),e);
		if(found != edges.end()){msg("jump edge : already selected"); continue;}
		
		// check holes
		imp::Vec3 mid = (points[i] + points[j]) * 0.5;
		if( !poly.inside(mid) ) {msg("jump edge : inside hole"); continue;}
		
		// check intersection with other edges
		bool xPoly = false;
		for(auto e2:edges)
		{
			if(e.connectedTo(e2))continue;
			imp::Intersection xee(e,e2);
			if(xee.compute()){xPoly=true;break;}
		}
		if(xPoly){msg("jump edge : intersection"); continue;}
		
		edges.push_back(e);
	}
	
	return edges;
}
