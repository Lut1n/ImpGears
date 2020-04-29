#ifndef IMP_FLOATIMAGE_H
#define IMP_FLOATIMAGE_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Core/Vec2.h>
#include <ImpGears/Core/Vec4.h>
#include <ImpGears/Core/Grid.h>

IMPGEARS_BEGIN

class IMP_API FloatImage : public Object
{
 
public:
    using f32 = float;

    Meta_Class(FloatImage);

    FloatImage(int w, int h, int chnls);
    ~FloatImage();

    void copy(const FloatImage::Ptr& other);
    void copy(const FloatImage::Ptr& other, const std::vector<int>& mask);

    void setPixel(int x, int y, const Vec4& col);
    Vec4 getPixel(int x, int y) const;

    void setPixel(const Vec2& uv, const Vec4& col);
    Vec4 getPixel(const Vec2& uv) const;

    void fill(const Vec4& col);

    void resize(int w, int h, int chnls);

    int height() const;
    int width() const;
    int channels() const;

    const Grid<3,f32>::Ptr asGrid() const;
    const f32* data() const;
    f32* data();
    
private:
    Grid<3,f32>::Ptr    _grid;
    Dimension<3>        _dims;
    f32*                _buffer;
};

IMPGEARS_END

#endif // IMP_FLOATIMAGE_H
