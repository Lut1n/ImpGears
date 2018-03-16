#include <SceneGraph/Shader.h>
#include <SceneGraph/OpenGL.h>

#include <cstdio>
#include <iostream>


IMPGEARS_BEGIN

Shader* Shader::m_instance = nullptr;


//--------------------------------------------------------------
Uniform::Uniform(const std::string& id, Type type)
{
	this->id = id;
	this->type = type;
	this->sampler = nullptr;
}

//--------------------------------------------------------------
Uniform::~Uniform()
{
	
}

//--------------------------------------------------------------
void Uniform::set(float float1)
{
	this->type = Type_1f;
	value.value_1f = float1;
}

//--------------------------------------------------------------
void Uniform::set(const Vector3* vec3)
{
	this->type = Type_3f;
	value.value_3f = vec3;
}

//--------------------------------------------------------------
void Uniform::set(int int1)
{
	this->type = Type_1i;
	value.value_1i = int1;
}

//--------------------------------------------------------------
void Uniform::set(const float* float1Array, int count)
{
	this->type = Type_1fv;
	value.value_1fv = float1Array;
	this->count = count;
}

//--------------------------------------------------------------
void Uniform::set(const Vector3* vec3Array, int count)
{
	this->type = Type_3fv;
	value.value_3fv = vec3Array;
	this->count = count;
}

//--------------------------------------------------------------
void Uniform::set(const int* int1Array, int count)
{
	this->type = Type_1iv;
	value.value_1iv = int1Array;
	this->count = count;
}

//--------------------------------------------------------------
/*void Uniform::set(const Matrix3* mat3Array, int count)
{
	this->type = Type_Mat3v;
	value.value_mat3v = mat3Array;
	this->count = count;
}*/

//--------------------------------------------------------------
void Uniform::set(const Matrix4* mat4Array, int count)
{
	this->type = Type_Mat4v;
	value.value_mat4v = mat4Array;
	this->count = count;
}

//--------------------------------------------------------------
void Uniform::set(const Texture* texture, std::int32_t textureUnit)
{
	this->type = Type_Sampler;
	value.value_1i = textureUnit;
	sampler = texture;
}

//--------------------------------------------------------------
void Uniform::updateUniform(const Shader& program) const
{
	std::int32_t uniformLocation = program.getUniformLocation( this->getID() );
	
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
	else if(type == Type_Sampler)
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + value.value_1i);
		glBindTexture(GL_TEXTURE_2D, sampler->getVideoID());
		glUniform1i(uniformLocation, value.value_1i);
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
	: _valid(false)
{
    // Création des shaders
    m_vertexID = glCreateShader (GL_VERTEX_SHADER) ;
    _valid = GL_CHECKERROR("create vertex shader");

    m_fragmentID = glCreateShader (GL_FRAGMENT_SHADER) ;
    _valid &= GL_CHECKERROR("cretae fragment shader");


    // Envoi du code source
    glShaderSource (m_vertexID, 1, &vertexShader, NULL) ;
    _valid &= GL_CHECKERROR("set vertex shader source");

    glShaderSource (m_fragmentID, 1, &fragmentShader, NULL) ;
    _valid &= GL_CHECKERROR("set fragment shader source");

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
    _valid &= GL_CHECKERROR("shader program creation");

    // Attache des shaders au programme (on peut les détacher avec la fonction glDetachShader)
    glAttachShader (m_programID, m_vertexID);
    _valid &= GL_CHECKERROR("attach vertex shader");
    glAttachShader (m_programID, m_fragmentID);
    _valid &= GL_CHECKERROR("attach fragment shader");

    // Linkage du programme
    glLinkProgram (m_programID);
    _valid &= GL_CHECKERROR("link shader program");
}

//--------------------------------------------------------------
void Shader::setTextureUniform(const char* name, const Texture* texture, std::int32_t textureUnit)
{
    // enable();
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture->getVideoID());

    std::int32_t location = glGetUniformLocation(m_programID, name);
    if(location == -1)
        fprintf(stderr, "impError : location of uniform (%s) failed\n", name);
    GL_CHECKERROR(name);

    glUniform1i(location, textureUnit);
    GL_CHECKERROR(name);
}

