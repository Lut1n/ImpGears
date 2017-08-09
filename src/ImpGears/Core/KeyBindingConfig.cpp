#include "Core/KeyBindingConfig.h"

#include <cstring>

IMPGEARS_BEGIN

void parseKeyMap(const char* line, char* name, Uint32* code)
{
	strcpy(name, line);
	const char* codePtr = IMP_NULL;
	Uint32 len = strlen(line);

	for(Uint32 c=0; c<len; ++c)
	{
		if(line[c] == '=')
		{
			codePtr = &line[c+1];
			name[c] = '\0';
			break;
		}		
	}

	sscanf(codePtr, "%d", code);
}

//--------------------------------------------------------------
KeyBindingConfig::KeyBindingConfig()
{
}

//--------------------------------------------------------------
KeyBindingConfig::~KeyBindingConfig()
{
}

//--------------------------------------------------------------
void KeyBindingConfig::Load(Parser* _parser)
{
	char name[512];
	Uint32 keycode;

	String line;
	while(!_parser->isEnd() && _parser->readLine(line))
	{
		line.removeSpaces();

		if(line.getSize() > 3)
		{
			parseKeyMap(line.getValue(), name, &keycode);
			m_keysMap[name] = keycode;
		}
	}
}

//--------------------------------------------------------------
void KeyBindingConfig::Save(Parser* _parser)
{
	// implementation needed
}

//--------------------------------------------------------------
Uint32 KeyBindingConfig::getKey(const char* name) const
{
	return m_keysMap.at(name);
}

IMPGEARS_END
