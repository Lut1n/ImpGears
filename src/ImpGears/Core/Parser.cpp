#include <Core/Parser.h>

#include <cstdlib>
#include <cstring>

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
bool Parser::readLine(std::string& line, Uint32 maxLineLength)
{
	Int32 pos0 = ftell(stream);
	char buffer[maxLineLength+1];
	Int32 read = Read(buffer, maxLineLength);

	if( read == 0 || buffer[0] == '\0' )
	{
		line = "";
		return false;
	}

	/// find '\n' character
	for(Int32 c=0; c<read; ++c)
	{
		if(buffer[c] == '\n' || buffer[c] == '\r')
		{
			buffer[c] = '\0';
			Int32 pos1 = (pos0+1) + c;
			if( c+1<read && (buffer[c+1] == '\r' || buffer[c+1] == '\n') )
				pos1 += 1;
			fseek(stream, pos1, SEEK_SET);
			line = buffer;
			return true;
		}
	}

	Int32 pos1 = (pos0+1) + read;
	fseek(stream, pos1, SEEK_SET);
	buffer[read] = '\0';
	line = buffer;
	return true;
}

//--------------------------------------------------------------
void Parser::writeLine(const std::string& line)
{
	Write(line.c_str(), line.size());
	Write("\n", 1);
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
