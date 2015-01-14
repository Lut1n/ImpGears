#ifndef IGPATTERN_H
#define IGPATTERN_H

#include "../../base/impBase.hpp"
#include "../../io/IGStreamable.h"

class IGPattern : public IGStreamable
{
	public:
		IGPattern();
		virtual ~IGPattern();

    virtual void Load(IGParser* _parser);
    virtual void Save(IGParser* _parser);

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

#endif // IGPATTERN_H
