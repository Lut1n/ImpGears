#ifndef IMP_IMAGE_H
#define IMP_IMAGE_H

#include <Core/Object.h>
#include <Core/Vec4.h>
#include <Core/Grid.h>

IMPGEARS_BEGIN

class IMP_API Image : public Object
{
 
public:
    
    using b8 = std::uint8_t;
    
    Meta_Class(Image);
    
    Image(int w, int h, int chnls);
    
    ~Image();
    
    void setPixel(int x, int y, const Vec4& col);
    Vec4 getPixel(int x, int y) const;
    void fill(const Vec4& col);
    
    int height() const;
    int width() const;
    int channels() const;
    
    const Grid<3,b8>::Ptr asGrid() const;
	const b8* data() const;
    
private:
    
    Grid<3,b8>::Ptr _grid;
    Dimension<3>    _dims;
    b8*             _buffer;
};

IMPGEARS_END

#endif // IMP_IMAGE_H
