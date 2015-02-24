#ifndef IMP_STRING_H
#define IMP_STRING_H

#include "impBase.h"

IMPGEARS_BEGIN

class IMP_API String
{
	public:
		String();
		String(const char* value);
		virtual ~String();

		void setValue(const char* value);
		const char* getValue() const;

		Uint32 getSize() const;

	protected:
	private:

		char* m_value;
};

IMPGEARS_END

#endif // IMP_STRING_H
