#include <Graphics/Uniform.h>
#include <Core/Math.h>

#include <cstdio>
#include <iostream>


IMPGEARS_BEGIN

//--------------------------------------------------------------
Uniform::Uniform(const std::string& id, Type type, Mode mode)
    : _id(id)
    , _type(type)
    , _mode(mode)
{
}

//--------------------------------------------------------------
Uniform::~Uniform()
{
}

//--------------------------------------------------------------
void Uniform::clone(const Uniform& other)
{
    _id = other._id;
    _type = other._type;
    _mode = other._mode;
    _value_1i = other._value_1i;
    _value_1f = other._value_1f;
    _value_2f = other._value_2f;
    _value_3f = other._value_3f;
    _value_4f = other._value_4f;
    _value_mat3 = other._value_mat3;
    _value_mat4 = other._value_mat4;
    _sampler = other._sampler;
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
void Uniform::set(const ImageSampler::Ptr& sampler, int textureUnit)
{
    _type = Type_Sampler;
    _value_1i = textureUnit;
    _sampler = sampler;
}

//--------------------------------------------------------------
void Uniform::mix(const Uniform::Ptr& u1, const Uniform::Ptr& u2, float delta)
{
    if(u1->_type != u2->_type) return;

    _mode = u1->_mode;
    _type = u1->_type;

    if(_mode == Mode_Flat)
    {
        if(_type == Type_1i) _value_1i = u1->_value_1i;
        if(_type == Type_1f) _value_1f = u1->_value_1f;
        if(_type == Type_2f) _value_2f = u1->_value_2f;
        if(_type == Type_3f) _value_3f = u1->_value_3f;
        if(_type == Type_4f) _value_4f = u1->_value_4f;
        if(_type == Type_Mat3) _value_mat3 = u1->_value_mat3;
        if(_type == Type_Mat4) _value_mat4 = u1->_value_mat4;
        if(_type == Type_Sampler) {_sampler = u1->_sampler; _value_1i = u1->_value_1i; }

        return;
    }
    else if(_mode == Mode_Varying)
    {
        if(_type == Type_1i) _value_1i = imp::mix(u1->_value_1i,u2->_value_1i, delta);
        if(_type == Type_1f) _value_1f = imp::mix(u1->_value_1f,u2->_value_1f, delta);
        if(_type == Type_2f) _value_2f = imp::mix(u1->_value_2f,u2->_value_2f, delta);
        if(_type == Type_3f) _value_3f = imp::mix(u1->_value_3f,u2->_value_3f, delta);
        if(_type == Type_4f) _value_4f = imp::mix(u1->_value_4f,u2->_value_4f, delta);
        if(_type == Type_Mat3) _value_mat3 = u1->_value_mat3;
        if(_type == Type_Mat4) _value_mat4 = u1->_value_mat4;
        if(_type == Type_Sampler) {_sampler = u1->_sampler; _value_1i = u1->_value_1i; }
    }
}

IMPGEARS_END
