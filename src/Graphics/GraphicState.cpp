
#include "Graphics/GraphicState.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GraphicState::GraphicState()
	: _target(IMP_NULL)
	, _parameters(IMP_NULL)
	, _shader(IMP_NULL)
{};

//--------------------------------------------------------------
GraphicState::GraphicState(imp::RenderTarget* target, imp::RenderParameters* params, imp::Shader* shader)
	: _target(target)
	, _parameters(params)
	, _shader(shader)
{};

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
	std::vector<std::shared_ptr<GraphicState> >::reverse_iterator it = _stack.rbegin();
	for(it = _stack.rbegin(); it != _stack.rend(); it++)
	{
		GraphicState* st = it->get();
		if(st && st->_target != nullptr)
			return st->_target.get();
	}
	return nullptr;
}

//--------------------------------------------------------------
imp::RenderParameters* GraphicStatesManager::getParameters()
{
	std::vector< std::shared_ptr<GraphicState> >::reverse_iterator it = _stack.rbegin();
	for(it = _stack.rbegin(); it != _stack.rend(); it++)
	{
		GraphicState* st = it->get();
		if(st && st->_parameters != nullptr)
			return st->_parameters.get();
	}
	return nullptr;
}

//--------------------------------------------------------------
imp::Shader* GraphicStatesManager::getShader()
{
	std::vector< std::shared_ptr<GraphicState> >::reverse_iterator it = _stack.rbegin();
	for(it = _stack.rbegin(); it != _stack.rend(); it++)
	{
		GraphicState* st = it->get();
		if(st && st->_shader != nullptr)
			return st->_shader.get();
	}
	return nullptr;
}

//--------------------------------------------------------------
void GraphicStatesManager::pushState(GraphicState* st)
{ 
	_stack.push_back( std::shared_ptr<GraphicState>(st) );
}

//--------------------------------------------------------------
void GraphicStatesManager::popState()
{
	_stack.pop_back();
}

IMPGEARS_END