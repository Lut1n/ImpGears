#include <SceneGraph/Camera.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Camera* Camera::s_activeCamera = nullptr;

//--------------------------------------------------------------
Camera::Camera(Vec3 up,bool active)
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
	Vec3 absoluteTarget = _absolutePosition + _target - getPosition();
	_viewMatrix = Matrix4::view(_absolutePosition, absoluteTarget, _upDir);
}

//--------------------------------------------------------------
void Camera::render()
{
}

//--------------------------------------------------------------
void Camera::update()
{
}

IMPGEARS_END
