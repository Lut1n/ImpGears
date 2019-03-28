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
	Matrix(const Ty* buf, bool transp = false) { set(buf,transp); }
	virtual ~Matrix();

	const Ty& at(int c, int r) const;
	Ty& at(int c, int r);
	
	void set(const Ty* buf, bool transp = false);
	
	void transpose();
	
	Matrix operator*(const Matrix& other) const;
	Matrix& operator*=(const Matrix& other);
	
	Matrix operator*(Ty scalar) const;
	Matrix& operator*=(Ty scalar);
	
	// template<int Cn2,int Rn2>
	// Matrix<Cn,Rn2,Ty> operator*(const Matrix<Cn2,Rn2,Ty>& other) const;

	const Matrix& operator=(const Matrix& other){ set(other.data(),false); return *this; }
	
	const Ty& operator()(int c, int r) const {return at(c,r);}
	Ty& operator()(int c, int r) {return at(c,r);}
	
	const Ty* data() const {return _data;}
	Ty* data() {return _data;}
	
	static int columns() {return Cn;}
	static int rows() {return Rn;}
	static int size() {return Cn*Rn;}
	static bool square() {return Cn==Rn;}

protected:

	Ty _data[Cn*Rn];
};


//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>::Matrix()
{
	for(int c=0;c<Cn;++c)
		for(int r=0;r<Rn;++r) at(c,r) = (c==r)? (Ty)1 : (Ty)0;
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>::Matrix(const Matrix& mat)
{
	set(mat.data(),false);
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>::~Matrix()
{
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
const Ty& Matrix<Cn,Rn,Ty>::at(int c, int r) const
{
	return _data[c*Rn+r];
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Ty& Matrix<Cn,Rn,Ty>::at(int c, int r)
{
	return _data[c*Rn+r];
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
void Matrix<Cn,Rn,Ty>::set(const Ty* buf, bool transp)
{
	int index=0;
	for(int c=0;c<Cn;++c)
		for(int r=0;r<Rn;++r)
		{
			Ty& val = transp? at(r,c) : at(c,r);
			val = buf[index++];
		}
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
void Matrix<Cn,Rn,Ty>::transpose()
{
	Matrix<Cn,Rn,Ty> copy(*this);
	for(int c=0;c<Cn;++c)
		for(int r=0;r<Rn;++r) at(c,r) = copy(r,c);
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty> Matrix<Cn,Rn,Ty>::operator*(const Matrix& other) const
{
	return Matrix<Cn,Rn,Ty>(*this) *= other;
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>& Matrix<Cn,Rn,Ty>::operator*=(const Matrix& other)
{
	Matrix<Cn,Rn,Ty> last(*this);
	for(int c=0; c<Cn; ++c)
		for(int r=0; r<Rn; ++r)
		{
			at(c,r) = 0.0;
			for(int k=0;k<Cn;++k) at(c,r) += last(k,r) * other(c,k);
		}
	return *this;
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty> Matrix<Cn,Rn,Ty>::operator*(Ty scalar) const
{
	return Matrix<Cn,Rn,Ty>(*this) *= scalar;
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Cn,Rn,Ty>& Matrix<Cn,Rn,Ty>::operator*=(Ty scalar)
{
	Matrix<Cn,Rn,Ty> last(*this);
	for(int c=0; c<Cn; ++c)
		for(int r=0; r<Rn; ++r) at(c,r) *= scalar;
	return *this;
}

IMPGEARS_END

#endif // IMP_MATRIX_H
