#include <RendererGL/GlRenderer.h>
#include <RendererGL/CubeMap.h>

#include <cstdlib>

#include <RendererGL/BloomFX.h>
#include <RendererGL/EnvironmentFX.h>
#include <RendererGL/LightingModel.h>
#include <RendererGL/BlendAll.h>
#include <RendererGL/RenderToCubeMap.h>
#include <RendererGL/ShadowCasting.h>
#include <RendererGL/AmbientOcclusion.h>
#include <ImpGears/SceneGraph/RenderToSamplerNode.h>
#include <RendererGL/GlslCode.h>
#include <ImpGears/Geometry/InstancedGeometry.h>

#include <RendererGL/FrameBuffer.h>
#include <RendererGL/GlError.h>

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
#define FRAMEOP_ID_SSAOMIX 10

#define MRT_OUT_COLOR 0
#define MRT_OUT_EMISSIVE 1
#define MRT_OUT_NORMAL 2
#define MRT_OUT_REFLECTIVITY 3
#define MRT_OUT_SHININESS 4
#define MRT_OUT_DEPTH 5

#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_shadow_depth = GLSL_CODE(
in vec3 v_mv;

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    float impNear = 0.1; float impFar = 128.0;
    float depth = (length(v_mv.xyz) - impNear) / (impFar - impNear);
    // float depth = (abs(v_mv.z) - impNear) / (impFar-impNear);
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

