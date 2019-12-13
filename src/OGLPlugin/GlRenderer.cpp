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
#include <OGLPlugin/AmbientOcclusion.h>

#define FRAMEOP_ID_ENVFX 0
#define FRAMEOP_ID_SHAMIX 1
#define FRAMEOP_ID_COLORMIX 2
#define FRAMEOP_ID_ENVMIX 3
#define FRAMEOP_ID_BLOOMMIX 4
#define FRAMEOP_ID_SHAFX 5
#define FRAMEOP_ID_PHONG 6
#define FRAMEOP_ID_COPY 7
#define FRAMEOP_ID_BLOOM 8
#define FRAMEOP_ID_SSAO 9

#define MRT_OUT_COLOR 0
#define MRT_OUT_EMISSIVE 1
#define MRT_OUT_NORMAL 2
#define MRT_OUT_REFLECTIVITY 3
#define MRT_OUT_SHININESS 4
#define MRT_OUT_DEPTH 5

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
    float depth = (length(v_mv.xyz) - near) / (far-near);
    out_color = vec4(vec3(depth),1.0);
}
);


IMPGEARS_BEGIN

//--------------------------------------------------------------
GlRenderer::GlRenderer()
    : SceneRenderer()
{
    // CubeMaps rendering setup
    _environmentMap = CubeMapSampler::create(1024.0,1024.0,4,Vec4(1.0));
    _environmentRenderer = RenderToCubeMap::create(this);
    _environmentRenderer->setCubeMap(_environmentMap);

    _shadowsMap = CubeMapSampler::create(1024,1024.0,4,Vec4(1.0));
    _shadowsRenderer = RenderToCubeMap::create(this);
    _shadowsRenderer->setCubeMap(_shadowsMap);
    _shadowsmapShader = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi,
                "glsl_shadow_depth");
    _shadowsmapShader->_fragCode_lighting = glsl_shadow_depth;


    // mrt setup
    int mrt_size = 6;
    _internalFrames = RenderTarget::create();
    _internalFrames->build(1024.0,1024.0,mrt_size,true);
    _internalFrames->setClearColor(MRT_OUT_COLOR, Vec4(1.0));
    _internalFrames->setClearColor(MRT_OUT_EMISSIVE, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_NORMAL, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_REFLECTIVITY, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_SHININESS, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_DEPTH, Vec4(1.0));



    // pipeline setup
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
    // AmbientOcclusion::Ptr ssaoFX = AmbientOcclusion::create();

    bloomFX->setInput( _internalFrames->get(MRT_OUT_EMISSIVE), 0 );
    lighting->setInput( _internalFrames->get(MRT_OUT_NORMAL), 0 );
    lighting->setInput( _internalFrames->get(MRT_OUT_DEPTH), 1 );
    lighting->setInput( _internalFrames->get(MRT_OUT_SHININESS), 2 );
    environFX->setInput( _internalFrames->get(MRT_OUT_NORMAL), 0 );
    environFX->setInput( _internalFrames->get(MRT_OUT_DEPTH), 1 );
    environFX->setInput( _internalFrames->get(MRT_OUT_REFLECTIVITY), 2 );
    shadowFX->setInput( _internalFrames->get(MRT_OUT_DEPTH), 0 );
    blendTmp->setInput( _internalFrames->get(MRT_OUT_COLOR), 0 );
    // ssaoFX->setInput( _internalFrames->get(MRT_OUT_NORMAL), 0 );
    // ssaoFX->setInput( _internalFrames->get(MRT_OUT_DEPTH), 1 );


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
    // _pipeline->setOperation( ssaoFX, FRAMEOP_ID_SSAO );

    _pipeline->bindExternal( FRAMEOP_ID_BLOOM, _internalFrames, 0, MRT_OUT_EMISSIVE);
    _pipeline->bindExternal( FRAMEOP_ID_PHONG, _internalFrames, 0, MRT_OUT_NORMAL);
    _pipeline->bindExternal( FRAMEOP_ID_PHONG, _internalFrames, 1, MRT_OUT_DEPTH);
    _pipeline->bindExternal( FRAMEOP_ID_PHONG, _internalFrames, 2, MRT_OUT_SHININESS);
    _pipeline->bindExternal( FRAMEOP_ID_ENVFX, _internalFrames, 0, MRT_OUT_NORMAL);
    _pipeline->bindExternal( FRAMEOP_ID_ENVFX, _internalFrames, 1, MRT_OUT_DEPTH);
    _pipeline->bindExternal( FRAMEOP_ID_ENVFX, _internalFrames, 2, MRT_OUT_REFLECTIVITY);
    _pipeline->bindExternal( FRAMEOP_ID_SHAFX, _internalFrames, 0, MRT_OUT_DEPTH);
    _pipeline->bindExternal( FRAMEOP_ID_COLORMIX, _internalFrames, 0, MRT_OUT_COLOR);
    // _pipeline->bindExternal( FRAMEOP_ID_SSAO, _internalFrames, 0, MRT_OUT_NORMAL);
    // _pipeline->bindExternal( FRAMEOP_ID_SSAO, _internalFrames, 1, MRT_OUT_DEPTH);

    _pipeline->bind( FRAMEOP_ID_SHAMIX, FRAMEOP_ID_PHONG, 0);
    _pipeline->bind( FRAMEOP_ID_SHAMIX, FRAMEOP_ID_SHAFX, 1);
    _pipeline->bind( FRAMEOP_ID_ENVMIX, FRAMEOP_ID_ENVFX, 0);
    _pipeline->bind( FRAMEOP_ID_COLORMIX, FRAMEOP_ID_SHAMIX, 1);
    _pipeline->bind( FRAMEOP_ID_ENVMIX, FRAMEOP_ID_COLORMIX, 1);
    _pipeline->bind( FRAMEOP_ID_BLOOMMIX, FRAMEOP_ID_ENVMIX, 0);
    _pipeline->bind( FRAMEOP_ID_BLOOMMIX, FRAMEOP_ID_BLOOM, 1);
    _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_BLOOMMIX, 0);

    // _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_SSAO, 0);
}

