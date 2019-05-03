#ifndef IMP_RASTERIZER_H
#define IMP_RASTERIZER_H

#include <Core/Math.h>
#include <Core/Vec3.h>
#include <Graphics/Image.h>
#include <Graphics/Uniform.h>

#include <map>
#include <vector>

IMPGEARS_BEGIN

struct IMP_API CnstUniforms
{
	std::map<std::string,Uniform::Ptr> _values;
	
	bool contains(const std::string& name) const;
	
	void clear();
	
	const Matrix3& getMat3(const std::string& name) const;
	const Matrix4& getMat4(const std::string& name) const;
	const Vec3& getVec3(const std::string& name) const;
	const Vec4& getVec4(const std::string& name) const;
	const TextureSampler::Ptr getSampler(const std::string& name) const;
	
	void set(const Uniform::Ptr& uniform);
	void set(const std::string& name, const Matrix3& mat3);
	void set(const std::string& name, const Matrix4& mat4);
	void set(const std::string& name, const Vec3& v3);
	void set(const std::string& name, const Vec4& v4);
	void set(const std::string& name, const TextureSampler::Ptr& sampler);
	
	Matrix3 _defaultMat3;
	Matrix4 _defaultMat4;
	Vec3 _defaultVec3;
	Vec4 _defaultVec4;
	TextureSampler::Ptr _defaultSampler;
};

struct IMP_API Uniforms
{
    std::map<std::string, int> _adr;
    std::vector<float> _allocator;
    float*  _buf;
    
    void set(const std::string& id, const imp::Vec3& v)
    {
		if(!contains(id)) alloc(id,3);
        memcpy(_buf+_adr[id],v.data(),3*sizeof(float));
    }
    
    bool contains(const std::string& id) { return _adr.find(id) != _adr.end(); }
    
    void alloc(const std::string& id, int size)
	{
		int index = _allocator.size();
		_allocator.resize(index+size);
        _buf = _allocator.data();
		_adr[id] = index;
	}
    
    imp::Vec3 get(const std::string& id) { return imp::Vec3(_buf+_adr[id]); }
    
    void mix(const Uniforms& v1, const Uniforms& v2, float delta)
    {
        _adr = v1._adr; _allocator.resize(v1._allocator.size());
        _buf = _allocator.data();
        for(int i=0; i<(int)_allocator.size(); ++i) _buf[i] = imp::mix( v1._buf[i], v2._buf[i], delta );
    }
};

using ImageBuf = std::map<int,imp::Image::Ptr>;
using UniformBuf = std::vector<Uniforms>;

struct IMP_API FragCallback : public imp::Object
{
    Meta_Class(FragCallback)
    virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr) = 0;
};

class IMP_API Rasterizer : public imp::Object
{
public:
    
    ImageBuf _targets;
    UniformBuf _uniforms;
	CnstUniforms _cnstUniforms;
    imp::Vec4 _defaultColor;
    FragCallback::Ptr _fragCallback;
    
    Meta_Class(Rasterizer);
    
    Rasterizer();
    
    ~Rasterizer();
    
    void setTarget(int index, imp::Image::Ptr target);
    void clearTarget();
    void setTarget(imp::Image::Ptr target);
    void setColor(const imp::Vec4& col);
    void clearUniforms();
	void setCnstUniforms(const CnstUniforms& cu);
	void clearCnstUniforms();
    void setUniforms2(const Uniforms& u1, const Uniforms& u2);
    void setUniforms3(const Uniforms& u1, const Uniforms& u2, const Uniforms& u3);
    void setFragCallback(FragCallback::Ptr cb);
    void useDefaultFragCallback();
    
    void rectangle(const Vec3& p1, const Vec3& p2);

    void square(const Vec3& p1, int sideSize);

    void grid(const Vec3& p1, const Vec3& p2, int gridOffset = 0);

    void dot(const Vec3& p1);

    void hSymmetry();
    
	void line(const Vec3& p1, const Vec3& p2);
	
	void hLine(const Vec3& p1, const Vec3& p2);
	
	void triangle(const Vec3& p1, const Vec3& p2, const Vec3& p3);
	
	void wireTriangle(const Vec3& p1, const Vec3& p2, const Vec3& p3);
    
private:
};

IMPGEARS_END

#endif // IMP_RASTERIZER_H
