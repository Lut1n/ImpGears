/*
io/IGParser.h

Define an object for reading from and writing to a file.

Currently works only with binary files. Next step is to add support for xml files.
*/

#ifndef IGPARSER_H
#define IGPARSER_H

#include "cstdio"

#include "../base/impBase.hpp"

#define IGPARSER_READWRITE_DEC(T)\
    imp::Uint32 Read(T* _buffer, imp::Uint32 _size);\
    void Write(const T* _buffer, imp::Uint32 _size);\

enum IGAccessMode
{
    IGAccessMode_Read,
    IGAccessMode_Write,
    IGAccessMode_Append
};

enum IGFileType
{
    IGFileType_Text,
    IGFileType_Binary
};

class IGParser
{
private:

    FILE* stream;
    IGAccessMode accessMode;
    IGFileType fileType;

public:

    IGParser(const char* _filename, IGFileType _type, IGAccessMode _mode);
    ~IGParser();

    IGPARSER_READWRITE_DEC(char)

    IGPARSER_READWRITE_DEC(imp::Int8)

    IGPARSER_READWRITE_DEC(imp::Uint8)

    IGPARSER_READWRITE_DEC(float)
};

#endif // IGPARSER_H
