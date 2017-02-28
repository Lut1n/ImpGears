#ifndef IMP_GUI_STATE_H
#define IMP_GUI_STATE_H

#include "Core/impBase.h"

IMPGEARS_BEGIN

class IMP_API GuiState
{
	public:

	GuiState();
	
	virtual ~GuiState();

	static GuiState* getInstance();

	void setResolution(float resX, float resY) {_resolutionX = resX; _resolutionY = resY;}
	void getResolution(float& resX, float& resY) const { resX=_resolutionX; resY=_resolutionY; }

	private:

	static GuiState* _instance;

	float _resolutionX;
	float _resolutionY;

};


IMPGEARS_END

#endif // IMP_GUI_STATE_H
