#ifndef IMP_GRID_H
#define IMP_GRID_H

#include <Core/Object.h>
#include <Core/Vec.h>

#include <vector>
#include <cstring>

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

using IntRect = Vec<4,int>;

template<int Dim, typename Ty>
class IMP_API Grid : public Object
{
    public:
	
	Meta_Class(Grid)
	
	Grid(const Dimension<Dim>& size)
	{
		unsigned int s = 1;
		_size = size;
        int f = 1;
		for(int i=0;i<Dim;++i){
            s*=_size[i];
            _rowFactor[Dim-1-i]=f; f*=_size[Dim-1-i];
        }
		_buffer.resize(s);
	}
	
	virtual ~ Grid(){}
	
	unsigned int index(const Dimension<Dim>& pos) const
	{
        return _rowFactor.dot(pos);
	}
	
	Dimension<Dim> vector(unsigned int index) const
	{
		int f=_buffer.size();
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
	
	Ty get(const Dimension<Dim>& pos) const
	{
		return _buffer[index(pos)];
	}
	
	Dimension<Dim> size() const { return _size; }
	
	void fill(Ty val)
	{
		for(unsigned int i=0;i<_buffer.size();++i) _buffer[i]=val;
	}
	
	const std::vector<Ty>& buffer() const {return _buffer;}
	Ty* data() {return _buffer.data();}
	
protected:
	
	Dimension<Dim>	_size;
    Dimension<Dim>  _rowFactor;
	
	// index order
	// (0,0) = 0
	// (0,1) = 1
	// (1,0) = _size[1]
	std::vector<Ty>	_buffer;
};


template<typename Ty>
class IMP_API Layer : public Grid<2,Ty>
{
public:
	Meta_Class(Layer)
	
	Layer(const Dimension<2>& size)
		: Grid<2,Ty>(size){}
	
	
	void set(const Dimension<2>& pos, Ty val)
	{
		Grid<2,Ty>::set(pos,val);
	}
	
	void set(const Layer& src, const IntRect& dScope)
	{
		const int x=0,y=1,w=2,h=3;
		
		for(int i=0; i<dScope[h]; ++i)
		{
			std::uint32_t sIndex = src.index(Dimension<2>(i,0));
			std::uint32_t dIndex = this->index(Dimension<2>(dScope[y]+i,dScope[x]));

			std::uint32_t len = std::max(0,std::min(dScope[x]+dScope[w], this->_size[x]) - dScope[x]);
			// std::cout << "memcpy " << sIndex << " to " << dIndex << " len=" << len << std::endl;

			std::memcpy(&(this->_buffer[dIndex]), &(src._buffer[sIndex]), len);
		}
	}
};

template<typename Ty>
using Chunk = Grid<3,Ty>;

using Dim2 = Dimension<2>;
using Dim3 = Dimension<3>;

IMPGEARS_END

#endif // IMP_GRID_H
