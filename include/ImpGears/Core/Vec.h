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

		Vec(const Vec& v) { set(v.data()); }

		Vec(const Ty* buf) { set(buf); }

		virtual ~Vec() {}

		Vec operator*(Ty scalar) const { Vec res(*this); FOR_I( res[i]*=scalar; ) return res; }

		Vec operator/(Ty scalar) const { Vec res(*this); FOR_I( res[i]/=scalar; ) return res; }

		const Vec& operator=(const Vec& other) { FOR_I( _data[i]=other[i]; ) return *this; }

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
		
		void set(Ty v) { FOR_I( _data[i]=v; ) }

		void set(const Ty* buf) { FOR_I( _data[i]=buf[i]; ) }
		
		template <typename ... Args>
		void set(Ty v0, Args... vn) { std::vector<Ty> ls = {vn...}; _data[0]=v0; for(int i=1;i<Dim;++i)_data[i]=ls[i-1]; }

		Ty& operator[](unsigned int i) { return _data[i]; }

		Ty operator[](unsigned int i) const { return _data[i]; }

		Ty dot(const Vec& other) const { Ty res=0; FOR_I( res+=_data[i]*other[i]; ) return res; }

		Ty length2() const { return dot(*this); }

		Ty length() const { return std::sqrt(length2()); }

		Ty normalize() { Ty l = length(); operator/=(l); return l; }

		const Ty* data() const { return _data; }
		Ty* data() { return _data; }

    protected:

        Ty _data[Dim];
};

IMPGEARS_END

#undef FOR_I

#endif // IMP_VEC_H
