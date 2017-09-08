#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/GraphicState.h>
#include <SceneGraph/RenderTarget.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/VBOData.h>

#include <SFML/Graphics.hpp>

#include <Geometry/ProceduralGeometry.h>
#include <cmath>

#include <Core/Perlin.h>

#define SPHERIC_RATIO 1.0

int displayVertexCount = 0;

std::string GLSL_FRACTAL = IMP_GLSL_SRC
(
    
float hash(vec3 xyz)
{
    return  fract( sin( dot(xyz, vec3(12.19823371,10.27653,35.746541)) )* 5.2653872 );
}

float noise(vec3 xyz, float freq)
{
    vec3 iv = floor(xyz * freq);
    vec3 fv = fract(xyz * freq);
    
    fv = 3.0*fv*fv - 2.*fv*fv*fv;
    
    float v0 = hash( (iv + vec3(0.0,0.0,0.0))/freq );
    float v1 = hash( (iv + vec3(0.0,0.0,1.0))/freq );
    float v2 = hash( (iv + vec3(0.0,1.0,0.0))/freq );
    float v3 = hash( (iv + vec3(0.0,1.0,1.0))/freq );
    float v4 = hash( (iv + vec3(1.0,0.0,0.0))/freq );
    float v5 = hash( (iv + vec3(1.0,0.0,1.0))/freq );
    float v6 = hash( (iv + vec3(1.0,1.0,0.0))/freq );
    float v7 = hash( (iv + vec3(1.0,1.0,1.0))/freq );
    
    float v01 = mix(v0,v1,fv.z);
    float v23 = mix(v2,v3,fv.z);
    float v45 = mix(v4,v5,fv.z);
    float v67 = mix(v6,v7,fv.z);
    
    float v0123 = mix(v01,v23,fv.y);
    float v4567 = mix(v45,v67,fv.y);
    
    return mix(v0123,v4567,fv.x);
}

float fractal(vec3 xyz)
{
    const int octaves = 6;
    float total = 0.0;
    float freq = 4.0;
    float persist = 0.7;
    
    float result = 0.0;
    for(int i=0; i<octaves;++i)
    {
        result += noise(xyz,freq) * persist;
        
        total += persist;
        persist *= 0.5;
        freq *= 2.0;
    }
    
    return result/total;
}
);

class LodSceneNode : public imp::SceneNode
{
public:
	
        LodSceneNode()
		{
		}
		
        virtual ~LodSceneNode()	{}

        virtual void render()
		{
            imp::Matrix4 viewMat = imp::Matrix4::getIdentityMat();
            if(imp::Camera::getActiveCamera())
                    viewMat = imp::Camera::getActiveCamera()->getViewMatrix();
            
                    
            imp::Matrix4 modelMat = getModelMatrix();
            imp::Matrix4 normalMat = getNormalMatrix();

            unsigned int i = 0;
            for(imp::SceneNodeIt it = _lodSubNodes.begin(); it != _lodSubNodes.end(); it++, ++i)
            {
                imp::SceneNode* sub = it->get();
                sub->setParentModelMatrices(modelMat, normalMat);
                sub->setRotation(sub->getRx()+0.001, sub->getRy()+0.002, sub->getRz()+0.001);
                imp::Matrix4 resultMat = viewMat * sub->getModelMatrix();
                
                double tx = resultMat.getValue(3,0);
                double ty = resultMat.getValue(3,1);
                double tz = resultMat.getValue(3,2);
                
                double distance = sqrtf(tx*tx + ty*ty + tz*tz);
                
                if(distance > _distThresholds[i].first && distance < _distThresholds[i].second)
                    sub->renderAll();
            }
		}
		
        void addLODSubNode(const std::shared_ptr<SceneNode>& sceneNode, double dmin, double dmax)
        {
            _lodSubNodes.push_back(sceneNode);
            _distThresholds.push_back(std::pair<double,double>(dmin,dmax));
        }
        
        /*void removeLODSubNode(const std::shared_ptr<SceneNode>& sceneNode)
        {
        }*/

    protected:
	
		imp::SceneNodeList _lodSubNodes;
        std::vector<std::pair<double,double>> _distThresholds;
};


//--------------------------------------------------------------
/// =========== VERTEX SHADER SOURCE =====================
std::string vertexCodeSource = IMP_GLSL_SRC(

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

varying vec4 v_vertexColor;
varying float v_camera;
varying vec3 v_position;

void main(){

    gl_Position = u_projection * u_view * u_model * gl_Vertex;
    v_camera = abs( (u_view * u_model * gl_Vertex).z );
    v_position = (gl_Vertex).xyz;

    v_vertexColor = vec4(1.f, 1.f, 1.f, 1.f);
}

);

