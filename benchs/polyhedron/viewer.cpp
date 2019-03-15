#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/GraphicState.h>
#include <SceneGraph/RenderTarget.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/VBOData.h>

#include <Geometry/Polyhedron.h>
using namespace imp;

#include <SFML/Graphics.hpp>

#include "shader.h"

void intoCCW( std::vector<float>& buf )
{
	for(int i=0;i<(int)buf.size();i+=9)
	{
		Vec3 p1(buf[i+0],buf[i+1],buf[i+2]);
		Vec3 p2(buf[i+3],buf[i+4],buf[i+5]);
		Vec3 p3(buf[i+6],buf[i+7],buf[i+8]);
		
		Vec3 t12=p2-p1, t13=p3-p1;
		bool ccw = t13.angleFrom(t12,p1) > 0;
		if(!ccw) 
		{
			buf[i+3]=p3[0]; buf[i+4]=p3[1]; buf[i+5]=p3[2];
			buf[i+6]=p2[0]; buf[i+7]=p2[1]; buf[i+8]=p2[2];
		}
	}
}

std::vector<float> intoLineBuf(const std::vector<float>& buf)
{
	std::vector<float> res;
	for(int i=0;i<(int)buf.size();i+=9)
	{
		Vec3 p1(buf[i+0],buf[i+1],buf[i+2]);
		Vec3 p2(buf[i+3],buf[i+4],buf[i+5]);
		Vec3 p3(buf[i+6],buf[i+7],buf[i+8]);
		
		res.push_back(p1[0]); res.push_back(p1[1]); res.push_back(p1[2]);
		res.push_back(p2[0]); res.push_back(p2[1]); res.push_back(p2[2]);
		
		res.push_back(p1[0]); res.push_back(p1[1]); res.push_back(p1[2]);
		res.push_back(p3[0]); res.push_back(p3[1]); res.push_back(p3[2]);
		
		res.push_back(p2[0]); res.push_back(p2[1]); res.push_back(p2[2]);
		res.push_back(p3[0]); res.push_back(p3[1]); res.push_back(p3[2]);
	}
	
	return res;
}

class Wire : public VBOData, public SceneNode
{
    public:
		
		Meta_Class(Wire)
	
        Wire(Vec3 p1, Vec3 p2)
		{
			 _vertexBuffer.push_back(p1[0]);
			 _vertexBuffer.push_back(p1[1]);
			 _vertexBuffer.push_back(p1[2]);
			 
			 _vertexBuffer.push_back(p2[0]);
			 _vertexBuffer.push_back(p2[1]);
			 _vertexBuffer.push_back(p2[2]);
			 
			setPrimitive(Primitive_Lines);
			
			std::uint32_t vertexBuffSize = _vertexBuffer.size()*sizeof(float);
			requestVBO(vertexBuffSize);
			setVertices(_vertexBuffer.data(), vertexBuffSize);
			
			_shader = Shader::create(vertexSimple.c_str(), fragSimple.c_str());
			u_color = Uniform::create("u_color", Uniform::Type_3f);
			color = Vec3(0.0,0.0,1.0);
			u_color->set(&color);
			_shader->addUniform(u_color);
			getGraphicState()->setShader(_shader);
		}
		
		void setColor(const Vec3& color)
		{
			this->color = color;
			u_color->set(&this->color);
		}
		
        virtual ~Wire()	{}
        virtual void render() { drawVBO(); }

    protected:
	
		std::vector<float> _vertexBuffer;
		
		Shader::Ptr _shader;
		Vec3 color;
		Uniform::Ptr u_color;
};

class Spheroid : public VBOData, public SceneNode
{
    public:
		
		Meta_Class(Spheroid)
	
        Spheroid(int subdivision = 4, double size = 0.5, double bump = 0.0, bool wireframe = false)
		{
			loaded = false;
			geo = Geometry::createTetrahedron(subdivision);
			geo.sphericalNormalization(1.0);
			geo.scale(Vec3(size,size,size));
			
			_shader = Shader::create(vertexSimple.c_str(), fragSimple.c_str());
			u_color = Uniform::create("u_color", Uniform::Type_3f);
			color = Vec3(0.0,0.0,1.0);
			u_color->set(&color);
			_shader->addUniform(u_color);
			getGraphicState()->setShader(_shader);
			wf = wireframe;
		}
		
