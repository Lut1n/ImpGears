#include <SFML/Graphics.hpp>

#include <Geometry/Polygon.h>
#include <Geometry/ComplexPolygon.h>

#include <Graphics/Rasterizer.h>
#include <Graphics/Image.h>
#include <Core/Perlin.h>

using Vec3 = imp::Vec3;
using Vec4 = imp::Vec4;
using Edge = imp::Edge;
using Path = imp::Path;
using SimplePolygon = imp::Polygon;
using ComplexPolygon = imp::ComplexPolygon;
using CPO = imp::ComplexPolygon;


SimplePolygon generateSaw();
ComplexPolygon circle(float radius, int sub=10);
SimplePolygon generateButterfly(float radius, float angle);
SimplePolygon generateAnvil(float radius, float angle);
void smaller(SimplePolygon& poly,float scale=0.8);
void drawPlain(imp::Rasterizer& rast, SimplePolygon& poly);
void draw(imp::Rasterizer& rast, SimplePolygon& poly);
void drawFan(imp::Rasterizer& rast, SimplePolygon& poly, float smaller=1.0);
void render(imp::Rasterizer& rast, const SimplePolygon& polygon, Vec4 color, bool wireframe=false, float sml=1.0);
void render(imp::Rasterizer& rast, const ComplexPolygon& poly, Vec4 color, bool wireframe=false, float sml=1.0);


imp::Rasterizer rast; 
void display(CPO& poly, int l, int t, bool wf = false)
{
	Vec3 center(((float)l+0.5)*800/4,((float)t+0.5)*600/4,0);
	CPO toDisplay = poly; toDisplay += center;
	render(rast,toDisplay,imp::Vec4(0,255,0,255),wf);
}

ComplexPolygon glyph(Vec3 center1, float d1, Vec3 center2, float d2)
{
	ComplexPolygon c1 = circle(d1,4); c1.rotation(3.14/4);
	ComplexPolygon c2 = circle(d2,4); c2.rotation(3.14/4);
	c1 += Vec3(center1);
	c2 += Vec3(center2);
	return c1 - c2;
}

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int width=800,height=600;
	
	sf::RenderWindow window(sf::VideoMode(width, height), "SimplePolygon bench", sf::Style::Default, sf::ContextSettings(24));
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
	
	float div = 800/4;
	float div2 = 600/4;
	Vec3 t1(div,0,0), t2(div*2,0,0),t3(div*3,0,0);
	Vec3 b1(div,600,0),b2(div*2,600,0),b3(div*3,600,0);
	Vec3 l1(0,div2,0),l2(0,div2*2,0),l3(0,div2*3,0);
	Vec3 r1(800,div2,0),r2(800,div2*2,0),r3(800,div2*3,0);
	rast.line(t1, b1);
	rast.line(t2, b2);
	rast.line(t3, b3);
	rast.line(l1, r1);
	rast.line(l2, r2);
	rast.line(l3, r3);
	
	CPO glyph_union = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(0.0,-7.0,0.0), 10);
	CPO glyph_diff = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(-5.0,5.0,0.0), 15);
	CPO glyph_inter = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(0.0,7.0,0.0), 10);
	display(glyph_union, 1, 0);
	display(glyph_diff, 2, 0);
	display(glyph_inter, 3, 0);
	
	
	
	CPO cp1,cp2;
	Vec3 center(400,300,0);
	
	CPO prim = circle(70,4) - circle(50,4);
	CPO part1 = prim;
	CPO part2 = prim; part2 *= Vec3(0.5,0.5,0.5);
	
	cp1 = part1; cp2 = part2;
	cp1 -= Vec3(30,0,0);  cp2 += Vec3(30,0,0);
	CPO ttover_p1 = cp1;
	CPO ttover_p2 = cp2;
	CPO ttover_add = cp1 + cp2;
	CPO ttover_sub = cp1 - cp2;
	CPO ttover_int = cp1 * cp2;
	
	cp1 = part1; cp2 = part2;
	cp1 -= Vec3(35,0,0);  cp2 += Vec3(62,0,0);
	CPO bover_p1 = cp1;
	CPO bover_p2 = cp2;
	CPO bover_add = cp1 + cp2;
	CPO bover_sub = cp1 - cp2;
	CPO bover_int = cp1 * cp2;
	
	cp1 = part1; cp2 = part2;
	cp1 += Vec3(0,0,0);  cp2 += Vec3(18,0,0);
	CPO hover_p1 = cp1;
	CPO hover_p2 = cp2;
	CPO hover_add = cp1 + cp2;
	CPO hover_sub = cp1 - cp2;
	CPO hover_int = cp1 * cp2;
	
	display(ttover_p1,0,1,true); display(ttover_p2,0,1,true);
	display(ttover_add, 1,1);	// crash
	display(ttover_sub, 2,1);
	display(ttover_int, 3,1);
	
	display(bover_p1,0,2,true); display(bover_p2,0,2,true);
	display(bover_add, 1,2);
	display(bover_sub, 2,2);
	display(bover_int, 3,2);
	
	display(hover_p1,0,3,true); display(hover_p2,0,3,true);
	display(hover_add, 1,3);
	display(hover_sub, 2,3);		// crash
	display(hover_int, 3,3);
	
	// cp1 += center;
	// cp2 += center;
	// render(rast,cp1,imp::Vec4(0,255,0,255),false);
	// render(rast,cp2,imp::Vec4(0,255,0,255),false);
	
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

