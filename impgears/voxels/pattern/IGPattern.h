#ifndef IMP_PATTERN_H
#define IMP_PATTERN_H

#include "base/impBase.hpp"
#include "io/Streamable.h"

IMPGEARS_BEGIN

class Pattern : public imp::Streamable
{
	public:
		Pattern();
		virtual ~Pattern();

    virtual void Load(imp::Parser* _parser);
    virtual void Save(imp::Parser* _parser);

	imp::Uint32 GetSizeX()
	{
		return sizeX;
	}

	imp::Uint32 GetSizeY()
	{
		return sizeY;
	}

	imp::Uint32 GetSizeZ()
	{
		return sizeZ;
	}

	imp::Uint8* GetData()
	{
		return data;
	}

	protected:
	private:

	imp::Int8 sizeX, sizeY, sizeZ;

	imp::Uint8* data;
};

IMPGEARS_END

#endif // IMP_PATTERN_H
