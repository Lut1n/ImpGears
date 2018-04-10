#include <Geometry/ProceduralGeometry.h>
#include <SceneGraph/BmpLoader.h>
#include <Core/Vec4.h>
#include <Core/Perlin.h>
  
#include <ctime>
#include <vector>
#include <map>
  
  
#define CONFIG_NEAR 0.1
#define CONFIG_FAR 100.0
#define CONFIG_FOV 60.0
#define CONFIG_WIDTH 2000.0
#define CONFIG_HEIGHT 2000.0
  
 static const float s_ambiant[3] = {0.1,0.1,0.1};
 static const float s_diffuse[3] = {0.7,0.7,0.7};
 static const float s_specular[3] = {0.5,0.5,0.5};
  
  
  
  
using Vec4 = imp::Vec4;
using Vec3 = imp::Vec3;
  
struct RenderState
{
    imp::Matrix4 projection;
    imp::Matrix4 view;
    imp::Matrix4 model;
    Vec4 viewport;
};
  
RenderState state;
  
struct Mesh
{
    // std::vector<float> vertexBuffer;
    // Vec3 color;
};
   
// -----------------------------------------------------------------------------------------------------------------------
struct Callback 
{
    virtual Vec4 operator()(float coordX, float coordY) = 0;
};
   
// -----------------------------------------------------------------------------------------------------------------------
void performFrag(imp::ImageData& target, Callback& callback)
{
    float w = target.getWidth();
    float h = target.getHeight();
      
    int total = w*h;
    int n=0;
      
    std::cout << "perform clear operation : 0%";
      
    for(unsigned int i=0; i<w; ++i)
    {
        for(unsigned int j=0; j<h; ++j)
        {
            Vec4 color = callback( i/w, j/h ) * 255.0;
            target.setPixel(j, i, color);
        }
              
        n+=h;
          
        float perc = (float)n/(float)total;
          
        std::cout << "\rperform clear operation : " << std::floor(perc*100) << "% ";
    }
    std::cout << "done" << std::endl;
}
   
// -----------------------------------------------------------------------------------------------------------------------
struct ClearColor : public Callback
{
    Vec4 color;
    virtual Vec4 operator()(float coordX, float coordY) { return color; }
};
   
// -----------------------------------------------------------------------------------------------------------------------
struct Triangle
{
    Vec3 p1, p2, p3;
      
    Triangle(){}
    Triangle(Vec3& p1, Vec3& p2, Vec3& p3) : p1(p1), p2(p2), p3(p3) {}
    Triangle(Vec3* buf) { p1 = buf[0]; p2 = buf[1]; p3 = buf[2]; }
    Triangle(float* buf) { p1 = Vec3(buf[0],buf[1],buf[2]); p2 = Vec3(buf[3],buf[4],buf[5]); p3 = Vec3(buf[6],buf[7],buf[8]); }
      
    Triangle operator*(imp::Matrix4& mat4) { Triangle res; res.p1 = (Vec3)(Vec4(p1) * mat4); res.p2 = (Vec3)(Vec4(p2) * mat4);  res.p3 = (Vec3)(Vec4(p3) * mat4); return res; }
    Triangle operator*(Vec3& v3) { Triangle res; res.p1 = p1*v3; res.p2 = p2*v3;  res.p3 = p3*v3; return res; }
	
	Vec3& operator[](unsigned int i) { if(i==0)return p1; if(i==1)return p2; if(i==2)return p3; return p1; }
	Vec3 operator[](unsigned int i) const { if(i==0)return p1; if(i==1)return p2; if(i==2)return p3; return p1; }
};
 
struct Light
{
    Vec3 position;
    Vec3 color;
    float attn;
};

Light light_1;
 
// -----------------------------------------------------------------------------------------------------------------------
Vec3 pow(Vec3& v, int p)
{
    Vec3 res= v;
    for(int i=0; i<p; ++i)
    {
        res = res * v;
    }
    return res;
}
	
   
// -----------------------------------------------------------------------------------------------------------------------
Vec3 lerp(const Vec3& p1, const Vec3& p2, float delta)
{
	return p1*(1.0-delta) + p2*delta;
}

// -----------------------------------------------------------------------------------------------------------------------
struct Varying
{
    std::map<int, int> _adr;
    std::vector<Vec3> _buf;
    
    void push(int id, const Vec3& v3)
    {
        _adr[id] = _buf.size();
        _buf.resize(_buf.size()+1);
        set(id,v3);
    }
    void set(int id, const Vec3& v3)
    {
		if(_adr.find(id) == _adr.end()) push(id, v3);
		else
		{
			int adr = _adr[id];
			_buf[adr] = v3;
		}
    }
    Vec3& get(int id)
    {
        int adr = _adr[id];
        return _buf[adr];
    }
    void copyAdr(const Varying& other)
    {
        std::map<int, int>::const_iterator it;
        for(it = other._adr.begin(); it != other._adr.end(); it++)
        {
            _adr[ it->first ] = it->second;
        }
        _buf.resize(other._buf.size());
    }
    
