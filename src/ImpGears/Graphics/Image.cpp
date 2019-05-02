#include <Graphics/Image.h>
#include <cstring>

IMPGEARS_BEGIN

#define dotcpy(arr_out,arr_in,N) {for(int i=0;i<N;++i)arr_out[i]=arr_in[i];}

//--------------------------------------------------------------
void memset8(Image::b8* ptr,const Vec4& color, int n,int c)
{
    for(int i=0;i<n;i+=c)
    {
        for(int j=0;j<c;++j)ptr[i+j]=color[j];
    }
}

//--------------------------------------------------------------
void memset32(Image::b8* ptr,const Vec4& color, int n)
{
    std::uint32_t v32;
    Image::b8* v8 = reinterpret_cast<Image::b8*>(&v32);
    dotcpy(v8,color,4);
    
    std::uint32_t* ptr32 = reinterpret_cast<std::uint32_t*>(ptr);
    
    for(int i=0;i<n/4;++i)ptr32[i]=v32;
}

//--------------------------------------------------------------
Image::Image(int w, int h, int chnls)
{
	resize(w,h,chnls);
}

//--------------------------------------------------------------
Image::~Image(){}

//--------------------------------------------------------------
void Image::copy(const Image::Ptr& other)
{
	std::vector<int> mask(channels());
	for(int i=0;i<channels();++i) mask[i]=i;
	copy(other,mask);
}

void Image::copy(const Image::Ptr& other, const std::vector<int>& mask)
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
void Image::setPixel(int x, int y, const Vec4& col)
{
    y = std::min(std::max(y,0),_dims[0]-1);
    x = std::min(std::max(x,0),_dims[1]-1);
    
    int idx=_grid->index(Dimension<3>(y,x,0));
    dotcpy((&_buffer[idx]),col,_dims[2]);
}

//--------------------------------------------------------------
Vec4 Image::getPixel(int x, int y) const
{
    y = std::min(std::max(y,0),_dims[0]-1);
    x = std::min(std::max(x,0),_dims[1]-1);
    
    Vec4 col;
    int idx=_grid->index(Dimension<3>(y,x,0));
    dotcpy(col,(&_buffer[idx]),_dims[2]);
    return col;
}

//--------------------------------------------------------------
void Image::fill(const Vec4& col)
{
    if(_dims[2]==4)
        memset32(_buffer,col, _dims[0]*_dims[1]*_dims[2]);
    else
        memset8(_buffer,col, _dims[0]*_dims[1]*_dims[2],_dims[2]);
}

//--------------------------------------------------------------
int Image::height() const
{
    return _dims[0];
}

//--------------------------------------------------------------
int Image::width() const
{
    return _dims[1];
}

//--------------------------------------------------------------
int Image::channels() const
{
    return _dims[2];
}

//--------------------------------------------------------------
const Grid<3,Image::b8>::Ptr Image::asGrid() const
{
    return _grid;
}

//--------------------------------------------------------------
const Image::b8* Image::data() const
{
	return _buffer;
}

//--------------------------------------------------------------
Image::b8* Image::data()
{
	return _buffer;
}

//--------------------------------------------------------------
void Image::resize(int w, int h, int chnls)
{
	_grid = Grid<3,Image::b8>::create(Dimension<3>(h,w,chnls));
	_dims = _grid->size();
	_buffer = _grid->data();
}

IMPGEARS_END
