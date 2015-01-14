#include "Shader.h"
#include "../graphics/GLcommon.h"


IMPGEARS_BEGIN

Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
    // Création des shaders
    m_vertexID = glCreateShader (GL_VERTEX_SHADER) ;
    m_fragmentID = glCreateShader (GL_FRAGMENT_SHADER) ;

    // Envoi du code source
    glShaderSource (m_vertexID, 1, &vertexShader, NULL) ;
    glShaderSource (m_fragmentID, 1, &fragmentShader, NULL) ;

    // Compilation des shaders
    glCompileShader (m_vertexID) ;
    glCompileShader (m_fragmentID) ;

    // Création du programme
    m_programID = glCreateProgram () ;

    // Attache des shaders au programme (on peut les détacher avec la fonction glDetachShader)
    glAttachShader (m_programID, m_vertexID) ;
    glAttachShader (m_programID, m_fragmentID) ;

    // Linkage du programme
    glLinkProgram (m_programID) ;
}

Shader::~Shader()
{
    // Destruction des shaders
    glDeleteShader (m_vertexID) ;
    glDeleteShader (m_fragmentID);

    // Destruction du programme
    glDeleteProgram (m_programID);
}

void Shader::enable()
{
    glEnable(GL_DEPTH_TEST);
    glUseProgram (m_programID) ;
}

void Shader::disable()
{
    glUseProgram (0) ;
}

IMPGEARS_END
