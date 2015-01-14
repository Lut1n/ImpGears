/*
io/IGStreamable.h

Define an interface for reading or writing a object from or to a file.
*/

#ifndef IGSTREAMABLE_H
#define IGSTREAMABLE_H

#include "IGParser.h"

class IGStreamable
{
public:
    IGStreamable();

    virtual void Load(IGParser* _parser) = 0;
    virtual void Save(IGParser* _parser) = 0;
};

#endif // IGSTREAMABLE_H
