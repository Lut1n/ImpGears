#include <SFML/Graphics.hpp>

#include "SimplePolygon.h"
#include "ComplexPolygon.h"

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
	SimplePolygon* cy;
	
	void setSimplePolygon(SimplePolygon* _cy){cy=_cy;}
	
	CellFrag(imp::Rasterizer* rast):rast(rast){}
	
    virtual void exec(imp::ImageBuf& targets, const Vec3& pt, imp::Uniforms* uniforms = nullptr)
	{
		float mx = 0.0;
		float val = (cy->_path.vertices[0]-pt).length2();
		for(auto v : cy->_path.vertices)
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



SimplePolygon generateSaw();
SimplePolygon generateCirle(float radius, int sub=10);
SimplePolygon generateButterfly(float radius, float angle);
SimplePolygon generateAnvil(float radius, float angle);
void smaller(SimplePolygon& poly,float scale=0.8);
void drawPlain(imp::Rasterizer& rast, SimplePolygon& poly);
void draw(imp::Rasterizer& rast, SimplePolygon& poly);
void drawFan(imp::Rasterizer& rast, SimplePolygon& poly, float smaller=1.0);
void render(imp::Rasterizer& rast, const SimplePolygon& polygon, Vec4 color, bool wireframe=false, float sml=1.0);
void render(imp::Rasterizer& rast, const ComplexPolygon& poly, Vec4 color, bool wireframe=false, float sml=1.0);
CellFrag::Ptr cell;

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int width=500,height=500;
	
	sf::RenderWindow window(sf::VideoMode(width, height), "SimplePolygon bench", sf::Style::Default, sf::ContextSettings(24));
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
	
	SimplePolygon cycle1 = generateCirle(100,10);
	SimplePolygon saw = generateSaw();
	SimplePolygon polygon2 = generateAnvil(100.0, 30.0);
	SimplePolygon polygon3 = generateButterfly(100,30.0);
	SimplePolygon polygon2_bis = polygon2;
	Path sawCutData;
	sawCutData.vertices.push_back(Vec3(-5.0,-20.0,0.0));
	sawCutData.vertices.push_back(Vec3(5.0,-20.0,0.0));
	sawCutData.vertices.push_back(Vec3(0.0,20.0,0.0));
	sawCutData.vertices.push_back(Vec3(-2.0,7.0,0.0));
	SimplePolygon sawCut(sawCutData);
	
	polygon2  += Vec3(100,200,0);
	cycle1  += Vec3(300,200,0);
	polygon3  += Vec3(100,400,0);
	polygon2_bis.rotation(-2.5);
	polygon2_bis  += Vec3(100,110,0);
	
	ComplexPolygon holed(cycle1._path);
	cycle1 -= Vec3(300,200,0);
	cycle1 *= 0.3;
	cycle1.rotation(45.5);
	cycle1 += Vec3(315,250,0);
	
	holed = holed - ComplexPolygon(cycle1._path);
	
	saw *= Vec3(5,5,0);
	sawCut *= Vec3(5,5,0);
	
	SimplePolygon saw2 = saw;
	SimplePolygon sawCut2 = sawCut;
	
	SimplePolygon saw3 = saw;
	SimplePolygon sawCut3 = sawCut;
	
	saw += Vec3(350,400,0);
	sawCut += Vec3(370,400,0);
	saw2 += Vec3(100,400,0);
	sawCut2 += Vec3(120,400,0);
	saw3 += Vec3(350,200,0);
	sawCut3 += Vec3(370,200,0);
	
	ComplexPolygon poly2sub = ComplexPolygon(polygon2._path)+ComplexPolygon(polygon2_bis._path);
	ComplexPolygon cutedSaw = ComplexPolygon(saw._path)-ComplexPolygon(sawCut._path);
	ComplexPolygon forgedSaw = ComplexPolygon(saw2._path)+ComplexPolygon(sawCut2._path);
	
	poly2sub = poly2sub.resolveHoles();
	cutedSaw = cutedSaw.resolveHoles();
	forgedSaw = forgedSaw.resolveHoles();
	holed = holed.resolveHoles();
	
	render(rast,holed,imp::Vec4(250,208,20,255));
	render(rast,polygon3,imp::Vec4(10,100,10,255));
	render(rast,cutedSaw,imp::Vec4(20,20,250,255));
	render(rast,forgedSaw,imp::Vec4(20,20,250,255),false,1.0);
	render(rast,poly2sub,imp::Vec4(250,208,20,255),false,1.0);
	rast.setFragCallback(cell);
	render(rast,saw3,imp::Vec4(20,20,250,255),true);
	render(rast,sawCut3,imp::Vec4(250,20,20,255),true);
	
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

SimplePolygon generateCirle(float radius, int sub)
{
	Path polygon;
	for(int i=0;i<sub;++i)
	{
		float r = (float)i * 3.14*2.0 / sub;
		imp::Vec3 p = Vec3(std::cos(r),std::sin(r),0) * radius;
		polygon.addVertex(p);
	}
	return SimplePolygon(polygon);
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
	for(auto geo : geometries) {smaller(geo,sml);cell->setSimplePolygon(&geo);if(wireframe)draw(rast,geo); else drawPlain(rast,geo);}
}

void render(imp::Rasterizer& rast, const ComplexPolygon& poly, Vec4 color, bool wireframe, float sml)
{
	for(auto cy : poly._cycles)
	{
		Vec4 col = color;
		if(cy.windingNumber() < 0)col = Vec4(255,255,255,510)-color;
		SimplePolygon primitive(cy);
		render(rast,primitive,col, wireframe, sml);
	}
}