//---------------------------------------------------------------
void GlRenderer::initialize()
{
    // CubeMaps rendering setup
    _environmentMap = CubeMapSampler::create(_environmentResolution,_environmentResolution,4,Vec4(1.0));
    _environmentRenderer = RenderToCubeMap::create(this);
    _environmentRenderer->setCubeMap(_environmentMap);
    _environmentRenderer->setResolution(_environmentResolution);

    _shadowsMap = CubeMapSampler::create(_shadowResolution,_shadowResolution,4,Vec4(1.0));
    _shadowsRenderer = RenderToCubeMap::create(this);
    _shadowsRenderer->setCubeMap(_shadowsMap);
    _shadowsRenderer->setResolution(_shadowResolution);
    _shadowsmapShader = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi,
                "glsl_shadow_depth");
    _shadowsmapShader->_fragCode_lighting = glsl_shadow_depth;

    _shadowsmapShader_instanced = ReflexionModel::create(
                ReflexionModel::Lighting_Customized,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_2_Col_Emi,
                "glsl_shadow_depth");
    _shadowsmapShader_instanced->_fragCode_lighting = glsl_shadow_depth;
    _shadowsmapShader_instanced->_vertCode = glsl_instancedBasicVert;

    // mrt setup
    int mrt_size = 6;
    bool enableMSAA = isMsaaEnabled();
    
    _internalFrames = RenderTarget::create();
    _internalFrames->build(1024.0,1024.0,mrt_size,true,enableMSAA);
    _internalFrames->setClearColor(MRT_OUT_COLOR, Vec4(1.0));
    _internalFrames->setClearColor(MRT_OUT_EMISSIVE, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_NORMAL, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_REFLECTIVITY, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_SHININESS, Vec4(0.0));
    _internalFrames->setClearColor(MRT_OUT_DEPTH, Vec4(1.0));

    if(enableMSAA)
    {
        _resolvedFrames = RenderTarget::create();
        _resolvedFrames->build(1024.0,1024.0,mrt_size,false,false);
    }

    // pipeline setup
    _pipeline = Pipeline::create(this);
    _pipeline->setViewport(_outputViewport);

    BloomFX::Ptr bloomFX = BloomFX::create();
    BlendAll::Ptr bloom_mix = BlendAll::create(BlendAll::Max);
    BlendAll::Ptr color_mix = BlendAll::create(BlendAll::Mult);
    BlendAll::Ptr sha_mix = BlendAll::create(BlendAll::Mult);
    BlendAll::Ptr ssao_mix = BlendAll::create(BlendAll::Mult);
    BlendAll::Ptr env_mix = BlendAll::create(BlendAll::Mult);
    CopyFrame::Ptr toScreen = CopyFrame::create();
    EnvironmentFX::Ptr environFX = EnvironmentFX::create();
    LightingModel::Ptr lighting = LightingModel::create();
    ShadowCasting::Ptr shadowFX = ShadowCasting::create();
    AmbientOcclusion::Ptr ssaoFX = AmbientOcclusion::create();

    _lighting = lighting;
    _lighting->setLightPower(_lightpower);
    _lighting->setAmbient(_ambient);

    // build dependances
    _pipeline->setOperation( environFX, FRAMEOP_ID_ENVFX );
    _pipeline->setOperation( sha_mix, FRAMEOP_ID_SHAMIX );
    _pipeline->setOperation( color_mix, FRAMEOP_ID_COLORMIX );
    _pipeline->setOperation( env_mix, FRAMEOP_ID_ENVMIX );
    _pipeline->setOperation( bloom_mix, FRAMEOP_ID_BLOOMMIX );
    _pipeline->setOperation( shadowFX, FRAMEOP_ID_SHAFX );
    _pipeline->setOperation( lighting, FRAMEOP_ID_PHONG );
    _pipeline->setOperation( toScreen, FRAMEOP_ID_COPY );
    _pipeline->setOperation( bloomFX, FRAMEOP_ID_BLOOM );
    _pipeline->setOperation( ssaoFX, FRAMEOP_ID_SSAO );
    _pipeline->setOperation( ssao_mix, FRAMEOP_ID_SSAOMIX );
    
    RenderTarget::Ptr pipelineInputFrames = _internalFrames;
    if(enableMSAA) pipelineInputFrames = _resolvedFrames;

    _pipeline->bindExternal( FRAMEOP_ID_BLOOM, pipelineInputFrames, 0, MRT_OUT_EMISSIVE);
    _pipeline->bindExternal( FRAMEOP_ID_PHONG, pipelineInputFrames, 0, MRT_OUT_NORMAL);
    _pipeline->bindExternal( FRAMEOP_ID_PHONG, pipelineInputFrames, 1, MRT_OUT_DEPTH);
    _pipeline->bindExternal( FRAMEOP_ID_PHONG, pipelineInputFrames, 2, MRT_OUT_SHININESS);
    _pipeline->bindExternal( FRAMEOP_ID_ENVFX, pipelineInputFrames, 0, MRT_OUT_NORMAL);
    _pipeline->bindExternal( FRAMEOP_ID_ENVFX, pipelineInputFrames, 1, MRT_OUT_DEPTH);
    _pipeline->bindExternal( FRAMEOP_ID_ENVFX, pipelineInputFrames, 2, MRT_OUT_REFLECTIVITY);
    _pipeline->bindExternal( FRAMEOP_ID_SHAFX, pipelineInputFrames, 0, MRT_OUT_DEPTH);
    _pipeline->bindExternal( FRAMEOP_ID_COLORMIX, pipelineInputFrames, 0, MRT_OUT_COLOR);
    _pipeline->bindExternal( FRAMEOP_ID_SSAO, pipelineInputFrames, 0, MRT_OUT_NORMAL);
    _pipeline->bindExternal( FRAMEOP_ID_SSAO, pipelineInputFrames, 1, MRT_OUT_DEPTH);

    _pipeline->bind( FRAMEOP_ID_SSAOMIX, FRAMEOP_ID_SSAO, 0);
    _pipeline->bind( FRAMEOP_ID_SSAOMIX, FRAMEOP_ID_PHONG, 1);
    _pipeline->bind( FRAMEOP_ID_SHAMIX, FRAMEOP_ID_SSAOMIX, 0);
    _pipeline->bind( FRAMEOP_ID_SHAMIX, FRAMEOP_ID_SHAFX, 1);
    _pipeline->bind( FRAMEOP_ID_ENVMIX, FRAMEOP_ID_ENVFX, 0);
    _pipeline->bind( FRAMEOP_ID_COLORMIX, FRAMEOP_ID_SHAMIX, 1);
    _pipeline->bind( FRAMEOP_ID_ENVMIX, FRAMEOP_ID_COLORMIX, 1);
    _pipeline->bind( FRAMEOP_ID_BLOOMMIX, FRAMEOP_ID_ENVMIX, 0);
    _pipeline->bind( FRAMEOP_ID_BLOOMMIX, FRAMEOP_ID_BLOOM, 1);
    _pipeline->bind( FRAMEOP_ID_COPY, FRAMEOP_ID_BLOOMMIX, 0);

    _debugToScreen = CopyFrame::create();
    _debugToScreen->clearOutput();
    _debugToScreen->setInput(pipelineInputFrames->get(0), 0);
    _debugToScreen->setup(Vec4(0.0,0.0,512.0,512.0));
}

