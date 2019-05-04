#include <Core/Math.h>
#include <Graphics/Rasterizer.h>

#include <utility>	// std::swap

IMPGEARS_BEGIN

//--------------------------------------------------------------
struct DefaultPlainColor : public FragCallback
{
	Meta_Class(DefaultPlainColor)

	Rasterizer* _rast;

	DefaultPlainColor(Rasterizer* rast)
		: _rast(rast)
	{}
		
	virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr)
	{
		for(auto img:targets)img.second->setPixel(pt[0],pt[1],_rast->_defaultColor);
	}
};


//-------------------------------------------------------------- 
bool CnstUniforms::contains(const std::string& name) const
{
	return _values.find(name)!=_values.end();
}

//-------------------------------------------------------------- 
void CnstUniforms::clear()
{
	_values.clear();
}

//-------------------------------------------------------------- 
void CnstUniforms::reserve(const CnstUniforms& u)
{
	for(auto kv : u._values)
		if( !contains(kv.first) )
			_values[kv.first] = Uniform::create(kv.first, kv.second->getType());
}

//-------------------------------------------------------------- 
void CnstUniforms::copy(const CnstUniforms& u)
{
	mix(u,u,0.0);
}

//-------------------------------------------------------------- 
void CnstUniforms::mix(const CnstUniforms& u1,const CnstUniforms& u2, float delta)
{
	reserve(u1);
	for(auto kv : _values)
		_values[kv.first]->mix(u1.get(kv.first),u2.get(kv.first),delta);
}

//-------------------------------------------------------------- 
const Uniform::Ptr CnstUniforms::get(const std::string& name) const
{
	if(contains(name)) return _values.at(name);
	else return nullptr;
}

//-------------------------------------------------------------- 
const Matrix3& CnstUniforms::getMat3(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getMat3();
	else return _defaultMat3;
}

//-------------------------------------------------------------- 
const Matrix4& CnstUniforms::getMat4(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getMat4();
	else return _defaultMat4;
}

//-------------------------------------------------------------- 
const Vec3& CnstUniforms::getVec3(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getFloat3();
	else return _defaultVec3;
}

//-------------------------------------------------------------- 
const Vec4& CnstUniforms::getVec4(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getFloat4();
	else return _defaultVec4;
}

//-------------------------------------------------------------- 
const TextureSampler::Ptr CnstUniforms::getSampler(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getSampler();
	else return _defaultSampler;
}

//-------------------------------------------------------------- 
void CnstUniforms::set(const std::string& name, const Matrix3& mat3)
{
	if(contains(name)) _values[name]->set(mat3);
}

//-------------------------------------------------------------- 
void CnstUniforms::set(const std::string& name, const Matrix4& mat4)
{
	if(contains(name)) _values[name]->set(mat4);
}

//-------------------------------------------------------------- 
void CnstUniforms::set(const std::string& name, const Vec3& v3)
{
	if(contains(name)) _values[name]->set(v3);
}

//-------------------------------------------------------------- 
void CnstUniforms::set(const std::string& name, const Vec4& v4)
{
	if(contains(name)) _values[name]->set(v4);
}

//-------------------------------------------------------------- 
void CnstUniforms::set(const std::string& name, const TextureSampler::Ptr& sampler)
{
	if(contains(name)) _values[name]->set(sampler);
}

//-------------------------------------------------------------- 
void CnstUniforms::set(const Uniform::Ptr& uniform)
{
	_values[uniform->getID()] = uniform;
}

//-------------------------------------------------------------- 
Rasterizer::Rasterizer()
	: _defaultColor(255.0,255.0,255.0,255.0)
{
	useDefaultFragCallback();
}

//--------------------------------------------------------------
Rasterizer::~Rasterizer(){}

