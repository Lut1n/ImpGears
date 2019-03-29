#include <iostream>
#include <SceneGraph/GraphicState.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GraphicState::GraphicState()
	: _target(nullptr)
	, _parameters(nullptr)
	, _shader(nullptr)
{};

//--------------------------------------------------------------
void GraphicState::setTarget(const imp::RenderTarget::Ptr& target)
{
	_target = target;
}

//--------------------------------------------------------------
void GraphicState::setParameters(const imp::RenderParameters::Ptr& params)
{
	_parameters = params;
}

//--------------------------------------------------------------
void GraphicState::setShader(const imp::Shader::Ptr& shader)
{
	_shader = shader;
}


//--------------------------------------------------------------
GraphicStatesManager::GraphicStatesManager()
{
}

//--------------------------------------------------------------
GraphicStatesManager::~GraphicStatesManager()
{
}

//--------------------------------------------------------------
imp::RenderTarget* GraphicStatesManager::getTarget()
{
	std::vector< GraphicState* >::reverse_iterator it = _stack.rbegin();
	for(it = _stack.rbegin(); it != _stack.rend(); it++)
	{
		GraphicState* st = (*it);
		if(st && st->_target != nullptr)
			return st->_target.get();
	}
	return nullptr;
}

//--------------------------------------------------------------
imp::RenderParameters* GraphicStatesManager::getParameters()
{
	std::vector< GraphicState* >::reverse_iterator it = _stack.rbegin();
	for(it = _stack.rbegin(); it != _stack.rend(); it++)
	{
		GraphicState* st = (*it);
		if(st && st->_parameters != nullptr)
			return st->_parameters.get();
	}
	return nullptr;
}

//--------------------------------------------------------------
imp::Shader* GraphicStatesManager::getShader()
{
	std::vector< GraphicState* >::reverse_iterator it = _stack.rbegin();
	for(it = _stack.rbegin(); it != _stack.rend(); it++)
	{
		GraphicState* st = (*it);
		if(st && st->_shader != nullptr)
			return st->_shader.get();
	}
	return nullptr;
}

//--------------------------------------------------------------
void GraphicStatesManager::pushState(GraphicState* st)
{ 
	_stack.push_back( st );
	applyCurrentState();
}

//--------------------------------------------------------------
void GraphicStatesManager::popState()
{
	GraphicState* lastState = _stack.back();
	_stack.pop_back();
    revert(lastState);
}

//--------------------------------------------------------------
void GraphicStatesManager::applyCurrentState()
{
	imp::RenderTarget* resultTarget = getTarget();
	imp::RenderParameters* resultParameters = getParameters();
	imp::Shader* resultShader = getShader();
	
	if(resultTarget != nullptr)
	{
		resultTarget->bind();
	}
	
	if(resultParameters != nullptr)
	{
		// TODO : move clear into Camera node
		resultParameters->apply(_stack.size() <= 1);
	}
	
	if(resultShader != nullptr)
	{
		resultShader->enable();
		// TODO : test each uniform
		resultShader->updateAllUniforms();
	}
}

//--------------------------------------------------------------
void GraphicStatesManager::revert(GraphicState* lastState)
{
    if(lastState == nullptr)
    {
        return;
    }
    
	imp::RenderTarget* resultTarget = getTarget();
	imp::RenderParameters* resultParameters = getParameters();
	imp::Shader* resultShader = getShader();
	
	if(lastState->_target != nullptr && resultTarget != nullptr)
	{
		resultTarget->bind();
	}
	
	if(lastState->_parameters != nullptr && resultParameters != nullptr)
	{
		resultParameters->apply(false);
	}
	
	if(lastState->_shader != nullptr && resultShader != nullptr)
	{
		resultShader->enable();
		// TODO : test each uniform
		resultShader->updateAllUniforms();
	}
}

IMPGEARS_END
