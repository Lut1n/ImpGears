#include <ImpGears/Core/Matrix3.h>
#include <ImpGears/Core/Matrix4.h>

using namespace imp;

template<int D>
void round(Matrix<D,D,float>& mat, float precision = 0.001)
{
	for(int i=0;i<D;++i) for(int j=0;j<D;++j)
	{
		int integ = mat(i,j) / precision;
		mat(i,j) = float(integ) * precision;
	}
}

template<int D>
void print(const Matrix<D,D,float>& mat, std::string name = "mat3")
{
	std::cout << name << " : " << std::endl;
	
	for(int i=0;i<D;++i)
	{
		std::cout << "| ";
		for(int j=0;j<D;++j) std::cout << mat(j,i) << ", ";
		std::cout << "|" << std::endl;
	}
}

int main(int argc,char* argv[])
{
	using TestedMat = Matrix4;
	
	// TestedMat mat = TestedMat::rotationXYZ( Vec3(1.0,2.0,3.0) );
	// TestedMat mat = TestedMat::scale( Vec3(1.0,2.0,3.0) );
	// TestedMat mat = TestedMat::translation( Vec3(1.0,2.0,3.0) );
	// TestedMat mat = TestedMat::perspectiveProj(60.0, 3.0/4.0, 0.1, 100.0);	// not good
	// TestedMat mat = TestedMat::orthographicProj(-5.0, 10.0, -7.0, 24.0, 0.01, 128.0);
	// TestedMat mat = TestedMat::orthographicProj(620.0, 480.0, 0.1, 100.0);
	TestedMat mat = TestedMat::view(Vec3(1.0,2.0,3.0), Vec3(8.3,1.8,4.37), Vec3(0.0,0.0,1.0));
	TestedMat imat = mat.inverse();
	
	TestedMat one = mat * imat;
	round(one);
	
	print(mat,"mat");
	print(imat,"inv(mat)");
	print(one,"mat * inv(mat)");
	
	return 0;
}

