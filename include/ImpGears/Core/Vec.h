#ifndef IMP_VEC_H
#define IMP_VEC_H

#include <Core/Object.h>

#include <vector>
// #include <Core/Matrix4.h>

IMPGEARS_BEGIN

#define FOR_I(CMD) for(int i=0;i<Dim;++i){CMD}

template<int Dim, typename Ty>
class IMP_API Vec : public Object
{
	
    public:
		
		Meta_Class(Vec)

		Vec() { set((Ty)0); }
		
		Vec(Ty v) { set(v); }
		
		template <typename ... Args>
		Vec(Ty v0, Args... vn) { set(v0,vn...); }
		
		template<int Dim2>
		Vec(const Vec<Dim2,Ty>& v, Ty dv=(Ty)1) { set<Dim2>(v,dv); }

		Vec(const Ty* buf) { set(buf); }

		virtual ~Vec() {}

		Vec operator*(Ty scalar) const { Vec res(*this); FOR_I( res[i]*=scalar; ) return res; }

		Vec operator/(Ty scalar) const { Vec res(*this); FOR_I( res[i]/=scalar; ) return res; }
		
		template<int Dim2>
		const Vec& operator=(const Vec<Dim2,Ty>& other) { set<Dim2>(other); return *this; }
		
		template<int Dim2>
		operator Vec<Dim2,Ty>() const { return Vec<Dim2,Ty>(*this); }

		Vec operator+(const Vec& other) const { Vec res(*this); FOR_I( res[i]+=other[i]; ) return res; }

		Vec operator-(const Vec& other) const { Vec res(*this); FOR_I( res[i]-=other[i]; ) return res; }

		Vec operator-() const { return Vec() - *this; }

		Vec operator*(const Vec& other) const { Vec res(*this); FOR_I( res[i]*=other[i]; ) return res; }

		Vec operator/(const Vec& other) const { Vec res(*this); FOR_I( res[i]/=other[i]; ) return res; }

		bool operator==(const Vec& other) const { FOR_I( if(_data[i]!=other[i])return false; ) return true; }

		bool operator!=(const Vec& other) const { return !(other==*this); }

		const Vec& operator*=(const Vec& other) { FOR_I( _data[i]*=other[i]; ) return *this; }
		const Vec& operator/=(const Vec& other) { FOR_I( _data[i]/=other[i]; ) return *this; }
		const Vec& operator+=(const Vec& other) { FOR_I( _data[i]+=other[i]; ) return *this; }
		const Vec& operator-=(const Vec& other) { FOR_I( _data[i]-=other[i]; ) return *this; }
		const Vec& operator/=(Ty scalar) { FOR_I( _data[i]/=scalar; ) return *this; }
		
		template<int Dim2>
		void set(const Vec<Dim2,Ty>& v,Ty dv=(Ty)1) { if(Dim2>=Dim) set(v.data()); else set(v.data(),Dim2,dv); }
		
		void set(Ty v) { FOR_I( _data[i]=v; ) }

		void set(const Ty* buf, int s=Dim,Ty dv=(Ty)1) { FOR_I( _data[i]=(i<s)?buf[i]:dv; ) }
		
		template <typename ... Args>
		void set(Ty v0, Args... vn)
		{
			std::vector<Ty> ls = {v0,vn...}; int dim=std::min((int)ls.size(),Dim);
			for(int i=0;i<dim;++i) _data[i] = ls[i];
		}

		Ty& operator[](unsigned int i) { return _data[i]; }

		Ty operator[](unsigned int i) const { return _data[i]; }

		Ty dot(const Vec& other) const { Ty res=0; FOR_I( res+=_data[i]*other[i]; ) return res; }

		Ty length2() const { return dot(*this); }

		Ty length() const { return std::sqrt(length2()); }

		Ty normalize() { Ty l = length(); operator/=(l); return l; }

		const Ty* data() const { return _data; }
		Ty* data() { return _data; }
		
		static int size() {return Dim;}

    protected:

        Ty _data[Dim];
};

IMPGEARS_END

#undef FOR_I

#endif // IMP_VEC_H
