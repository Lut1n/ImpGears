#include <Graphics/Uniform.h>

#include <cstdio>
#include <iostream>


IMPGEARS_BEGIN


//--------------------------------------------------------------
Uniform::Uniform(const std::string& id, Type type)
	: _id(id)
	, _type(type)
{
}

//--------------------------------------------------------------
Uniform::~Uniform()
{
}

//--------------------------------------------------------------
void Uniform::set(float float1)
{
	_type = Type_1f;
	_value_1f = float1;
}

//--------------------------------------------------------------
void Uniform::set(const Vec2& vec2)
{
	_type = Type_2f;
	_value_2f = vec2;
}

//--------------------------------------------------------------
void Uniform::set(const Vec3& vec3)
{
	_type = Type_3f;
	_value_3f = vec3;
}

//--------------------------------------------------------------
void Uniform::set(const Vec4& vec4)
{
	_type = Type_4f;
	_value_4f = vec4;
}

//--------------------------------------------------------------
void Uniform::set(int int1)
{
	_type = Type_1i;
	_value_1i = int1;
}

//--------------------------------------------------------------
void Uniform::set(const Matrix3& mat3)
{
	_type = Type_Mat3;
	_value_mat3 = mat3;
}

//--------------------------------------------------------------
void Uniform::set(const Matrix4& mat4)
{
	_type = Type_Mat4;
	_value_mat4 = mat4;
}

//--------------------------------------------------------------
void Uniform::set(const TextureSampler::Ptr& sampler, int textureUnit)
{
	_type = Type_Sampler;
	_value_1i = textureUnit;
	_sampler = sampler;
}

IMPGEARS_END
