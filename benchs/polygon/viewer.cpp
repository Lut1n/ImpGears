#include <SFML/Graphics.hpp>

#include "PolygonSet.h"

#include <Graphics/Rasterizer.h>
#include <Graphics/Image.h>
#include <Core/Perlin.h>


struct MyFrag : public imp::FragCallback
{
    Meta_Class(MyFrag)
	
	imp::Rasterizer* rast;
	
	MyFrag(imp::Rasterizer* rast):rast(rast){}
	
    virtual void exec(imp::ImageBuf& targets, const Vec3& pt, imp::Uniforms* uniforms = nullptr)
	{
		double v = imp::perlinOctave(pt[0]/500.0, pt[1]/500.0, 0.0, 6, 0.7, 10.0);
		Vec4 color(v,v,v,0);
		color += 1.0; color *= 0.5;
		color[3]=1.0;
		color *= rast->_defaultColor;
		
		for(auto img:targets)img->setPixel(pt[0],pt[1],color);
	}
};

struct CellFrag : public imp::FragCallback
{
    Meta_Class(CellFrag)
	
	imp::Rasterizer* rast;
	Cycle* cy;
	
	void setCycle(Cycle* _cy){cy=_cy;}
	
	CellFrag(imp::Rasterizer* rast):rast(rast){}
	
    virtual void exec(imp::ImageBuf& targets, const Vec3& pt, imp::Uniforms* uniforms = nullptr)
	{
		float mx = 0.0;
		float val = (cy->vertices[0]-pt).length2();
		for(auto v : cy->vertices)
		{
			float len = (pt-v).length2();
			if(len < val) val=len;
			else if(len > mx) mx=len;
		}
		
		val /= mx;
		// val = 1.0 - val;
		Vec4 color(val,val,val,1.0);
		// color += 1.0; color *= 0.5;
		// color[3]=1.0;
		color *= rast->_defaultColor;
		
		for(auto img:targets)img->setPixel(pt[0],pt[1],color);
	}
};



Cycle generateSaw();
Cycle generateCirle(float radius, int sub=10);
Cycle generateButterfly(float radius, float angle);
Cycle generateAnvil(float radius, float angle);
void smaller(Cycle& poly,float scale=0.8);
void drawPlain(imp::Rasterizer& rast, Cycle& poly);
void draw(imp::Rasterizer& rast, Cycle& poly);
void drawFan(imp::Rasterizer& rast, Cycle& poly, float smaller=1.0);
void render(imp::Rasterizer& rast, const Cycle& polygon, Vec4 color, bool wireframe=false, float sml=1.0);
void render(imp::Rasterizer& rast, const PolygonSet& poly, Vec4 color, bool wireframe=false, float sml=1.0);
CellFrag::Ptr cell;

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int width=500,height=500;
	
	sf::RenderWindow window(sf::VideoMode(width, height), "Cycle bench", sf::Style::Default, sf::ContextSettings(24));
	sf::Texture texture;
	texture.create(width, height);
	sf::Sprite sprite;
	sprite.setTexture(texture);
	
	imp::Image::Ptr image = imp::Image::create(width,height,4);
    imp::Vec4 bgcol(50,128,200,255);
	image->fill(bgcol);
	
	imp::Rasterizer rast;
	rast.setTarget(image);
	MyFrag::Ptr frag = MyFrag::create(&rast);
	cell = CellFrag::create(&rast);
	rast.setFragCallback(frag);
	
	Cycle cycle1 = generateCirle(100,10);
	Cycle saw = generateSaw();
	Cycle polygon2 = generateAnvil(100.0, 30.0);
	Cycle polygon3 = generateButterfly(100,30.0);
	Cycle polygon2_bis = polygon2;
	Cycle sawCut;
	sawCut.vertices.push_back(Vec3(-5.0,-20.0,0.0));
	sawCut.vertices.push_back(Vec3(5.0,-20.0,0.0));
	sawCut.vertices.push_back(Vec3(0.0,20.0,0.0));
	sawCut.vertices.push_back(Vec3(-2.0,7.0,0.0));
	polygon2  += Vec3(100,200,0);
	cycle1  += Vec3(300,200,0);
	polygon3  += Vec3(100,400,0);
	polygon2_bis.rotation(-2.5);
	polygon2_bis  += Vec3(100,110,0);
	
	PolygonSet holed; holed.addCycle(cycle1);
	cycle1 -= Vec3(300,200,0);
	cycle1 *= 0.3;
	cycle1 += Vec3(310,250,0);
	holed.addCycle(cycle1,true);
	
	saw *= Vec3(5,5,0);
	sawCut *= Vec3(5,5,0);
	
	Cycle saw2 = saw;
	Cycle sawCut2 = sawCut;
	
	Cycle saw3 = saw;
	Cycle sawCut3 = sawCut;
	
	saw += Vec3(350,400,0);
	sawCut += Vec3(370,400,0);
	saw2 += Vec3(100,400,0);
	sawCut2 += Vec3(120,400,0);
	saw3 += Vec3(350,200,0);
	sawCut3 += Vec3(370,200,0);
	
	std::cout << "windingNumber poly2 bis = " << polygon2_bis.windingNumber() << std::endl;
	std::cout << "windingNumber saw = " << saw.windingNumber() << std::endl;
	std::cout << "windingNumber sawCut2 = " << sawCut2.windingNumber() << std::endl;
	
	PolygonSet poly2sub = PolygonSet(polygon2)+PolygonSet(polygon2_bis);
	std::cout << "break hole poly2sub :" << std::endl;
	poly2sub.resolveHoles();
	PolygonSet cutedSaw = PolygonSet(saw)-PolygonSet(sawCut);
	PolygonSet forgedSaw = PolygonSet(saw2)*PolygonSet(sawCut2);
	std::cout << "saw2 cnt = "  <<forgedSaw._cycles.size() << std::endl;
	std::cout << "break hole cutedSaw :" << std::endl;
	cutedSaw.resolveHoles();
	std::cout << "break hole forgedSaw :" << std::endl;
	forgedSaw.resolveHoles();
	std::cout << "break hole holed :" << std::endl;
	holed.resolveHoles();
	
	render(rast,holed,imp::Vec4(250,208,20,255));
	render(rast,polygon3,imp::Vec4(10,100,10,255));
	render(rast,cutedSaw,imp::Vec4(20,20,250,255));
	render(rast,forgedSaw,imp::Vec4(20,20,250,255));
	rast.setFragCallback(cell);
	render(rast,saw3,imp::Vec4(20,20,250,255),true);
	render(rast,sawCut3,imp::Vec4(250,20,20,255),true);
	render(rast,poly2sub,imp::Vec4(250,208,20,255),false,1.0);
	
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

