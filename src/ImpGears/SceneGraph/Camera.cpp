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
	/*Matrix4 transform =
		Matrix4::scale(scale.x(), scale.y(), scale.z())
		* Matrix4::rotationX(getRx())
		* Matrix4::rotationY(getRy())
		* Matrix4::rotationZ(getRz());*/
	Vec3 abs_pos = Vec4(getPosition()) * getModelMatrix();
	Vec3 abs_tgt = Vec4(_target) * getModelMatrix();
	_viewMatrix = Matrix4::view(abs_pos, abs_tgt, _upDir);
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
