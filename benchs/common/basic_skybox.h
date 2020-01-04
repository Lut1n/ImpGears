#ifndef BASIC_SKYBOX_H
#define BASIC_SKYBOX_H

//--------------------------------------------------------------
#define GLSL_CODE( code ) #code
//--------------------------------------------------------------
static std::string glsl_env = GLSL_CODE(

uniform samplerCube u_input_cube;
uniform float u_reversed;
uniform mat4 u_view;
uniform vec3 u_lightPos;
varying vec3 v_texCoord;
varying vec3 v_m;

vec4 i_col(vec3 uvw){return texture(u_input_cube, uvw).xyzw;}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    vec3 uvw = v_m;
    // uvw.y *= -1.0;
    uvw = u_reversed>0.5? -uvw : uvw;
    out_color = i_col(uvw);
    out_emissive = i_col(uvw);
    out_normal = vec3(0.0);
    out_reflectivity = 0.0;
    out_shininess = 0.0;
    out_depth = 1.0;
}
);


static std::string skyVert = GLSL_CODE(

// vertex attributes ( @see glBindAttribLocation )
in vec3 a_vertex;   // location 0
in vec3 a_color;    // location 1
in vec3 a_normal;   // location 2
in vec2 a_texcoord; // location 3

// uniforms
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat3 u_normal;

// fragment shader stage input
out vec2 v_texCoord;
out vec3 v_m;
out vec3 v_mv;
out vec3 v_n;
out vec3 v_vertex;
out vec3 v_color;

void main()
{
    vec3 v = normalize(a_vertex) * 1024.0;
    mat4 a_view = u_view;
    // a_view[3].xyz = vec3(0.0,0.0,0.0);
    vec4 mv_pos = a_view * u_model * vec4(v,1.0);
    gl_Position = u_proj * mv_pos;
    gl_Position.z = gl_Position.w; // force depth == 1.0
    v_n = normalize(u_normal * a_normal);
    v_color = a_color;

    v_texCoord = a_texcoord;
    v_m = (u_model * vec4(v,1.0)).xyz;
    v_mv = mv_pos.xyz;
    v_vertex = v;
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
        sources[0] = ImageIO::load("./arrakisday/arrakisday_ft.tga");
        sources[1] = ImageIO::load("./arrakisday/arrakisday_bk.tga");
        sources[2] = ImageIO::load("./arrakisday/arrakisday_up.tga");
        sources[3] = ImageIO::load("./arrakisday/arrakisday_dn.tga");
        sources[4] = ImageIO::load("./arrakisday/arrakisday_rt.tga");
        sources[5] = ImageIO::load("./arrakisday/arrakisday_lf.tga");
        _cubemap = CubeMapSampler::create(sources);

        _shader = ReflexionModel::create(
                    ReflexionModel::Lighting_Customized,
                    ReflexionModel::Texturing_Samplers_CNE,
                    ReflexionModel::MRT_2_Col_Emi);
        _shader->_fragCode_lighting = glsl_env;
        _shader->_vertCode = skyVert;

        setReflexion(_shader);
        getState()->setUniform("u_input_cube", _cubemap, 3);
        float reversed = 0.0;
        getState()->setUniform("u_reversed", reversed);
    }

    SkyBox( const std::vector<std::string>& filenames )
        : GeoNode(Geometry::create(), false)
    {
        *_geo = Geometry::cube();
        Geometry::intoCCW(*_geo, false);
        _geo->scale(Vec3(50.0));
        _geo->generateNormals(Geometry::NormalGenMode_PerFace);
        _geo->generateColors(Vec3(1.0));
        _geo->generateTexCoords();

        std::vector<Image::Ptr> sources(6);
        for(int i=0; i<6;++i) sources[i] = ImageIO::load( filenames[i] );
        _cubemap = CubeMapSampler::create(sources);

        _shader = ReflexionModel::create(
                    ReflexionModel::Lighting_Customized,
                    ReflexionModel::Texturing_Samplers_CNE,
                    ReflexionModel::MRT_2_Col_Emi);
        _shader->_fragCode_lighting = glsl_env;
        _shader->_vertCode = skyVert;

        setReflexion(_shader);
        getState()->setUniform("u_input_cube", _cubemap, 3);
        float reversed = 0.0;
        getState()->setUniform("u_reversed", reversed);
    }

    virtual ~SkyBox(){}
};

#endif //BASIC_GEOMETRIES_H
