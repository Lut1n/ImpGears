#include "IGParser.h"

//--------------------------------------------------------------
#define IGPARSER_READWRITE(T)\
    imp::Uint32 IGParser::Read(T* _buffer, imp::Uint32 _size)\
    {\
        return (imp::Uint32) fread((void*)_buffer, sizeof(T), _size, stream);\
    }\
    void IGParser::Write(const T* _buffer, imp::Uint32 _size)\
    {\
        fwrite((const void*)_buffer, sizeof(T), _size, stream);\
    }

//--------------------------------------------------------------
IGParser::IGParser(const char* _filename, IGFileType _type, IGAccessMode _mode)
{
    char faccess = ' ';
    if( _mode == IGAccessMode_Write )
        faccess = 'w';
    else if( _mode == IGAccessMode_Read )
        faccess = 'r';
    else if( _mode == IGAccessMode_Append )
        faccess = 'a';

    char ftype = ' ';
    if( _type == IGFileType_Text )
        ftype = 't';
    else if( _type == IGFileType_Binary )
        ftype = 'b';

    char fmode[2];
    sprintf(fmode, "%c%c", faccess, ftype);

    stream = fopen(_filename, fmode);

    accessMode = _mode;
    fileType = _type;
}

//--------------------------------------------------------------
IGParser::~IGParser()
{
    if( accessMode == IGAccessMode_Write ||
        accessMode == IGAccessMode_Append )
            fflush(stream);

    fclose(stream);
}

//--------------------------------------------------------------
IGPARSER_READWRITE(char)

//--------------------------------------------------------------
IGPARSER_READWRITE(imp::Int8)

//--------------------------------------------------------------
IGPARSER_READWRITE(imp::Uint8)

//--------------------------------------------------------------
IGPARSER_READWRITE(float)
