#ifndef IMP_STRING_H
#define IMP_STRING_H

#include "impBase.h"

IMPGEARS_BEGIN

class IMP_API String
{
	public:
		String();
		String(const char* value);
		String(const String& other);
		virtual ~String();

		const String& operator=(const String& other);

		void setValue(const char* value);
		const char* getValue() const;

		Uint32 getSize() const;

		void removeSpaces();

		Uint32 countOccurrence(char c) const;
		void split(char sep, String** array, Uint32& count) const;

		char getChar(Uint32 position) const;
		Uint32 find(char c) const;

	protected:
	private:

		char* m_value;
};

IMPGEARS_END

#endif // IMP_STRING_H
