#include <OGLPlugin/Program.h>
#include <OGLPlugin/GlError.h>

#include <cstdio>
#include <iostream>


IMPGEARS_BEGIN

std::uint32_t Program::_s_count = 0;

std::string Program::s_compilation_log;

//--------------------------------------------------------------
Program::Program(const std::string& name)
    : _name(name)
{
    _programID = glCreateProgram() ;
    GL_CHECKERROR(_name + " - create program");
    _vertID = glCreateShader(GL_VERTEX_SHADER);
    GL_CHECKERROR(_name + " - create vert shader");
    _fragID = glCreateShader(GL_FRAGMENT_SHADER);
    GL_CHECKERROR(_name + " - create frag shader");
    _s_count++;
}

//--------------------------------------------------------------
Program::Program(const std::string& vertCode, const std::string& fragCode, const std::string& name)
    : _name(name)
{
    _programID = glCreateProgram() ;
    GL_CHECKERROR(_name + " - create program");
    _vertID = glCreateShader(GL_VERTEX_SHADER);
    GL_CHECKERROR(_name + " - create vert shader");
    _fragID = glCreateShader(GL_FRAGMENT_SHADER);
    GL_CHECKERROR(_name + " - create frag shader");
    _s_count++;

    load(vertCode,fragCode);
}

//--------------------------------------------------------------
void Program::load(const std::string& vertCode, const std::string& fragCode)
{
    const char* src = vertCode.c_str();
    glShaderSource(_vertID, 1, &src, NULL) ;
    GL_CHECKERROR(_name + " - load vert source");
    src = fragCode.c_str();
    glShaderSource(_fragID, 1, &src, NULL) ;
    GL_CHECKERROR(_name + " - load frag source");

    compile(_vertID);
    compile(_fragID);

    glAttachShader(_programID, _vertID);
    GL_CHECKERROR(_name + " - attach vertex shader");
    glAttachShader(_programID, _fragID);
    GL_CHECKERROR(_name + " - attach fragment shader");

    glLinkProgram(_programID);
    GL_CHECKERROR(_name + " - link shader program");
}

void Program::compile(std::uint32_t srcId)
{
    GLint result = GL_TRUE;
    glCompileShader(srcId) ;
    glGetShaderiv(srcId, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        GLint len;
        glGetShaderiv(srcId, GL_INFO_LOG_LENGTH, &len);
        GL_CHECKERROR(_name + " - shader compilation");
        s_compilation_log.resize(len);
        glGetShaderInfoLog(srcId, len, &result, &s_compilation_log[0] );
        GL_CHECKERROR(_name + " - shader compilation info log");
        std::cout << "[impError] " + _name + " - shader compilation failed." << std::endl;
        std::cout << s_compilation_log << std::endl;
    }
}

//--------------------------------------------------------------
Program::~Program()
{
    glDeleteShader(_vertID);
    glDeleteShader(_fragID);
    glDeleteProgram(_programID);
    _s_count--;
}

//--------------------------------------------------------------
void Program::use()
{
    glUseProgram (_programID) ;
    GL_CHECKERROR(_name + " - use program");
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
        GL_CHECKERROR(_name + " - parameter location");

        if(location == -1)
            std::cout << "impError :" + _name + " - location of uniform (" << id << ") failed" << std::endl;
        _locationCache[id] = location;
        return location;
    }
}

//--------------------------------------------------------------
void Program::rename(const std::string& name)
{
    _name = name;
}

IMPGEARS_END