//--------------------------------------------------------------
void Rasterizer::setTarget(int index, imp::Image::Ptr target)
{
	_targets[index] = target;
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
	setTarget(0,target);
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
void Rasterizer::setCnstUniforms(const CnstUniforms& cu)
{
	_cnstUniforms = cu;
}

//--------------------------------------------------------------
void Rasterizer::clearCnstUniforms()
{
	_cnstUniforms.clear();
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
	for(int i=p1[0];i<=p2[0];++i) for(int j=p1[1];j<=p2[1];++j)
	{
		_fragCallback->exec(_targets,Vec3(i,j,0),_cnstUniforms);
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
	for(int i=p1[0];i<=p2[0];++i) for(int j=p1[1];j<=p2[1];++j)
	{
		if((i+j+gridOffset)%2 == 0)_fragCallback->exec(_targets,Vec3(i,j,0),_cnstUniforms);
	}
}

//--------------------------------------------------------------
void Rasterizer::dot(const Vec3& p1)
{
	_fragCallback->exec(_targets,p1,_cnstUniforms);
}

//--------------------------------------------------------------
void Rasterizer::hSymmetry()
{
}

//--------------------------------------------------------------
void Rasterizer::line(const Vec3& p1, const Vec3& p2)
{
	// Digital differential analyzer (DDA)
	float lineWidth = 1.0;
	lineWidth -= 1.0; // 0 is 1 pixel
	
	Uniforms uniforms;
	Vec3 dxy = p2 - p1;
	
	Vec3 wl(1,0,0);
	int step = std::abs(dxy[1]);
	if(std::abs(dxy[0]) >= std::abs(dxy[1]))
	{
		step = std::abs(dxy[0]);
		wl = Vec3(0,1,0);
	}
	dxy /= step;

	Uniforms* uptr = nullptr;
	if(_uniforms.size() >= 2) uptr = &uniforms;
	
	Vec3 position = p1;
	for(int i=0;i<=step;++i)
	{
		float rel = imp::clamp((float)i/(float)step);
		uniforms.mix(_uniforms[0],_uniforms[1],rel);
		
		for(int w=-lineWidth;w<=lineWidth;++w)
		{
			Vec3 p = position + wl*w;
			_fragCallback->exec(_targets,p,_cnstUniforms,uptr);
		}
		position += dxy;
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
		if(_uniforms.size() >= 2)
		{
			uniforms.mix(_uniforms[l], _uniforms[r], rel);
			_fragCallback->exec(_targets,imp::Vec3(x,pts[l].y(),pts[l].z()),_cnstUniforms,&uniforms);
		}
		else
		{
			_fragCallback->exec(_targets,imp::Vec3(x,pts[l].y(),pts[l].z()),_cnstUniforms);
		}
	}
}

//--------------------------------------------------------------
void Rasterizer::triangle(const Vec3& p1, const Vec3& p2, const Vec3& p3)
{
	// Scan Line
	
	UniformBuf local = _uniforms;
	UniformBuf uniforms; uniforms.resize(2);
	imp::Vec3 line[2];
		
	Vec3 vertices[3] = {p1,p2,p3};

	int bottom=0,center=1,top=2;
	if(vertices[bottom].y()>vertices[center].y()) std::swap(bottom,center);
	if(vertices[bottom].y()>vertices[top].y()) std::swap(bottom,top);
	if(vertices[center].y()>vertices[top].y()) std::swap(center,top);

	for(int y=std::floor(vertices[bottom].y());y<std::floor(vertices[top].y())+1;++y)
	{
		int a=center,b=top;
		if(y < vertices[center].y()) {a=bottom; b=center;}
		
		float rel0 = imp::clamp(imp::linearstep(vertices[a].y(), vertices[b].y(), (float)y));
		float rel1 = imp::clamp(imp::linearstep(vertices[bottom].y(), vertices[top].y(), (float)y));
		
		line[0] = imp::mix(vertices[a],vertices[b],rel0);
		line[1] = imp::mix(vertices[bottom],vertices[top],rel1);
	
		if(local.size() >= 3)
		{
			uniforms[0].mix(local[a],local[b],rel0);
			uniforms[1].mix(local[bottom],local[top],rel1);
			setUniforms2(uniforms[0],uniforms[1]);
		}
		
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
