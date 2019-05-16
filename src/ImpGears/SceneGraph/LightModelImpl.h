#include <SceneGraph/LightModel.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
struct NoLighting : public LightModel::LightingCallback
{
	Meta_Class(NoLighting)
	
	virtual void applyLighting(const UniformMap& uniforms, Varyings& varyings, LightModel::TexturingCallback::Ptr& texturing, std::vector<Vec3>& outColor)
    {
		Vec2 tex = Vec2(varyings.get("texUV"));
		Vec3 color = texturing->textureColor(tex,uniforms,varyings) * varyings.get("color") * uniforms.getVec3("u_color");
		Vec3 texemi = texturing->textureEmissive(tex,uniforms,varyings);
		color = dotMax(texemi,color);
		outColor[0] = color;
		outColor[1] = texemi;
    }
};

//--------------------------------------------------------------
struct PhongLighting : public LightModel::LightingCallback
{
	Meta_Class(PhongLighting)
	
	virtual void applyLighting(const UniformMap& uniforms, Varyings& varyings, LightModel::TexturingCallback::Ptr& texturing, std::vector<Vec3>& outColor)
    {
		const Matrix4& view = uniforms.getMat4("u_view");
		const Vec3& lightPos = uniforms.getVec3("u_lightPos");
		const Vec3& lightCol = uniforms.getVec3("u_lightCol");
		const Vec3& lightAtt = uniforms.getVec3("u_lightAtt");
		float lightPower = lightAtt[0];
		float shininess = lightAtt[1];
		
		Vec2 tex = Vec2(varyings.get("texUV"));
		Vec3 color = texturing->textureColor(tex,uniforms,varyings) * varyings.get("color") * uniforms.getVec3("u_color");
		Vec3 frag_pos = varyings.get("m_vert");
		Vec3 normal = varyings.get("normal");
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
		
		float lambertian = std::max(light_dir.dot(normal),0.f);
		float specular = 0.0;
		
		if(lambertian > 0.0)
		{
			// view dir
			Vec3 view_pos = Vec3(-view(3,0),-view(3,1),-view(3,2)) * tbn;
			Vec3 view_dir = view_pos - frag_pos;
			view_dir.normalize();
			
			// blinn phong
			light_dir = light_dir * tbn; 
			Vec3 halfDir = light_dir + view_dir;
			Vec3 texnormal = texturing->textureNormal(tex,uniforms,varyings);
			float specAngle = std::max( halfDir.dot(texnormal), 0.f );
			specular = std::pow(specAngle, shininess);
		}
		
		// Color model :
		// ambiantColor
		// + diffColor * lambertian * lightColor * lightPower / distance
		// + specColor * specular * lightColor * lightPower / distance
		
		Vec3 colModelRes = color*0.01
		+ color*0.7 * lambertian * lightCol * lightPower / distance
		+ color*0.3 * specular * lightCol * lightPower / distance;

		Vec3 texemi = texturing->textureEmissive(tex,uniforms,varyings);
		colModelRes = dotMax(texemi,colModelRes);
		outColor[0] = colModelRes;
		outColor[1] = texemi;
    }
};

//--------------------------------------------------------------
struct PlainColorCb : public LightModel::TexturingCallback
{
	Meta_Class(PlainColorCb)
	
	virtual Vec3 textureColor(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		return Vec3(1.0);
	}
	virtual Vec3 textureNormal(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		return Vec3(0.0,0.0,1.0);
	}
	virtual Vec3 textureEmissive(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		return Vec3(0.0);
	}
};

//--------------------------------------------------------------
struct SamplerCbCN : public LightModel::TexturingCallback
{
	Meta_Class(SamplerCbCN)
	
	Vec3 trySample(const Vec2& uv, const UniformMap& uniforms, const std::string& sname)
	{
		ImageSampler::Ptr spl = uniforms.getSampler(sname);
		if(spl) return Vec3(spl->get(uv));
		return Vec3(1.0);
	}
	
