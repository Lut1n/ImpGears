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


imp::Pixel toPx(const Vec4& v)
{
	imp::Pixel px; px.x = v[0]; px.y = v[1]; px.z = v[2]; px.w = v[3];
	return px;
}

Vec4 toV4(const imp::Pixel& v)
{
	return Vec4(v.x,v.y,v.z,v.w);
}
  
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
            target.setPixel(j, i, toPx(color));
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
 
// -----------------------------------------------------------------------------------------------------------------------
bool pointInTriangle(Vec3& frag, Triangle& triangle)
{
    Vec3 z(0.0,0.0,1.0);
       
    Vec3 n1 = z.cross(triangle.p2-triangle.p1);
    Vec3 n2 = z.cross(triangle.p3-triangle.p2);
    Vec3 n3 = z.cross(triangle.p1-triangle.p3);
       
    float r1 = n1.dot(frag-triangle.p1);
    float r2 = n2.dot(frag-triangle.p2);
    float r3 = n3.dot(frag-triangle.p3);
       
    return (r1>=0.0 && r2>=0.0 && r3>=0.0);// || (r1<=0.0 && r2<=0.0 && r3<=0.0);
}
// -----------------------------------------------------------------------------------------------------------------------
void sortY2(Triangle mvpVertex, Triangle& v)
{
    Vec3 t;
       
    if(mvpVertex.p1.y() > mvpVertex.p2.y() ) { t= mvpVertex.p1; mvpVertex.p1 = mvpVertex.p2; mvpVertex.p2 = t;     t= v.p1; v.p1 = v.p2; v.p2 = t; }
    if(mvpVertex.p1.y() > mvpVertex.p3.y() ) { t= mvpVertex.p1; mvpVertex.p1 = mvpVertex.p3; mvpVertex.p3 = t;     t= v.p1; v.p1 = v.p3; v.p3 = t; }
    if(mvpVertex.p2.y() > mvpVertex.p3.y() ) { t= mvpVertex.p3; mvpVertex.p3 = mvpVertex.p2; mvpVertex.p2 = t;     t= v.p3; v.p3 = v.p2; v.p2 = t; }
}
   
// -----------------------------------------------------------------------------------------------------------------------
void sortX2(Triangle mvpVertex, Triangle& v)
{
    Vec3 t;
       
    if(mvpVertex.p1.x() > mvpVertex.p2.x() ) { t= mvpVertex.p1; mvpVertex.p1 = mvpVertex.p2; mvpVertex.p2 = t;     t= v.p1; v.p1 = v.p2; v.p2 = t; }
}
  
// -----------------------------------------------------------------------------------------------------------------------
std::vector<float> lerpArr(float v1, float v2, float vt, const std::vector<float>& varying_1, const std::vector<float>& varying_2)
{
    float d1 = std::abs(vt - v1);
    float d2 = std::abs(vt - v2);
    float tt = d1+d2;
    if(tt == 0.0)
    {
        d1 = 1.0;
        d2 = 0.0;
        tt = 1.0;
    }
    float delta = d2/tt;
      
    // assuming varying_1 size == varying_2 size
    std::vector<float> varying_t;
    for(unsigned int i=0; i<varying_1.size(); ++i)
    {
        varying_t.push_back( varying_1[i]*delta + varying_2[i]*(1.0-delta) );
    }
    return varying_t;
}
 
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
void applySort(Triangle& tri, const Vec4& vmap)
{
	Triangle ori = tri;
	tri[0] = ori[ vmap[0] ];
	tri[1] = ori[ vmap[1] ];
	tri[2] = ori[ vmap[2] ];
}
// -----------------------------------------------------------------------------------------------------------------------
struct Interpolator
{
	std::vector<Triangle> _buf;
	
	std::map<int,int> _triMap;
	
	
};
// -----------------------------------------------------------------------------------------------------------------------
 struct FragCallback
 {
	virtual Vec4 operator()(const Vec3& frag, const Vec3& mFrag, const Vec3& color)  = 0;
 };
 
// -----------------------------------------------------------------------------------------------------------------------
Vec4 computeBounds(Triangle& tri)
{
    float boundA_x = tri.p1.x();
    float boundA_y = tri.p1.y();
    float boundB_x = tri.p1.x();
    float boundB_y = tri.p1.y();
     
    if(tri.p2.x() < boundA_x ) { boundA_x = tri.p2.x();}
    if(tri.p3.x() < boundA_x ) { boundA_x = tri.p3.x();}
                                                                                     
    if(tri.p2.y() < boundA_y ) { boundA_y = tri.p2.y();}
    if(tri.p3.y() < boundA_y ) { boundA_y = tri.p3.y();}
                                                                                     
    if(tri.p2.x() > boundB_x ) { boundB_x = tri.p2.x();}
    if(tri.p3.x() > boundB_x ) { boundB_x = tri.p3.x();}
                                                                                     
    if(tri.p2.y() > boundB_y ) { boundB_y = tri.p2.y();}
    if(tri.p3.y() > boundB_y ) { boundB_y = tri.p3.y();}
     
    float w = state.viewport[2]*0.5;
    float h = state.viewport[3]*0.5;
     
    if(boundA_x < -w) boundA_x = -w;
    if(boundA_y < -h) boundA_y= -h;
    if(boundB_x > w) boundB_x = w;
    if(boundB_y > h) boundB_y = h;
     
    return Vec4(boundA_x,boundB_x,boundA_y,boundB_y);
}

