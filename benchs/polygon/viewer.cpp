#include <SFML/Graphics.hpp>

#include <ImpGears/Graphics/Rasterizer.h>
#include <ImpGears/Graphics/Image.h>

#include <ImpGears/Geometry/Triangulator.h>


using Vec3 = imp::Vec3;
using Vec4 = imp::Vec4;
using Edge = imp::Edge;
using Path = imp::Path;
using CPO = imp::Polygon;


CPO circle(float radius, int sub=10);
void render(imp::Rasterizer& rast, const CPO& poly, const Vec4& color1, const Vec4& color2);

imp::Rasterizer rast; 

imp::Vec4 selectColor(int c)
{
	imp::Vec4 color(255,255,255,255);
	switch(c)
	{
		case 0: color = imp::Vec4(255,0,0,255); break;
		case 1: color = imp::Vec4(0,255,0,255); break;
		case 2: color = imp::Vec4(0,0,255,255); break;
		case 3: color = imp::Vec4(0,255,255,255); break;
		case 4: color = imp::Vec4(255,0,255,255); break;
		case 5: color = imp::Vec4(255,255,0,255); break;
		default : color = imp::Vec4(255,255,255,255); break;
	};
	return color;
}

void display(CPO& poly, int l, int t, int col = 0, int col2 = 1)
{
	imp::Vec4 color1 = selectColor(col);
	imp::Vec4 color2 = selectColor(col2);
	
	Vec3 center(((float)l+0.5)*1000/5,((float)t+0.5)*600/4,0);
	CPO toDisplay = poly; toDisplay += center;

	CPO geometries = imp::Triangulation::triangulate(toDisplay);
	
	render(rast,geometries,color1,color2);
}

CPO glyph(Vec3 center1, float d1, Vec3 center2, float d2)
{
	CPO c1 = circle(d1,4); c1.rotation(3.14/4);
	CPO c2 = circle(d2,4); c2.rotation(3.14/4);
	c1 += Vec3(center1);
	c2 += Vec3(center2);
	return c1 - c2;
}

void gridIntersection(std::vector<CPO>& polys)
{
	CPO cp1,cp2;
	Vec3 center(400,300,0);
	
	CPO prim = circle(60,12) - circle(40,12);
	CPO part1 = prim;
	CPO part2 = prim; part2 *= Vec3(0.5,0.5,0.5);
	
	cp1 = part1; cp2 = part2;
	cp1 -= Vec3(30,0,0);  cp2 += Vec3(20,0,0);
	CPO ttover_p1 = cp1;
	CPO ttover_p2 = cp2;
	CPO ttover_add = cp1 + cp2;
	CPO ttover_sub = cp1 - cp2;
	CPO ttover_int = cp1 * cp2;
	CPO ttover_sub2 = cp2 - cp1;
	
	cp1 = part1; cp2 = part2;
	cp1 -= Vec3(30,0,0);  cp2 += Vec3(50,0,0);
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
	
	CPO glyph_union = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(0.0,-7.0,0.0), 15);
	CPO glyph_diff = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(-5.0,5.0,0.0), 15);
	CPO glyph_inter = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(0.0,7.0,0.0), 15);
	CPO glyph_diff2 = glyph(Vec3(0.0,0.0,0.0), 20, Vec3(5.0,5.0,0.0), 15);
	display(glyph_union, 1, 0);
	display(glyph_diff, 2, 0);
	display(glyph_inter, 3, 0);
	display(glyph_diff2, 4, 0);
	
	std::vector<CPO> polys;
	// gridAnalytic(polys);
	gridIntersection(polys);
	
	display(polys[0],0,1,1,2); display(polys[1],0,1,1,2);
	display(polys[2], 1,1,0,1);
	display(polys[3], 2,1,0,1);
	display(polys[4], 3,1,0,1);
	display(polys[5], 4,1,0,1);
	
	display(polys[6],0,2,1,2); display(polys[7],0,2,1,2);
	display(polys[8], 1,2,0,1);
	display(polys[9], 2,2,0,1);
	display(polys[10], 3,2,0,1);
	display(polys[11], 4,2,0,1);
	
	display(polys[12],0,3,1,2); display(polys[13],0,3,1,2);
	display(polys[14], 1,3,0,1);
	display(polys[15], 2,3,0,1);
	display(polys[16], 3,3,0,1);
	display(polys[17], 4,3,0,1);
	
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

void render(imp::Rasterizer& rast, const CPO& input, const Vec4& color1, const Vec4& color2)
{
	for(auto geo : input._paths)
	{
		rast.setColor(color1);
		rast.triangle(geo.vertex(0),geo.vertex(1),geo.vertex(2));
		
		rast.setColor(color2);	
		for(int i=0;i<geo.count();++i)
		{
			rast.line(geo.vertex(i-1),geo.vertex(i));
			rast.square(geo.vertex(i),3);
		}
	}
}
