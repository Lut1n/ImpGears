#ifndef IMP_GRID_H
#define IMP_GRID_H

#include <Core/Object.h>
#include <Core/Vec.h>

#include <vector>

IMPGEARS_BEGIN

template<int Dim>
class IMP_API Dimension : public Vec<Dim,int>
{
	public:
		
	Meta_Class(Dimension)
	
	Dimension() : Vec<Dim,int>() {}
	Dimension(int sqval) : Vec<Dim,int>(sqval) {}
	template <typename ... Args>
	Dimension(int v0, Args... vn) : Vec<Dim,int>(v0,vn...) {}
	Dimension(const Dimension& d) : Vec<Dim,int>(d) {}
	Dimension(const int* buf) : Vec<Dim,int>(buf) {}
};


template<int Dim, typename Ty>
class IMP_API Grid : public Object
{
    public:
	
	Meta_Class(Grid)
	
	Grid(const Dimension<Dim>& size)
	{
		unsigned int s = 1;
		_size = size;
		for(int i=0;i<_size.size();++i) s*=_size[i];
		_buffer.resize(s);
	}
	
	virtual ~ Grid(){}
	
	unsigned int index(const Dimension<Dim>& pos) const
	{
		int f=1;
		unsigned int r=0;
		
		for(int i=_size.size()-1;i>=0;--i)
		{
			r+=pos[i]*f;
			f*=_size[i];
		}
		return r;
	}
	
	Dimension<Dim> vector(unsigned int index) const
	{
		int f=rawsize();
		Dimension<Dim> r;
		for(int i=0;i<_size.size();++i)
		{
			f/=_size[i];
			r[i]=std::floor(index/f);
			index-=r[i]*f;
		}
		return r;
	}
	
	void set(const Dimension<Dim>& pos, Ty val)
	{
		_buffer[index(pos)] = val;
	}
	
	void set(const Dimension<Dim>& pos, const Grid<Dim,Ty>& other)
	{
		
	}
	
	Ty get(const Dimension<Dim>& pos) const
	{
		return _buffer[index(pos)];
	}
	
	Dimension<Dim> size() const { return _size; }
	
	int rawsize() const
	{
		return _buffer.size();
	}
	
protected:
	
	Dimension<Dim>	_size;
	std::vector<Ty>	_buffer;
};


template<typename Ty>
using Layer = Grid<2,Ty>;

template<typename Ty>
using Chunk = Grid<3,Ty>;

using Dim2 = Dimension<2>;
using Dim3 = Dimension<3>;

IMPGEARS_END

#endif // IMP_GRID_H
