#ifndef IMP_MATRIX_H
#define IMP_MATRIX_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Core/Vec.h>
#include <ImpGears/Core/Export.h>

IMPGEARS_BEGIN

template<int Cn, int Rn, typename Ty>
class Matrix : public Object
{
	// Column major
	// Prefix notation
	
public:

	Meta_Class(Matrix)

	Matrix();
	template<int Cn2, int Rn2>
	Matrix(const Matrix<Cn2,Rn2,Ty>& mat);
	Matrix(const Ty* buf, bool transp = false) { set(buf,transp); }
	virtual ~Matrix();

	const Ty& at(int c, int r) const;
	Ty& at(int c, int r);
	
	void set(const Ty* buf, bool transp = false);
	
	Matrix<Rn,Cn,Ty> transpose() const;
	
	Matrix operator*(const Matrix& other) const;
	Matrix& operator*=(const Matrix& other);
	
	Matrix operator*(Ty scalar) const;
	Matrix& operator*=(Ty scalar);
	
	template<int Cn2,int Rn2>
	Matrix<Cn,Rn2,Ty> operator*(const Matrix<Cn2,Rn2,Ty>& other) const;

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
template<int Cn2, int Rn2>
Matrix<Cn,Rn,Ty>::Matrix(const Matrix<Cn2,Rn2,Ty>& mat)
{
	if(Cn==Cn2 && Rn==Rn2)
	{
		set(mat.data(),false);
	}
	else
	{
		for(int c=0;c<Cn;++c)
			for(int r=0;r<Rn;++r) 
			{
				if(c>=Cn2 || r>=Rn2)
					at(c,r) = (c==r)? (Ty)1 : (Ty)0;
				else
					at(c,r) = mat(c,r);
			}
	}
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
	if(transp)
	{
		for(int r=0;r<Rn;++r) 
			for(int c=0;c<Cn;++c) at(c,r) = buf[index++];
	}
	else
	{
		for(int c=0;c<Cn;++c)
			for(int r=0;r<Rn;++r) at(c,r) = buf[index++];
	}
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Matrix<Rn,Cn,Ty> Matrix<Cn,Rn,Ty>::transpose() const
{
	Matrix<Rn,Cn,Ty> res;
	for(int c=0;c<Cn;++c)
		for(int r=0;r<Rn;++r) res(r,c) = at(c,r);
	return res;
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
			for(int k=0;k<Cn;++k) at(c,r) += last(c,k) * other(k,r);
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

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
template<int Cn2,int Rn2>
Matrix<Cn,Rn2,Ty> Matrix<Cn,Rn,Ty>::operator*(const Matrix<Cn2,Rn2,Ty>& other) const
{
	Matrix<Cn,Rn2,Ty> res;
	if( Cn2!=Rn ) return res;
	
	for(int c=0; c<Cn; ++c)
		for(int r=0; r<Rn2; ++r)
		{
			res(c,r) = (Ty)0;
			for(int k=0;k<Rn;++k) res(c,r) += at(c,k) * other(k,r);
		}

	return res;
}

//--------------------------------------------------------------
template<int Dim, typename Ty>
Vec<Dim,Ty>& operator*=(Vec<Dim,Ty>& vec, const Matrix<Dim,Dim,Ty>& mat)
{
	Vec<Dim,Ty> last(vec);
	for(int r=0; r<Dim; ++r)
	{
		vec[r] = (Ty)0;
		for(int k=0;k<Dim;++k) vec[r] += last[k] * mat(k,r);
	}
	return vec;
}

//--------------------------------------------------------------
template<int Cn,int Rn, typename Ty>
Vec<Rn,Ty> operator*(const Vec<Cn,Ty>& vec, const Matrix<Cn,Rn,Ty>& mat)
{
	Vec<Rn,Ty> res;
	for(int r=0; r<Rn; ++r)
	{
		res[r] = (Ty)0;
		for(int k=0;k<Cn;++k) res[r] += vec[k] * mat(k,r);
	}
	return res;
}

IMPGEARS_END

#endif // IMP_MATRIX_H
