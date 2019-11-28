#include <OGLPlugin/GlRenderer.h>
#include <OGLPlugin/CubeMap.h>

#include <cstdlib>

#include <OGLPlugin/BloomFX.h>
#include <OGLPlugin/EnvironmentFX.h>
#include <OGLPlugin/FrameToScreen.h>
#include <OGLPlugin/BlendAll.h>

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
    if(id <0 || id>=(int)_targets.size()) return nullptr;

    ImageSampler::Ptr sampler = _renderTargets->get(id);
    if(dlFromGPU)
    {
        _renderPlugin->bringBack( sampler );
    }

    return sampler->getSource();
}

//---------------------------------------------------------------
void GlRenderer::applyRenderVisitor(const Graph::Ptr& scene, Camera::Ptr overrideCamera)
{
    Visitor::Ptr visitor = _visitor;
    _visitor->reset();
    scene->accept(visitor);

    RenderQueue::Ptr queue = _visitor->getQueue();
    Matrix4 view;
    if(overrideCamera) view = overrideCamera->getViewMatrix();
    else if(queue->_camera) view = queue->_camera->getViewMatrix();

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

            if(mat->_baseColor)
                    queue->_states[i]->setUniform("u_sampler_color", mat->_baseColor, 0);
            if(mat->_normalmap)
                    queue->_states[i]->setUniform("u_sampler_normal", mat->_normalmap, 1);
            if(mat->_emissive)
                    queue->_states[i]->setUniform("u_sampler_emissive", mat->_emissive, 2);

            queue->_states[i]->setUniform("u_view", view);

            // ---- if we want a normal matrix for view space instead of model space ----
            // Matrix4 model = queue->_states[i]->getUniforms()["u_model"]->getMat4();
            // Matrix3 normalMat = Matrix3(model * view).inverse().transpose();
            // queue->_states[i]->setUniform("u_normal", normalMat );

            queue->_states[i]->setUniform("u_lightPos", lightPos);
            queue->_states[i]->setUniform("u_lightCol", lightCol);
            queue->_states[i]->setUniform("u_lightAtt", latt);
            queue->_states[i]->setUniform("u_color", color);
            applyState(queue->_states[i]);
            drawGeometry(geode);
        }
        else if(clear)
        {
            applyClear(clear);
        }
    }

    _renderPlugin->unbind();
}

//---------------------------------------------------------------
void GlRenderer::render(const Graph::Ptr& scene)
{
    if(_renderPlugin == nullptr) return;

    if(_renderTargets == nullptr && _targets.size() > 0)
    {
        std::vector<ImageSampler::Ptr> samplers(_targets.size());
        for(int i=0;i<(int)_targets.size();++i)
            samplers[i] = ImageSampler::create(_targets[i]);

        _renderTargets = RenderTarget::create();
        _renderTargets->build(samplers, true);
    }


    static EnvironmentFX::Ptr environment;
    Graph::Ptr cloned = scene;
    if(isFeatureEnabled(Feature_Shadow))
    {
        if(!environment)
        {
            environment = EnvironmentFX::create();
            environment->setup();
        }
        cloned = environment->begin(this, scene);

    }


    if(!_direct && _renderTargets)
    {
        _renderPlugin->init(_renderTargets);
        _renderPlugin->bind(_renderTargets);
        _renderTargets->change();
    }
    else
    {
        _renderPlugin->unbind();
    }

    applyRenderVisitor(cloned);

    if(isFeatureEnabled(Feature_Shadow))
    {
        environment->end(this, scene);
    }


    if(!_direct && isFeatureEnabled(Feature_Bloom))
    {
        static std::vector<ImageSampler::Ptr> input_bloom;
        static std::vector<ImageSampler::Ptr> output_bloom;
        static std::vector<ImageSampler::Ptr> input_blend;
        static std::vector<ImageSampler::Ptr> output_blend;
        static std::vector<ImageSampler::Ptr> empty;
        static BlendAll::Ptr blendAll;
        static FrameToScreen::Ptr toScreen;


        if(_bloomFX == nullptr)
        {
            _bloomFX = new BloomFX();
            blendAll = BlendAll::create();
            toScreen = FrameToScreen::create();

            input_bloom = { _renderTargets->get(0), _renderTargets->get(1) };
            ImageSampler::Ptr sample = ImageSampler::create(512,512,4,Vec4(0.0));
            output_bloom.push_back(sample);
            input_blend.push_back(_renderTargets->get(0));
            input_blend.push_back(sample);
            output_blend.push_back(ImageSampler::create(512,512,4,Vec4(0.0)));

            _bloomFX->setup( input_bloom, output_bloom );
            blendAll->setup(input_blend, output_blend);
            toScreen->setup(output_blend, empty);

            // toScreen->setup(_renderTargets->getList(), empty);
        }

        switch(getOutputFrame())
        {
        case RenderFrame_Default:
            toScreen->setInput( output_blend[0] ) ;
            break;
        case RenderFrame_ShadowMap:
            toScreen->setInput( output_blend[0] ) ;
            break;
        case RenderFrame_Environment:
            toScreen->setInput( output_blend[0] ) ;
            break;
        case RenderFrame_Lighting:
            toScreen->setInput( _renderTargets->get(0) ) ;
            break;
        case RenderFrame_Depth:
            toScreen->setInput( output_blend[0] ) ;
            break;
        case RenderFrame_Emissive:
            toScreen->setInput( _renderTargets->get(1) ) ;
            break;
        case RenderFrame_Bloom:
            toScreen->setInput( output_bloom[0] ) ;
            break;
        case RenderFrame_Normals:
            toScreen->setInput( output_blend[0] ) ;
            break;
        case RenderFrame_Metalness:
            toScreen->setInput( output_blend[0] ) ;
            break;
        default: // assume RenderFrame_Default
            toScreen->setInput( output_blend[0] ) ;
            break;
        }


        _bloomFX->apply(this);
        blendAll->apply(this);
        toScreen->apply(this);


        return;
    }
}


//---------------------------------------------------------------
void GlRenderer::applyState(const State::Ptr& state)
{
    if(_renderPlugin == nullptr) return;

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
void GlRenderer::applyClear(ClearNode* clearNode)
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
