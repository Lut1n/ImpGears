#include <Graphics/CpuBlinnPhong.h>
#include <Core/Matrix3.h>

IMPGEARS_BEGIN

void CpuBlinnPhong::exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms)
{
	const Matrix4& view = cu.at("u_view")->getMat4();
	const Vec3& lightPos = cu.at("u_lightPos")->getFloat3();
	const Vec3& lightCol = cu.at("u_lightCol")->getFloat3();
	const Vec3& lightAtt = cu.at("u_lightAtt")->getFloat3();
	float lightPower = lightAtt[0];
	float shininess = lightAtt[1];
	
	
	TextureSampler::Ptr normal_spl = cu.at("u_nsampler")->getSampler();
	TextureSampler::Ptr color_spl = cu.at("u_sampler")->getSampler();
	
	Vec3 tex = uniforms->get("texUV");
	Vec3 color = uniforms->get("color");
	Vec3 frag_pos = uniforms->get("m_vert");
	Vec3 normal = uniforms->get("normal");
	Vec3 light_dir = lightPos - frag_pos;
	float distance = light_dir.length();
	distance = distance * distance;
	light_dir.normalize();
	
	// TBN Matrix for computation in tangent space
	Vec3 n_z = normal;
	Vec3 n_x = Vec3::X;
	Vec3 n_y = n_z.cross(n_x); n_y.normalize();
	n_x = n_y.cross(n_z); n_x.normalize();
	Matrix3 tbn(n_x,n_y,n_z); tbn = tbn.inverse();
	
	if(normal_spl)
	{
		Vec4 frag_n = normal_spl->get(tex);
		normal = Vec3(frag_n[0],frag_n[1],frag_n[2]) * 2.0 - 1.0; /*normal[2] = 2.0;*/ // normal.normalize();
	}
	
	float lambertian = std::max(light_dir.dot(normal),0.f);
	float specular = 0.0;
	
	if(lambertian > 0.0)
	{
		
		// view dir
		Vec3 view_pos = Vec3(view(3,0),view(3,1),view(3,2)) * tbn;
		Vec3 view_dir = view_pos - frag_pos;
		view_dir.normalize();
		
		// blinn phong
		light_dir = light_dir * tbn; 
		Vec3 halfDir = light_dir + view_dir;
		float specAngle = std::max( halfDir.dot(normal), 0.f );
		specular = std::pow(specAngle, shininess);
	}
	
	// Color model :
	// ambiantColor
	// + diffColor * lambertian * lightColor * lightPower / distance
	// + specColor * specular * lightColor * lightPower / distance
	
	if(color_spl)
	{
		Vec4 frag_c = color_spl->get(tex);
		color *= Vec3(frag_c[0],frag_c[1],frag_c[2]);
	}
	
	Vec3 colModelRes = color*0.01
	+ color*0.7 * lambertian * lightCol * lightPower / distance
	+ color*0.3 * specular * lightCol * lightPower / distance;
	
	Vec4 px(colModelRes,1.0); px = dotClamp( px );
	targets[0]->setPixel(pt[0], pt[1], px * 255.0 );
}

IMPGEARS_END
