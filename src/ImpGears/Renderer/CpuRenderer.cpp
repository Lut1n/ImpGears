#include <Renderer/CpuRenderer.h>

#include <Renderer/CpuBlinnPhong.h>

#include "LightModelImpl.h"

#include <cstdlib>

IMPGEARS_BEGIN

//--------------------------------------------------------------
CpuRenderer::CpuRenderer()
    : SceneRenderer()
{
    imp::Vec4 vp(0.0,0.0,512.0,512.0);
    Image::Ptr defaultTarget = Image::create((int)vp[2] , (int)vp[3], 4);
    _geoRenderer.setViewport( vp );
    _geoRenderer.setTarget(0,defaultTarget, Vec4(0.0));
}

//--------------------------------------------------------------
CpuRenderer::~CpuRenderer()
{
}

//---------------------------------------------------------------
void CpuRenderer::render(const Graph::Ptr& scene)
{
    Visitor::Ptr visitor = _visitor;
    _visitor->reset();
    scene->accept(visitor);

    RenderQueue::Ptr queue = _visitor->getQueue();
    Matrix4 view;
    if(queue->_camera) view = queue->_camera->getViewMatrix();

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
}


//--------------------------------------------------------------
int prepareReflexion(GeometryRenderer& _geoRenderer, ReflexionModel::Ptr& program)
{
    ReflexionModel::AbstractFrag::Ptr frag = ReflexionModel::AbstractFrag::create();

    GeometryRenderer::VertCallback::Ptr vertCb = DefaultVertCb::create();
    FragCallback::Ptr fragCb = frag;

    ReflexionModel::Lighting li = program->getLighting();
    if(li == ReflexionModel::Lighting_None) frag->_lighting = NoLighting::create();
    else if(li == ReflexionModel::Lighting_Phong) frag->_lighting = PhongLighting::create();
    else frag->_lighting = program->_lightingCb;

    ReflexionModel::Texturing te = program->getTexturing();
    if(te == ReflexionModel::Texturing_PlainColor) frag->_texturing = PlainColorCb::create();
    else if(te == ReflexionModel::Texturing_Samplers_CN) frag->_texturing = SamplerCbCN::create();
    else if(te == ReflexionModel::Texturing_Samplers_CNE) frag->_texturing = SamplerCbCNE::create();
    else frag->_texturing = program->_texturingCb;

    ReflexionModel::MRT mrt = program->getMRT();
    if(mrt == ReflexionModel::MRT_1_Col) frag->_mrt = Mrt1ColorCb::create();
    else if(mrt == ReflexionModel::MRT_2_Col_Emi) frag->_mrt = Mrt2ColorEmiCb::create();
    else frag->_mrt = program->_mrtCb;

    if(vertCb == nullptr) _geoRenderer.setDefaultVertCallback();
    else _geoRenderer.setVertCallback(vertCb);
    if(fragCb == nullptr) _geoRenderer.setDefaultFragCallback();
    else _geoRenderer.setFragCallback(fragCb);
    return 0;
}

void updateUniform(GeometryRenderer& geoRenderer, Uniform::Ptr& uniform)
{
    std::string name = uniform->getID();
    Uniform::Type type = uniform->getType();

    if(type == Uniform::Type_Mat4)
    {
        if(name == "u_proj") geoRenderer.setProj( uniform->getMat4() );
        else if(name == "u_view") geoRenderer.setView( uniform->getMat4() );
        else if(name == "u_model") geoRenderer.setModel( uniform->getMat4() );
        // todo
    }
    else
    {
        geoRenderer.setUniform( uniform );
    }
}

//---------------------------------------------------------------
void CpuRenderer::applyState(const State::Ptr& state)
{
    if(state->getFaceCullingMode() == State::FaceCullingMode_None)
        _geoRenderer.setCullMode(GeometryRenderer::Cull_None);
    else if(state->getFaceCullingMode() == State::FaceCullingMode_Back)
        _geoRenderer.setCullMode(GeometryRenderer::Cull_Back);
    else if(state->getFaceCullingMode() == State::FaceCullingMode_Front)
        _geoRenderer.setCullMode(GeometryRenderer::Cull_Front);

    _geoRenderer.setViewport(state->getViewport());

    if(!_direct)
    {
        for(int i=0;i<(int)_targets.size();++i)
            _geoRenderer.setTarget(i,_targets[i],Vec4(0.0));
    }

    ReflexionModel::Ptr reflexion = state->getReflexion();
    prepareReflexion(_geoRenderer,reflexion);

    const std::map<std::string,Uniform::Ptr>& uniforms = state->getUniforms();
    for(auto u : uniforms) updateUniform(_geoRenderer, u.second);
}

//---------------------------------------------------------------
void CpuRenderer::applyClear(ClearNode* clearNode)
{
    int count = _geoRenderer.getTargetCount();
    for(int i=0;i<count;++i)
        _geoRenderer.setClearColor( i, clearNode->getColor()*255.0 );

    _geoRenderer.init();
    _geoRenderer.clearTargets();
}

//---------------------------------------------------------------
void CpuRenderer::drawGeometry(GeoNode* geoNode)
{
    _geoRenderer.render( *(geoNode->_geo) );
}

IMPGEARS_END
