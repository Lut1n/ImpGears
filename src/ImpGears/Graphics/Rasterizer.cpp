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
		
	virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings)
	{
		for(auto img:targets)img.second->setPixel(pt[0],pt[1],_rast->_defaultColor);
	}
};


//-------------------------------------------------------------- 
bool UniformMap::contains(const std::string& name) const
{
	return _values.find(name)!=_values.end();
}

//-------------------------------------------------------------- 
void UniformMap::clear()
{
	_values.clear();
}

//-------------------------------------------------------------- 
void UniformMap::reserve(const UniformMap& u)
{
	for(auto kv : u._values)
		if( !contains(kv.first) )
			_values[kv.first] = Uniform::create(kv.first, kv.second->getType());
}

//-------------------------------------------------------------- 
void UniformMap::copy(const UniformMap& u)
{
	mix(u,u,0.0);
}

//-------------------------------------------------------------- 
void UniformMap::mix(const UniformMap& u1,const UniformMap& u2, float delta)
{
	reserve(u1);
	for(auto kv : _values)
		_values[kv.first]->mix(u1.get(kv.first),u2.get(kv.first),delta);
}

//-------------------------------------------------------------- 
const Uniform::Ptr UniformMap::get(const std::string& name) const
{
	if(contains(name)) return _values.at(name);
	else return nullptr;
}

//-------------------------------------------------------------- 
const Matrix3& UniformMap::getMat3(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getMat3();
	else return _defaultMat3;
}

//-------------------------------------------------------------- 
const Matrix4& UniformMap::getMat4(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getMat4();
	else return _defaultMat4;
}

//-------------------------------------------------------------- 
const Vec3& UniformMap::getVec3(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getFloat3();
	else return _defaultVec3;
}

//-------------------------------------------------------------- 
const Vec4& UniformMap::getVec4(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getFloat4();
	else return _defaultVec4;
}

//-------------------------------------------------------------- 
const ImageSampler::Ptr UniformMap::getSampler(const std::string& name) const
{
	if(contains(name)) return _values.at(name)->getSampler();
	else return _defaultSampler;
}

//-------------------------------------------------------------- 
void Varyings::set(const std::string& id, const imp::Vec3& v)
{
	if(!contains(id)) alloc(id,3);
	memcpy(_buf+_adr[id],v.data(),3*sizeof(float));
}

//--------------------------------------------------------------     
bool Varyings::contains(const std::string& id) const
{
	return _adr.find(id) != _adr.end();
}

//-------------------------------------------------------------- 
void Varyings::alloc(const std::string& id, int size)
{
	int index = _allocator.size();
	_allocator.resize(index+size);
	_buf = _allocator.data();
	_adr[id] = index;
}

//-------------------------------------------------------------- 
Vec3 Varyings::get(const std::string& id) const
{
	Vec3 res;
	if(_adr.find(id)!=_adr.end()) res=Vec3(_buf+_adr.at(id));
	return res;
}

//-------------------------------------------------------------- 
void Varyings::mix(const Varyings& v1, const Varyings& v2, float delta)
{
	_adr = v1._adr; _allocator.resize(v1._allocator.size());
	_buf = _allocator.data();
	for(int i=0; i<(int)_allocator.size(); ++i) _buf[i] = imp::mix( v1._buf[i], v2._buf[i], delta );
}

//-------------------------------------------------------------- 
void UniformMap::set(const std::string& name, const Matrix3& mat3)
{
	if(contains(name)) _values[name]->set(mat3);
}

//-------------------------------------------------------------- 
void UniformMap::set(const std::string& name, const Matrix4& mat4)
{
	if(contains(name)) _values[name]->set(mat4);
}

//-------------------------------------------------------------- 
void UniformMap::set(const std::string& name, const Vec3& v3)
{
	if(contains(name)) _values[name]->set(v3);
}

//-------------------------------------------------------------- 
void UniformMap::set(const std::string& name, const Vec4& v4)
{
	if(contains(name)) _values[name]->set(v4);
}

//-------------------------------------------------------------- 
void UniformMap::set(const std::string& name, const ImageSampler::Ptr& sampler)
{
	if(contains(name)) _values[name]->set(sampler);
}

//-------------------------------------------------------------- 
void UniformMap::set(const Uniform::Ptr& uniform)
{
	_values[uniform->getID()] = uniform;
}

