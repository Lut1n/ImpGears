#ifndef BASIC_SKYBOX_H
#define BASIC_SKYBOX_H

//--------------------------------------------------------------
#define GLSL_CODE( code ) #code
//--------------------------------------------------------------
static std::string glsl_env = GLSL_CODE(

uniform samplerCube u_input_cube;
uniform mat4 u_view;
uniform vec3 u_lightPos;
varying vec3 v_texCoord;
varying vec3 v_m;

vec4 i_col(vec3 uvw){return texture(u_input_cube, uvw).xyzw;}

void lighting(out vec4 out_lighting,out vec4 out_emissive,out vec3 out_normal,out float out_metalness,out float out_depth)
{
    out_emissive = i_col(-v_m);
    out_lighting = i_col(-v_m);
}
);

struct SkyBox : public GeoNode
{
public:

    Meta_Class(SkyBox)

    CubeMapSampler::Ptr _cubemap;
    ReflexionModel::Ptr _shader;

    SkyBox() : GeoNode(Geometry::create(), false)
    {
        *_geo = Geometry::cube();
        Geometry::intoCCW(*_geo, false);
        _geo->scale(Vec3(50.0));
        _geo->generateNormals(Geometry::NormalGenMode_PerFace);
        _geo->generateColors(Vec3(1.0));
        _geo->generateTexCoords();

        std::vector<Image::Ptr> sources(6);
        sources[0] = ImageIO::load("./arrakisday/arrakisday_rt.tga");
        sources[1] = ImageIO::load("./arrakisday/arrakisday_lf.tga");
        sources[2] = ImageIO::load("./arrakisday/arrakisday_dn.tga");
        sources[3] = ImageIO::load("./arrakisday/arrakisday_up.tga");
        sources[4] = ImageIO::load("./arrakisday/arrakisday_bk.tga");
        sources[5] = ImageIO::load("./arrakisday/arrakisday_ft.tga");
        _cubemap = CubeMapSampler::create(sources);

        _shader = ReflexionModel::create(
                    ReflexionModel::Lighting_Customized,
                    ReflexionModel::Texturing_Samplers_CNE,
                    ReflexionModel::MRT_2_Col_Emi);
        _shader->_fragCode_lighting = glsl_env;

        setReflexion(_shader);
        getState()->setUniform("u_input_cube", _cubemap, 3);
    }

    virtual ~SkyBox(){}
};

#endif //BASIC_GEOMETRIES_H
