#ifndef IMP_SCENEVISITOR_H
#define IMP_SCENEVISITOR_H

#include <SceneGraph/Visitor.h>
#include <SceneGraph/SceneNode.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/State.h>
#include <Core/Matrix4.h>

#include <vector>

IMPGEARS_BEGIN

class IMP_API SceneVisitor : public Visitor<SceneNode*>
{
public:

	Meta_Class(SceneVisitor)
	
	using MatrixStack = std::vector<Matrix4>;
	using StateStack = std::vector<State::Ptr>;

	SceneVisitor();
	virtual ~SceneVisitor();

	virtual void apply( SceneNode* node );
	virtual void applyDefault( SceneNode* node );
	virtual void applyCamera( Camera* node );
	virtual void push( SceneNode* node );
	virtual void pop();

protected:

	MatrixStack _matrices;
	StateStack _states;
	Uniform::Ptr u_proj;
	Uniform::Ptr u_view;
	Uniform::Ptr u_model;
};

IMPGEARS_END

#endif // IMP_SCENEVISITOR_H
