#ifndef IMP_IMG_OPERATION_H
#define IMP_IMG_OPERATION_H

#include <Core/Object.h>
#include <Core/Matrix3.h>
#include <Graphics/Sampler.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
struct IMP_API ImageOperation : public Object
{	
    Meta_Class(ImageOperation)

    void execute(Image::Ptr& src);

    virtual void apply(const Vec2& uv, Vec4& outColor) = 0;
};

//--------------------------------------------------------------
struct IMP_API HashOperation : public ImageOperation
{
    float _seed;

    HashOperation() : _seed(0.0) {}

    void setSeed(float seed) { _seed=seed; }

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API NoiseOperation : public ImageOperation
{
    Meta_Class(NoiseOperation)

    Image::Ptr _hash;
    float _freq;

    NoiseOperation() : _freq(1.0) {}

    void setHashmap(const Image::Ptr& hashmap) { _hash = hashmap; }
    void setFreq(float f) { _freq = f; }

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API VoronoiOperation : public ImageOperation
{
    Meta_Class(VoronoiOperation)

    Image::Ptr _hash;
    float _freq;

    VoronoiOperation() : _freq(1.0) {}

    void setHashmap(const Image::Ptr& hashmap) { _hash = hashmap; }
    void setFreq(float f) { _freq = f; }

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API FbmOperation : public ImageOperation
{
    Image::Ptr _hash;
    float _freq;
    int _type;
    int _octaves;
    float _persist;
    bool _initialized;

    NoiseOperation _perlinOp;
    VoronoiOperation _voronoiOp;

    FbmOperation() : _freq(1.0), _initialized(false) {}

    void setHashmap(const Image::Ptr& hashmap) { _hash = hashmap; }
    void setFreq(float f) { _freq = f; }
    void setType(int i) { _type = i; }
    void setOctaves(int o) { _octaves = o; }
    void setPersist(float p) { _persist = p; }

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API SignalOperation : public ImageOperation
{
    Vec3 _vec;
    float _freq;
    float _ampl;
    float _phase;
    float _base;
    float _noiseIntensity;
    Image::Ptr _noise;

    // directional signal
    // radial signal
    // (see polarSignal)
    int _type;

    void setTargetVec(const Vec3& tv) { _vec=tv; }
    void setFreq(float freq) { _freq=freq; }
    void setAmpl(float ampl) { _ampl=ampl; }
    void setPhase(float ph) { _phase=ph; }
    void setBase(float base) { _base=base; }
    void setType(int ty) {_type=ty;}
    void setNoise(const Image::Ptr& n, float intensity = 1.0) {_noise=n; _noiseIntensity=intensity;}

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API BumpToNormalOperation : public ImageOperation
{
    Image::Ptr _target;

    void setTarget(const Image::Ptr& target) {_target=target;}

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API PerturbateOperation : public ImageOperation
{
    Image::Ptr _target;
    Image::Ptr _uvmap;
    float _intensity;

    void setTarget(const Image::Ptr& target) {_target=target;}
    void setUVMap(const Image::Ptr& uvmap) {_uvmap=uvmap;}
    void setIntensity(float intensity) {_intensity=intensity;}

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API ColorMixOperation : public ImageOperation
{
    Image::Ptr _target;
    Vec4 _color1;
    Vec4 _color2;

    void setTarget(const Image::Ptr& target) {_target=target;}
    void setColor1(const Vec4& c) {_color1=c;}
    void setColor2(const Vec4& c) {_color2=c;}

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API MorphoOperation : public ImageOperation
{
    Image::Ptr _target;
    Matrix3 _mat;
    int _type;

    MorphoOperation() : _type(1.0) {}

    void setTarget(const Image::Ptr& target) {_target=target;}
    void setMatrix(const Matrix3& mat) {_mat=mat;}
    void setType(int type) {_type=type;}

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

//--------------------------------------------------------------
struct IMP_API Conv2dOperation : public ImageOperation
{
    Image::Ptr _target;
    Matrix3 _mat;

    void setTarget(const Image::Ptr& target) {_target=target;}
    void setMatrix(const Matrix3& mat) {_mat=mat;}

    virtual void apply(const Vec2& uv, Vec4& outColor);
};

IMPGEARS_END

#endif // IMP_IMG_OPERATION_H
