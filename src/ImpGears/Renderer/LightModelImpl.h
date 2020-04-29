#include <ImpGears/SceneGraph/ReflexionModel.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
struct NoLighting : public ReflexionModel::LightingCallback
{
    Meta_Class(NoLighting)

    virtual void applyLighting(const UniformMap& uniforms, Varyings& varyings, ReflexionModel::TexturingCallback::Ptr& texturing, std::vector<Vec3>& outColor)
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
struct PhongLighting : public ReflexionModel::LightingCallback
{
    Meta_Class(PhongLighting)

    virtual void applyLighting(const UniformMap& uniforms, Varyings& varyings, ReflexionModel::TexturingCallback::Ptr& texturing, std::vector<Vec3>& outColor)
    {
        const Matrix4& view = uniforms.getMat4("u_view");
        const Vec3& lightPos = uniforms.getVec3("u_lightPos");
        // const Vec3& lightCol = uniforms.getVec3("u_lightCol");
        const Vec3& lightAtt = uniforms.getVec3("u_lightAtt");
        float lightPower = lightAtt[0];
        float shininess = lightAtt[1];

        Vec2 tex = Vec2(varyings.get("texUV"));
        Vec3 color = texturing->textureColor(tex,uniforms,varyings);
        color *= varyings.get("color") * uniforms.getVec3("u_color");
        Vec3 frag_pos = varyings.get("m_vert");
        Vec3 normal = varyings.get("normal");
        Vec3 light_dir = lightPos - frag_pos;
        float distance = light_dir.length();
        distance = distance * distance;
        light_dir.normalize();

        // TBN Matrix for computation in tangent space
        Vec3 n_z = normal;
        Vec3 n_x = Vec3::X, n_x2 = Vec3::Y;
        Vec3 n_y = n_z.cross(n_x);
        Vec3 n_y2 = n_z.cross(n_x2);
        if(n_y2.length2() > n_y.length2()) n_y = n_y2;

        n_y.normalize();
        n_x = n_y.cross(n_z); n_x.normalize();
        Matrix3 inv_tbn(n_x,n_y,n_z); inv_tbn = inv_tbn.inverse();

        Vec3 texnormal = texturing->textureNormal(tex,uniforms,varyings);

        light_dir = light_dir * inv_tbn;
        float lambertian = std::max(light_dir.dot(texnormal),0.f);
        float specular = 0.0;

        if(lambertian > 0.0)
        {
            // view dir
            Vec3 view_pos = Vec3(-view(3,0),-view(3,1),-view(3,2));
            Vec3 view_dir = (view_pos - frag_pos) * inv_tbn;
            view_dir.normalize();

            // blinn phong
            Vec3 halfDir = light_dir + view_dir; halfDir.normalize();
            float specAngle = std::max( halfDir.dot(texnormal), 0.f );
            specular = std::pow(specAngle, shininess);
        }

        // Color model :
        // ambiantColor
        // + diffColor * lambertian * lightColor * lightPower / distance
        // + specColor * specular * lightColor * lightPower / distance

        float intensity = lightPower/distance;
        float ambient = 0.02;
        Vec3 colModelRes = color*(ambient+(lambertian+specular)*intensity);

        Vec3 texemi = texturing->textureEmissive(tex,uniforms,varyings);
        colModelRes = dotMax(texemi,colModelRes);
        outColor[0] = colModelRes;
        outColor[1] = texemi;
    }
};

//--------------------------------------------------------------
struct PlainColorCb : public ReflexionModel::TexturingCallback
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
struct SamplerCbCN : public ReflexionModel::TexturingCallback
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
struct SamplerCbCNE : public ReflexionModel::TexturingCallback
{
    Meta_Class(SamplerCbCNE)

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
struct Mrt1ColorCb : public ReflexionModel::MRTCallback
{
    Meta_Class(Mrt1ColorCb)

    virtual void applyMRT(ImageBuf& targets, const Vec3& pt, const std::vector<Vec3>& outColor)
    {
        Vec4 px(outColor[0],1.0); px = dotClamp( px );
        targets[0]->setPixel(pt[0], pt[1], px * 255.0 );
    }
};

//--------------------------------------------------------------
struct Mrt2ColorEmiCb : public ReflexionModel::MRTCallback
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

    virtual void apply(const Vec3& vert, Attributes& att)
    {
        const Matrix4& model = _uniforms->getMat4("u_model");
        const Matrix4& view = _uniforms->getMat4("u_view");
        const Matrix4& proj = _uniforms->getMat4("u_proj");
        const Matrix3& normalMat = _uniforms->getMat3("u_normal");

        Vec4 vertex = vert;
        Vec4 mvertex = vertex * model;
        Vec4 mvvertex = mvertex * view;
        Vec4 mvpvertex = mvvertex * proj;
        Vec3 normal = att.normal * normalMat;

        mvertex /= mvertex.w();
        mvvertex /= mvvertex.w();
        mvpvertex /= mvpvertex.w();

        _varyings->set("vert",vertex);
        _varyings->set("m_vert",mvertex);
        _varyings->set("mv_vert",mvvertex);
        _varyings->set("mvp_vert",mvpvertex);
        _varyings->set("color",att.color);
        _varyings->set("normal",normal);
        _varyings->set("texUV",att.texUV);
    }
};

IMPGEARS_END
