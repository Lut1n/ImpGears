#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/GeoNode.h>
#include <ImpGears/SceneGraph/QuadNode.h>
#include <ImpGears/SceneGraph/RenderPass.h>
#include <ImpGears/Descriptors/ImageIO.h>
#include <ImpGears/Descriptors/FileInfo.h>
#include <ImpGears/Descriptors/JsonImageOp.h>

#include <ImpGears/Renderer/CpuRenderer.h>

#include <ImpGears/Plugins/RenderPlugin.h>

#include <ImpGears/Graphics/ImageOperation.h>

// #include <SFML/Graphics.hpp>

using namespace imp;

// common stuff
#include "../utils/inc_experimental.h"
#define IMPLEMENT_APP_CONTEXT
#include "../utils/AppContext.h"
#define IMPLEMENT_BASIC_GEOMETRIES
#include "../utils/basic_geometries.h"
#include "../utils/basic_skybox.h"

Geometry generateTerrain(const ImageSampler::Ptr& hm)
{
    imp::Geometry geometry = imp::Geometry::cube();
    geometry = geometry.subdivise(10);
    Geometry::intoCCW(geometry);
    geometry.generateColors(Vec3(1.0));
    geometry.generateTexCoords(Geometry::TexGenMode_Cubic,1.0);

    for(int k=0;k<geometry.size();++k)
    {
        Vec3 v = geometry._vertices[k];
        Vec2 uv = Vec2(v[0],v[2])*0.5 + 0.5;
        v[1] *= (hm->get(uv)[0] * 5.0)/2.0;
        geometry._vertices[k] = v;
    }
    geometry.scale(Vec3(4.0,1.0,4.0));
    geometry.generateNormals(Geometry::NormalGenMode_PerFace);
    // geometry.interpolateNormals();

    return geometry;
}

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(ImageSampler::Ptr& sampler, ImageSampler::Ptr& color)
{
    if( !fileExists("./cache/scene_terrain.tga") || !fileExists("./cache/scene_color.tga"))
    {
        std::filesystem::create_directories("cache");
        generateImageFromJson("textures.json");
    }

    sampler =ImageSampler::create();
    sampler->setSource( ImageIO::load("./cache/scene_terrain.tga") );
    sampler->setWrapping(ImageSampler::Wrapping_Repeat);
    sampler->setFiltering(ImageSampler::Filtering_Linear);

    color =ImageSampler::create();
    color->setSource( ImageIO::load("./cache/scene_color.tga") );
    color->setWrapping(ImageSampler::Wrapping_Repeat);
    color->setFiltering(ImageSampler::Filtering_Linear);
}

struct MyContext
{
    Graph::Ptr graph;
    Camera::Ptr camera;
    Node::Ptr light;

