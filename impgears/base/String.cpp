#include "String.h"
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

String::~String()
{
	setValue(IMP_NULL);
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

IMPGEARS_END