Cycle generateSaw()
{
	Cycle cy;
	
	cy.addVertex(Vec3(-10.0,10.0,0.0));
	cy.addVertex(Vec3(-10.0,-10.0,0.0));
	cy.addVertex(Vec3(5.0,-10.0,0.0));
	cy.addVertex(Vec3(20.0,-10.0,0.0));
	cy.addVertex(Vec3(5.0,-5.0,0.0));
	cy.addVertex(Vec3(-5.0,0.0,0.0));
	cy.addVertex(Vec3(20.0,10.0,0.0));
	
	return cy;
}

Cycle generateCirle(float radius, int sub)
{
	Cycle polygon;
	for(int i=0;i<sub;++i)
	{
		float r = (float)i * 3.14*2.0 / sub;
		imp::Vec3 p = Vec3(std::cos(r),std::sin(r),0) * radius;
		polygon.addVertex(p);
	}
	return polygon;
}

Cycle generateButterfly(float radius, float angle)
{
	Cycle polygon;
	float r = angle * 3.14 / 180.0;
	float c = std::cos(r);
	float s = std::sin(r);
	polygon.addVertex(Vec3(-c*0.5,s,0) * radius);
	polygon.addVertex(Vec3(-c*0.5,-s,0) * radius);
	polygon.addVertex(Vec3(c,s*2,0) * radius);
	polygon.addVertex(Vec3(c*1.5,-s,0) * radius);
	polygon.addVertex(Vec3(c*1.5,s,0) * radius);
	polygon.addVertex(Vec3(c,-s*2,0) * radius);
	return polygon;
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

void drawPlain(imp::Rasterizer& rast, Cycle& poly)
{
	for(int i=0;i<poly.count();++i)
		rast.triangle(poly.vertex(i-2),poly.vertex(i-1),poly.vertex(i));
}

void draw(imp::Rasterizer& rast, Cycle& poly)
{
	for(int i=0;i<poly.count();++i)
	{
		drawArrow(rast,poly.vertex(i-1),poly.vertex(i));
		rast.square(poly.vertex(i),5);
	}
}

void drawFan(imp::Rasterizer& rast, Cycle& poly, float scale)
{
	Vec3 center = poly.gravity();
	for(int i=0;i<poly.count();++i)
	{
		Cycle p; p.addVertex(center); p.addVertex(poly.vertex(i-1)); p.addVertex(poly.vertex(i));
		smaller(p,scale);
		draw(rast,p);
	}
}

void smaller(Cycle& poly,float scale)
{
	Vec3 center = poly.gravity();
	for(auto& v : poly.vertices)v=center+(v-center)*scale;
}

Cycle generateAnvil(float radius, float angle)
{
	Cycle result;
	
	result.addVertex(Vec3(0.0,0.0,0.0));
	result.addVertex(Vec3(0.5,-1.0,0.0)*radius);
	result.addVertex(Vec3(-0.7,0.3,0.0)*radius);
	result.addVertex(Vec3(0.7,0.3,0.0)*radius);
	result.addVertex(Vec3(-0.5,-1.0,0.0)*radius);
	
	return result;
}

void render(imp::Rasterizer& rast, const Cycle& polygon, Vec4 color, bool wireframe, float sml)
{
	if(polygon.count() < 3){std::cout << "less than 3 vertex"<<std::endl; return;}
	rast.setColor(color);
	Cycle cy = polygon.simplify();
	// draw(rast,cy);
	std::vector<Cycle> geometries = cy.triangulate();	// get triangles to render
	for(auto geo : geometries) {smaller(geo,sml);cell->setCycle(&geo);if(wireframe)draw(rast,geo); else drawPlain(rast,geo);}
}

void render(imp::Rasterizer& rast, const PolygonSet& poly, Vec4 color, bool wireframe, float sml)
{
	for(auto cy : poly._cycles)
	{
		Vec4 col = color;
		if(cy.windingNumber() < 0)col = Vec4(255,255,255,510)-color;
		render(rast,cy,col, wireframe, sml);
	}
}