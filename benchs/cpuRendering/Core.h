#include <Geometry/ProceduralGeometry.h>
#include <SceneGraph/BmpLoader.h>
#include <Core/Vec4.h>
#include <Core/Perlin.h>
  
#include <ctime>
#include <vector>
#include <map>

#define foreach(N,f) for(int i=0;i<N;++i){f;}

// -----------------------------------------------------------------------------------------------------------------------
float clamp(float x, float a = 0.0, float b = 1.0)
{
    return x<a?a:(x>b?b:x);
}

// -----------------------------------------------------------------------------------------------------------------------
void clamp(Vec3& v, float a = 0.0, float b = 1.0)
{
    for(int i=0;i<3;++i)v[i]=clamp(v[i],a,b);
}

// -----------------------------------------------------------------------------------------------------------------------
float step(float edge1, float edge2, float x)
{
    float res = (x-edge1)/(edge2-edge1);
    return res;
}

// -----------------------------------------------------------------------------------------------------------------------
float smoothstep(float edge1, float edge2, float x)
{
    float res = (x-edge1)/(edge2-edge1);
    res = res * res * (3.0 - 2.0 * res);
    return res;
}

// -----------------------------------------------------------------------------------------------------------------------
float mix(float a, float b, float x)
{
    return (b - a)*x + a;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec3 mix(const Vec3& a, const Vec3& b, float x)
{
    return (b - a)*x + a;
}

// -----------------------------------------------------------------------------------------------------------------------
void swap(float& a, float& b)
{
    float t=a; a=b; b=t;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec3 pow(const Vec3& v, int p)
{
    Vec3 res = v;
    for(int i=0; i<p; ++i) res = res * v;
    return res;
}

// -----------------------------------------------------------------------------------------------------------------------
float min3(float a, float b, float c)
{
    return std::min(a,std::min(b,c));
}

// -----------------------------------------------------------------------------------------------------------------------
float max3(float a, float b, float c)
{
    return std::max(a,std::max(b,c));
}

// -----------------------------------------------------------------------------------------------------------------------
struct Triangle
{
    Vec3 p1, p2, p3;
	
	Vec3& operator[](unsigned int i) { if(i==0)return p1; if(i==1)return p2; if(i==2)return p3; return p1; }
	const Vec3& operator[](unsigned int i) const { if(i==0)return p1; if(i==1)return p2; if(i==2)return p3; return p1; }
      
    Triangle(){}
    Triangle(Vec3& p1, Vec3& p2, Vec3& p3) : p1(p1), p2(p2), p3(p3) {}
    Triangle(Vec3* buf) { for(int i=0;i<3;++i) (*this)[i] = buf[i]; }
    Triangle(float* buf) { for(int i=0;i<3;++i) (*this)[i] = Vec3(buf[i*3+0],buf[i*3+1],buf[i*3+2]); }
      
    Triangle operator*(imp::Matrix4& mat4) { Triangle res, &me=*this; for(int i=0;i<3;++i)res[i] = Vec4(me[i])*mat4; return res; }
    Triangle operator*(Vec3& v3) { Triangle res, &me=*this; for(int i=0;i<3;++i)res[i] = me[i]*v3; return res; }
    Triangle operator+(Vec3& v3) { Triangle res, &me=*this; for(int i=0;i<3;++i)res[i] = me[i]+v3; return res; }
    
    Vec4 getBounds()
    {
        Vec4 bounds;
        bounds[0] = min3(p1.x(),p2.x(),p3.x());
        bounds[1] = max3(p1.x(),p2.x(),p3.x());
        bounds[2] = min3(p1.y(),p2.y(),p3.y());
        bounds[3] = max3(p1.y(),p2.y(),p3.y());
        return bounds;
    }
};

// -----------------------------------------------------------------------------------------------------------------------
struct SortMap
{
    Vec4 _arr;
    void init(float a, float b, float c)
    {
        for(int i=0;i<4;++i)_arr[i]=i;
        if(a>b){swap(a,b); swap(_arr[0],_arr[1]); }
        if(a>c){swap(a,c); swap(_arr[0],_arr[2]); }
        if(b>c){swap(b,c); swap(_arr[1],_arr[2]); }
    }
    
    void init(float a, float b)
    {
        for(int i=0;i<4;++i)_arr[i]=i;
        if(a>b){swap(a,b); swap(_arr[0],_arr[1]); }
    }
    
    void apply(Triangle& tri)
    {
        Triangle ori = tri;
        for(int i=0;i<3;++i)tri[i]=ori[_arr[i]];
    }
};