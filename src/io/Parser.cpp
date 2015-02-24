#include "io/Parser.h"

#include <cstdlib>

IMPGEARS_BEGIN

//--------------------------------------------------------------
#define IMP_PARSER_READWRITE(T)\
    imp::Uint32 Parser::Read(T* _buffer, imp::Uint32 _size)\
    {\
        return (imp::Uint32) fread((void*)_buffer, sizeof(T), _size, stream);\
    }\
    void Parser::Write(const T* _buffer, imp::Uint32 _size)\
    {\
        fwrite((const void*)_buffer, sizeof(T), _size, stream);\
    }

//--------------------------------------------------------------
Parser::Parser(const char* _filename, FileType _type, AccessMode _mode)
{
    char faccess = ' ';
    if( _mode == AccessMode_Write )
        faccess = 'w';
    else if( _mode == AccessMode_Read )
        faccess = 'r';
    else if( _mode == AccessMode_Append )
        faccess = 'a';

    char ftype = ' ';
    if( _type == FileType_Text )
        ftype = 't';
    else if( _type == FileType_Binary )
        ftype = 'b';

    char fmode[3];
    sprintf(fmode, "%c%c", faccess, ftype);

    stream = fopen(_filename, fmode);

    accessMode = _mode;
    fileType = _type;
}

//--------------------------------------------------------------
Parser::~Parser()
{
    if( accessMode == AccessMode_Write ||
        accessMode == AccessMode_Append )
            fflush(stream);

    fclose(stream);
}

//--------------------------------------------------------------
const String Parser::readLine()
{
	char* line = IMP_NULL;
	size_t lsize = 0;
	getline(&line, &lsize, stream);

	String strline(line);
	free(line);

	return strline;
}

//--------------------------------------------------------------
bool Parser::isEnd() const
{
	return (feof(stream) != 0);
}

//--------------------------------------------------------------
IMP_PARSER_READWRITE(char)

//--------------------------------------------------------------
IMP_PARSER_READWRITE(imp::Int8)

//--------------------------------------------------------------
IMP_PARSER_READWRITE(imp::Uint8)

//--------------------------------------------------------------
IMP_PARSER_READWRITE(float)

IMPGEARS_END
