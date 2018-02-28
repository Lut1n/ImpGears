#ifndef IMP_GRAPHICSTATE_H
#define IMP_GRAPHICSTATE_H

#include <Core/Object.h>
#include <SceneGraph/RenderTarget.h>
#include <SceneGraph/RenderParameters.h>
#include <SceneGraph/Shader.h>

#include <vector>
#include <memory>


IMPGEARS_BEGIN

//--------------------------------------------------------------
struct IMP_API GraphicState : public Object
{
	RenderTarget::Ptr			_target;
	RenderParameters::Ptr 	_parameters;
	Shader::Ptr 					_shader;
	
	Meta_Class(GraphicState)
	
	GraphicState();
	
	void setTarget(const RenderTarget::Ptr& target);
	void setParameters(const RenderParameters::Ptr& params);
	void setShader(const Shader::Ptr& shader);
};


//--------------------------------------------------------------
class IMP_API GraphicStatesManager : public Object
{
	public:
	
	Meta_Class(GraphicStatesManager)
	
	GraphicStatesManager();
	
	virtual ~GraphicStatesManager();

	imp::RenderTarget* getTarget();

	imp::RenderParameters* getParameters();

	imp::Shader* getShader();

	void pushState( GraphicState* st);

	void popState();
	
	void applyCurrentState();
	
	void revert(GraphicState* lastState);
	
	protected:

	std::vector< GraphicState* > _stack;
};

IMPGEARS_END

#endif // IMP_GRAPHICSTATE_H