//-------------------------------------------------------------- 
Rasterizer::Rasterizer()
	: _defaultColor(255.0,255.0,255.0,255.0)
{
	_inputVaryings.resize(3);
	_subVaryings.resize(3);
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
void Rasterizer::clearVaryings()
{
	_inputVaryings[0] = Varyings();
	_inputVaryings[1] = Varyings();
	_inputVaryings[2] = Varyings();
}

//--------------------------------------------------------------
void Rasterizer::setUniformMap(const UniformMap& um)
{
	_uniforms = um;
}

//--------------------------------------------------------------
void Rasterizer::clearUniformMap()
{
	_uniforms.clear();
}

//--------------------------------------------------------------
void Rasterizer::setVaryings2(const Varyings& v1, const Varyings& v2)
{
	_inputVaryings[0] = v1;
	_inputVaryings[1] = v2;
}

//--------------------------------------------------------------
void Rasterizer::setVaryings3(const Varyings& v1, const Varyings& v2, const Varyings& v3)
{
	_inputVaryings[0] = v1;
	_inputVaryings[1] = v2;
	_inputVaryings[2] = v3;
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
		_fragCallback->exec(_targets,Vec3(i,j,0),_uniforms,_inputVaryings[0]);
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
		if((i+j+gridOffset)%2 == 0)_fragCallback->exec(_targets,Vec3(i,j,0),_uniforms,_inputVaryings[0]);
	}
}

//--------------------------------------------------------------
void Rasterizer::dot(const Vec3& p1)
{
	_fragCallback->exec(_targets,p1,_uniforms,_inputVaryings[0]);
}

//--------------------------------------------------------------
void Rasterizer::hSymmetry()
{
}

//--------------------------------------------------------------
void Rasterizer::line(const Vec3& p1, const Vec3& p2)
{
	line(p1,p2,_inputVaryings[0],_inputVaryings[1]);
}

//--------------------------------------------------------------
void Rasterizer::line(const Vec3& p1, const Vec3& p2, const Varyings& v1, const Varyings& v2)
{
	// Digital differential analyzer (DDA)
	float lineWidth = 1.0;
	lineWidth -= 1.0; // 0 is 1 pixel
	
	Varyings varyings;
	Vec3 dxy = p2 - p1;
	
	Vec3 wl(1,0,0);
	int step = std::abs(dxy[1]);
	if(std::abs(dxy[0]) >= std::abs(dxy[1]))
	{
		step = std::abs(dxy[0]);
		wl = Vec3(0,1,0);
	}
	dxy /= step;
	
	Vec3 position = p1;
	for(int i=0;i<=step;++i)
	{
		float rel = imp::clamp((float)i/(float)step);
		varyings.mix(v1,v2,rel);
		
		for(int w=-lineWidth;w<=lineWidth;++w)
		{
			Vec3 p = position + wl*w;
			_fragCallback->exec(_targets,p,_uniforms,varyings);
		}
		position += dxy;
	}
}

//--------------------------------------------------------------
void Rasterizer::hLine(const Vec3& p1, const Vec3& p2)
{
	hLine(p1,p2,_inputVaryings[0],_inputVaryings[1]);
}

//--------------------------------------------------------------
void Rasterizer::hLine(const Vec3& p1, const Vec3& p2, const Varyings& v1, const Varyings& v2)
{
	const Varyings* vptr_l = &v1;
	const Varyings* vptr_r = &v2;
	int l=0,r=1;
	Vec3 pts[2] = {p1,p2};
	if(pts[l].x() > pts[r].x()){l=1;r=0; vptr_l=&v2; vptr_r=&v1; }

	Varyings varyings;
	for(int x=std::floor(pts[l].x());x<std::floor(pts[r].x())+1;++x)
	{
		float rel = imp::clamp(imp::linearstep(pts[l].x(),pts[r].x(), (float)x));
		varyings.mix(*vptr_l, *vptr_r, rel);
		_fragCallback->exec(_targets,imp::Vec3(x,pts[l].y(),pts[l].z()),_uniforms,varyings);
	}
}

//--------------------------------------------------------------
void Rasterizer::triangle(const Vec3& p1, const Vec3& p2, const Vec3& p3)
{
	// Scan Line
	Vec3 vertices[3] = {p1,p2,p3};
	Vec3 line[2];

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
	
		_subVaryings[0].mix(_inputVaryings[a],_inputVaryings[b],rel0);
		_subVaryings[1].mix(_inputVaryings[bottom],_inputVaryings[top],rel1);
		hLine(line[0],line[1],_subVaryings[0],_subVaryings[1]);
	}
}

//--------------------------------------------------------------
void Rasterizer::wireTriangle(const Vec3& p1, const Vec3& p2, const Vec3& p3)
{
	line(p1,p2,_inputVaryings[0],_inputVaryings[1]);
	line(p1,p3,_inputVaryings[0],_inputVaryings[2]);
	line(p2,p3,_inputVaryings[1],_inputVaryings[2]);
}

IMPGEARS_END
