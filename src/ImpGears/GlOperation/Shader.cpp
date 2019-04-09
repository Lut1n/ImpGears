#include "Shader.h"
#include "GlError.h"

#include <cstdio>
#include <iostream>


IMPGEARS_BEGIN

std::string Shader::s_compilation_log;

//--------------------------------------------------------------
Shader::Shader()
{
}

//--------------------------------------------------------------
Shader::Shader(const std::string& vertCode, const std::string& fragCode)
{
    load(vertCode,fragCode);
}

//--------------------------------------------------------------
void Shader::load(const std::string& vertCode, const std::string& fragCode)
{
	_programID = glCreateProgram () ;
	_vertID = glCreateShader (GL_VERTEX_SHADER);
	_fragID = glCreateShader (GL_FRAGMENT_SHADER);

	const char* src = vertCode.c_str();
	glShaderSource (_vertID, 1, &src, NULL) ;
	src = fragCode.c_str();
	glShaderSource (_fragID, 1, &src, NULL) ;

	compile(_vertID);
	compile(_fragID);

	glAttachShader (_programID, _vertID);
	GL_CHECKERROR("attach vertex shader");
	glAttachShader (_programID, _fragID);
	GL_CHECKERROR("attach fragment shader");

	glLinkProgram (_programID);
	GL_CHECKERROR("link shader program");
}

void Shader::compile(std::uint32_t srcId)
{
    GLint result = GL_TRUE;
	glCompileShader (srcId) ;
	glGetShaderiv(srcId, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE)
	{
		GLint len;
		glGetShaderiv(srcId, GL_INFO_LOG_LENGTH, &len);
		s_compilation_log.resize(len);
		glGetShaderInfoLog(srcId, len, &result, &s_compilation_log[0] );
		std::cout << "[impError] shader compilation failed." << std::endl;
		std::cout << s_compilation_log << std::endl;
	}
}

//--------------------------------------------------------------
Shader::~Shader()
{
	glDeleteShader (_vertID) ;
	glDeleteShader (_fragID);
	glDeleteProgram (_programID);
}

//--------------------------------------------------------------
void Shader::use()
{
	glUseProgram (_programID) ;
}

//--------------------------------------------------------------
std::int32_t Shader::locate(const std::string& id) const
{
	std::int32_t location = glGetUniformLocation(_programID, id.c_str());
	GL_CHECKERROR("parameter location");
	return location;
}

IMPGEARS_END