    void init(AppContext& app)
    {
        ImageSampler::Ptr sampler, color, emi, normals;
        loadSamplers(sampler,color);
        emi = ImageSampler::create(8, 8, 4, Vec4(1.0));

        ImageSampler::Ptr reflectivityMap = ImageSampler::create(8,8,3,Vec4(1.0));

        normals = ImageSampler::create(128.0,128.0,4,Vec4(0.0,0.0,1.0,1.0));
        normals->setWrapping(ImageSampler::Wrapping_Repeat);
        normals->setFiltering(ImageSampler::Filtering_Linear);
        BumpToNormalOperation op;
        op.setTarget(sampler->getSource());
        op.execute(normals->getSource());

        graph = Graph::create();
        ClearNode::Ptr root = ClearNode::create();

        camera = Camera::create(Vec3(0.0,1.0,0.0));
        camera->setPosition(Vec3(10.0, 10.0, 10.0));
        camera->setTarget(Vec3(0.0, 0.0, 0.0));

        imp::ReflexionModel::Ptr r = imp::ReflexionModel::create(
                imp::ReflexionModel::Lighting_Phong,
                imp::ReflexionModel::Texturing_Samplers_CNE,
                imp::ReflexionModel::MRT_1_Col, "shader 1");

        imp::ReflexionModel::Ptr r2 = imp::ReflexionModel::create(
                imp::ReflexionModel::Lighting_Phong,
                imp::ReflexionModel::Texturing_Samplers_CNE,
                imp::ReflexionModel::MRT_1_Col, "shader 2");

        Material::Ptr material = Material::create(Vec3(0.3,1.0,0.4), 32.0);
        material->_baseColor = color;
        // material->_emissive = color;
        material->_normalmap = normals;
        material->_reflectivity = reflectivityMap;

        Material::Ptr light_material = Material::create(Vec3(1.0), 32.0);
        light_material->_emissive = emi;

        Geometry::Ptr terrain = Geometry::create();
        *terrain = generateTerrain(sampler);
        GeoNode::Ptr terrainNode = GeoNode::create(terrain, false);
        terrainNode->setReflexion(r);
        terrainNode->setMaterial(material);

        Geometry::Ptr base = Geometry::create(); *base = Geometry::cube();
        Geometry::intoCCW(*base);
        base->generateColors(Vec3(1.0));
        base->generateTexCoords(Geometry::TexGenMode_Cubic,1.0);
        base->generateNormals(Geometry::NormalGenMode_PerFace);
        base->scale( Vec3(10.0, 1.0, 10.0) );
        GeoNode::Ptr baseNode = GeoNode::create(base, false);
        baseNode->setReflexion(r);
        baseNode->setMaterial(material);
        baseNode->setPosition( Vec3(0.0,-10.0,0.0) );

        Geometry::Ptr coords = buildCoord();
        coords->scale(Vec3(30.0));
        GeoNode::Ptr coordsNode = GeoNode::create(coords, false);
        coordsNode->setReflexion(r2);
        coordsNode->setMaterial(light_material);
        coordsNode->setPosition(Vec3(0.0,5.0,0.0));
        coordsNode->setScale(Vec3(0.5));

        LightNode::Ptr lightNode = LightNode::create(Vec3(1.0),10.0);

        Geometry::Ptr pointGeo = Geometry::create();
        *pointGeo = Geometry::sphere(2, 0.4);
        pointGeo->setPrimitive(Geometry::Primitive_Triangles);
        pointGeo->generateColors(Vec3(0.3,0.3,1.0));
        pointGeo->generateNormals();
        pointGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
        GeoNode::Ptr pointNode = GeoNode::create(pointGeo, false);
        // pointNode->setPosition(Vec3(0.0,0.0,0.0));
        pointNode->setReflexion(r);
        pointNode->setMaterial(light_material);

        pointNode->addNode(lightNode);
        light = pointNode;

        SkyBox::Ptr sky = SkyBox::create();

        root->addNode(camera);
        root->addNode(terrainNode);
        root->addNode(baseNode);
        root->addNode(coordsNode);
        root->addNode(pointNode);
        if(app.plugin) root->addNode(sky);
        graph->setRoot(root);

        RenderPass::Ptr rp_info = RenderPass::create();
        rp_info->enablePass(RenderPass::Pass_EnvironmentMapping);
        RenderPass::Ptr rp_info2 = RenderPass::create();
        rp_info2->enablePass(RenderPass::Pass_ShadowMapping);
        coordsNode->setRenderPass(rp_info);
        sky->setRenderPass(rp_info);
        pointNode->setRenderPass(rp_info);

        terrainNode->setRenderPass(rp_info2);
        baseNode->setRenderPass(rp_info2);

        graph->getInitState()->setViewport( app.viewport );
        graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));
    }

    void update(double t)
    {
        Vec3 lp(cos(t)*5.0,7.0,sin(t)*5.0);
        light->setPosition(lp);

        Vec3 lp2(cos(-t * 0.2),1.0,sin(-t * 0.2));
        camera->setPosition(lp2*Vec3(10.0,5.0,10.0));
        camera->setTarget(Vec3(0.0f, 0.f, 0.0f));
    }

    void draw()
    {

    }
};

int main(int argc, char* argv[])
{
    AppContext app;
    app.setArgs(argc, argv);

    SceneRenderer::Ptr renderer = app.loadRenderer("Terrain scene");
    // renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    // renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    // renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    // renderer->enableFeature(SceneRenderer::Feature_SSAO, true);
    
    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);

    MyContext* context = new MyContext(); context->init(app);

    while (app.begin())
    {
        context->update(app.elapsedTime());
        renderer->render( context->graph );
        app.end();
    }

    exit(EXIT_SUCCESS);
}
