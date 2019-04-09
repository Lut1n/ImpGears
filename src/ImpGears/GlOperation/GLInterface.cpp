#include <SceneGraph/ClearNode.h>
#include <SceneGraph/State.h>
#include <Graphics/Image.h>
#include <Geometry/Geometry.h>

#include "GlError.h"
#include "Texture.h"
#include "GLInterface.h"
#include "BufferObject.h"
#include "Shader.h"

#include <iostream>

IMPGEARS_BEGIN

/// =========== VERTEX SHADER SOURCE =====================
static std::string basicVert = IMP_GLSL_SRC(

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
void main() { gl_Position = u_projection * u_view * u_model * gl_Vertex; }

);

/// =========== FRAGMENT SHADER SOURCE =====================
static std::string basicFrag = IMP_GLSL_SRC(

uniform vec3 u_color;
void main() { gl_FragData[0] = vec4(u_color, 1.0); }

);

//--------------------------------------------------------------
struct GeoData : public RefactoInterface::Data
{
	GeoData() { ty=RefactoInterface::Ty_Vbo; }
	
	BufferObject vbo;
};

//--------------------------------------------------------------
struct TexData : public RefactoInterface::Data
{
	TexData() { ty=RefactoInterface::Ty_Tex; }
	
	Texture tex;
};

//--------------------------------------------------------------
struct ShaData : public RefactoInterface::Data
{
	ShaData() { ty=RefactoInterface::Ty_Shader; }
	
	Shader sha;
};

//--------------------------------------------------------------
void GLInterface::init()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(0);
	}

	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	std::cout << "OGL version " << major << "." << minor << std::endl;
	std::cout << "OpenGL version supported by this platform (" << glGetString(GL_VERSION) << ")" << std::endl;
}

//--------------------------------------------------------------
void GLInterface::apply(const ClearNode* clear)
{
	GLbitfield bitfield = 0;
	
	if(clear->isColorEnable())
	{
		Vec4 color = clear->getColor();
		glClearColor(color.x(), color.y(), color.z(), color.w());
		bitfield = bitfield | GL_COLOR_BUFFER_BIT;
	}
	
	if(clear->isDepthEnable())
	{
		glClearDepth( clear->getDepth() );
		bitfield = bitfield | GL_DEPTH_BUFFER_BIT;
	}
	
	glClear(bitfield);
}

//--------------------------------------------------------------
void GLInterface::setCulling(int mode)
{
	if(mode == 0)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW); // GL_CW or GL_CCW

		if(mode == 1)
			glCullFace(GL_BACK); // GL_FRONT, GL_BACK or GL_FRONT_AND_BACK
		else if(mode == 2)
			glCullFace(GL_FRONT);
	}
}

//--------------------------------------------------------------
void GLInterface::setBlend(int mode)
{
	if(mode == 0)
	{
		glDisable(GL_BLEND);
	}
	else if(mode == 1)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

//--------------------------------------------------------------
void GLInterface::setLineW(float lw)
{
	glLineWidth(lw);
}

//--------------------------------------------------------------
void GLInterface::setViewport(Vec4 vp)
{
	glViewport(vp[0], vp[1], vp[2], vp[3]);
}

//--------------------------------------------------------------
void GLInterface::setDepthTest(int mode)
{
	if(mode == 1)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

//--------------------------------------------------------------
GLInterface::Data* GLInterface::load(const Geometry* geo)
{
	GeoData* d = new GeoData();
	d->vbo.load(*geo);
	return d;
}

//--------------------------------------------------------------
GLInterface::Data* GLInterface::load(const Image* img)
{
	TexData* d = new TexData();
	d->tex.loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
	return d;
}

//--------------------------------------------------------------
GLInterface::Data* GLInterface::load(const std::string& vert, const std::string& frag)
{
	bool alt = vert.empty();
	ShaData* d = new ShaData();
	if(alt) d->sha.load(basicVert.c_str(),basicFrag.c_str());
	else d->sha.load(vert.c_str(),frag.c_str());
	return d;
}

//--------------------------------------------------------------
void GLInterface::update(Data* data, const Image* img)
{
	if(data->ty == Ty_Tex)
	{
		TexData* d = static_cast<TexData*>(data);
		d->tex.loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
	}
}

//--------------------------------------------------------------
void GLInterface::bind(Data* data)
{
	if(data->ty == Ty_Tgt)
	{
		// _target->bind();
	}
	else if(data->ty == Ty_Shader)
	{
		ShaData* d = static_cast<ShaData*>( data );
		d->sha.use();
		// _shader->enable();
		// _shader->updateAllUniforms();	
	}
	else if(data->ty == Ty_Vbo)
	{
	}
	else if(data->ty == Ty_Tex)
	{
		TexData* d = static_cast<TexData*>( data );
		d->tex.bind();
	}
}

//--------------------------------------------------------------
void GLInterface::draw(Data* data)
{
	if(data->ty == Ty_Vbo)
	{
		GeoData* d = static_cast<GeoData*>(data);
		d->vbo.draw();
	}
}

//--------------------------------------------------------------
void GLInterface::update(Data* data, const Uniform* uniform)
{
	std::string uId = uniform->getID();
	Uniform::Type type = uniform->getType();
	Uniform::Value value = uniform->getValue();
	ShaData* sha = static_cast<ShaData*>(data);
	std::int32_t uniformLocation = sha->sha.locate(uId);
    if(uniformLocation == -1)
        std::cout << "impError : location of uniform (" << uId << ") failed" << std::endl;
    GL_CHECKERROR("parameter location");
	
	if(type == Uniform::Type_1f)
	{
		glUniform1f(uniformLocation, value.value_1f);
	}
	else if(type == Uniform::Type_3f)
	{
		glUniform3f(uniformLocation, value.value_3f->x(), value.value_3f->y(), value.value_3f->z());
	}
	else if(type == Uniform::Type_1i)
	{
		glUniform1i(uniformLocation, value.value_1i);
	}
	else if(type == Uniform::Type_1fv)
	{
		glUniform1fv(uniformLocation, uniform->getCount(), value.value_1fv);
	}
	else if(type == Uniform::Type_3fv)
	{
		// glUniform3f(uniformLocation, value.value_3f->x(), value.value_3f->y(), value.value_3f->z());
	}
	else if(type == Uniform::Type_1iv)
	{
		glUniform1iv(uniformLocation, uniform->getCount(), value.value_1iv);
	}
	/*else if(type == Uniform::Type_Mat3v)
	{
		// 
	}*/
	else if(type == Uniform::Type_Mat4v)
	{
		glUniformMatrix4fv(uniformLocation, 1, false, value.value_mat4v->data());
	}
	/*else if(type == Uniform::Type_Sampler)
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + value.value_1i);
		glBindTexture(GL_TEXTURE_2D, uniform->getSampler()->getVideoID());
		glUniform1i(uniformLocation, value.value_1i);
	}*/
	else
	{
		// 
	}
	
	GLenum errorState = glGetError();
	if(errorState != GL_NO_ERROR)
	{
		std::string strErr;
		glErrToString(errorState, strErr);
		std::cout << "impError : Send param (" << uId << ") to shader (error " 
		<< errorState << " : " << strErr << ")" << std::endl;
		std::cout << "loc " << uniformLocation << " in " << sha->sha.id() << std::endl;
	}
	
    GL_CHECKERROR("end of uniform setting");
}

IMPGEARS_END
