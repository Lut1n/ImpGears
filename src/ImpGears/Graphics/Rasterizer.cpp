#include <Core/Math.h>
#include <Graphics/Rasterizer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
struct DefaultPlainColor : public FragCallback
{
    Meta_Class(DefaultPlainColor)
    
    Rasterizer* _rast;
    
    DefaultPlainColor(Rasterizer* rast)
        : _rast(rast)
    {}
        
    virtual void exec(ImageBuf& targets, const Vec3& pt, Uniforms* uniforms = nullptr)
    {
        for(auto img:targets)img->setPixel(pt[0],pt[1],_rast->_defaultColor);
    }
};
   
//-------------------------------------------------------------- 
Rasterizer::Rasterizer()
    : _defaultColor(255.0,255.0,255.0,255.0)
{
    useDefaultFragCallback();
}

//--------------------------------------------------------------
Rasterizer::~Rasterizer(){}

//--------------------------------------------------------------
void Rasterizer::addTarget(imp::Image::Ptr target)
{
    _targets.push_back(target);
}

//--------------------------------------------------------------
void Rasterizer::clearTarget()
{
    _targets.clear();
}

//--------------------------------------------------------------
void Rasterizer::setTarget(imp::Image::Ptr target)
{
    clearTarget();
    addTarget(target);
}

//--------------------------------------------------------------
void Rasterizer::setColor(const imp::Vec4& col)
{
    _defaultColor = col;
}

//--------------------------------------------------------------
void Rasterizer::clearUniforms()
{
    _uniforms.clear();
}

//--------------------------------------------------------------
void Rasterizer::setUniforms2(const Uniforms& u1, const Uniforms& u2)
{
    clearUniforms();
    _uniforms.push_back(u1);
    _uniforms.push_back(u2);
}

//--------------------------------------------------------------
void Rasterizer::setUniforms3(const Uniforms& u1, const Uniforms& u2, const Uniforms& u3)
{
    clearUniforms();
    _uniforms.push_back(u1);
    _uniforms.push_back(u2);
    _uniforms.push_back(u3);
}

//--------------------------------------------------------------
void Rasterizer::setFragCallback(FragCallback::Ptr cb)
{
    _fragCallback = cb;
}

//--------------------------------------------------------------
void Rasterizer::useDefaultFragCallback()
{
    _fragCallback = DefaultPlainColor::create(this);
}

//--------------------------------------------------------------
void Rasterizer::rectangle(const Vec3& p1, const Vec3& p2)
{
    for(int i=p1[0];i<=p2[0];++i)
    {
        for(int j=p1[1];j<=p2[1];++j)
        {
            _fragCallback->exec(_targets,Vec3(i,j,0));
        }
    }
}

//--------------------------------------------------------------
void Rasterizer::square(const Vec3& p1, int sideSize)
{
    int d =sideSize/2;
    int l=p1[0]-d;
    int b=p1[1]-d;
    int r = l+sideSize-1;
    int t = b+sideSize-1;
    rectangle(Vec3(l,b,0), Vec3(r,t,0));
}

//--------------------------------------------------------------
void Rasterizer::grid(const Vec3& p1, const Vec3& p2, int gridOffset)
{
    for(int i=p1[0];i<=p2[0];++i)
    {
        for(int j=p1[1];j<=p2[1];++j)
        {
            if((i+j+gridOffset)%2 == 0)_fragCallback->exec(_targets,Vec3(i,j,0));
        }
    }
}

//--------------------------------------------------------------
void Rasterizer::dot(const Vec3& p1)
{
    _fragCallback->exec(_targets,p1);
}

//--------------------------------------------------------------
void Rasterizer::hSymmetry()
{
}

//--------------------------------------------------------------
void Rasterizer::line(const Vec3& p1, const Vec3& p2)
{
    Uniforms uniforms;
    float len = (p2 - p1).length();
    
    for(int t=0;t<std::floor(len)+1;++t)
    {
        float rel = imp::clamp((float)t/(float)len);
        Vec3 position = imp::mix(p1,p2,rel);
        
        if(_uniforms.size() >= 2)
        {
            uniforms.mix(_uniforms[0], _uniforms[1], rel);
            _fragCallback->exec(_targets,position,&uniforms);
        }
        else
        {
            _fragCallback->exec(_targets,position);
        }
    }
}

//--------------------------------------------------------------
void Rasterizer::hLine(const Vec3& p1, const Vec3& p2)
{
    int l=0,r=1;
    Vec3 pts[2] = {p1,p2};
    if(pts[l].x() > pts[r].x()){l=1;r=0;}
    
    Uniforms uniforms;
    for(int x=std::floor(pts[l].x());x<std::floor(pts[r].x())+1;++x)
    {
        float rel = imp::clamp(imp::linearstep(pts[l].x(),pts[r].x(), (float)x));
        uniforms.mix(_uniforms[l], _uniforms[r], rel);
        _fragCallback->exec(_targets,imp::Vec3(x,pts[l].y(),pts[l].z()),&uniforms);
    }
}

//--------------------------------------------------------------
void Rasterizer::triangle(const Vec3& p1, const Vec3& p2, const Vec3& p3)
{
    UniformBuf local = _uniforms;
    UniformBuf uniforms; uniforms.resize(2);
    imp::Vec3 line[2];
        
    Vec3 vertices[3] = {p1,p2,p3};
    
    int bottom=0,center=1,top=2;
    if(vertices[bottom].y()>vertices[center].y()) imp::swap(bottom,center);
    if(vertices[bottom].y()>vertices[top].y()) imp::swap(bottom,top);
    if(vertices[center].y()>vertices[top].y()) imp::swap(center,top);
    
    for(int y=std::floor(vertices[bottom].y());y<std::floor(vertices[top].y())+1;++y)
    {
        int a=center,b=top;
        if(y < vertices[center].y()) {a=bottom; b=center;}
        
        float rel0 = imp::clamp(imp::linearstep(vertices[a].y(), vertices[b].y(), (float)y));
        float rel1 = imp::clamp(imp::linearstep(vertices[bottom].y(), vertices[top].y(), (float)y));
        
        uniforms[0].mix(local[a],local[b],rel0);
        uniforms[1].mix(local[bottom],local[top],rel1);
        
        line[0] = imp::mix(vertices[a],vertices[b],rel0);
        line[1] = imp::mix(vertices[bottom],vertices[top],rel1);

        setUniforms2(uniforms[0],uniforms[1]);
        hLine(line[0],line[1]);
    }
}

//--------------------------------------------------------------
void Rasterizer::wireTriangle(const Vec3& p1, const Vec3& p2, const Vec3& p3)
{
    UniformBuf local = _uniforms;
    
    if(local.size() >= 3)
    {
        setUniforms2(local[0],local[1]);
        line(p1,p2);
        setUniforms2(local[0],local[2]);
        line(p1,p3);
        setUniforms2(local[1],local[2]);
        line(p2,p3);
    }
    else
    {
        line(p1,p2);
        line(p1,p3);
        line(p2,p3);
    }
}

IMPGEARS_END
