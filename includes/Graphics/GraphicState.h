#ifndef IMP_GRAPHICSTATE_H
#define IMP_GRAPHICSTATE_H

#include "Core/impBase.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderParameters.h"
#include "Graphics/Shader.h"

#include <vector>
#include <memory>


IMPGEARS_BEGIN

//--------------------------------------------------------------
struct IMP_API GraphicState
{
	std::shared_ptr<imp::RenderTarget>			_target;
	std::shared_ptr<imp::RenderParameters> 	_parameters;
	std::shared_ptr<imp::Shader> 					_shader;
	
	GraphicState();
	
	GraphicState(imp::RenderTarget* target, imp::RenderParameters* params, imp::Shader* shader);
};


//--------------------------------------------------------------
class IMP_API GraphicStatesManager
{
	public:
	
	GraphicStatesManager();
	
	virtual ~GraphicStatesManager();

	imp::RenderTarget* getTarget();

	imp::RenderParameters* getParameters();

	imp::Shader* getShader();

	void pushState(GraphicState* st);

	void popState();
	protected:

	std::vector< std::shared_ptr<GraphicState> > _stack;
};

IMPGEARS_END

#endif // IMP_GRAPHICSTATE_H