//--------------------------------------------------------------
/// =========== GROUND =====================
std::string fragmentCodeSource = GLSL_FRACTAL + IMP_GLSL_SRC(

varying vec4 v_vertexColor;

varying float v_camera;
varying vec3 v_position;

void main(){

    float fogStart = 0.1;
    float fogEnd = 3.0;
    
    float frustum_near = 0.1;
    float frustum_far = 128.0;
    
    float d = clamp(v_camera,frustum_near,frustum_far);
    float f = (d-fogStart)/(fogEnd-fogStart);
    
    vec3 fogColor = vec3(0.1,0.5,0.1) * fractal(v_position);
    
	vec4 texColor = vec4(fogColor, 1.0);
	
    gl_FragData[0] = texColor * v_vertexColor;
}

);


//--------------------------------------------------------------
/// =========== OCEAN =====================
std::string fragmentCodeSource_Ocean = IMP_GLSL_SRC(

varying vec4 v_vertexColor;

varying float v_camera;
varying vec3 v_position;

void main()
{    
    float poleStart = 0.8;
    float pole = min(0.0,abs(v_position.z)-poleStart)/(1.0-poleStart);
    
	vec4 texColor = vec4(0.0,0.0,1.0, 0.3);
	
    texColor.xyz += vec3(pole,pole,0.0);
    
    gl_FragData[0] = texColor * v_vertexColor;
}

);


//--------------------------------------------------------------
/// =========== OCEAN =====================
std::string fragmentCodeSource_Atmosphere = GLSL_FRACTAL + IMP_GLSL_SRC(

varying vec4 v_vertexColor;

varying float v_camera;
varying vec3 v_position;

void main()
{
    float startCloud = 0.5;
    
    float a = max(fractal(v_position)-startCloud, 0.0)/ (1.0-startCloud);
    
	vec4 texColor = vec4(1.0,1.0,1.0, a);
	
    gl_FragData[0] = texColor * v_vertexColor;
}

);

//--------------------------------------------------------------
/// =========== OCEAN =====================
std::string fragmentCodeSource_Stars = IMP_GLSL_SRC(

varying vec4 v_vertexColor;

varying float v_camera;
varying vec3 v_position;

void main()
{
	vec4 texColor = vec4(1.0,1.0,1.0, 1.0);
	
    gl_FragData[0] = texColor * v_vertexColor;
}

);

class MyFunctor : public imp::SignalFunctor
{
public:
    virtual double apply(double x, double y, double z)
    {
        double alti = imp::perlinOctave(x, y, z, 8, 0.7, 1.0,1.0);
        
        return alti;//>0.0?(alti>0.1?0.1:alti):0.0;
    }
};

class Stars : public imp::VBOData, public imp::SceneNode
{
    public:
	
        Stars(int subdivision = 0)
		{
			imp::Geometry cube = imp::Geometry::createTetrahedron(10);
			cube.sphericalNormalization(1.0);
            
			cube.fillBuffer( _vertexBuffer );

			imp::Uint32 vertexBuffSize = _vertexBuffer.size()*sizeof(float);

			requestVBO(vertexBuffSize);
			setVertices(_vertexBuffer.data(), vertexBuffSize);
			
			setScale(imp::Vector3(10.0,10.0,10.0));
			
			 setPrimitive(Primitive_Points);
		}
		
        virtual ~Stars()	{}

        virtual void render()
		{
			setRotation(getRx()+0.001, getRy()+0.002, getRz()+0.001);
			
			drawVBO();
		}

    protected:
	
		std::vector<float> _vertexBuffer;
};

class Spheroid : public imp::VBOData, public imp::SceneNode
{
    public:
	
        Spheroid(int subdivision = 100, double size = 0.5, double bump = 0.0)
		{
			imp::Geometry cube = imp::Geometry::createTetrahedron(subdivision);
			cube.sphericalNormalization(SPHERIC_RATIO);
            
            MyFunctor functor;
            cube.bump(&functor,bump);
            
			cube.fillBuffer( _vertexBuffer );

			imp::Uint32 vertexBuffSize = _vertexBuffer.size()*sizeof(float);

			requestVBO(vertexBuffSize);
			setVertices(_vertexBuffer.data(), vertexBuffSize);
			
			setScale(imp::Vector3(size,size,size));
			
			 setPrimitive(Primitive_Triangles);
		}
		
