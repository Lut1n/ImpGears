#include <ImpGears/Graphics/FloatImage.h>
#include <cstring>

IMPGEARS_BEGIN

#define dotcpy(arr_out,arr_in,N) {for(int i=0;i<N;++i)arr_out[i]=arr_in[i];}

//--------------------------------------------------------------
/*void memset8(Image::f32* ptr,const Vec4& color, int n,int c)
{
    for(int i=0;i<n;i+=c)
    {
        for(int j=0;j<c;++j)ptr[i+j]=color[j];
    }
}*/

//--------------------------------------------------------------
/*void memset32(FloatImage::f32* ptr,const Vec4& color, int n)
{
    std::uint32_t v32;
    FloatImage::f32* v32 = reinterpret_cast<FloatImage::f32*>(&v32);
    dotcpy(v32,color,4);

    std::uint32_t* ptr32 = reinterpret_cast<std::uint32_t*>(ptr);

    for(int i=0;i<n/4;++i)ptr32[i]=v32;
}*/

//--------------------------------------------------------------
FloatImage::FloatImage(int w, int h, int chnls)
{
    resize(w,h,chnls);
}

//--------------------------------------------------------------
FloatImage::~FloatImage(){}

//--------------------------------------------------------------
void FloatImage::copy(const FloatImage::Ptr& other)
{
    std::vector<int> mask(channels());
    for(int i=0;i<channels();++i) mask[i]=i;
    copy(other,mask);
}

void FloatImage::copy(const FloatImage::Ptr& other, const std::vector<int>& mask)
{
    int c = mask.size();
    resize(other->width(), other->height(), c);
    for(int i=0;i<width();++i) for(int j=0;j<height();++j)
    {
        Vec4 res;
        Vec4 pixel = other->getPixel(i,j);
        for(int k=0;k<c;++k) res[k] = pixel[ mask[k] ];
        setPixel(i,j,res);
    }
}

//--------------------------------------------------------------
void FloatImage::setPixel(int x, int y, const Vec4& col)
{
    y = std::min(std::max(y,0),_dims[0]-1);
    x = std::min(std::max(x,0),_dims[1]-1);

    int idx=_grid->index(Dimension<3>(y,x,0));
    dotcpy((&_buffer[idx]),col,_dims[2]);
}

//--------------------------------------------------------------
Vec4 FloatImage::getPixel(int x, int y) const
{
    y = std::min(std::max(y,0),_dims[0]-1);
    x = std::min(std::max(x,0),_dims[1]-1);

    Vec4 col;
    int idx=_grid->index(Dimension<3>(y,x,0));
    dotcpy(col,(&_buffer[idx]),_dims[2]);
    return col;
}

//--------------------------------------------------------------
void FloatImage::setPixel(const Vec2& uv, const Vec4& col)
{
    setPixel(uv[0],uv[1],col);
}

//--------------------------------------------------------------
Vec4 FloatImage::getPixel(const Vec2& uv) const
{
    return getPixel(uv[0],uv[1]);
}

//--------------------------------------------------------------
void FloatImage::fill(const Vec4& col)
{
    _grid->fill(col[0]);
}

//--------------------------------------------------------------
int FloatImage::height() const
{
    return _dims[0];
}

//--------------------------------------------------------------
int FloatImage::width() const
{
    return _dims[1];
}

//--------------------------------------------------------------
int FloatImage::channels() const
{
    return _dims[2];
}

//--------------------------------------------------------------
const Grid<3,FloatImage::f32>::Ptr FloatImage::asGrid() const
{
    return _grid;
}

//--------------------------------------------------------------
const FloatImage::f32* FloatImage::data() const
{
    return _buffer;
}

//--------------------------------------------------------------
FloatImage::f32* FloatImage::data()
{
    return _buffer;
}

//--------------------------------------------------------------
void FloatImage::resize(int w, int h, int chnls)
{
    _grid = Grid<3,FloatImage::f32>::create(Dimension<3>(h,w,chnls));
    _dims = _grid->size();
    _buffer = _grid->data();
}

IMPGEARS_END