        Spheroid(const Polyhedron& buf, bool wireframe = false)
		{
			loaded = false;
			buf.getTriangles(geo._vertices);
			
			_shader = Shader::create(vertexSimple.c_str(), fragSimple.c_str());
			u_color = Uniform::create("u_color", Uniform::Type_3f);
			color = Vec3(0.0,0.0,1.0);
			u_color->set(&color);
			_shader->addUniform(u_color);
			getGraphicState()->setShader(_shader);
			wf = wireframe;
		}
		
		void reduceEachTriangle(float f)
		{
			std::vector<Vec3>& ph1 = geo._vertices;
			for(int i=0;i<(int)ph1.size();i+=3)
			{
				Vec3 mid = ph1[i+0] + ph1[i+1] + ph1[i+2]; mid *= (1.0/3.0);
				
				for(int j=0;j<3;++j)
				{
					Vec3 d = ph1[i+j] - mid;
					d *= f;
					ph1[i+j] = mid + d;
				}
			}
		}
		
		void setColor(const Vec3& color)
		{
			this->color = color;
			u_color->set(&this->color);
		}

		Geometry geo;
		
        virtual ~Spheroid()	{}
        virtual void render()
		{ 
			if(!loaded)
			{
				geo.fillBuffer( _vertexBuffer );
				
				if(wf)
				{
					setPrimitive(Primitive_Lines);
					_vertexBuffer = intoLineBuf( _vertexBuffer );
				}
				else
				{
					setPrimitive(Primitive_Triangles);
					intoCCW(_vertexBuffer);
				}
				
				std::uint32_t vertexBuffSize = _vertexBuffer.size()*sizeof(float);
				requestVBO(vertexBuffSize);
				setVertices(_vertexBuffer.data(), vertexBuffSize);
				// setScale(Vec3(size,size,size));
				loaded = true;
			}
			
			drawVBO();
		}
		
		void setPos(Vec3 p)
		{
			geo.origin(-p);
		}

    protected:
	
		std::vector<float> _vertexBuffer;
		
		bool wf;
		bool loaded;
		Shader::Ptr _shader;
		Vec3 color;
		Uniform::Ptr u_color;
};


struct Graph
{
	GraphicRenderer::Ptr renderer;
	Camera::Ptr camera;
	
	Graph()
	{
		renderer = GraphicRenderer::create();
		
		camera = Camera::create();
		camera->setPosition(Vec3(1.5f, 0.0f, 0.0f));
		camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
		renderer->setSceneRoot(camera);
		renderer->getRenderParameters()->setFaceCullingMode(RenderParameters::FaceCullingMode_None);
		renderer->getRenderParameters()->setPerspectiveProjection(60.0, 1.0, 0.1, 128.0);
		renderer->getRenderParameters()->setViewport(0.0,0.0,500.0,500.0);
		renderer->getRenderParameters()->setLineWidth(3.0);
	}
	
	void add(SceneNode::Ptr node)
	{
		camera->addSubNode(node);
	}
	
	void sphere(const Vec3& position, const Vec3& color, int sub = 4, float size = 0.5, float bump = 0.0, bool wireframe = false)
	{
		Spheroid::Ptr solid = Spheroid::create(sub,size,bump,wireframe);
		solid->setPos(position);
		solid->setColor(color);
		add(solid);
	}
	
	void wire(const Vec3& p1, const Vec3& p2, const Vec3& color)
	{
		Wire::Ptr solid = Wire::create(p1,p2);
		solid->setColor(color);
		add(solid);
	}
	
	void point(const Vec3& position, const Vec3& color)
	{
		Spheroid::Ptr solid = Spheroid::create(2,0.02,0.0,false);
		solid->setPos(position);
		solid->setColor(color);
		add(solid);
	}
};

int main(int argc, char* argv[])
{
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(500, 500), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	
	Graph graph;
	#include "initGraph.cpp"
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }
		
        if(clock.getElapsedTime().asSeconds() > 2.0*3.14) clock.restart();
		double t = clock.getElapsedTime().asMilliseconds() / 1000.0;
		
        graph.camera->setPosition(Vec3(cos(t)*2.0,sin(t)*2.0,0.5));
		graph.camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
		
		// rendering
		graph.renderer->renderScene();
		
		window.display();
    }

	delete VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
