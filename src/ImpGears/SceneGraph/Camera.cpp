#include <SceneGraph/Camera.h>

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
	Matrix4 modelMat = getModelMatrix();
	Vec3 finalPos( modelMat(3,0),modelMat(3,1),modelMat(3,2) );
	Vec3 finalTarget = finalPos + _target - getPosition();
	
	_viewMatrix = Matrix4::view(finalPos, finalTarget, _upDir);
}

//--------------------------------------------------------------
void Camera::render()
{
}

IMPGEARS_END