	virtual Vec3 textureColor(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		ImageSampler::Ptr spl = uniforms.getSampler("u_sampler_color");
		if(spl) return Vec3(spl->get(uv));
		return Vec3(1.0);
	}
	virtual Vec3 textureNormal(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		ImageSampler::Ptr spl = uniforms.getSampler("u_sampler_normal");
		if(spl) return Vec3(spl->get(uv)) * 2.0 - 1.0;
		return Vec3(0.0,0.0,1.0);
	}
	virtual Vec3 textureEmissive(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		return Vec3(0.0);
	}
};

//--------------------------------------------------------------
struct SamplerCbCNE : public LightModel::TexturingCallback
{
	Meta_Class(SamplerCbCN)
	
	virtual Vec3 textureColor(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		ImageSampler::Ptr spl = uniforms.getSampler("u_sampler_color");
		if(spl) return Vec3(spl->get(uv));
		return Vec3(1.0);
	}
	virtual Vec3 textureNormal(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		ImageSampler::Ptr spl = uniforms.getSampler("u_sampler_normal");
		if(spl) return Vec3(spl->get(uv)) * 2.0 - 1.0;
		return Vec3(0.0,0.0,1.0);
	}
	virtual Vec3 textureEmissive(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		ImageSampler::Ptr spl = uniforms.getSampler("u_sampler_emissive");
		if(spl) return Vec3(spl->get(uv));
		return Vec3(0.0);
	}
};

//--------------------------------------------------------------
struct Mrt1ColorCb : public LightModel::MRTCallback
{
	Meta_Class(Mrt1ColorCb)
	
	virtual void applyMRT(ImageBuf& targets, const Vec3& pt, const std::vector<Vec3>& outColor)
	{
		Vec4 px(outColor[0],1.0); px = dotClamp( px );
		targets[0]->setPixel(pt[0], pt[1], px * 255.0 );
	}
};

//--------------------------------------------------------------
struct Mrt2ColorEmiCb : public LightModel::MRTCallback
{
	Meta_Class(Mrt2ColorEmiCb)
	
	virtual void applyMRT(ImageBuf& targets, const Vec3& pt, const std::vector<Vec3>& outColor)
	{
		Vec4 px(outColor[0],1.0); px = dotClamp( px );
		Vec4 emi(outColor[1],1.0); emi = dotClamp( emi );
		targets[0]->setPixel(pt[0], pt[1], px * 255.0 );
		targets[1]->setPixel(pt[0], pt[1], emi * 255.0 );
	}
};

//--------------------------------------------------------------
struct DefaultVertCb : public GeometryRenderer::VertCallback
{
	Meta_Class(DefaultVertCb)

	virtual void exec(const Vec3& vert, GeometryRenderer::Attributes& att, const UniformMap& uniforms, Varyings& out_varyings)
	{
		const Matrix4& model = uniforms.getMat4("u_model");
		const Matrix4& view = uniforms.getMat4("u_view");
		const Matrix4& proj = uniforms.getMat4("u_proj");
		const Matrix3& normalMat = uniforms.getMat3("u_normal");
		
		Vec4 vertex = vert;
		Vec4 mvertex = vertex * model;
		Vec4 mvvertex = mvertex * view;
		Vec4 mvpvertex = mvvertex * proj;
		Vec3 normal = att.normal * normalMat;
		
		mvertex /= mvertex.w();
		mvvertex /= mvvertex.w();
		mvpvertex /= mvpvertex.w();
		
		out_varyings.set("vert",vertex);
		out_varyings.set("m_vert",mvertex);
		out_varyings.set("mv_vert",mvvertex);
		out_varyings.set("mvp_vert",mvpvertex);
		out_varyings.set("color",att.color);
		out_varyings.set("normal",normal);
		out_varyings.set("texUV",att.texUV);
	}
};

IMPGEARS_END
