#include <SceneGraph/Camera.h>

#include <SceneGraph/GraphicRenderer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Camera* Camera::s_activeCamera = nullptr;

//--------------------------------------------------------------
Camera::Camera(bool active,Vec3 up)
{
	_upDir = up;
	if(active) activate();
}

//--------------------------------------------------------------
Camera::~Camera()
{
}

//--------------------------------------------------------------
void Camera::lookAt()
{
	Matrix4 transform =
		Matrix4::getScaleMat(scale.x(), scale.y(), scale.z())
		* Matrix4::getRotationMat(getRx(), getRy(), getRz());
	Vec3 abs_pos = getPosition() * getModelMatrix();
	Vec3 abs_tgt = _target * getModelMatrix();
	_viewMatrix = Matrix4::getViewMat(abs_pos, abs_tgt, _upDir);
}

//--------------------------------------------------------------
void Camera::render()
{
	// lookAt();

	GraphicRenderer* renderer = GraphicRenderer::getInstance();
	GraphicStatesManager& states = renderer->getStateManager();

	if(states.getShader())
		states.getShader()->setView( _viewMatrix );
}

IMPGEARS_END
