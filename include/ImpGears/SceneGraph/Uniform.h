#ifndef IMP_UNIFORM_H
#define IMP_UNIFORM_H

#include <vector>
#include <memory>

#include <Core/Object.h>
#include <Core/Matrix4.h>
#include <SceneGraph/Sampler.h>
#include <SceneGraph/RenderPlugin.h>

IMPGEARS_BEGIN

struct ShaderDsc : public Object
{
	Meta_Class(ShaderDsc);
	
	std::string vertCode;
	std::string fragCode;
	
	RenderPlugin::Data::Ptr _d;
};

/// \brief Defines a shader uniform.
class IMP_API Uniform : public Object
{
	public:
	
	enum Type
	{
		Type_Undefined,
		Type_1f,
		//Type_2f,
		Type_3f,
		//Type_4f,
		
		Type_1i,
		//Type_2i,
		//Type_3i,
		//Type_4i,
		
		//Type_1ui,
		//Type_2ui,
		//Type_3ui,
		//Type_4ui,
		
		Type_1fv,
		//Type_2fv,
		Type_3fv,
		//Type_4fv,
		
		Type_1iv,
		//Type_2iv,
		//Type_3iv,
		//Type_4iv,
		
		//Type_1uiv,
		//Type_2uiv,
		//Type_3uiv,
		//Type_4uiv,
		
		//Type_Mat2v,
		// Type_Mat3v,
		Type_Mat4v,
		Type_Sampler
	};
	
	union Value
	{
		float	 value_1f;
		const Vec3* value_3f;
		int		 value_1i;
		const float*	 value_1fv;
		const Vec3* value_3fv;
		const int*	 value_1iv;
		// const Matrix3* value_mat3v;
		const Matrix4* value_mat4v;
	};
	
	Value getValue() const { return value; }
	Type getType() const { return type; }
	const Sampler::Ptr getSampler() const {return _sampler;}
	std::uint32_t getCount() const {return count;}
	
	Meta_Class(Uniform)
	
	Uniform(const std::string& id, Type type);
	
	~Uniform();
	
	void set(float float1);
	
	void set(const Vec3* vec3);
	
	void set(int int1);
	
	void set(const float* float1Array, int count = 1);
	
	void set(const Vec3* vec3Array, int count);
	
	void set(const int* int1Array, int count = 1);
	
	// void set(const Matrix3* mat3Array, int count);
	
	void set(const Matrix4* mat4Array, int count = 1);
	
	void set(const Sampler::Ptr& sampler, std::int32_t textureUnit = 0);
	
	const std::string& getID() const {return id;}
	
	// void updateUniform(const Shader& program) const;
	
	private:
	
	std::string id;
	std::uint32_t count;
	Type type;
	Value value;
	Sampler::Ptr _sampler;
	
};

IMPGEARS_END

#endif // IMP_UNIFORM_H
