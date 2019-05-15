#ifndef IMP_RASTERIZER_H
#define IMP_RASTERIZER_H

#include <Core/Math.h>
#include <Core/Vec3.h>
#include <Graphics/Image.h>
#include <Graphics/Uniform.h>

#include <map>
#include <vector>

IMPGEARS_BEGIN

struct IMP_API UniformMap
{
	std::map<std::string,Uniform::Ptr> _values;
	
	bool contains(const std::string& name) const;
	
	void clear();
	
	const Uniform::Ptr get(const std::string& name) const;
	const Matrix3& getMat3(const std::string& name) const;
	const Matrix4& getMat4(const std::string& name) const;
	const Vec3& getVec3(const std::string& name) const;
	const Vec4& getVec4(const std::string& name) const;
	const ImageSampler::Ptr getSampler(const std::string& name) const;
	
	void set(const Uniform::Ptr& uniform);
	void set(const std::string& name, const Matrix3& mat3);
	void set(const std::string& name, const Matrix4& mat4);
	void set(const std::string& name, const Vec3& v3);
	void set(const std::string& name, const Vec4& v4);
	void set(const std::string& name, const ImageSampler::Ptr& sampler);
	
	void reserve(const UniformMap& u);
	void copy(const UniformMap& u);
	void mix(const UniformMap& u1,const UniformMap& u2, float delta);
	
	Matrix3 _defaultMat3;
	Matrix4 _defaultMat4;
	Vec3 _defaultVec3;
	Vec4 _defaultVec4;
	ImageSampler::Ptr _defaultSampler;
};

struct IMP_API Varyings
{
    std::map<std::string, int> _adr;
    std::vector<float> _allocator;
    float*  _buf;
    
    void set(const std::string& id, const imp::Vec3& v);
    
    bool contains(const std::string& id) const;
    
    void alloc(const std::string& id, int size);
    
    Vec3 get(const std::string& id) const;
    
    void mix(const Varyings& v1, const Varyings& v2, float delta);
};

using ImageBuf = std::map<int,imp::Image::Ptr>;
using VaryingsBuf = std::vector<Varyings>;

struct IMP_API FragCallback : public imp::Object
{
    Meta_Class(FragCallback)
    virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings) = 0;
};

class IMP_API Rasterizer : public imp::Object
{
public:
    
    ImageBuf _targets;
    VaryingsBuf _inputVaryings;
	VaryingsBuf _subVaryings;
	UniformMap _uniforms;
    imp::Vec4 _defaultColor;
    FragCallback::Ptr _fragCallback;
    
    Meta_Class(Rasterizer);
    
    Rasterizer();
    
    ~Rasterizer();
    
    void setTarget(int index, imp::Image::Ptr target);
    void clearTarget();
    void setTarget(imp::Image::Ptr target);
    void setColor(const imp::Vec4& col);
    void clearVaryings();
	void setUniformMap(const UniformMap& um);
	void clearUniformMap();
    void setVaryings2(const Varyings& v1, const Varyings& v2);
    void setVaryings3(const Varyings& v1, const Varyings& v2, const Varyings& v3);
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
    
protected:
	
	void line(const Vec3& p1, const Vec3& p2, const Varyings& v1, const Varyings& v2);
	void hLine(const Vec3& p1, const Vec3& p2, const Varyings& v1, const Varyings& v2);
};

IMPGEARS_END

#endif // IMP_RASTERIZER_H
