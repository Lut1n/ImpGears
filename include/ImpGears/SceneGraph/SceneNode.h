#ifndef IMP_SCENENODE_H
#define IMP_SCENENODE_H

#include <Core/Object.h>
#include <Core/Vec3.h>
#include <Core/Matrix4.h>
#include <SceneGraph/Visitor.h>
#include <SceneGraph/State.h>

#include <list>

IMPGEARS_BEGIN

/// \brief Defines a scene node. Has to be added to a scene for rendering.
class IMP_API SceneNode : public Object
{
public:

	Meta_Class(SceneNode)

	using NodeList = std::list<SceneNode::Ptr>;

	SceneNode();
	virtual ~SceneNode();

	virtual void update() {}
	virtual void render() {};

	void setPosition(const Vec3& position) { _position=position; }
	void setRotation(const Vec3& rotation) { _rotation=rotation; }
	void setScale(const Vec3& scale) { _scale=scale; }

	const Vec3& getPosition() const {return _position;}
	const Vec3& getRotation() const {return _rotation;}
	const Vec3& getScale() const {return _scale;}
	
	void addNode(const SceneNode::Ptr& sceneNode);
	void remNode(const SceneNode::Ptr& sceneNode);
	
	void accept( Visitor<SceneNode*>::Ptr& visitor );
	
	void computeMatrices();

	const Matrix4& getModelMatrix() const { return _modelMatrix; }
	const Matrix4& getNormalMatrix() const { return _normalMatrix; }
	
	virtual State::Ptr getState() { return _state; }

protected:
	
	State::Ptr _state;
	Vec3 _position;
	Vec3 _rotation;
	Vec3 _scale;
	
	Matrix4 _modelMatrix;
	Matrix4 _normalMatrix;
	
	NodeList _children;
};

IMPGEARS_END

#endif // IMP_SCENENODE_H
