#ifndef GLCOMMON_H_INCLUDED
#define GLCOMMON_H_INCLUDED

// #define GLEW_STATIC
#include <GL/glew.h>
//#include <GL/glut.h>
//#include <GL/glu.h>
#include <GL/gl.h>

#include <cstdio>
#include <string>

#define GL_CHECKERROR(msg) GLcheckError(msg, __FILE__, __LINE__);

inline void glErrToString(GLenum error, std::string& string)
{
	switch(error)
	{
	case GL_NO_ERROR :
		string = "No error has been recorded.\
		The value of this symbolic constant is guaranteed to be 0.";
		break;
	case GL_INVALID_ENUM :
		string = "An unacceptable value is specified for an enumerated argument.\
		The offending command is ignored\
		and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_VALUE :
		string = "A numeric argument is out of range.\
		The offending command is ignored\
		and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_OPERATION :
		string = "The specified operation is not allowed in the current state.\
		The offending command is ignored\
		and has no other side effect than to set the error flag.";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION :
		string = "The framebuffer object is not complete. The offending command\
		is ignored and has no other side effect than to set the error flag.";
		break;
	case GL_OUT_OF_MEMORY :
		string = "There is not enough memory left to execute the command.\
		The state of the GL is undefined,\
		except for the state of the error flags,\
		after this error is recorded.";
		break;
	case GL_STACK_UNDERFLOW :
		string = "An attempt has been made to perform an operation that would\
		cause an internal stack to underflow.";
		break;
	case GL_STACK_OVERFLOW :
		string = "An attempt has been made to perform an operation that would\
		cause an internal stack to overflow.";
		break;
	default:
		string = "opengl unknown error.";
		break;
	}
}


inline void GLcheckError(const char* _debugMsg, const char* _file, int _line)
{
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
	{
		std::string strErr;
		glErrToString(error, strErr);
		fprintf(stderr, "[impError] %s - GL error in %s at line %d.\n%s\n", _debugMsg, _file, _line, strErr.c_str() );
	}
}

#endif // GLCOMMON_H_INCLUDED
