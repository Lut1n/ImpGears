#include <OGLPlugin/Program.h>
#include <OGLPlugin/GlError.h>

#include <cstdio>
#include <iostream>


IMPGEARS_BEGIN

std::string Program::s_compilation_log;

//--------------------------------------------------------------
Program::Program()
{
}

//--------------------------------------------------------------
Program::Program(const std::string& vertCode, const std::string& fragCode)
{
    load(vertCode,fragCode);
}

//--------------------------------------------------------------
void Program::load(const std::string& vertCode, const std::string& fragCode)
{
    _programID = glCreateProgram () ;
    GL_CHECKERROR("create program");
    _vertID = glCreateShader (GL_VERTEX_SHADER);
    GL_CHECKERROR("create vert shader");
    _fragID = glCreateShader (GL_FRAGMENT_SHADER);
    GL_CHECKERROR("create frag shader");

    const char* src = vertCode.c_str();
    glShaderSource (_vertID, 1, &src, NULL) ;
    GL_CHECKERROR("load vert source");
    src = fragCode.c_str();
    glShaderSource (_fragID, 1, &src, NULL) ;
    GL_CHECKERROR("load frag source");

    compile(_vertID);
    compile(_fragID);

    glAttachShader (_programID, _vertID);
    GL_CHECKERROR("attach vertex shader");
    glAttachShader (_programID, _fragID);
    GL_CHECKERROR("attach fragment shader");

    glLinkProgram (_programID);
    GL_CHECKERROR("link shader program");
}

void Program::compile(std::uint32_t srcId)
{
    GLint result = GL_TRUE;
    glCompileShader (srcId) ;
    glGetShaderiv(srcId, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        GLint len;
        glGetShaderiv(srcId, GL_INFO_LOG_LENGTH, &len);
        GL_CHECKERROR("shader compilation");
        s_compilation_log.resize(len);
        glGetShaderInfoLog(srcId, len, &result, &s_compilation_log[0] );
        GL_CHECKERROR("shader compilation info log");
        std::cout << "[impError] shader compilation failed." << std::endl;
        std::cout << s_compilation_log << std::endl;
    }
}

//--------------------------------------------------------------
Program::~Program()
{
    glDeleteShader (_vertID) ;
    glDeleteShader (_fragID);
    glDeleteProgram (_programID);
}

//--------------------------------------------------------------
void Program::use()
{
    glUseProgram (_programID) ;
    GL_CHECKERROR("use program");
}

//--------------------------------------------------------------
std::int32_t Program::locate(const std::string& id) const
{
    auto found = _locationCache.find(id);
    if(found != _locationCache.end())
    {
        return found->second;
    }
    else
    {
        std::int32_t location = glGetUniformLocation(_programID, id.c_str());
        GL_CHECKERROR("parameter location");

        if(location == -1)
            std::cout << "impError : location of uniform (" << id << ") failed" << std::endl;
        _locationCache[id] = location;
        return location;
    }
}

IMPGEARS_END