#define Varying_Color 0
#define Varying_Vert 1
#define Varying_MVert 2
#define Varying_MVVert 3
#define Varying_MVPVert 4
  
Varying varying1,  varying2,  varying3, varyingX;

float modulation(float b1, float b2, float p)
{
	float res  = (p-b1)/(b2-b1);
	res = res<0.0?0.0:res;
	res = res>1.0?1.0:res;
	return res;
}

// -----------------------------------------------------------------------------------------------------------------------
void renderTriangle(Triangle& vertex, Triangle& mvertex,Triangle& mvVertex, Triangle& mvpVertex, Triangle& tricolor, imp::ImageData& target, imp::ImageData& back, FragCallback& fragCallback)
{
    Vec4 bounds = computeBounds(mvpVertex);
	Vec4 sortmap;
    initSort(mvpVertex.p1.y(),mvpVertex.p2.y(),mvpVertex.p3.y(), sortmap);
	
    applySort(mvertex,sortmap);
    applySort(tricolor,sortmap);
    applySort(mvVertex,sortmap);
    applySort(mvpVertex,sortmap);
    applySort(vertex,sortmap);
	
	varying1.set(Varying_Color, tricolor.p1);				varying2.set(Varying_Color, tricolor.p2);				varying3.set(Varying_Color, tricolor.p3);
	varying1.set(Varying_Vert, vertex.p1); 				varying2.set(Varying_Vert, vertex.p2);					varying3.set(Varying_Vert, vertex.p3);
	varying1.set(Varying_MVert, mvertex.p1);			varying2.set(Varying_MVert, mvertex.p2);			varying3.set(Varying_MVert, mvertex.p3);
	varying1.set(Varying_MVVert, mvVertex.p1);		varying2.set(Varying_MVVert, mvVertex.p2);		varying3.set(Varying_MVVert, mvVertex.p3);
	varying1.set(Varying_MVPVert, mvpVertex.p1);		varying2.set(Varying_MVPVert, mvpVertex.p2);		varying3.set(Varying_MVPVert, mvpVertex.p3);
	
     
    float w = state.viewport[2]*0.5;
    float h = state.viewport[3]*0.5;
       
    // scanline algorithm
    for(int j=bounds[2]; j<bounds[3]; ++j)
    {
        Triangle mixcolor, mixm, mixmv, mixmvp, mixvertex;
        float rel = modulation(mvpVertex.p1.y(), mvpVertex.p3.y(), j);
        varyingX.lerp(varying1, varying3, rel);
        mixcolor.p1 = 		varyingX.get(Varying_Color); 	
		mixvertex.p1 = 		varyingX.get(Varying_Vert); 		
        mixm.p1 = 			varyingX.get(Varying_MVert); 	
        mixmv.p1  =  		varyingX.get(Varying_MVVert); 	
        mixmvp.p1  = 		varyingX.get(Varying_MVPVert); 
         
        if(j < mvpVertex.p2.y())
        {
			float rel = modulation(mvpVertex.p1.y(), mvpVertex.p2.y(), j);
			varyingX.lerp(varying1, varying2, rel);
        }
        else
        {
			float rel = modulation(mvpVertex.p2.y(), mvpVertex.p3.y(), j);
			varyingX.lerp(varying2, varying3, rel);
        }
		mixcolor.p2 =		 	varyingX.get(Varying_Color); 	
		mixvertex.p2 = 			varyingX.get(Varying_Vert); 
		mixm.p2 = 				varyingX.get(Varying_MVert); 		
		mixmv.p2  = 			varyingX.get(Varying_MVVert); 	
		mixmvp.p2  = 			varyingX.get(Varying_MVPVert); 	
        
		initSort(mixmvp.p1.x(),mixmvp.p2.x(), sortmap);
        applySort(mixm, sortmap);
        applySort(mixcolor, sortmap);
        applySort(mixmv, sortmap);
        applySort(mixmvp, sortmap);
        applySort(mixvertex, sortmap);
        
        for(int i=bounds[0]; i<bounds[1]; ++i)
        {
			float rel = modulation(mixmvp.p1.x(), mixmvp.p2.x(), i);
              
            // intersect test
            imp::Vec3 uv(i,j,0.0);
            if( rel > 0.0 && rel < 1.0 )
            {
                Vec3 varying_mv  = lerp(mixmv.p1, mixmv.p2, rel);
                   
                // depth test
                float new_depth = (std::min(1.0, abs(varying_mv.z()-0.1) / 20.0)) * 255.0;
                float curr_depth = back.getPixel(i+w, j+h).x;
                if( new_depth < curr_depth)
                {
                    Vec3 varying_color = lerp(mixcolor.p1, mixcolor.p2, rel);
                    Vec3 varying_vertex = lerp(mixvertex.p1, mixvertex.p2, rel);
                    Vec3 varying_m = lerp(mixm.p1, mixm.p2, rel);
                     
                    Vec4 color = fragCallback(varying_vertex, varying_m, varying_color) * 255.0;
                     
                    target.setPixel(i+w, j+h, toPx(color));
					Vec4 depthPx(new_depth,0.0,0.0,1.0);
                    back.setPixel(i+w, j+h, toPx(depthPx));
                }
            }
        }
    }
}
   
