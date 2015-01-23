#include "Shader.h"
#include "../graphics/GLcommon.h"

#include <cstdio>


IMPGEARS_BEGIN

//--------------------------------------------------------------
Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
    GLint result;

    // Création des shaders
    m_vertexID = glCreateShader (GL_VERTEX_SHADER) ;
    m_fragmentID = glCreateShader (GL_FRAGMENT_SHADER) ;

    // Envoi du code source
    glShaderSource (m_vertexID, 1, &vertexShader, NULL) ;
    glShaderSource (m_fragmentID, 1, &fragmentShader, NULL) ;

    // Compilation des shaders
    glCompileShader (m_vertexID) ;
    glGetShaderiv(m_vertexID, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        GLint length;
        char* log;
        glGetShaderiv(m_vertexID, GL_INFO_LOG_LENGTH, &length);
        log = new char[length];
        glGetShaderInfoLog(m_vertexID, length, &result, log);
        fprintf(stderr, "[impError] Shader compilation failed.\n%s", log);
        delete [] log;
    }

    glCompileShader (m_fragmentID) ;
    glGetShaderiv(m_fragmentID, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        GLint length;
        char* log;
        glGetShaderiv(m_fragmentID, GL_INFO_LOG_LENGTH, &length);
        log = new char[length];
        glGetShaderInfoLog(m_fragmentID, length, &result, log);
        fprintf(stderr, "[impError] Shader compilation failed.\n%s", log);
        delete [] log;
    }

    // Création du programme
    m_programID = glCreateProgram () ;

    // Attache des shaders au programme (on peut les détacher avec la fonction glDetachShader)
    glAttachShader (m_programID, m_vertexID) ;
    glAttachShader (m_programID, m_fragmentID) ;

    // Linkage du programme
    glLinkProgram (m_programID) ;
}

//--------------------------------------------------------------
void Shader::setTextureParameter(const char* name, const Texture* texture, Int32 textureUnit)
{
    enable();
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture->getVideoID());

    glUniform1i(glGetUniformLocation(m_programID, name), textureUnit);
    GLenum errorState = glGetError();
    if (errorState != GL_NO_ERROR )
        fprintf(stderr, "impError : Send texture(%s) to shader (error %d : %s)\n", name, errorState, gluErrorString(errorState));
}

//--------------------------------------------------------------
void Shader::setFloatParameter(const char* name, float value)
{
    enable();
    glUniform1f(glGetUniformLocation(m_programID, name), static_cast<GLfloat>(value));
    GLenum errorState = glGetError();
    if(errorState != GL_NO_ERROR)
        fprintf(stderr, "impError : Send float(%s (value : %f)) to shader (error %d : %s)\n", name, value, errorState, gluErrorString(errorState));
}

//--------------------------------------------------------------
void Shader::setMat4Parameter(const char* name, const Mat4& mat4)
{
    enable();
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name), 1, false, mat4.getData());
    GLenum errorState = glGetError();
    if(errorState != GL_NO_ERROR)
        fprintf(stderr, "impError : Send Mat4(%s) to shader (error %d : %s)\n", name, errorState, gluErrorString(errorState));
}

//--------------------------------------------------------------
Shader::~Shader()
{
    disable();
    // Destruction des shaders
    glDeleteShader (m_vertexID) ;
    glDeleteShader (m_fragmentID);

    // Destruction du programme
    glDeleteProgram (m_programID);
}

//--------------------------------------------------------------
void Shader::enable()
{
    glEnable(GL_DEPTH_TEST);
    glUseProgram (m_programID) ;
}

//--------------------------------------------------------------
void Shader::disable()
{
    glUseProgram (0) ;
}

IMPGEARS_END
