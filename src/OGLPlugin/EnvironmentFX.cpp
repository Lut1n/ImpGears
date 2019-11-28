#include <OGLPlugin/EnvironmentFX.h>
#include <OGLPlugin/GlRenderer.h>

#include <SceneGraph/QuadNode.h>
#include <Descriptors/ImageIO.h>

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
    out_emissive = vec4(0.0,0.0,0.0,1.0);
    out_lighting = i_col(-v_m);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
EnvironmentFX::EnvironmentFX()
{

}

//--------------------------------------------------------------
EnvironmentFX::~EnvironmentFX()
{

}

//--------------------------------------------------------------
void EnvironmentFX::setup()
{
    std::vector<Image::Ptr> sources(6);
    sources[0] = ImageIO::load("./arrakisday/arrakisday_rt.tga");
    sources[1] = ImageIO::load("./arrakisday/arrakisday_lf.tga");
    sources[2] = ImageIO::load("./arrakisday/arrakisday_dn.tga");
    sources[3] = ImageIO::load("./arrakisday/arrakisday_up.tga");
    sources[4] = ImageIO::load("./arrakisday/arrakisday_bk.tga");
    sources[5] = ImageIO::load("./arrakisday/arrakisday_ft.tga");

    sampler = CubeMapSampler::create(sources);

    Geometry cube = Geometry::cube();
    Geometry::intoCCW(cube);
    cube.scale(Vec3(50.0));
    cube.generateNormals(Geometry::NormalGenMode_PerFace);
    cube.generateColors(Vec3(1.0));
    cube.generateTexCoords();

    _shader = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi);
    _shader->_fragCode_lighting = glsl_env;

    Geometry::Ptr geoptr = Geometry::create(); *geoptr = cube;
    node = GeoNode::create(geoptr);
    node->setReflexion(_shader);
    node->getState()->setUniform("u_input_cube", sampler, 3);
}


//--------------------------------------------------------------
Graph::Ptr EnvironmentFX::begin(GlRenderer* renderer, const Graph::Ptr& scene)
{
    Graph::Ptr cloned = Graph::create();
    cloned->getInitState()->clone(scene->getInitState());
    Node::Ptr root = node;
    cloned->setRoot(root);
    root->addNode(scene->getRoot());

    return cloned;
}

//--------------------------------------------------------------
void EnvironmentFX::end(GlRenderer* renderer, const Graph::Ptr& scene)
{
    Node::Ptr root = node;
    root->remNode(scene->getRoot());
}

IMPGEARS_END
