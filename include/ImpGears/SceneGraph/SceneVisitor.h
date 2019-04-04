#ifndef IMP_SCENEVISITOR_H
#define IMP_SCENEVISITOR_H

#include <SceneGraph/Visitor.h>
#include <SceneGraph/SceneNode.h>

#include <vector>

IMPGEARS_BEGIN

class IMP_API SceneVisitor : public Visitor<SceneNode*>
{
public:

	Meta_Class(SceneVisitor)

	SceneVisitor();
	SceneVisitor(GraphicStatesManager* mngr);
	virtual ~SceneVisitor();
	
	void setStateManager(GraphicStatesManager* mngr) { _stateMngr=mngr; }

	virtual void apply( SceneNode* node );
	
	// temporary
	void synchronizeStack();

protected:
	
	GraphicStatesManager* _stateMngr;
};

IMPGEARS_END

#endif // IMP_SCENEVISITOR_H