//--------------------------------------------------------------
void Shader::setVector3ArrayUniform(const char* name, float* vector3Array, std::uint32_t count)
{
    // enable();
    std::int32_t location = glGetUniformLocation(m_programID, name);
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
void Shader::setFloatUniform(const char* name, float value)
{
    // enable();
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
void Shader::setMatrix4Uniform(const char* name, const Matrix4& Matrix4)
{
    // enable();
    std::int32_t location = glGetUniformLocation(m_programID, name);
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
void Shader::setVector3Uniform(const char* name, const Vector3& vec3)
{
    // enable();
    std::int32_t location = glGetUniformLocation(m_programID, name);
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
void Shader::setUniform(const Uniform& param)
{
	param.updateUniform( *this );
}

//--------------------------------------------------------------
void Shader::setProjection(const Matrix4& projection)
{
    setMatrix4Uniform("u_projection", projection);
}

//--------------------------------------------------------------
void Shader::setView(const Matrix4& view)
{
    setMatrix4Uniform("u_view", view);
}

//--------------------------------------------------------------
void Shader::setModel(const Matrix4& model)
{
    setMatrix4Uniform("u_model", model);
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
	if( !_valid )
	{
		std::cout << "shader " << getObjectID() << " is not valid" << std::endl;
	}

    glUseProgram (m_programID) ;
    GL_CHECKERROR("use shader program");
    m_instance = this;
/*
        glValidateProgram(m_programID);
        GL_CHECKERROR("validate shader program");
*/

}

//--------------------------------------------------------------
std::int32_t Shader::getUniformLocation(const std::string& id) const
{
	std::int32_t location = glGetUniformLocation(m_programID, id.c_str());
    if(location == -1)
        fprintf(stderr, "impError : location of uniform (%s) failed\n", id.c_str());
    GL_CHECKERROR("parameter location");
	
	return location;
}
		
//--------------------------------------------------------------
void Shader::disable()
{
    glUseProgram (0) ;
    m_instance = nullptr;
}

//--------------------------------------------------------------
void Shader::addUniform(const std::shared_ptr<Uniform>& param)
{
	_parameters.push_back( param );
}

//--------------------------------------------------------------
void Shader::removeUniform(const Uniform::Ptr& param)
{
	bool found = false;
	std::uint32_t index = -1;
	for(std::uint32_t i=0; i<_parameters.size(); ++i)
		if(_parameters[i]->getID() == param->getID())
		{
			index = i;
			found = true;
			break;
		}
	
	if(found)
	{
		for(std::uint32_t i=index+1; i<_parameters.size(); ++i)
			_parameters[i-1] = _parameters[i];
		
		_parameters.resize(_parameters.size()-1);
	}
}

//--------------------------------------------------------------
void Shader::clearUniforms()
{
	_parameters.clear();
}

//--------------------------------------------------------------
Uniform::Ptr Shader::getUniform(const std::string& name)
{
	for(std::uint32_t i=0; i<_parameters.size(); ++i)
		if(_parameters[i]->getID() == name)
			return _parameters[i];
		
	return nullptr;
}

//--------------------------------------------------------------
void Shader::updateAllUniforms()
{
	for(std::uint32_t i=0; i<_parameters.size(); ++i)
	{
		_parameters[i]->updateUniform( *this );
	}
}

//--------------------------------------------------------------
void Shader::updateUniform(const std::string& name)
{
	getUniform(name)->updateUniform( *this );
}

//--------------------------------------------------------------
void Shader::addTextureUniform(const std::string& name, const Texture* texture, std::int32_t textureUnit)
{
	std::shared_ptr<Uniform> param( new Uniform(name, Uniform::Type_Sampler) );
	param->set(texture, textureUnit);
	addUniform(param);
}

//--------------------------------------------------------------
void Shader::addFloatUniform(const std::string& name, float value)
{
	std::shared_ptr<Uniform> param( new Uniform(name, Uniform::Type_1f) );
	param->set(value);
	addUniform(param);
}

//--------------------------------------------------------------
void Shader::addMatrix4Uniform(const std::string& name, const Matrix4* mat4)
{
	std::shared_ptr<Uniform> param( new Uniform(name, Uniform::Type_Mat4v) );
	param->set(mat4);
	addUniform(param);
}

//--------------------------------------------------------------
void Shader::addVector3ArrayUniform(const std::string& name, float* vector3Array, std::uint32_t count)
{
	std::shared_ptr<Uniform> param( new Uniform(name, Uniform::Type_3fv) );
	param->set(vector3Array,count);
	addUniform(param);
}

//--------------------------------------------------------------
void Shader::addVector3Uniform(const std::string& name, const Vector3* vec3)
{
	std::shared_ptr<Uniform> param( new Uniform(name, Uniform::Type_3f) );
	param->set(vec3);
	addUniform(param);
}

//--------------------------------------------------------------
void Shader::addProjection(const Matrix4* projection)
{
	addMatrix4Uniform("u_projection", projection);
}

//--------------------------------------------------------------
void Shader::addView(const Matrix4* view)
{
	addMatrix4Uniform("u_view", view);
}

//--------------------------------------------------------------
void Shader::addModel(const Matrix4* model)
{
	addMatrix4Uniform("u_model", model);
}

IMPGEARS_END
