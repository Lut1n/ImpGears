#ifndef IMP_SAMPLER_H
#define IMP_SAMPLER_H

#include <Core/Object.h>
#include <Core/Vec2.h>
#include <Core/Vec3.h>
#include <Graphics/Image.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
template<typename TyIn, typename TyOut>
class IMP_API Sampler
{
public:

    Meta_Class(Sampler)

    enum Filtering
    {
        Filtering_Nearest,
        Filtering_Linear,
        Filtering_Smooth
    };

    enum Wrapping
    {
        Wrapping_Clamp,
        Wrapping_Mirror,
        Wrapping_Repeat
    };

    Sampler(Filtering filtering = Filtering_Nearest) : _filtering(filtering) {}

    virtual TyOut operator()(const TyIn& t) = 0;
    void setFiltering(Filtering filtering) {_filtering = filtering;}
    Filtering getFiltering() const {return _filtering;}

    Filtering _filtering;
};

//--------------------------------------------------------------
class IMP_API ImageSampler : public Sampler< Vec2, Vec4 >
{
public:

    Meta_Class(ImageSampler)

    ImageSampler(Image::Ptr src = nullptr, Wrapping wrapping = Wrapping_Clamp);
    ImageSampler(int w, int h, int chnl, const Vec4& color);
    void setSource(Image::Ptr src);
    const Image::Ptr& getSource() const;
    Image::Ptr& getSource();

    virtual Vec4 operator()(const Vec2& uv);

    void set(const Vec2& uv, const Vec4& color);
    void set(float u, float v, const Vec4& color);

    Vec4 get(const Vec2& uv);
    Vec4 get(float u, float v);

    void setWrapping(Wrapping wrapping);
    Wrapping getWrapping() const;

    void setInternalSrc(int w, int h, int chnl);

protected:

    Image::Ptr _src;
    Wrapping _wrapping;
    Vec2 _dims;
};


//--------------------------------------------------------------
class IMP_API CubeMapSampler : public Sampler< Vec3, Vec4 >
{
public:

    Meta_Class(CubeMapSampler)

    CubeMapSampler(Image::Ptr src = nullptr, Wrapping wrapping = Wrapping_Clamp);
    CubeMapSampler(const std::vector<Image::Ptr>& src, Wrapping wrapping = Wrapping_Clamp);
    CubeMapSampler(int w, int h, int chnl, const Vec4& color);
    void setSource(const std::vector<Image::Ptr>& src);
    const std::vector<Image::Ptr>& getSource() const;

    virtual Vec4 operator()(const Vec3& uv);

    void set(const Vec3& uvw, const Vec4& color);
    void set(float u, float v, float w, const Vec4& color);

    Vec4 get(const Vec3& uvw);
    Vec4 get(float u, float v, float w);

    void setWrapping(Wrapping wrapping);
    Wrapping getWrapping() const;

    void setInternalSrc(int w, int h, int chnl);

protected:

    std::vector<Image::Ptr> _src;
    Wrapping _wrapping;
    Vec2 _dims;
};

IMPGEARS_END

#endif // IMP_SAMPLER_H
