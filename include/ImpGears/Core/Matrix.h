#ifndef IMP_MATRIX_H
#define IMP_MATRIX_H

#include <Core/Object.h>

IMPGEARS_BEGIN

template<int Cn, int Rn, typename Ty>
class IMP_API Matrix : public Object
{
public:

	Meta_Class(Matrix)

	Matrix();
	Matrix(const Matrix& mat);
	Matrix(const float* buf, bool transp = false) { set(buf,transp); }
	virtual ~Matrix();

	const float& at(int c, int r) const;
	float& at(int c, int r);
	
	void set(const float* buf, bool transp = false);
	
	void transpose();

	const Matrix& operator=(const Matrix& other){ set(other.data(),false); return *this; }
	
	const float& operator()(int c, int r) const;
	float& operator()(int c, int r);
	
	const float* data() const {return _data;}
	float* data() {return _data;}
	
	static int columns() {return Cn;}
	static int rows() {return Rn;}
	static int size() {return Cn*Rn;}
	static bool square() {return Cn==Rn;}

protected:

	float _data[Cn*Rn];
};


template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>::Matrix()
{
	for(int c=0;c<Cn;++c)
		for(int r=0;r<Rn;++r) at(c,r) = (c==r)? (Ty)1 : (Ty)0;
}

template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>::Matrix(const Matrix& mat)
{
	set(mat.data(),false);
}

template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>::~Matrix()
{
}

template<int Cn,int Rn, typename Ty>
const float& Matrix<Cn,Rn,Ty>::at(int c, int r) const
{
	return _data[c*Rn+r];
}

template<int Cn,int Rn, typename Ty>
float& Matrix<Cn,Rn,Ty>::at(int c, int r)
{
	return _data[c*Rn+r];
}

template<int Cn,int Rn, typename Ty>
const float& Matrix<Cn,Rn,Ty>::operator()(int c, int r) const
{
	return _data[c*Rn+r];
}

template<int Cn,int Rn, typename Ty>
float& Matrix<Cn,Rn,Ty>::operator()(int c, int r)
{
	return _data[c*Rn+r];
}

template<int Cn,int Rn, typename Ty>
void Matrix<Cn,Rn,Ty>::set(const float* buf, bool transp)
{
	int index=0;
	for(int c=0;c<Cn;++c)
		for(int r=0;r<Rn;++r)
		{
			float& val = transp? at(c,r) : at(r,c);
			val = buf[index++];
		}
}

template<int Cn,int Rn, typename Ty>
void Matrix<Cn,Rn,Ty>::transpose()
{
	Matrix<Cn,Rn,Ty> copy(*this);
	for(int c=0;c<Cn;++c)
		for(int r=0;r<Rn;++r) at(c,r) = copy(r,c);
}

IMPGEARS_END

#endif // IMP_MATRIX_H
