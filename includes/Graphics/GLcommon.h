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
	// string = std::string( gluErrorString(error) );
	string = "impError : need error handler implementation\n";
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
