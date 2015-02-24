#include "shaders/Shader.h"
#include "graphics/GLcommon.h"

#include <cstdio>


IMPGEARS_BEGIN

Shader* Shader::m_instance = IMP_NULL;

//--------------------------------------------------------------
Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
    // Création des shaders
    m_vertexID = glCreateShader (GL_VERTEX_SHADER) ;
    GL_CHECKERROR("create vertex shader");

    m_fragmentID = glCreateShader (GL_FRAGMENT_SHADER) ;
    GL_CHECKERROR("cretae fragment shader");


    // Envoi du code source
    glShaderSource (m_vertexID, 1, &vertexShader, NULL) ;
    GL_CHECKERROR("set vertex shader source");

    glShaderSource (m_fragmentID, 1, &fragmentShader, NULL) ;
    GL_CHECKERROR("set fragment shader source");

    GLint result = GL_TRUE;

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
    GL_CHECKERROR("shader program creation");

    // Attache des shaders au programme (on peut les détacher avec la fonction glDetachShader)
    glAttachShader (m_programID, m_vertexID);
    GL_CHECKERROR("attach vertex shader");
    glAttachShader (m_programID, m_fragmentID);
    GL_CHECKERROR("attach fragment shader");

    // Linkage du programme
    glLinkProgram (m_programID);
    GL_CHECKERROR("link shader program");
}

//--------------------------------------------------------------
void Shader::setTextureParameter(const char* name, const Texture* texture, Int32 textureUnit)
{
    enable();
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture->getVideoID());

    Int32 location = glGetUniformLocation(m_programID, name);
    if(location == -1)
        fprintf(stderr, "impError : location of uniform (%s) failed\n", name);
    GL_CHECKERROR(name);

    glUniform1i(location, textureUnit);
    GL_CHECKERROR(name);
}

//--------------------------------------------------------------
void Shader::setVector3ArrayParameter(const char* name, float* vector3Array, Uint32 count)
{
    enable();
    Int32 location = glGetUniformLocation(m_programID, name);
    glUniform3fv(location, count, vector3Array);
    GLenum errorState = glGetError();
    if(errorState != GL_NO_ERROR)
        fprintf(stderr, "impError : Send vector3 array(%s) to shader (error %d : %s)\n", name, errorState, gluErrorString(errorState));
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
void Shader::setMatrix4Parameter(const char* name, const Matrix4& Matrix4)
{
    enable();
    Int32 location = glGetUniformLocation(m_programID, name);
    if(location == -1)
        fprintf(stderr, "impError : location of uniform (%s) failed\n", name);
    GL_CHECKERROR("Matrix4 location");
    glUniformMatrix4fv(location, 1, false, Matrix4.getData());
    GLenum errorState = glGetError();
    if(errorState != GL_NO_ERROR)
        fprintf(stderr, "impError : Send Matrix4(%s) to shader (error %d : %s)\n", name, errorState, gluErrorString(errorState));
}

//--------------------------------------------------------------
void Shader::setVector3Parameter(const char* name, const Vector3& vec3)
{
    enable();
    Int32 location = glGetUniformLocation(m_programID, name);
    if(location == -1)
        fprintf(stderr, "impError : location of uniform (%s) failed\n", name);
    GL_CHECKERROR("vec3 location");

    glUniform3f(location, vec3.getX(), vec3.getY(), vec3.getZ());
    GLenum errorState = glGetError();
    if(errorState != GL_NO_ERROR)
        fprintf(stderr, "impError : Send vector3 (%s) to shader (error %d : %s)\n", name, errorState, gluErrorString(errorState));
}

//--------------------------------------------------------------
void Shader::setProjection(const Matrix4& projection)
{
    setMatrix4Parameter("u_projection", projection);
}

//--------------------------------------------------------------
void Shader::setView(const Matrix4& view)
{
    setMatrix4Parameter("u_view", view);
}

//--------------------------------------------------------------
void Shader::setModel(const Matrix4& model)
{
    setMatrix4Parameter("u_model", model);
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
/*
    glEnable(GL_DEPTH_TEST);
    bool isprogram = glIsProgram(m_programID);
    if(!isprogram)
        fprintf(stdout, "this is not a program\n");
*/
    glUseProgram (m_programID) ;
    GL_CHECKERROR("use shader program");
    m_instance = this;
/*
        glValidateProgram(m_programID);
        GL_CHECKERROR("validate shader program");
*/

}

//--------------------------------------------------------------
void Shader::disable()
{
    glUseProgram (0) ;
    m_instance = IMP_NULL;
}

IMPGEARS_END
