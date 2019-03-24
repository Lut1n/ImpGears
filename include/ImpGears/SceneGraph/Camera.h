#ifndef IMP_CAMERA_H
#define IMP_CAMERA_H

#include <SceneGraph/SceneNode.h>

IMPGEARS_BEGIN

/// \brief Defines a scene camera.
class IMP_API Camera : public SceneNode
{
public:
	
	Meta_Class(Camera)
	
	Camera(bool active = true, Vec3 up=Vec3(0.0,0.0,1.0));
	virtual ~Camera();

	virtual void render();
	virtual void lookAt();

	void activate() {s_activeCamera = this;}
	Vec3 getUpVector() const { return _upDir; }
	void setTarget(const imp::Vec3& target){_target=target;}
	const Matrix4& getViewMatrix() const{return _viewMatrix;}
	
	static Camera* getActiveCamera() {return s_activeCamera;}

private:

	Vec3 _upDir;
	Vec3 _target;
	Matrix4 _viewMatrix;

	static Camera* s_activeCamera;
};

IMPGEARS_END

#endif // IMP_CAMERA_H