SimplePolygon generateSaw()
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
	
	return SimplePolygon(cy);
}

ComplexPolygon circle(float radius, int sub)
{
	Path polygon;
	for(int i=0;i<sub;++i)
	{
		float r = (float)i * 3.14*2.0 / sub;
		imp::Vec3 p = Vec3(std::cos(r),std::sin(r),0) * radius;
		polygon.addVertex(p);
	}
	
	std::vector<Path> buf; buf.push_back(polygon);
	
	return ComplexPolygon(buf);
}

SimplePolygon generateButterfly(float radius, float angle)
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
	return SimplePolygon(polygon);
}

void drawArrow(imp::Rasterizer& rast, Vec3 p1, Vec3 p2)
{
	rast.line(p1,p2);
	Vec3 n = p1-p2; n.normalize();
	Vec3 bt = imp::Vec3::Z.cross(n); bt.normalize();
	
	Vec3 a1 = p2 + n*20.0 - bt*5.0;
	Vec3 a2 = p2 + n*20.0 + bt*5.0;
	rast.line(p2,a1);
	rast.line(p2,a2);
}

void drawPlain(imp::Rasterizer& rast, SimplePolygon& poly)
{
	for(int i=0;i<poly._path.count();++i)
		rast.triangle(poly._path.vertex(i-2),poly._path.vertex(i-1),poly._path.vertex(i));
}

void draw(imp::Rasterizer& rast, SimplePolygon& poly)
{
	for(int i=0;i<poly._path.count();++i)
	{
		drawArrow(rast,poly._path.vertex(i-1),poly._path.vertex(i));
		rast.square(poly._path.vertex(i),5);
	}
}

void smaller(SimplePolygon& poly,float scale)
{
	Vec3 center = poly.gravity();
	Path path = poly._path;
	for(auto& v : path.vertices)v=center+(v-center)*scale;
	poly = SimplePolygon(path);
}

SimplePolygon generateAnvil(float radius, float angle)
{
	Path result;
	
	result.addVertex(Vec3(0.0,0.0,0.0));
	result.addVertex(Vec3(0.5,-1.0,0.0)*radius);
	result.addVertex(Vec3(-0.7,0.3,0.0)*radius);
	result.addVertex(Vec3(0.7,0.3,0.0)*radius);
	result.addVertex(Vec3(-0.5,-1.0,0.0)*radius);
	
	return SimplePolygon(result);
}

void render(imp::Rasterizer& rast, const SimplePolygon& polygon, Vec4 color, bool wireframe, float sml)
{
	if(polygon._path.count() < 3){std::cout << "less than 3 vertex"<<std::endl; return;}
	rast.setColor(color);
	SimplePolygon cy = polygon.simplify();
	// draw(rast,cy);
	std::vector<SimplePolygon> geometries = cy.triangulate();	// get triangles to render
	for(auto geo : geometries) {smaller(geo,sml);if(wireframe)draw(rast,geo); else drawPlain(rast,geo);}
}

void render(imp::Rasterizer& rast, const ComplexPolygon& input, Vec4 color, bool wireframe, float sml)
{
	ComplexPolygon poly = input.resolveHoles();
	
	for(auto cy : poly._cycles)
	{
		Vec4 col = color;
		if(cy.windingNumber() < 0)col = Vec4(255,255,255,510)-color;
		SimplePolygon primitive(cy);
		render(rast,primitive,col, wireframe, sml);
	}
}
