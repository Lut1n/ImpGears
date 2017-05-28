#include "Graphics/Shader.h"
#include "Graphics/GLcommon.h"

#include <cstdio>


IMPGEARS_BEGIN

Shader* Shader::m_instance = IMP_NULL;


//--------------------------------------------------------------
Shader::Parameter::Parameter(const std::string& id, Type type)
{
	this->id = id;
	this->type = type;
}

//--------------------------------------------------------------
Shader::Parameter::~Parameter()
{
	
}

//--------------------------------------------------------------
void Shader::Parameter::set(float float1)
{
	this->type = Type_1f;
	value.value_1f = float1;
}

//--------------------------------------------------------------
void Shader::Parameter::set(const Vector3* vec3)
{
	this->type = Type_3f;
	value.value_3f = vec3;
}

//--------------------------------------------------------------
void Shader::Parameter::set(int int1)
{
	this->type = Type_1i;
	value.value_1i = int1;
}

//--------------------------------------------------------------
void Shader::Parameter::set(const float* float1Array, int count)
{
	this->type = Type_1fv;
	value.value_1fv = float1Array;
	this->count = count;
}

//--------------------------------------------------------------
void Shader::Parameter::set(const Vector3* vec3Array, int count)
{
	this->type = Type_3fv;
	value.value_3fv = vec3Array;
	this->count = count;
}

//--------------------------------------------------------------
void Shader::Parameter::set(const int* int1Array, int count)
{
	this->type = Type_1iv;
	value.value_1iv = int1Array;
	this->count = count;
}

//--------------------------------------------------------------
/*void Shader::Parameter::set(const Matrix3* mat3Array, int count)
{
	this->type = Type_Mat3v;
	value.value_mat3v = mat3Array;
	this->count = count;
}*/

//--------------------------------------------------------------
void Shader::Parameter::set(const Matrix4* mat4Array, int count)
{
	this->type = Type_Mat4v;
	value.value_mat4v = mat4Array;
	this->count = count;
}

//--------------------------------------------------------------
void Shader::Parameter::updateUniform(const Shader& program) const
{
	Int32 uniformLocation = program.getParameterLocation( this->getID() );
	
	if(type == Type_1f)
	{
		glUniform1f(uniformLocation, value.value_1f);
	}
	else if(type == Type_3f)
	{
		glUniform3f(uniformLocation, value.value_3f->getX(), value.value_3f->getY(), value.value_3f->getZ());
	}
	else if(type == Type_1i)
	{
		glUniform1i(uniformLocation, value.value_1i);
	}
	else if(type == Type_1fv)
	{
		glUniform1fv(uniformLocation, count, value.value_1fv);
	}
	else if(type == Type_3fv)
	{
		// glUniform3f(uniformLocation, value.value_3f->getX(), value.value_3f->getY(), value.value_3f->getZ());
	}
	else if(type == Type_1iv)
	{
		glUniform1iv(uniformLocation, count, value.value_1iv);
	}
	/*else if(type == Type_Mat3v)
	{
		// 
	}*/
	else if(type == Type_Mat4v)
	{
		glUniformMatrix4fv(uniformLocation, 1, false, value.value_mat4v->getData());
	}
	else
	{
		// 
	}
	
	GLenum errorState = glGetError();
	if(errorState != GL_NO_ERROR)
	{
		std::string strErr;
		glErrToString(errorState, strErr);
		fprintf(stderr, "impError : Send param (%s) to shader (error %d : %s)\n", id.c_str(), errorState, strErr.c_str() );
	}
}

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
        fprintf(stderr, "[impError] Vertex shader compilation failed.\n%s", log);
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
        fprintf(stderr, "[impError] Fragment shader compilation failed.\n%s", log);
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
	{
		std::string strErr;
		glErrToString(errorState, strErr);
		fprintf(stderr, "impError : Send vector3 array(%s) to shader (error %d : %s)\n", name, errorState, strErr.c_str() );
	}
}

//--------------------------------------------------------------
void Shader::setFloatParameter(const char* name, float value)
{
    enable();
    glUniform1f(glGetUniformLocation(m_programID, name), static_cast<GLfloat>(value));
    GLenum errorState = glGetError();
    if(errorState != GL_NO_ERROR)
	{
		std::string strErr;
		glErrToString(errorState, strErr);
		fprintf(stderr, "impError : Send float(%s (value : %f)) to shader (error %d : %s)\n", name, value, errorState, strErr.c_str() );
	}
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
	{
		std::string strErr;
		glErrToString(errorState, strErr);
		fprintf(stderr, "impError : Send Matrix4(%s) to shader (error %d : %s)\n", name, errorState, strErr.c_str() );
	}
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
	{
		std::string strErr;
		glErrToString(errorState, strErr);
		fprintf(stderr, "impError : Send vector3 (%s) to shader (error %d : %s)\n", name, errorState, strErr.c_str() );
	}
}

//--------------------------------------------------------------
void Shader::setParameter(const Parameter& param)
{
	param.updateUniform( *this );
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
Int32 Shader::getParameterLocation(const std::string& id) const
{
	Int32 location = glGetUniformLocation(m_programID, id.c_str());
    if(location == -1)
        fprintf(stderr, "impError : location of uniform (%s) failed\n", id.c_str());
    GL_CHECKERROR("parameter location");
	
	return location;
}
		
//--------------------------------------------------------------
void Shader::disable()
{
    glUseProgram (0) ;
    m_instance = IMP_NULL;
}

//--------------------------------------------------------------
void Shader::addParameter(Parameter* param)
{
	_parameters.push_back(param);
}

//--------------------------------------------------------------
void Shader::removeParameter(Parameter* param)
{
	bool found = false;
	Uint32 index = -1;
	for(Uint32 i=0; i<_parameters.size(); ++i)
		if(_parameters[i]->getID() == param->getID())
		{
			index = i;
			found = true;
			break;
		}
	
	if(found)
	{
		for(Uint32 i=index+1; i<_parameters.size(); ++i)
			_parameters[i-1] = _parameters[i];
		
		_parameters.resize(_parameters.size()-1);
	}
}

//--------------------------------------------------------------
void Shader::clearParameter()
{
	_parameters.clear();
}

//--------------------------------------------------------------
Shader::Parameter* Shader::getParameter(const std::string& name)
{
	for(Uint32 i=0; i<_parameters.size(); ++i)
		if(_parameters[i]->getID() == name)
			return _parameters[i];
		
	return IMP_NULL;
}

//--------------------------------------------------------------
void Shader::updateAllParameters()
{
	for(Uint32 i=0; i<_parameters.size(); ++i)
	{
		_parameters[i]->updateUniform( *this );
	}
}

//--------------------------------------------------------------
void Shader::updateParameter(const std::string& name)
{
	getParameter(name)->updateUniform( *this );
}

IMPGEARS_END
