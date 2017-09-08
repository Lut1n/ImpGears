/*
System/IGParser.h

Define an object for reading from and writing to a file.

Currently works only with binary files. Next step is to add support for xml files.
*/

#ifndef IMP_PARSER_H
#define IMP_PARSER_H

#include "cstdio"
#include <iostream>

#include "Core/impBase.h"

IMPGEARS_BEGIN

#define IMP_PARSER_READWRITE_DEC(T)\
    imp::Uint32 Read(T* _buffer, imp::Uint32 _size);\
    void Write(const T* _buffer, imp::Uint32 _size);\

class IMP_API Parser
{
public:

    enum AccessMode
    {
        AccessMode_Read,
        AccessMode_Write,
        AccessMode_Append
    };

    enum FileType
    {
        FileType_Text,
        FileType_Binary
    };

    Parser(const char* _filename, FileType _type, AccessMode _mode);
    ~Parser();

    IMP_PARSER_READWRITE_DEC(char)

    IMP_PARSER_READWRITE_DEC(imp::Int8)

    IMP_PARSER_READWRITE_DEC(imp::Uint8)

    IMP_PARSER_READWRITE_DEC(float)

	bool readLine(std::string& line, Uint32 maxLineLength = 256);
	void writeLine(const std::string& line);

	bool isEnd() const;

private:

    FILE* stream;
    AccessMode accessMode;
    FileType fileType;
};

IMPGEARS_END

#endif // IMP_PARSER_H