// -----------------------------------------------------------------------------------------------------------------------
void render(std::vector<float>& buf, std::vector<float>& col, imp::ImageData& target, imp::ImageData& back, FragCallback& fragCallback)
{   
    imp::Matrix4 mv = state.model * state.view;
    imp::Matrix4 mvp = mv * state.projection;
    Vec3 win(state.viewport[2]*0.5, state.viewport[3]*0.5, 1.0);
          
    std::cout << "render mesh : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 9)
    {
        Triangle vertex(&buf[i]);
       
        Triangle mvpVertex = vertex * mvp * win;
          
        Triangle mvPosition = vertex * mv;
         
        Triangle mvertex = vertex * state.model;
         
        Triangle triColor(&col[i]);
         
        // VaryingBuffer varying1; varying1.push_back();
          
        renderTriangle(vertex, mvertex, mvPosition, mvpVertex, triColor,target, back, fragCallback);
          
        std::cout << "\rrender mesh : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
}
        
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateTorch(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius,radius,radius));
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "torch geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}     
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateRock(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(0.7);
    geometry.scale(Vec3(radius * 0.7,radius * 0.7,radius * 1.5));
    geometry.noiseBump(6, 0.7, 1.0, 0.2);
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "rock geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}   
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateHat(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius,radius,radius*0.3));
    geometry.noiseBump(6, 0.7, 1.0, 0.2);
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "hat geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generatePlane(const Vec3& center, float radius, int sub)
{
    std::vector<float> vertexBuffer;
       
    imp::Geometry geometry = imp::Geometry::createTetrahedron(sub);
    geometry.sphericalNormalization(1.0);
    geometry.scale(Vec3(radius,radius,0.1));
    geometry.noiseBump(6, 0.7, 1.0, 0.3);
    geometry.fillBuffer(vertexBuffer);
     
    std::cout << "plane geometry : " << vertexBuffer.size()/3 << " vertices" << std::endl;
       
    return vertexBuffer;
}
   
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.reserve(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 v(buf[i+0],buf[i+1],buf[i+2]);
        float disturb = imp::perlinOctave(v.x(), v.y(), v.z(), 8, 0.7, 1.0/32.0) * 2.0 - 1.0;
         
        float a = std::atan2(buf[i+0],buf[i+1]);
         
        float signal = (1.0+std::sin((a + disturb*4.0) * 16.0))*0.5;
         
        Vec3 colorA(0.0,0.7,0.3);
        Vec3 colorB(0.3,0.0,1.0);
         
        Vec3 finalColor = (colorB - colorA)*signal + colorA;
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors2(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.reserve(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 v(buf[i+0],buf[i+1],buf[i+2]);
        // float disturb = (perlin(v * (1.0/4.0)))*2.0 - 1.0;
        float disturb = (imp::perlinOctave(v.x(), v.y(), v.z(), 4, 0.7, 1.0/32.0) + 0.5);
         
        Vec3 colorA(0.0,0.3,0.0);
        Vec3 colorB(0.2,1.0,0.0);
         
        Vec3 finalColor = (colorB - colorA)*disturb + colorA;
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors3(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.reserve(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 finalColor(1.0,1.0,1.0);
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
// -----------------------------------------------------------------------------------------------------------------------  
std::vector<float> generateColors4(std::vector<float>& buf)
{
    std::vector<float> colorBuffer;
    colorBuffer.reserve(buf.size());
      
    std::cout << "generate color buffer : 0%";
      
    for(unsigned int i=0; i<buf.size(); i+= 3)
    {
        Vec3 v(buf[i+0],buf[i+1],buf[i+2]);
        float disturb = imp::perlinOctave(v.x(), v.y(), v.z(), 8, 0.7, 1.0/32.0) * 2.0 - 1.0;
         
        float a = std::atan2(buf[i+0],buf[i+1]);
         
        float signal = (1.0+std::sin((a + disturb*4.0) * 16.0))*0.5;
         
        Vec3 colorA(0.3,0.7,0.7);
        Vec3 colorB(0.7,0.2,0.3);
         
        Vec3 finalColor = (colorB - colorA)*signal + colorA;
         
        colorBuffer[i+0] = finalColor.x();
        colorBuffer[i+1] = finalColor.y();
        colorBuffer[i+2] = finalColor.z();
          
        std::cout << "\rgenerate color buffer : " << std::floor(i*100/buf.size()) << "% ";
    }
    std::cout << "done" << std::endl;
      
    return colorBuffer;
}
 
// -----------------------------------------------------------------------------------------------------------------------
struct DefaultRenderFrag : public FragCallback
{
	virtual Vec4 operator()(const Vec3& frag, const Vec3& mFrag, const Vec3& color)
	{
		Vec3 zero(0.0,0.0,0.0);
		Vec3 normal = frag; normal.normalize();
		Vec3 light_dir = light_1.position - mFrag;
		light_dir.normalize();
		 
		const float* md = state.view.getData();
		Vec3 cam_dir(md[12],md[13],md[14]); cam_dir = (cam_dir) - mFrag; cam_dir.normalize();
		 
		float a = light_dir.dot(normal);
		a = a<0.0?0.0:a;
		a = a>1.0?1.0:a;
		 
		Vec3 reflection = (normal * 2.0 * a) - light_dir;
		 
		float s = reflection.dot(cam_dir);
		s = s<0.0?0.0:s;
		s = s>1.0?1.0:s;
		 
		Vec3 spec = Vec3(1.0,1.0,1.0)*s;
		 
		Vec3 light_color = Vec3(0.1,0.1,0.1) + Vec3(0.7,0.7,0.7)*a + pow(spec, 8);
		 
		Vec3 base_color = Vec3(1.0,1.0,1.0);
		base_color = base_color * light_color * color;
		return base_color;
	}
};
// -----------------------------------------------------------------------------------------------------------------------
struct TerrRenderFrag : public FragCallback
{
	virtual Vec4 operator()(const Vec3& frag, const Vec3& mFrag, const Vec3& color)
	{
		Vec3 zero(0.0,0.0,0.0);
		Vec3 normal(0.0,0.0,1.0);
		Vec3 light_dir = light_1.position - mFrag;
		light_dir.normalize();
		 
		const float* md = state.view.getData();
		Vec3 cam_dir(md[12],md[13],md[14]); cam_dir = (cam_dir) - mFrag; cam_dir.normalize();
		 
		float a = light_dir.dot(normal);
		a = a<0.0?0.0:a;
		a = a>1.0?1.0:a;
		 
		Vec3 reflection = (normal * 2.0 * a) - light_dir;
		 
		float s = reflection.dot(cam_dir);
		s = s<0.0?0.0:s;
		s = s>1.0?1.0:s;
		 
		Vec3 spec = Vec3(1.0,1.0,1.0)*s;
		 
		Vec3 light_color = Vec3(0.1,0.1,0.1) + Vec3(0.7,0.7,0.7)*a + pow(spec, 8);
		 
		Vec3 base_color = Vec3(1.0,1.0,1.0);
		base_color = base_color * light_color * color;
		return base_color;
	}
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct LightRenderFrag : public FragCallback
{
	virtual Vec4 operator()(const Vec3& frag, const Vec3& mFrag, const Vec3& color)
	{
		return Vec4(1.0,1.0,1.0,1.0);
	}
};
 
 DefaultRenderFrag defaultFrag;
 LightRenderFrag lightFrag;
 TerrRenderFrag terrFrag;
 
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
               
            clear.color = Vec4(1.0,1.0,1.0,1.0);
            performFrag(backbuffer, clear);
               
            state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, -2.0);
            render(planeGeo, planeCol, target, backbuffer,terrFrag);
            state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, 1.0);
            render(rockGeo, rockCol, target, backbuffer,defaultFrag);
            state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, 3.0);
            render(hatGeo, hatCol, target, backbuffer,defaultFrag);
            state.model = imp::Matrix4::getTranslationMat(2.0, 2.0, -1.0);
            render(torchGeo, torchCol, target, backbuffer,lightFrag);
               
            BmpLoader::saveToFile(&target, "test.bmp");
            start = time(nullptr);
        }
   
    }
   
    return 0;
}