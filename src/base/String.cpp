#include "base/String.h"
#include <cstring>

IMPGEARS_BEGIN

String::String():
	m_value(IMP_NULL)
{
}

String::String(const char* value):
	m_value(IMP_NULL)
{
	setValue(value);
}

String::String(const String& other)
{
	setValue(other.getValue());
}

String::~String()
{
	setValue(IMP_NULL);
}

const String& String::operator=(const String& other)
{
	setValue(other.getValue());

	return *this;
}

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

const char* String::getValue() const
{
	return m_value;
}

Uint32 String::getSize() const
{
	return strlen(m_value);
}

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

IMPGEARS_END
