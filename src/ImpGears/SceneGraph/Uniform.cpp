#include <SceneGraph/Uniform.h>

#include <cstdio>
#include <iostream>


IMPGEARS_BEGIN


//--------------------------------------------------------------
Uniform::Uniform(const std::string& id, Type type)
{
	this->id = id;
	this->type = type;
}

//--------------------------------------------------------------
Uniform::~Uniform()
{
	
}

//--------------------------------------------------------------
void Uniform::set(float float1)
{
	this->type = Type_1f;
	value.value_1f = float1;
}

//--------------------------------------------------------------
void Uniform::set(const Vec3* vec3)
{
	this->type = Type_3f;
	value.value_3f = vec3;
}

//--------------------------------------------------------------
void Uniform::set(int int1)
{
	this->type = Type_1i;
	value.value_1i = int1;
}

//--------------------------------------------------------------
void Uniform::set(const float* float1Array, int count)
{
	this->type = Type_1fv;
	value.value_1fv = float1Array;
	this->count = count;
}

//--------------------------------------------------------------
void Uniform::set(const Vec3* vec3Array, int count)
{
	this->type = Type_3fv;
	value.value_3fv = vec3Array;
	this->count = count;
}

//--------------------------------------------------------------
void Uniform::set(const int* int1Array, int count)
{
	this->type = Type_1iv;
	value.value_1iv = int1Array;
	this->count = count;
}

//--------------------------------------------------------------
/*void Uniform::set(const Matrix3* mat3Array, int count)
{
	this->type = Type_Mat3v;
	value.value_mat3v = mat3Array;
	this->count = count;
}*/

//--------------------------------------------------------------
void Uniform::set(const Matrix4* mat4Array, int count)
{
	this->type = Type_Mat4v;
	value.value_mat4v = mat4Array;
	this->count = count;
}

//--------------------------------------------------------------
void Uniform::set(const TextureSampler::Ptr& sampler, std::int32_t textureUnit)
{
	this->type = Type_Sampler;
	value.value_1i = textureUnit;
	_sampler = sampler;
}

IMPGEARS_END
