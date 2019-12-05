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



#define GLSL_CODE( code ) #code

//--------------------------------------------------------------
static std::string glsl_shadow_depth = GLSL_CODE(
varying vec3 v_mv;

void lighting(out vec4 out_lighting,
              out vec4 out_emissive,
              out vec3 out_position,
              out vec3 out_normal,
              out float out_metalness,
              out float out_depth)
{
    float near = 0.1; float far = 128.0;
    float depth = (length(v_mv.xyz) - near) / far;
    out_lighting = vec4(vec3(depth),1.0);
}

);


IMPGEARS_BEGIN

//--------------------------------------------------------------
GlRenderer::GlRenderer()
    : SceneRenderer()
    , _bloomFX(nullptr)
    , _envFX(nullptr)
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
RenderQueue::Ptr GlRenderer::applyRenderVisitor(const Graph::Ptr& scene,
                                                Camera::Ptr overrideCamera,
                                                SceneRenderer::RenderFrame renderPass,
                                                ReflexionModel::Ptr overrideShader)
{
    Visitor::Ptr visitor = _visitor;
    _visitor->reset();
    scene->accept(visitor);

    RenderQueue::Ptr queue = _visitor->getQueue();
    Matrix4 view;
    if(queue->_camera) view = queue->_camera->getViewMatrix();
    if(overrideCamera) view = overrideCamera->getViewMatrix();

    Vec3 lightPos(0.0);
    Vec3 lightCol(1.0);
    Vec3 color(1.0);
    Vec3 latt(0.0);

    for(int i=0;i<(int)queue->_nodes.size();++i)
    {
        LightNode* light = closest(queue->_nodes[i], queue->_lights);
        if(light)
        {
            lightPos = light->_worldPosition;
            lightCol = light->_color;
            latt[0] = light->_power;
        }

        GeoNode* geode = dynamic_cast<GeoNode*>( queue->_nodes[i] );
        ClearNode* clear = dynamic_cast<ClearNode*>( queue->_nodes[i] );
        if(geode)
        {
            Material::Ptr mat = geode->_material;
            latt[1] = mat->_shininess;
            color = mat->_color;

            if(renderPass==SceneRenderer::RenderFrame_Default)
            {
                if(mat->_baseColor)
                        queue->_states[i]->setUniform("u_sampler_color", mat->_baseColor, 0);
                if(mat->_normalmap)
                        queue->_states[i]->setUniform("u_sampler_normal", mat->_normalmap, 1);
                if(mat->_emissive)
                        queue->_states[i]->setUniform("u_sampler_emissive", mat->_emissive, 2);
            }

            queue->_states[i]->setUniform("u_view", view);

            // ---- if we want a normal matrix for view space instead of model space ----
            // Matrix4 model = queue->_states[i]->getUniforms()["u_model"]->getMat4();
            // Matrix3 normalMat = Matrix3(model * view).inverse().transpose();
            // queue->_states[i]->setUniform("u_normal", normalMat );

            if(renderPass==SceneRenderer::RenderFrame_Default)
            {
                queue->_states[i]->setUniform("u_lightPos", lightPos);
                queue->_states[i]->setUniform("u_lightCol", lightCol);
                queue->_states[i]->setUniform("u_lightAtt", latt);
                queue->_states[i]->setUniform("u_color", color);
            }
            applyState(queue->_states[i], renderPass, overrideShader);
            if( renderPass==SceneRenderer::RenderFrame_ShadowMap )
            {
                _renderPlugin->setCulling(2);
            }


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

    _renderPlugin->unbind();

    return queue;
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
        _internalFrames->setClearColor(3, Vec4(0.0));
        _internalFrames->setClearColor(5, Vec4(1.0));
    }

    // if(_renderTargets == nullptr && _targets.size() > 0)
    // {
    //     std::vector<ImageSampler::Ptr> samplers(_targets.size());
    //     for(int i=0;i<(int)_targets.size();++i)
    //         samplers[i] = ImageSampler::create(_targets[i]);
    //
    //     _renderTargets = RenderTarget::create();
    //     _renderTargets->build(samplers, true);
    // }


    _renderPlugin->init(_internalFrames);
    _renderPlugin->bind(_internalFrames);
    _internalFrames->change();


    RenderQueue::Ptr queue = applyRenderVisitor(scene);
    const Camera* camera = queue->_camera;



    static RenderToCubeMap::Ptr environment_renderer;
    static RenderToCubeMap::Ptr shadow_renderer;
    static CubeMapSampler::Ptr environment, shadows;
    static ReflexionModel::Ptr shadows_shader;

    if(isFeatureEnabled(Feature_Environment))
    {
        if(environment_renderer == nullptr)
        {
            // environment = CubeMapSampler::create(1024.0,1024.0,4,Vec4(1.0));
            environment = CubeMapSampler::create(1024.0,1024.0,4,Vec4(1.0));
            environment_renderer = RenderToCubeMap::create(this);
            environment_renderer->setCubeMap(environment);
        }

        environment_renderer->render(scene, Vec3(0.0), SceneRenderer::RenderFrame_Environment);
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



    if(isFeatureEnabled(Feature_Bloom))
    {
        static std::vector<ImageSampler::Ptr> input_bloom;
        static std::vector<ImageSampler::Ptr> output_bloom;
        static std::vector<ImageSampler::Ptr> input_blend;
        static std::vector<ImageSampler::Ptr> output_blend;
        static std::vector<ImageSampler::Ptr> input_blendTmp;
        static std::vector<ImageSampler::Ptr> output_blendTmp;
        static std::vector<ImageSampler::Ptr> input_blendSha;
        static std::vector<ImageSampler::Ptr> output_blendSha;
        static std::vector<ImageSampler::Ptr> input_lighting;
        static std::vector<ImageSampler::Ptr> output_lighting;
        static std::vector<ImageSampler::Ptr> input_shadow_cast;
        static std::vector<ImageSampler::Ptr> output_shadow_cast;
        static std::vector<ImageSampler::Ptr> input_env;
        static std::vector<ImageSampler::Ptr> output_env;
        static std::vector<ImageSampler::Ptr> final_output;
        static BlendAll::Ptr blendAll;
        static BlendAll::Ptr blendTmp;
        static BlendAll::Ptr blendSha;
        static EnvironmentFX::Ptr environFX;
        static ShadowCasting::Ptr shadowFX;
        static LightingModel::Ptr lighting;
        static CopyFrame::Ptr toScreen;


        if(_bloomFX == nullptr)
        {
            _bloomFX = new BloomFX();
            blendAll = BlendAll::create(BlendAll::Max);
            blendTmp = BlendAll::create(BlendAll::Mult);
            blendSha = BlendAll::create(BlendAll::Mult);
            toScreen = CopyFrame::create();
            environFX = EnvironmentFX::create();
            lighting = LightingModel::create();
            shadowFX = ShadowCasting::create();

            input_lighting = { _internalFrames->get(3), _internalFrames->get(5), _internalFrames->get(4) };
            output_lighting.push_back( ImageSampler::create(1024,1024,4,Vec4(0.0)) );

            input_bloom = { _internalFrames->get(1) };
            output_bloom.push_back(ImageSampler::create(1024,1024,4,Vec4(0.0)));

            input_env.push_back( _internalFrames->get(3) );
            input_env.push_back( _internalFrames->get(5) );
            input_env.push_back( _internalFrames->get(4) );
            output_env.push_back(ImageSampler::create(1024,1024,4,Vec4(1.0)));

            // input_shadow_cast.push_back( _internalFrames->get(3) );
            input_shadow_cast.push_back( _internalFrames->get(5) );
            // input_shadow_cast.push_back( _internalFrames->get(0) );
            output_shadow_cast.push_back(ImageSampler::create(1024,1024,4,Vec4(0.0)));


            input_blendSha.push_back(output_lighting[0]);
            input_blendSha.push_back(output_shadow_cast[0]);
            output_blendSha.push_back(ImageSampler::create(1024,1024,4,Vec4(0.0)));

            input_blendTmp.push_back(_internalFrames->get(0));
            input_blendTmp.push_back(output_blendSha[0]);
            output_blendTmp.push_back(ImageSampler::create(1024,1024,4,Vec4(0.0)));

            input_blend.push_back(output_blendTmp[0]);
            input_blend.push_back(output_bloom[0]);
            output_blend.push_back(ImageSampler::create(1024,1024,4,Vec4(0.0)));

            _bloomFX->setup( input_bloom, output_bloom );
            blendAll->setup(input_blend, output_blend);
            blendTmp->setup(input_blendTmp, output_blendTmp);
            blendSha->setup(input_blendSha, output_blendSha);

            if(!_direct && _targets->count() > 0) final_output.push_back( _targets->get(0) );
            toScreen->setup(output_blend, final_output);
            environFX->setup(input_env, output_env);
            environFX->setCubeMap(environment);
            shadowFX->setup(input_shadow_cast, output_shadow_cast);
            shadowFX->setCubeMap(shadows);
            lighting->setup(input_lighting, output_lighting);
        }



        switch(getOutputFrame())
        {
        case RenderFrame_Default:
            toScreen->setInput( output_blend[0] ) ;
            break;
        case RenderFrame_Color:
            toScreen->setInput( _internalFrames->get(0) ) ;
            break;
        case RenderFrame_ShadowMap:
            toScreen->setInput( output_shadow_cast[0] ) ;
            break;
        case RenderFrame_Environment:
            toScreen->setInput( output_env[0] ) ;
            break;
        case RenderFrame_Lighting:
            toScreen->setInput( output_lighting[0] ) ;
            break;
        case RenderFrame_Depth:
            toScreen->setInput( _internalFrames->get(5) ) ;
            break;
        case RenderFrame_Emissive:
            toScreen->setInput( _internalFrames->get(1) ) ;
            break;
        case RenderFrame_Bloom:
            toScreen->setInput( output_bloom[0] ) ;
            break;
        case RenderFrame_Normals:
            toScreen->setInput( _internalFrames->get(3) ) ;
            break;
        case RenderFrame_Metalness:
            toScreen->setInput( _internalFrames->get(4) ) ;
            break;
        default: // assume RenderFrame_Default
            toScreen->setInput( output_blend[0] ) ;
            break;
        }


        _bloomFX->apply(this);

        if(camera)
        {
            environFX->setCameraPos(camera->getAbsolutePosition());
            shadowFX->setCameraPos(camera->getAbsolutePosition());
            shadowFX->setView(camera->getViewMatrix());
            lighting->setCameraPos(camera->getAbsolutePosition());
        }
        else
        {
            environFX->setCameraPos(Vec3(0.0));
            shadowFX->setCameraPos(Vec3(0.0));
            lighting->setCameraPos(Vec3(0.0));
        }
        if(queue->_lights.size() > 0)
        {
            Vec3 wPos = queue->_lights[0]->_worldPosition;
            Vec3 vPos = Vec3(Vec4(wPos,1.0) * camera->getViewMatrix());
            lighting->setLightPos( vPos );
            shadowFX->setLightPos( wPos );
            // shadowFX->setLightPos( vPos );
        }
        environFX->apply(this);
        shadowFX->apply(this);
        lighting->apply(this);
        blendSha->apply(this);
        blendTmp->apply(this);
        blendAll->apply(this);
        toScreen->apply(this);


        return;
    }



    // if(!_direct && _renderTargets)
    // {
    //     _renderPlugin->init(_renderTargets);
    //     _renderPlugin->bind(_renderTargets);
    //     _renderTargets->change();
    // }
    // else
    // {
    //     _renderPlugin->unbind();
    // }
}


//---------------------------------------------------------------
void GlRenderer::applyState(const State::Ptr& state,
                            SceneRenderer::RenderFrame renderPass,
                            ReflexionModel::Ptr overrideShader)
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
    if(overrideShader) reflexion = overrideShader;

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
