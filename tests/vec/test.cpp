#include <Core/Vec4.h>
#include <Core/Vec3.h>

int main(int argc, char* argv[])
{
	float f = 255.0;
	
	imp::Vec4 v(1.0,0.25,0.75,0.5);
	
	v = v * f;
	
	std::cout << "vec4 " << v[0] << "; " << v[1] << "; " << v[2] << "; " << v[3] << std::endl;
	
	imp::Vec<2,float> v2(v);
	
	imp::Vec<5,float> v5(v);
	
	
	std::cout << "vec2 " << v2[0] << "; " << v2[1] << std::endl;
	
	std::cout << "vec5 " << v5[0] << "; " << v5[1] << "; " << v5[2] << "; " << v5[3] << "; " << v5[4] << std::endl;
	
	{
		//imp::Vec<5,float> v5;
		//std::cout << "vec5 " << v[0] << "; " << v[1] << "; " << v[2] << "; " << v[3] << "; " << v[4] << std::endl;
		//v5.set(v);
		//std::cout << "vec5 " << v[0] << "; " << v[1] << "; " << v[2] << "; " << v[3] << "; " << v[4] << std::endl;
	}
	
	std::cout << "sizeof(std::vector<char>) =" << sizeof(std::vector<char>) << std::endl;
	std::cout << "sizeof(std::vector<short>) =" << sizeof(std::vector<short>) << std::endl;
	std::cout << "sizeof(std::vector<int>) =" << sizeof(std::vector<int>) << std::endl;
	
	imp::Vec3 rotated(10.0,5.0,7.0);
	rotated*=imp::Matrix3::rotationZ(90.0 * 3.141592 / 180.0);
	
	std::cout << "rot " << rotated[0] << "; " << rotated[1] << "; " << rotated[2] << std::endl;
	
	return 0;
}
