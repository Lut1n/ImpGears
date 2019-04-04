#ifndef IMP_GRAPHICRENDERER_H
#define IMP_GRAPHICRENDERER_H

#include <Core/Object.h>
#include <Core/Matrix4.h>

#include <SceneGraph/SceneNode.h>
#include <SceneGraph/RenderParameters.h>

IMPGEARS_BEGIN

/// \brief Defines The graphic renderer.
class IMP_API GraphicRenderer : public Object
{
public:

	Meta_Class(GraphicRenderer)
	
	GraphicRenderer();
	virtual ~GraphicRenderer();

	void renderScene(SceneNode::Ptr& scene);

	void setRenderParameters(RenderParameters::Ptr& parameters){_parameters = parameters; _state->setParameters(parameters);}
	RenderParameters::Ptr getRenderParameters() const {return _parameters;}
	
	GraphicStatesManager& getStateManager() {return _stateMgr;}

protected:
	
	GraphicState::Ptr _state;
	GraphicStatesManager _stateMgr;
	RenderParameters::Ptr _parameters;
};

IMPGEARS_END

#endif // IMP_GRAPHICRENDERER_H
