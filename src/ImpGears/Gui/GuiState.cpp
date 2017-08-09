#include "Gui/GuiState.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiState* GuiState::_instance = IMP_NULL;

//--------------------------------------------------------------
GuiState::GuiState()
	: _resolutionX(800.0)
	, _resolutionY(600.0)
{
}

//--------------------------------------------------------------
GuiState::~GuiState()
{
}

//--------------------------------------------------------------
GuiState* GuiState::getInstance()
{
	if(_instance == IMP_NULL)
	{
		_instance = new GuiState();
	}

	return _instance;
}

IMPGEARS_END
