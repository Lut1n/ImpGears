#include <iostream>
#include <SceneGraph/GraphicState.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GraphicStatesManager::GraphicStatesManager()
{
}

//--------------------------------------------------------------
GraphicStatesManager::~GraphicStatesManager()
{
}

//--------------------------------------------------------------
RenderTarget::Ptr GraphicStatesManager::getTarget()
{
	RenderTarget::Ptr res = nullptr;
	if(_stack.size() > 0) res = _stack.back()->getTarget();
	return res;
}

//--------------------------------------------------------------
RenderParameters::Ptr GraphicStatesManager::getParameters()
{
	RenderParameters::Ptr res;
	if(_stack.size() > 0) res = _stack.back();
	return res;
}

//--------------------------------------------------------------
Shader::Ptr GraphicStatesManager::getShader()
{
	Shader::Ptr res;
	if(_stack.size() > 0) res = _stack.back()->getShader();
	return res;
}

//--------------------------------------------------------------
void GraphicStatesManager::pushState(RenderParameters::Ptr st)
{ 
	RenderParameters::Ptr state = RenderParameters::create();
	if(_stack.size() > 0) state->clone(_stack.back(),RenderParameters::CloneOpt_All);
	state->clone(st,RenderParameters::CloneOpt_IfChanged);
	_stack.push_back(state);
}

//--------------------------------------------------------------
void GraphicStatesManager::popState()
{
	_stack.pop_back();
}

//--------------------------------------------------------------
void GraphicStatesManager::applyCurrentState()
{	
	if(_stack.size() > 0 && _stack.back() != nullptr)
	{
		_stack.back()->apply();
	}
}

IMPGEARS_END