        virtual ~Spheroid()	{}

        virtual void render()
		{
			drawVBO();
		}

    protected:
	
		std::vector<float> _vertexBuffer;
};

struct ViewerState
{
	std::shared_ptr<imp::GraphicRenderer> renderer;
	std::shared_ptr<imp::RenderTarget> screenTarget;

    std::shared_ptr<imp::Shader> _shader;
    std::shared_ptr<imp::Shader> _shaderOcean;
    std::shared_ptr<imp::Shader> _shaderAtmosphere;
    std::shared_ptr<imp::Shader> _shaderStars;
	std::shared_ptr<Spheroid> planetGround;
	std::shared_ptr<Spheroid> planetOcean;
	std::shared_ptr<Spheroid> planetAtmosphere;
	std::shared_ptr<Spheroid> lowResGround;
	std::shared_ptr<Spheroid> lowResOcean;
	std::shared_ptr<Spheroid> lowResAtmosphere;
	std::shared_ptr<Stars> stars;
    std::shared_ptr<LodSceneNode> _lod;

	std::shared_ptr<imp::Camera> camera;

	std::string filename;
	
	ViewerState()
	{	
		
		// screen render target
		screenTarget .reset( new imp::RenderTarget() );
		screenTarget->createScreenTarget(800,600);
		
		camera.reset( new imp::Camera() );
		camera->initFrustum(800, 600, 60.0*3.14159f/180.f, 0.1, 128.0);
		camera->setPosition(imp::Vector3(0.0f, 0.0f, 1.5f));
		camera->setTarget(imp::Vector3(0.0f, 0.0f, 0.0f));
		
		renderer.reset(new imp::GraphicRenderer() );
		// renderer->setCenterCursor(false);
        
        _shader.reset(new imp::Shader(vertexCodeSource.c_str(), fragmentCodeSource.c_str()));
        _shaderOcean.reset(new imp::Shader(vertexCodeSource.c_str(), fragmentCodeSource_Ocean.c_str()) );
        _shaderAtmosphere.reset(new imp::Shader(vertexCodeSource.c_str(), fragmentCodeSource_Atmosphere.c_str()) );
        _shaderStars.reset(new imp::Shader(vertexCodeSource.c_str(), fragmentCodeSource_Stars.c_str()) );
	
		planetGround.reset( new Spheroid(100.0,0.5, 0.2) );
        planetOcean.reset( new Spheroid(30.0, 0.5) );
        planetAtmosphere.reset(new Spheroid(30.0, 0.55) );
        stars.reset(new Stars() );
        lowResGround.reset( new Spheroid(8, 0.5, 0.2) );
        lowResOcean.reset( new Spheroid(8, 0.5) );
        lowResAtmosphere.reset( new Spheroid(8, 0.55) );
        
		camera->getGraphicState()->setTarget(screenTarget);
		planetGround->getGraphicState()->setShader(_shader);
		planetOcean->getGraphicState()->setShader(_shaderOcean);
        planetAtmosphere->getGraphicState()->setShader(_shaderAtmosphere);
        stars->getGraphicState()->setShader(_shaderStars);
        lowResGround->getGraphicState()->setShader(_shader);
        lowResOcean->getGraphicState()->setShader(_shaderOcean);
        lowResAtmosphere->getGraphicState()->setShader(_shaderAtmosphere);
        _lod.reset(new LodSceneNode());
        
		renderer->setSceneRoot(camera);
        camera->addSubNode(_lod);
		_lod->addLODSubNode(planetGround, 0.0, 3.0);
		_lod->addLODSubNode(planetOcean, 0.0, 3.0);
		_lod->addLODSubNode(planetAtmosphere, 0.0, 3.0);
		_lod->addLODSubNode(lowResGround, 3.0, 100.0);
		_lod->addLODSubNode(lowResOcean, 3.0, 100.0);
		_lod->addLODSubNode(lowResAtmosphere, 3.0, 100.0);
		camera->addSubNode(stars);
	}

};

int main(int argc, char* argv[])
{
    
    sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(800, 600), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	
	ViewerState v_state;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }
		
        
        if(clock.getElapsedTime().asSeconds() > 2.0*3.141592)
            clock.restart();
		
	
        double d = sin(clock.getElapsedTime().asMilliseconds() / 1000.0);
        v_state.camera->setPosition(imp::Vector3(0.0f, 0.0f, 1.0 + (d+1.0) * 3.0));
		
		// rendering
		v_state.renderer->renderScene();
		
		window.display();
    }

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