//---------------------------------------------------------------
void GlRenderer::setOuputViewport(const Vec4& vp)
{
    if(_pipeline==nullptr) initialize();
        
    _outputViewport = vp;
    _pipeline->setViewport(_outputViewport);
    _debugToScreen->setup(_outputViewport);
}

//---------------------------------------------------------------
void GlRenderer::setRenderPlugin(RenderPlugin* plugin)
{
    _renderPlugin = plugin;
    // if(_renderPlugin != nullptr) _renderPlugin->init();
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
RenderQueue::Ptr GlRenderer::applyRenderVisitor(const Graph::Ptr& scene, RenderQueue::Ptr queue)
{
    Visitor::Ptr visitor = _visitor;
    _visitor->reset( queue );
    scene->accept( *visitor );
    RenderQueue::Ptr resQueue = _visitor->getQueue();
    return resQueue;
}


//---------------------------------------------------------------
void GlRenderer::drawQueue( RenderQueue::Ptr& queue, State::Ptr overrideState,
                            SceneRenderer::RenderFrame renderPass )
{
    if(_localState==nullptr) _localState = State::create();
    Matrix4 view;
    if(queue->_camera) view = queue->_camera->getViewMatrix();

    for(int i=0;i<(int)queue->_renderBin.size();++i)
    {
        RenderState renderState = queue->_renderBin.at(i);

        GeoNode::Ptr geode = GeoNode::dMorph( renderState.node );
        ClearNode::Ptr clear = ClearNode::dMorph( renderState.node );

        if(geode)
        {
            // filter with render pass flag
            RenderPass::PassFlag expectedFlag = RenderPass::Pass_DefaultMRT;
            if( renderPass==SceneRenderer::RenderFrame_Environment )
                expectedFlag = RenderPass::Pass_EnvironmentMapping;
            if( renderPass==SceneRenderer::RenderFrame_ShadowMap )
                expectedFlag = RenderPass::Pass_ShadowMapping;

            RenderPass::Ptr renderPass_info = renderState.state->getRenderPass();
            if(!renderPass_info || !renderPass_info->isPassEnabled(expectedFlag)) continue;



            // state copy and override
            _localState->clone(renderState.state, State::CloneOpt_OverrideRef);
            if( overrideState )
            {
                _localState->clone( overrideState, State::CloneOpt_OverrideChangedRef );

                // case of shadow pass + instanced geometry
                InstancedGeometry::Ptr instanced = InstancedGeometry::dMorph(geode->_geo);
                if(renderPass == RenderFrame_ShadowMap &&  instanced)
                {
                    _localState->setReflexion(_shadowsmapShader_instanced);
                }
            }


            // update Material uniforms
            if(renderPass==SceneRenderer::RenderFrame_Default || renderPass==SceneRenderer::RenderFrame_Environment)
            {
                Material::Ptr mat = geode->_material;
                float shininess = mat->_shininess;
                Vec4 color = mat->_color;

                if(mat->_baseColor)
                        _localState->setUniform("u_sampler_color", mat->_baseColor, 0);
                if(mat->_normalmap)
                        _localState->setUniform("u_sampler_normal", mat->_normalmap, 1);
                if(mat->_emissive)
                        _localState->setUniform("u_sampler_emissive", mat->_emissive, 2);
                if(mat->_reflectivity)
                        _localState->setUniform("u_sampler_reflectivity", mat->_reflectivity, 3);

                _localState->setUniform("u_color", color);
                _localState->setUniform("u_shininess", shininess);
            }


            // ---- if we want a normal matrix for view space instead of model space ----
            // Matrix4 model = local_state->getUniforms()["u_model"]->getMat4();
            // Matrix3 normalMat = Matrix3(model * view).inverse().transpose();
            // state->setUniform("u_normal", normalMat );
            renderState.normal = Matrix3(renderState.model).inverse().transpose();
            renderState.proj = _localState->getProjectionMatrix();

            // update matrix uniforms
            _localState->setUniform("u_view", view);
            _localState->setUniform("u_proj", renderState.proj);
            _localState->setUniform("u_model", renderState.model);
            _localState->setUniform("u_normal", renderState.normal);


            applyState(_localState, renderPass);
            drawGeometry(geode.get());
        }
        else if(clear)
        {
            applyClear(clear.get(), renderPass);
        }
    }

    // sorting transparent render bin
    Vec3 camPos;
    if(queue->_camera) camPos = queue->_camera->getAbsolutePosition();
    queue->_transparentRenderBin.sortByDistance(camPos);

    // render tranparent geometry
    for(int i=(int)queue->_transparentRenderBin.size()-1;i>=0;--i)
    {
        RenderState renderState = queue->_transparentRenderBin.at(i);
        GeoNode::Ptr geode = GeoNode::dMorph( renderState.node );

        if(geode)
        {
            // filter with render pass flag
            RenderPass::PassFlag expectedFlag = RenderPass::Pass_DefaultMRT;
            if( renderPass==SceneRenderer::RenderFrame_Environment )
                expectedFlag = RenderPass::Pass_EnvironmentMapping;
            if( renderPass==SceneRenderer::RenderFrame_ShadowMap )
                expectedFlag = RenderPass::Pass_ShadowMapping;

            RenderPass::Ptr renderPass_info = renderState.state->getRenderPass();
            if(!renderPass_info || !renderPass_info->isPassEnabled(expectedFlag)) continue;

            // state copy and override
            _localState->clone(renderState.state, State::CloneOpt_OverrideRef);
            if( overrideState )
            {
                _localState->clone( overrideState, State::CloneOpt_OverrideChangedRef );

                // case of shadow pass + instanced geometry
                InstancedGeometry::Ptr instanced = InstancedGeometry::dMorph(geode->_geo);
                if(renderPass == RenderFrame_ShadowMap &&  instanced)
                {
                    _localState->setReflexion(_shadowsmapShader_instanced);
                }
            }

            // update Material uniforms
            if(renderPass==SceneRenderer::RenderFrame_Default || renderPass==SceneRenderer::RenderFrame_Environment)
            {
                Material::Ptr mat = geode->_material;
                float shininess = mat->_shininess;
                Vec4 color = mat->_color;

                if(mat->_baseColor)
                        _localState->setUniform("u_sampler_color", mat->_baseColor, 0);
                if(mat->_normalmap)
                        _localState->setUniform("u_sampler_normal", mat->_normalmap, 1);
                if(mat->_emissive)
                        _localState->setUniform("u_sampler_emissive", mat->_emissive, 2);
                if(mat->_reflectivity)
                        _localState->setUniform("u_sampler_reflectivity", mat->_reflectivity, 3);

                _localState->setUniform("u_color", color);
                _localState->setUniform("u_shininess", shininess);
            }

            // ---- if we want a normal matrix for view space instead of model space ----
            // Matrix4 model = local_state->getUniforms()["u_model"]->getMat4();
            // Matrix3 normalMat = Matrix3(model * view).inverse().transpose();
            // state->setUniform("u_normal", normalMat );
            renderState.normal = Matrix3(renderState.model).inverse().transpose();
            renderState.proj = _localState->getProjectionMatrix();

            // update matrix uniform
            _localState->setUniform("u_view", view);
            _localState->setUniform("u_proj", renderState.proj);
            _localState->setUniform("u_model", renderState.model);
            _localState->setUniform("u_normal", renderState.normal);


            applyState(_localState, renderPass);
            drawGeometry(geode.get());
        }
    }
}

// #include <QDebug>


void GlRenderer::applyRenderToSampler(RenderQueue::Ptr queue)
{
    for(int i=0;i<(int)queue->_renderBin.size();++i)
    {
        RenderToSamplerNode::Ptr render2sampler = RenderToSamplerNode::dMorph( queue->_renderBin.nodeAt(i) );
        if(render2sampler && render2sampler->ready()==false && render2sampler->scene())
        {
            Graph::Ptr scene = render2sampler->scene();
            if( render2sampler->cubemap() )
            {
                RenderToCubeMap::Ptr render2cm;
                if( render2sampler->getRenderData() )
                {
                    render2cm = RenderToCubeMap::dMorph( render2sampler->getRenderData() );
                }
                else
                {
                    render2cm = RenderToCubeMap::create(this);
                    render2cm->setCubeMap( render2sampler->cubemap() );
                    render2cm->setResolution(512.0);
                    render2sampler->setRenderData( render2cm );
                }
                render2cm->render(scene, Vec3(0.0));
            }
            else if( render2sampler->texture() )
            {
                RenderTarget::Ptr render2tex;
                if( render2sampler->getRenderData() )
                {
                    render2tex = RenderTarget::dMorph( render2sampler->getRenderData() );
                }
                else
                {
                    render2tex = RenderTarget::create();
                    render2tex->build({render2sampler->texture()}, true);
                    render2tex->setClearColor(0, Vec4(0.0));
                    render2sampler->setRenderData( render2tex );
                }
                RenderQueue::Ptr queue = RenderQueue::create();
                queue = applyRenderVisitor(scene, queue);
                _renderPlugin->init(render2tex);
                _renderPlugin->bind(render2tex);
                drawQueue(queue);
                _renderPlugin->unbind();
                render2tex->change();

            }
            render2sampler->makeReady();
        }

    }
}


//---------------------------------------------------------------
void GlRenderer::clearRenderCache()
{
    _renderCache.clear();
}

void checkQueue(RenderQueue::Ptr queue)
{
    std::cout << "node in queue = " << queue->_renderBin.size() << std::endl;
    int geodeCount = 0;
    int clearCount = 0;
    for(int i=0;i<(int)queue->_renderBin.size();++i)
    {
        RenderState renderState = queue->_renderBin.at(i);

        GeoNode::Ptr geode = GeoNode::dMorph( renderState.node );
        ClearNode::Ptr clear = ClearNode::dMorph( renderState.node );

        if(geode)
        {
            geodeCount++;
            if( renderState.state->getRenderPass() == nullptr ) std::cout << "geode render state has no renderPassInfo" << std::endl;
            if( renderState.state->getReflexion() == nullptr ) std::cout << "geode render state has no reflection model" << std::endl;
        }
        else if(clear)
        {
            clearCount++;
        }
        else
        {
            std::cout << "incorrect type node - " << renderState.node->getObjectID() << std::endl;
        }
    }
    std::cout << "geode in queue = " << geodeCount << std::endl;
    std::cout << "clear in queue = " << clearCount << std::endl;

    std::cout << "node in transparent queue = " << queue->_transparentRenderBin.size() << std::endl;
    geodeCount = 0;
    clearCount = 0;
    for(int i=0;i<(int)queue->_transparentRenderBin.size();++i)
    {
        RenderState renderState = queue->_renderBin.at(i);

        GeoNode::Ptr geode = GeoNode::dMorph( renderState.node );
        ClearNode::Ptr clear = ClearNode::dMorph( renderState.node );

        if(geode)
        {
            geodeCount++;
            if( renderState.state->getRenderPass() == nullptr ) std::cout << "geode render state has no renderPassInfo" << std::endl;
            if( renderState.state->getReflexion() == nullptr ) std::cout << "geode render state has no reflection model" << std::endl;
        }
        else if(clear)
        {
            clearCount++;
        }
        else
        {
            std::cout << "incorrect type node - " << renderState.node->getObjectID() << std::endl;
        }
    }
    std::cout << "geode in transparent queue = " << geodeCount << std::endl;
    std::cout << "clear in transparent queue = " << clearCount << std::endl;
}

//---------------------------------------------------------------
void GlRenderer::render(const Graph::Ptr& scene)
{
    if(_renderPlugin == nullptr) return;
    if(_pipeline==nullptr) initialize();


    ShadowCasting::Ptr shadowOp = ShadowCasting::dMorph( _pipeline->getOperation(FRAMEOP_ID_SHAFX) );
    shadowOp->setSampleCount(_shadowSamples);

    AmbientOcclusion::Ptr ssaoOp = AmbientOcclusion::dMorph( _pipeline->getOperation(FRAMEOP_ID_SSAO) );
    ssaoOp->setSampleCount(_ssaoSamples);

    if(_renderCache.find( scene ) == _renderCache.end())
        _renderCache[scene] = RenderQueue::create();

    RenderQueue::Ptr queue = _renderCache[scene];
    queue = applyRenderVisitor(scene,queue);
    // checkQueue(queue);
    applyRenderToSampler(queue);
    

    if(isMsaaEnabled()) glEnable(GL_MULTISAMPLE);
    else glDisable(GL_MULTISAMPLE);
    
    _renderPlugin->init(_internalFrames);
    _renderPlugin->bind(_internalFrames);
    drawQueue(queue);
    _renderPlugin->unbind();
    _internalFrames->change();

    LightNode* light0 = nullptr;
    if( !queue->_lights.empty() ) light0 = queue->_lights[0];

    bool env_enabled = isFeatureEnabled(Feature_Environment);
    bool shadows_enabled = isFeatureEnabled(Feature_Shadow) && light0;
    bool bloom_enabled = isFeatureEnabled(Feature_Bloom);
    bool ssao_enabled = isFeatureEnabled(Feature_SSAO);
    bool phong_enabled = isFeatureEnabled(Feature_Phong) && light0;

    if(env_enabled)
    {
        // update resolution if needed
        static int s_lastEnvRes = 128;
        if(_environmentResolution != s_lastEnvRes)
        {
            // qDebug() << "env resolution updating";
            _environmentMap = CubeMapSampler::create(_environmentResolution,_environmentResolution,4,Vec4(1.0));
            _environmentRenderer->setCubeMap(_environmentMap);
            _environmentRenderer->setResolution(_environmentResolution);
            s_lastEnvRes = _environmentResolution;
        }

       Vec3 p(0.0);
       if(queue->_camera)
           p = queue->_camera->getAbsolutePosition();

       _environmentRenderer->render(
                   queue, p,
                   SceneRenderer::RenderFrame_Environment);
    }

    if(shadows_enabled)
    {
        // update resolution if needed
        static int s_lastShadowRes = 256;
        if(_shadowResolution != s_lastShadowRes)
        {
            // qDebug() << "shadow resolution updating";
            _shadowsMap = CubeMapSampler::create(_shadowResolution,_shadowResolution,4,Vec4(1.0));
            _shadowsRenderer->setCubeMap(_shadowsMap);
            _shadowsRenderer->setResolution(_shadowResolution);
            s_lastShadowRes = _shadowResolution;
        }

       Vec3 p(0.0);
       if(light0) p = light0->_worldPosition;

       _shadowsRenderer->render(
                   queue, p,
                   SceneRenderer::RenderFrame_ShadowMap, _shadowsmapShader);
    }
    
    bool enableMSAA = isMsaaEnabled();
    RenderTarget::Ptr pipelineInputFrames = _internalFrames;
    if(enableMSAA)
    {
        _renderPlugin->init(_resolvedFrames);
        
        RenderTarget::Ptr rt_src = _internalFrames;
        RenderTarget::Ptr rt_dst = _resolvedFrames;
            
        FrameBuffer::Ptr fbo_src = FrameBuffer::dMorph(rt_src->getRenderData());
        FrameBuffer::Ptr fbo_dst = FrameBuffer::dMorph(rt_dst->getRenderData());
      
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_dst->getVideoID());
        GL_CHECKERROR("bind draw framebuffer");
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_src->getVideoID());
        GL_CHECKERROR("bind read framebuffer");
        
        // resolve msaa : blit each multisampled buffer to texture
        for(int i=0;i<6;++i)
        {
            glDrawBuffer( static_cast<GLenum>(GL_COLOR_ATTACHMENT0+i) );
            GL_CHECKERROR("set draw buffers");
            glReadBuffer( static_cast<GLenum>(GL_COLOR_ATTACHMENT0+i) );
            GL_CHECKERROR("set read buffers");
            
            glBlitFramebuffer(0, 0, fbo_src->width(), fbo_src->height(), 0, 0, fbo_dst->width(), fbo_dst->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
            GL_CHECKERROR("resolve msaa");
        }
        
        _resolvedFrames->change();
        pipelineInputFrames = _resolvedFrames;
    }

    _pipeline->setActive(FRAMEOP_ID_SHAFX, shadows_enabled);
    _pipeline->setActive(FRAMEOP_ID_ENVFX, env_enabled);
    _pipeline->setActive(FRAMEOP_ID_BLOOM, bloom_enabled);
    _pipeline->setActive(FRAMEOP_ID_SSAO, ssao_enabled);
    _pipeline->setActive(FRAMEOP_ID_PHONG, phong_enabled);

    static RenderFrame s_lastOutput = RenderFrame_Default;

    if( getOutputFrame() != s_lastOutput )
    {
       switch(getOutputFrame())
       {
       case RenderFrame_Default: // do nothing
           break;
       case RenderFrame_Color:
            _debugToScreen->setInput(pipelineInputFrames->get(MRT_OUT_COLOR), 0);
           break;
       case RenderFrame_ShadowMap:
           _debugToScreen->setInput(_pipeline->getOutputFrame(FRAMEOP_ID_SHAFX,0), 0);
           break;
       case RenderFrame_Environment:
            _debugToScreen->setInput(_pipeline->getOutputFrame(FRAMEOP_ID_ENVFX,0), 0);
           break;
       case RenderFrame_Lighting:
            _debugToScreen->setInput(_pipeline->getOutputFrame(FRAMEOP_ID_PHONG,0), 0);
           break;
       case RenderFrame_Depth:
           _debugToScreen->setInput(pipelineInputFrames->get(MRT_OUT_DEPTH), 0);
           break;
       case RenderFrame_Emissive:
           _debugToScreen->setInput(pipelineInputFrames->get(MRT_OUT_EMISSIVE), 0);
           break;
       case RenderFrame_Bloom:
            _debugToScreen->setInput(_pipeline->getOutputFrame(FRAMEOP_ID_BLOOM,0), 0);
           break;
       case RenderFrame_Normals:
           _debugToScreen->setInput(pipelineInputFrames->get(MRT_OUT_NORMAL), 0);
           break;
       case RenderFrame_Reflectivity:
           _debugToScreen->setInput(pipelineInputFrames->get(MRT_OUT_REFLECTIVITY), 0);
           break;
       case RenderFrame_SSAO:
            _debugToScreen->setInput(_pipeline->getOutputFrame(FRAMEOP_ID_SSAO,0), 0);
           break;
       default: // assume RenderFrame_Default
           break;
       }

       s_lastOutput = getOutputFrame();
    }

    _lighting->setLightPower(_lightpower);
    _lighting->setAmbient(_ambient);
    _pipeline->prepare(queue->_camera, light0, _shadowsMap, _environmentMap);
    _pipeline->run( FRAMEOP_ID_COPY );

    if(getOutputFrame() != RenderFrame_Default)
    {
        _debugToScreen->apply(this,false);
    }
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
void GlRenderer::applyClear(ClearNode* clearNode, SceneRenderer::RenderFrame /*renderPass*/)
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
