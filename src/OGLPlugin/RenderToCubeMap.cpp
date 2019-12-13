#include <OGLPlugin/RenderToCubeMap.h>
#include <OGLPlugin/GlError.h>
#include <OGLPlugin/GlRenderer.h>

#include <iostream>

IMPGEARS_BEGIN


//--------------------------------------------------------------
RenderToCubeMap::RenderToCubeMap(GlRenderer* renderer, CubeMapSampler::Ptr cubemap)
    : _renderer(renderer)
{
    _clone = Graph::create();
    _proj = Matrix4::perspectiveProj(90.0, 1.0, 0.1, 128.0);
    _camera = Camera::create();

    _directions[0] = Vec3(1.0,0.0,0.0);
    _directions[1] = Vec3(-1.0,0.0,0.0);
    _directions[2] = Vec3(0.0,1.0,0.0);
    _directions[3] = Vec3(0.0,-1.0,0.0);
    _directions[4] = Vec3(0.0,0.0,1.0);
    _directions[5] = Vec3(0.0,0.0,-1.0);

    _upDirections[0] = Vec3(0.0,1.0,0.0);
    _upDirections[1] = Vec3(0.0,1.0,0.0);
    _upDirections[2] = Vec3(0.0,0.0,-1.0);
    _upDirections[3] = Vec3(0.0,0.0,1.0);
    _upDirections[4] = Vec3(0.0,1.0,0.0);
    _upDirections[5] = Vec3(0.0,1.0,0.0);

    setCubeMap(cubemap);
    _state = State::create();
}

//--------------------------------------------------------------
RenderToCubeMap::~RenderToCubeMap()
{
}

//--------------------------------------------------------------
void RenderToCubeMap::setCubeMap(CubeMapSampler::Ptr cubemap)
{
    if(!cubemap || _cubemap == cubemap) return;

    _cubemap = cubemap;

    for(int i=0;i<6;++i)
    {
        RenderTarget::FaceSampler faceSampler = std::make_pair( cubemap, i );
        if(!_targets[i] || _targets[i]->getFace() != faceSampler)
        {
            _targets[i] = RenderTarget::create();
            _targets[i]->build(faceSampler, true);
            _targets[i]->setClearColor(0, Vec4(1.0));
        }
    }
}

//--------------------------------------------------------------
CubeMapSampler::Ptr RenderToCubeMap::getCubeMap()
{
    return _cubemap;
}

//--------------------------------------------------------------
void RenderToCubeMap::render(const Graph::Ptr& scene, const Vec3& center, SceneRenderer::RenderFrame frameType, ReflexionModel::Ptr overrideShader)
{
    _clone->getInitState()->clone(scene->getInitState());
    _state->setUniform("u_proj", _proj);
    _state->setViewport( Vec4(0.0,0.0,1024.0,1024.0) );
    if(overrideShader) _state->setReflexion( overrideShader );
    _clone->setRoot(scene->getRoot());

    for(int i=0;i<6;++i)
    {
        _camera->setPosition(center);
        _camera->setAbsolutePosition(center);
        _camera->setTarget( center + _directions[i] );
        _camera->setUpDir( _upDirections[i] * -1.0 );
        _camera->lookAt();

        _renderer->_renderPlugin->init(_targets[i]);
        _renderer->_renderPlugin->bind(_targets[i]);
        _targets[i]->change();

        RenderQueue::Ptr queue = _renderer->applyRenderVisitor(_clone);
        queue->_camera = _camera.get();
        _renderer->drawQueue(queue, _state, frameType);

        _renderer->_renderPlugin->unbind();
    }
}


IMPGEARS_END
