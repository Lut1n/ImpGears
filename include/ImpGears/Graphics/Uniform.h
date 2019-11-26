#ifndef IMP_UNIFORM_H
#define IMP_UNIFORM_H

#include <vector>
#include <memory>

#include <Core/Vec2.h>
#include <Core/Vec3.h>
#include <Core/Vec4.h>
#include <Core/Matrix3.h>
#include <Core/Matrix4.h>

#include <Graphics/Sampler.h>

IMPGEARS_BEGIN

class IMP_API Uniform : public Object
{
public:

    enum Type
    {
        Type_Undefined,
        Type_1f,
        Type_2f,
        Type_3f,
        Type_4f,

        Type_1i,

        Type_Mat3,
        Type_Mat4,
        Type_Sampler
    };

    enum Mode
    {
        Mode_Flat,
        Mode_Varying
    };

    Meta_Class(Uniform)

    Uniform(const std::string& id, Type type, Mode mode = Mode_Flat);

    ~Uniform();

    void clone(const Uniform& other);

    void set(float float1);

    void set(const Vec2& vec2);

    void set(const Vec3& vec3);

    void set(const Vec4& vec4);

    void set(int int1);

    void set(const Matrix3& mat3);

    void set(const Matrix4& mat4);

    void set(const ImageSampler::Ptr& sampler, int textureUnit = 0);

    const std::string& getID() const {return _id;}

    int getInt1() const { return _value_1i; }
    float getFloat1() const { return _value_1f; }
    const Vec2& getFloat2() const { return _value_2f; }
    const Vec3& getFloat3() const { return _value_3f; }
    const Vec4& getFloat4() const { return _value_4f; }
    const Matrix3& getMat3() const { return _value_mat3; }
    const Matrix4& getMat4() const { return _value_mat4; }

    Type getType() const { return _type; }
    const ImageSampler::Ptr getSampler() const {return _sampler;}

    void setMode(Mode mode) { _mode = mode; }
    Mode getMode() const { return _mode; }

    void mix(const Uniform::Ptr& u1, const Uniform::Ptr& u2, float delta);

private:

    std::string _id;
    Type _type;
    Mode _mode;
    int _value_1i;
    float _value_1f;
    Vec2 _value_2f;
    Vec3 _value_3f;
    Vec4 _value_4f;
    Matrix3 _value_mat3;
    Matrix4 _value_mat4;
    ImageSampler::Ptr _sampler;
};

IMPGEARS_END

#endif // IMP_UNIFORM_H
