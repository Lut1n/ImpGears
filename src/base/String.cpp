#include "base/String.h"
#include <cstring>

IMPGEARS_BEGIN

//--------------------------------------------------------------
String::String():
	m_value(IMP_NULL)
{
}

//--------------------------------------------------------------
String::String(const char* value):
	m_value(IMP_NULL)
{
	setValue(value);
}

//--------------------------------------------------------------
String::String(const String& other)
{
	setValue(other.getValue());
}

//--------------------------------------------------------------
String::~String()
{
	setValue(IMP_NULL);
}

//--------------------------------------------------------------
const String& String::operator=(const String& other)
{
	setValue(other.getValue());

	return *this;
}

//--------------------------------------------------------------
void String::setValue(const char* value)
{
	if(m_value != IMP_NULL)
		delete [] m_value;

	if(value != IMP_NULL)
	{
		m_value = new char[strlen(value)+1];
		strcpy(m_value, value);
	}
}

//--------------------------------------------------------------
const char* String::getValue() const
{
	return m_value;
}

//--------------------------------------------------------------
Uint32 String::getSize() const
{
	return strlen(m_value);
}

//--------------------------------------------------------------
void String::removeSpaces()
{
	char* buffer = new char[getSize()+1];

	Uint32 bufferPos = 0;
	for(Uint32 c=0; c<getSize(); ++c)
	{
		if(m_value[c] != ' ' && m_value[c] != '\t')
			buffer[bufferPos++] = m_value[c];
	}

	buffer[bufferPos] = '\0';

	setValue(buffer);

	delete [] buffer;
}

//--------------------------------------------------------------
Uint32 String::countOccurrence(char c) const
{
	Uint32 result = 0;
	for(Uint32 i=0; i<getSize(); ++i)
	{
		if(m_value[i] == c)
			++result;
	}

	return result;
}

//--------------------------------------------------------------
void String::split(char sep, String** array, Uint32& count) const
{
	count = countOccurrence(sep)+1;
	if(*array == IMP_NULL)
		*array = new String[count];

	Uint32 stringID = 0;
	Uint32 pos = 0;
	for(Uint32 i=1; i<getSize()+1; ++i)
	{
		if(m_value[i] == sep || m_value[i] == '\0')
		{
			char* buff = new char[i-pos];
			memcpy(buff, &m_value[pos], i-pos);
			(*array)[stringID++].setValue(buff);
			delete buff;
			pos = i+1;
		}
	}
}

//--------------------------------------------------------------
char String::getChar(Uint32 position) const
{
	if(m_value != IMP_NULL)
		return m_value[position];

	return 0;
}

//--------------------------------------------------------------
Uint32 String::find(char c) const
{
	for(Uint32 i=0; i<getSize(); ++i)
	{
		if(m_value[i] == c)
			return i;
	}

	return 0;
}

IMPGEARS_END
