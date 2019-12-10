#include <OGLPlugin/GlRenderer.h>
#include <OGLPlugin/CubeMap.h>

#include <cstdlib>

#include <OGLPlugin/BloomFX.h>
#include <OGLPlugin/EnvironmentFX.h>
#include <OGLPlugin/LightingModel.h>
#include <OGLPlugin/CopyFrame.h>
#include <OGLPlugin/BlendAll.h>
#include <OGLPlugin/RenderToCubeMap.h>
#include <OGLPlugin/ShadowCasting.h>

#define FRAMEOP_ID_ENVFX 0
#define FRAMEOP_ID_SHAMIX 1
#define FRAMEOP_ID_COLORMIX 2
#define FRAMEOP_ID_ENVMIX 3
#define FRAMEOP_ID_BLOOMMIX 4
#define FRAMEOP_ID_SHAFX 5
#define FRAMEOP_ID_PHONG 6
#define FRAMEOP_ID_COPY 7
#define FRAMEOP_ID_BLOOM 8

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_shadow_depth = GLSL_CODE(
varying vec3 v_mv;

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    float near = 0.1; float far = 128.0;
    float depth = (length(v_mv.xyz) - near) / far;
    out_color = vec4(vec3(depth),1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
GlRenderer::GlRenderer()
    : SceneRenderer()
{
}

//--------------------------------------------------------------
GlRenderer::~GlRenderer()
{
}

//--------------------------------------------------------------
void GlRenderer::loadRenderPlugin(const std::string& renderPlugin)
{
    /*_renderPlugin = PluginManager::open(renderPlugin);
    if(_renderPlugin == nullptr)
    {
        return;
    }
    _renderPlugin->init();*/
}

//---------------------------------------------------------------
void GlRenderer::setRenderPlugin(RenderPlugin* plugin)
{
    _renderPlugin = plugin;
    if(_renderPlugin != nullptr) _renderPlugin->init();
}

//---------------------------------------------------------------
Image::Ptr GlRenderer::getTarget(bool dlFromGPU, int id)
{
    if(id <0 || id>=(int)_targets->count()) return nullptr;

    ImageSampler::Ptr sampler = _targets->get(id);
    if(dlFromGPU)
    {
        _renderPlugin->bringBack( sampler );
    }

    return sampler->getSource();
}

//---------------------------------------------------------------
RenderQueue::Ptr GlRenderer::applyRenderVisitor(const Graph::Ptr& scene)
{
    Visitor::Ptr visitor = _visitor;
    _visitor->reset();
    scene->accept(visitor);

    RenderQueue::Ptr queue = _visitor->getQueue();
    return queue;
}


//---------------------------------------------------------------
void GlRenderer::drawQueue( RenderQueue::Ptr& queue, State::Ptr overrideState,
                            SceneRenderer::RenderFrame renderPass )
{
    State::Ptr local_state = State::create();
    Matrix4 view;
    if(queue->_camera) view = queue->_camera->getViewMatrix();
    // if(overrideCamera) view = overrideCamera->getViewMatrix();

    for(int i=0;i<(int)queue->_nodes.size();++i)
    {
        State::Ptr state = queue->_states[i];
        local_state->clone(state, State::CloneOpt_All);
        if( overrideState ) local_state->clone( overrideState, State::CloneOpt_IfChanged );

        GeoNode* geode = dynamic_cast<GeoNode*>( queue->_nodes[i] );
        ClearNode* clear = dynamic_cast<ClearNode*>( queue->_nodes[i] );

        if(geode)
        {
            Material::Ptr mat = geode->_material;
            float shininess = mat->_shininess;
            Vec3 color = mat->_color;

            if(renderPass==SceneRenderer::RenderFrame_Default || renderPass==SceneRenderer::RenderFrame_Environment)
            {
                if(mat->_baseColor)
                        state->setUniform("u_sampler_color", mat->_baseColor, 0);
                if(mat->_normalmap)
                        state->setUniform("u_sampler_normal", mat->_normalmap, 1);
                if(mat->_emissive)
                        state->setUniform("u_sampler_emissive", mat->_emissive, 2);
                if(mat->_reflectivity)
                        state->setUniform("u_sampler_reflectivity", mat->_reflectivity, 3);

                queue->_states[i]->setUniform("u_color", color);
                queue->_states[i]->setUniform("u_shininess", shininess);
            }


            // ---- if we want a normal matrix for view space instead of model space ----
            // Matrix4 model = state->getUniforms()["u_model"]->getMat4();
            // Matrix3 normalMat = Matrix3(model * view).inverse().transpose();
            // state->setUniform("u_normal", normalMat );
            state->setUniform("u_view", view);


            applyState(state, renderPass);
            if( renderPass==SceneRenderer::RenderFrame_ShadowMap ) _renderPlugin->setCulling(2);

            RenderPass::Ptr renderPass_info = geode->getState()->getRenderPass();

            RenderPass::PassFlag expectedFlag = RenderPass::Pass_DefaultMRT;
            if( renderPass==SceneRenderer::RenderFrame_Environment )
                expectedFlag = RenderPass::Pass_EnvironmentMapping;
            if( renderPass==SceneRenderer::RenderFrame_ShadowMap )
                expectedFlag = RenderPass::Pass_ShadowMapping;

            if(renderPass_info->isPassEnabled(expectedFlag)) drawGeometry(geode);
        }
        else if(clear)
        {
            applyClear(clear, renderPass);
        }
    }

    // _renderPlugin->unbind();
}

//---------------------------------------------------------------
void GlRenderer::render(const Graph::Ptr& scene)
{
    if(_renderPlugin == nullptr) return;

    if(_internalFrames == nullptr)
    {
        int mrt_size = 6;
        _internalFrames = RenderTarget::create();
        _internalFrames->build(1024.0,1024.0,mrt_size,true);
        _internalFrames->setClearColor(2, Vec4(0.0));   // normals
        _internalFrames->setClearColor(3, Vec4(0.0));   // reflectivity
        _internalFrames->setClearColor(5, Vec4(1.0));   // depth
    }


    _renderPlugin->init(_internalFrames);
    _renderPlugin->bind(_internalFrames);
    _internalFrames->change();


    RenderQueue::Ptr queue = applyRenderVisitor(scene);
    drawQueue(queue);
    const Camera* camera = queue->_camera;



    static RenderToCubeMap::Ptr environment_renderer;
    static RenderToCubeMap::Ptr shadow_renderer;
    static CubeMapSampler::Ptr environment, shadows;
    static ReflexionModel::Ptr shadows_shader;

    if(isFeatureEnabled(Feature_Environment))
    {
        if(environment_renderer == nullptr)
        {
            environment = CubeMapSampler::create(1024.0,1024.0,4,Vec4(1.0));
            environment_renderer = RenderToCubeMap::create(this);
            environment_renderer->setCubeMap(environment);
        }

        environment_renderer->render(scene, Vec3(0.0), SceneRenderer::RenderFrame_Environment); // todo : position as parameter
    }

    if(isFeatureEnabled(Feature_Shadow))
    {
        if(shadow_renderer == nullptr)
        {
            // shadows = CubeMapSampler::create(1024.0,1024.0,4,Vec4(1.0));
            shadows = CubeMapSampler::create(1024,1024.0,4,Vec4(1.0));
            shadow_renderer = RenderToCubeMap::create(this);
            shadow_renderer->setCubeMap(shadows);
            shadows_shader = ReflexionModel::create(
                        ReflexionModel::Lighting_Customized,
                        ReflexionModel::Texturing_Samplers_CNE,
                        ReflexionModel::MRT_2_Col_Emi,
                        "glsl_shadow_depth");
            shadows_shader->_fragCode_lighting = glsl_shadow_depth;
        }

        shadow_renderer->render(scene, queue->_lights[0]->getPosition(), SceneRenderer::RenderFrame_ShadowMap, shadows_shader);
    }


    if(_pipeline == nullptr)
    {
        _pipeline = Pipeline::create(this);
        BloomFX::Ptr bloomFX = BloomFX::create();
        BlendAll::Ptr blendAll = BlendAll::create(BlendAll::Max);
        BlendAll::Ptr blendTmp = BlendAll::create(BlendAll::Mult);
        BlendAll::Ptr blendSha = BlendAll::create(BlendAll::Mult);
        BlendAll::Ptr blendRefl = BlendAll::create(BlendAll::Mult);
        CopyFrame::Ptr toScreen = CopyFrame::create();
        EnvironmentFX::Ptr environFX = EnvironmentFX::create();
        LightingModel::Ptr lighting = LightingModel::create();
        ShadowCasting::Ptr shadowFX = ShadowCasting::create();


        bloomFX->setInput( _internalFrames->get(1), 0 );
        lighting->setInput( _internalFrames->get(2), 0 );
        lighting->setInput( _internalFrames->get(5), 1 );
        lighting->setInput( _internalFrames->get(4), 2 );
        environFX->setInput( _internalFrames->get(2), 0 );
        environFX->setInput( _internalFrames->get(5), 1 );
        environFX->setInput( _internalFrames->get(3), 2 );
        shadowFX->setInput( _internalFrames->get(5), 0 );
        blendTmp->setInput( _internalFrames->get(0), 0 );

        // build dependances
        _pipeline->setOperation( environFX, FRAMEOP_ID_ENVFX );
        _pipeline->setOperation( blendSha, FRAMEOP_ID_SHAMIX );
        _pipeline->setOperation( blendTmp, FRAMEOP_ID_COLORMIX );
        _pipeline->setOperation( blendRefl, FRAMEOP_ID_ENVMIX );
        _pipeline->setOperation( blendAll, FRAMEOP_ID_BLOOMMIX );
        _pipeline->setOperation( shadowFX, FRAMEOP_ID_SHAFX );
        _pipeline->setOperation( lighting, FRAMEOP_ID_PHONG );
        _pipeline->setOperation( toScreen, FRAMEOP_ID_COPY );
        _pipeline->setOperation( bloomFX, FRAMEOP_ID_BLOOM );
        _pipeline->bind( FRAMEOP_ID_SHAMIX, FRAMEOP_ID_PHONG, 0);
        _pipeline->bind( FRAMEOP_ID_SHAMIX, FRAMEOP_ID_SHAFX, 1);
        _pipeline->bind( FRAMEOP_ID_ENVMIX, FRAMEOP_ID_ENVFX, 0);
        _pipeline->bind( FRAMEOP_ID_COLORMIX, FRAMEOP_ID_SHAMIX, 1);
        _pipeline->bind( FRAMEOP_ID_ENVMIX, FRAMEOP_ID_COLORMIX, 1);
        _pipeline->bind( FRAMEOP_ID_BLOOMMIX, FRAMEOP_ID_ENVMIX, 0);
        _pipeline->bind( FRAMEOP_ID_BLOOMMIX, FRAMEOP_ID_BLOOM, 1);
        _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_BLOOMMIX, 0);

        _pipeline->prepare(camera,queue->_lights[0],shadows,environment);
        environFX->setup();
        blendSha->setup();
        blendTmp->setup();
        blendRefl->setup();
        blendAll->setup();
        shadowFX->setup();
        lighting->setup();
        toScreen->setup();
        bloomFX->setup();
    }



    // switch(getOutputFrame())
    // {
    // case RenderFrame_Default:
    //     toScreen->setInput( _pipeline->getOutputFrame(4,0) ) ;
    //     break;
    // case RenderFrame_Color:
    //     toScreen->setInput( _internalFrames->get(0) ) ;
    //     break;
    // case RenderFrame_ShadowMap:
    //     toScreen->setInput( _pipeline->getOutputFrame(5,0) ) ;
    //     break;
    // case RenderFrame_Environment:
    //     toScreen->setInput( _pipeline->getOutputFrame(0,0) ) ;
    //     break;
    // case RenderFrame_Lighting:
    //     toScreen->setInput( _pipeline->getOutputFrame(6,0) ) ;
    //     break;
    // case RenderFrame_Depth:
    //     toScreen->setInput( _internalFrames->get(5) ) ;
    //     break;
    // case RenderFrame_Emissive:
    //     toScreen->setInput( _internalFrames->get(1) ) ;
    //     break;
    // case RenderFrame_Bloom:
    //     toScreen->setInput( output_bloom[0] ) ;
    //     break;
    // case RenderFrame_Normals:
    //     toScreen->setInput( _internalFrames->get(2) ) ;
    //     break;
    // case RenderFrame_Reflectivity:
    //     toScreen->setInput( _internalFrames->get(3) ) ;
    //     break;
    // default: // assume RenderFrame_Default
    //     toScreen->setInput( _pipeline->getOutputFrame(4,0) ) ;
    //     break;
    // }

    _pipeline->prepare(camera,queue->_lights[0], shadows, environment);
    _pipeline->run();
}


//---------------------------------------------------------------
void GlRenderer::applyState(const State::Ptr& state,
                            SceneRenderer::RenderFrame renderPass)
{
    if(_renderPlugin == nullptr) return;

    if( renderPass==SceneRenderer::RenderFrame_ShadowMap )
        _renderPlugin->setCulling(2); // front culling
    else
        _renderPlugin->setCulling(state->getFaceCullingMode());
    _renderPlugin->setBlend(state->getBlendMode());
    _renderPlugin->setLineW(state->getLineWidth());
    _renderPlugin->setDepthTest(state->getDepthTest());
    _renderPlugin->setViewport(state->getViewport());

    ReflexionModel::Ptr reflexion = state->getReflexion();
    // if(overrideShader) reflexion = overrideShader;

    _renderPlugin->load(reflexion);
    _renderPlugin->bind(reflexion);

    const std::map<std::string,Uniform::Ptr>& uniforms = state->getUniforms();
    for(auto u : uniforms) GlRenderer::_renderPlugin->update(reflexion, u.second);
}

//---------------------------------------------------------------
void GlRenderer::applyClear(ClearNode* clearNode, SceneRenderer::RenderFrame renderPass)
{
    static ClearNode::Ptr clear;
    if(clear.get() == nullptr) clear = ClearNode::create();

    clear->setDepth( clearNode->getDepth() );
    clear->setColor( clearNode->getColor() );
    clear->enableDepth( clearNode->isDepthEnable() );
    clear->enableColor( clearNode->isColorEnable() );

    if(_renderPlugin != nullptr)
    {
        _renderPlugin->apply(clear);

        // if(renderPass == SceneRenderer::RenderFrame_Default)
        // {
        //     clear->setColor( Vec4(1.0) );
        //     _renderPlugin->apply(clear, 5);
        // }
    }
}

//---------------------------------------------------------------
void GlRenderer::drawGeometry(GeoNode* geoNode)
{
    if(_renderPlugin != nullptr )
    {
        _renderPlugin->load(geoNode->_geo);
        _renderPlugin->draw(geoNode->_geo);
    }
}

IMPGEARS_END