//--------------------------------------------------------------
GlRenderer::~GlRenderer()
{
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
                        local_state->setUniform("u_sampler_color", mat->_baseColor, 0);
                if(mat->_normalmap)
                        local_state->setUniform("u_sampler_normal", mat->_normalmap, 1);
                if(mat->_emissive)
                        local_state->setUniform("u_sampler_emissive", mat->_emissive, 2);
                if(mat->_reflectivity)
                        local_state->setUniform("u_sampler_reflectivity", mat->_reflectivity, 3);

                local_state->setUniform("u_color", color);
                local_state->setUniform("u_shininess", shininess);
            }


            // ---- if we want a normal matrix for view space instead of model space ----
            // Matrix4 model = local_state->getUniforms()["u_model"]->getMat4();
            // Matrix3 normalMat = Matrix3(model * view).inverse().transpose();
            // state->setUniform("u_normal", normalMat );
            local_state->setUniform("u_view", view);


            applyState(local_state, renderPass);
            if( renderPass==SceneRenderer::RenderFrame_ShadowMap ) _renderPlugin->setCulling(2);

            RenderPass::Ptr renderPass_info = local_state->getRenderPass();

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

    _renderPlugin->init(_internalFrames);
    _renderPlugin->bind(_internalFrames);
    _internalFrames->change();

    RenderQueue::Ptr queue = applyRenderVisitor(scene);
    drawQueue(queue);

   bool env_enabled = isFeatureEnabled(Feature_Environment);
   bool shadows_enabled = isFeatureEnabled(Feature_Shadow);
   bool bloom_enabled = isFeatureEnabled(Feature_Bloom);

   if(env_enabled)
   {
       _environmentRenderer->render(
                   scene, Vec3(0.0),
                   SceneRenderer::RenderFrame_Environment); // todo : position as parameter
   }

   if(shadows_enabled)
   {
       _shadowsRenderer->render(
                   scene, queue->_lights[0]->_worldPosition,
                   SceneRenderer::RenderFrame_ShadowMap, _shadowsmapShader);
   }

   _pipeline->setActive(FRAMEOP_ID_SHAFX, shadows_enabled);
   _pipeline->setActive(FRAMEOP_ID_ENVFX, env_enabled);
   _pipeline->setActive(FRAMEOP_ID_BLOOM, bloom_enabled);

   static RenderFrame s_lastOutput = RenderFrame_Default;

   if( getOutputFrame() != s_lastOutput )
   {
       switch(getOutputFrame())
       {
       case RenderFrame_Default:
            _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_BLOOMMIX, 0);
           break;
       case RenderFrame_Color:
            _pipeline->bindExternal( FRAMEOP_ID_COPY, _internalFrames, 0, MRT_OUT_COLOR);
           break;
       case RenderFrame_ShadowMap:
            _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_SHAFX, 0);
           break;
       case RenderFrame_Environment:
            _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_ENVFX, 0);
           break;
       case RenderFrame_Lighting:
            _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_PHONG, 0);
           break;
       case RenderFrame_Depth:
           _pipeline->bindExternal( FRAMEOP_ID_COPY, _internalFrames, 0, MRT_OUT_DEPTH);
           break;
       case RenderFrame_Emissive:
           _pipeline->bindExternal( FRAMEOP_ID_COPY, _internalFrames, 0, MRT_OUT_EMISSIVE);
           break;
       case RenderFrame_Bloom:
            _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_BLOOM, 0);
           break;
       case RenderFrame_Normals:
           _pipeline->bindExternal( FRAMEOP_ID_COPY, _internalFrames, 0, MRT_OUT_NORMAL);
           break;
       case RenderFrame_Reflectivity:
           _pipeline->bindExternal( FRAMEOP_ID_COPY, _internalFrames, 0, MRT_OUT_REFLECTIVITY);
           break;
       default: // assume RenderFrame_Default
            _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_BLOOMMIX, 0);
           break;
       }

       s_lastOutput = getOutputFrame();
   }

    _pipeline->prepare(queue->_camera, queue->_lights[0], _shadowsMap, _environmentMap);
    _pipeline->run( FRAMEOP_ID_COPY );
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

    if(_renderPlugin != nullptr) _renderPlugin->apply(clear);
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
