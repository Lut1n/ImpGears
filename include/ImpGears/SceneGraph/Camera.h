#ifndef IMP_CAMERA_H
#define IMP_CAMERA_H

#include <SceneGraph/Node.h>

IMPGEARS_BEGIN

class IMP_API Camera : public Node
{
public:
	
	Meta_Class(Camera)
	
	Camera(Vec3 up=Vec3(0.0,0.0,1.0),bool active = true);
	virtual ~Camera();

	virtual void lookAt();

	void activate() {s_activeCamera = this;}
	const Vec3& getUpDir() const { return _upDir; }
	void setTarget(const imp::Vec3& target){_target=target;}
	const Matrix4& getViewMatrix() const{return _viewMatrix;}
	void setAbsolutePosition(const Vec3& pos) { _absolutePosition=pos; }
	
	static Camera* getActiveCamera() {return s_activeCamera;}
	
	const Vec3& getAbsolutePosition() const {return _absolutePosition;}

private:

	Vec3 _upDir;
	Vec3 _absolutePosition;
	Vec3 _target;
	Matrix4 _viewMatrix;

	static Camera* s_activeCamera;
};

IMPGEARS_END

#endif // IMP_CAMERA_H