    void lerp(const Varying& varying_1, const Varying& varying_2, float delta)
    {
        const std::vector<Vec3>& _buf1 = varying_1._buf;
        const std::vector<Vec3>& _buf2 = varying_2._buf;
        
        if(_buf.size() != _buf1.size()) copyAdr(varying_1);
        
        for(unsigned int i=0; i<_buf1.size(); ++i)
        {
            _buf[i] = _buf1[i]*(1.0-delta) + _buf2[i]*delta;
        }
    }
};
// -----------------------------------------------------------------------------------------------------------------------
void initSort(float a, float b, Vec4& vmap)
{
    float t;
	vmap[0] = 0.0; vmap[1] = 1.0;
       
    if(a > b ) { t=a; a=b; b=t;     t=vmap[0]; vmap[0]=vmap[1]; vmap[1]=t; }
}
// -----------------------------------------------------------------------------------------------------------------------
void initSort(float a, float b, float c, Vec4& vmap)
{
    float t;
	vmap[0] = 0.0; vmap[1] = 1.0; vmap[2] = 2.0;
       
    if(a > b ) { t=a; a=b; b=t;     t=vmap[0]; vmap[0]=vmap[1]; vmap[1]=t; }
    if(a > c ) { t=a; a=c; c=t;     t=vmap[0]; vmap[0]=vmap[2]; vmap[2]=t; }
    if(b > c ) { t=c; c=b; b=t;     t=vmap[2]; vmap[2]=vmap[1]; vmap[1]=t; }
}
// -----------------------------------------------------------------------------------------------------------------------
static void applySort(Triangle& tri, const Vec4& vmap)
{
	Triangle ori = tri;
	tri[0] = ori[ vmap[0] ];
	tri[1] = ori[ vmap[1] ];
	tri[2] = ori[ vmap[2] ];
}

#define Varying_Color 0
#define Varying_Vert 1
#define Varying_MVert 2
#define Varying_MVVert 3
#define Varying_MVPVert 4

#include "engine.h"
#include "model.h"

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    Vec3 cam_position(0.0, 20.0, 0.0);
    Vec3 cam_target(0.0, 0.0, 0.0);
    Vec3 cam_up(0.0, 0.0, 1.0);
    Vec4 clear_color(0.7, 0.7, 1.0,1.0);
    Vec3 rock_center(0.0,0.0,0.0);
      
    state.viewport = Vec4(0.0,0.0,CONFIG_WIDTH,CONFIG_HEIGHT);
    state.projection = imp::Matrix4::getPerspectiveProjectionMat(CONFIG_FOV, CONFIG_WIDTH/CONFIG_HEIGHT, CONFIG_NEAR, CONFIG_FAR);
    state.view = imp::Matrix4::getViewMat(cam_position, cam_target, cam_up);
     
    light_1.position = Vec3(2.0,2.0,-1.0);
    light_1.color = Vec3(1.0,1.0,1.0);
    light_1.attn = 0.7;
    
    std::vector<float> quad;
    quad.push_back(-1.0); quad.push_back(-1.0); quad.push_back(0.0);
    quad.push_back(1.0); quad.push_back(-1.0); quad.push_back(0.0);
    quad.push_back(1.0); quad.push_back(1.0); quad.push_back(0.0);
    quad.push_back(1.0); quad.push_back(1.0); quad.push_back(0.0);
    quad.push_back(-1.0); quad.push_back(1.0); quad.push_back(0.0);
    quad.push_back(-1.0); quad.push_back(-1.0); quad.push_back(0.0);
    std::vector<float> qcolor; qcolor.resize(18, 0.5);
       
    std::vector<float> planeGeo = generatePlane(rock_center, 10.0, 30);
    std::vector<float> planeCol = generateColors2(planeGeo);
     
    std::vector<float> rockGeo = generateRock(rock_center, 3.0, 100);
    std::vector<float> rockCol = generateColors(rockGeo);
     
    std::vector<float> hatGeo = generateHat(rock_center, 4.0, 100);
    std::vector<float> hatCol = generateColors4(hatGeo);
    
    std::vector<float> torchGeo = generateTorch(rock_center, 0.2, 10);
    std::vector<float> torchCol = generateColors3(torchGeo);
       
    ClearColor clear;
       
    imp::ImageData target, backbuffer;
    target.build(CONFIG_WIDTH, CONFIG_HEIGHT, imp::PixelFormat_BGR8, nullptr);
    backbuffer.build(CONFIG_WIDTH, CONFIG_HEIGHT, imp::PixelFormat_BGR8, nullptr);
       
    double a = 0.0;
    std::time_t start = time(nullptr);
      
    while(true)
    {
        std::time_t curr = time(nullptr);
        double diff = curr - start;
        if(diff > 0.0)
        {
            a += 0.1;
            imp::Matrix4 rot = imp::Matrix4::getRotationMat(0.0, 0.0, a);
            cam_position = (Vec3)(Vec4(0.0, 10.0, 0.0,1.0) * rot);
            state.view = imp::Matrix4::getViewMat(cam_position, cam_target, cam_up);
           
            clear.color = clear_color;
            performFrag(target, clear);
            //renderVertex(quad,qcolor,target,backbuffer, clearVert, clearFrag);
               
            clear.color = Vec4(1.0,1.0,1.0,1.0);
            performFrag(backbuffer, clear);
               
            state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, -2.0);
            renderVertex(planeGeo, planeCol, target, backbuffer,defaultVert, terrFrag);
            state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, 1.0);
            renderVertex(rockGeo, rockCol, target, backbuffer,defaultVert, defaultFrag);
            state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, 3.0);
            renderVertex(hatGeo, hatCol, target, backbuffer,defaultVert, defaultFrag);
            state.model = imp::Matrix4::getTranslationMat(2.0, 2.0, -1.0);
            renderVertex(torchGeo, torchCol, target, backbuffer,defaultVert, lightFrag);
               
            BmpLoader::saveToFile(&target, "test.bmp");
            start = time(nullptr);
        }
   
    }
   
    return 0;
}
