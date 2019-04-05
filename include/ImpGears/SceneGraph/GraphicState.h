#ifndef IMP_GRAPHICSTATE_H
#define IMP_GRAPHICSTATE_H

#include <Core/Object.h>
#include <SceneGraph/RenderParameters.h>

#include <vector>
// #include <memory>


IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GraphicStatesManager : public Object
{
	public:
	
	Meta_Class(GraphicStatesManager)
	
	GraphicStatesManager();
	
	virtual ~GraphicStatesManager();

	RenderTarget::Ptr getTarget();

	RenderParameters::Ptr getParameters();

	Shader::Ptr getShader();

	void pushState( RenderParameters::Ptr st);

	void popState();
	
	void applyCurrentState();
	
	int size() const { return _stack.size(); }
	
	protected:

	std::vector< RenderParameters::Ptr > _stack;
};

IMPGEARS_END

#endif // IMP_GRAPHICSTATE_H
