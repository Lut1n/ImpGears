#include <SceneGraph/ClearNode.h>
#include <SceneGraph/State.h>
#include <SceneGraph/Sampler.h>
#include <SceneGraph/Target.h>
#include <Geometry/Geometry.h>

#include "GlError.h"
#include "Texture.h"
#include "GLInterface.h"
#include "BufferObject.h"
#include "FrameBuffer.h"
#include "Program.h"

#include <iostream>

IMP_EXTERN IMP_API imp::RenderPlugin::Ptr loadRenderPlugin()
{
	static imp::GlPlugin::Ptr singleton = NULL;
	if(singleton == NULL) singleton = imp::GlPlugin::create();
	return singleton;
}


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
struct VboData : public RenderPlugin::Data
{
	Meta_Class(VboData)
	VboData() { ty=RenderPlugin::Ty_Vbo; }
	
	BufferObject vbo;
};

//--------------------------------------------------------------
struct TexData : public RenderPlugin::Data
{
	Meta_Class(TexData)
	TexData() { ty=RenderPlugin::Ty_Tex; }
	
	Texture tex;
};

//--------------------------------------------------------------
struct ProgData : public RenderPlugin::Data
{
	Meta_Class(ProgData)
	ProgData() { ty=RenderPlugin::Ty_Shader; }
	
	Program sha;
};

//--------------------------------------------------------------
struct FboData : public RenderPlugin::Data
{
	Meta_Class(FboData)
	FboData() { ty=RenderPlugin::Ty_Tgt; }
	
	FrameBuffer frames;
};

//--------------------------------------------------------------
void GlPlugin::init()
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
void GlPlugin::apply(const ClearNode* clear)
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
void GlPlugin::setCulling(int mode)
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
void GlPlugin::setBlend(int mode)
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
void GlPlugin::setLineW(float lw)
{
	glLineWidth(lw);
}

//--------------------------------------------------------------
void GlPlugin::setViewport(Vec4 vp)
{
	glViewport(vp[0], vp[1], vp[2], vp[3]);
}

//--------------------------------------------------------------
void GlPlugin::setDepthTest(int mode)
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
GlPlugin::Data::Ptr GlPlugin::load(const Geometry* geo)
{
	VboData::Ptr d = VboData::create();
	d->vbo.load(*geo);
	return d;
}

//--------------------------------------------------------------
GlPlugin::Data::Ptr GlPlugin::load(const Sampler* sampler)
{
	Image::Ptr img = sampler->getSource();
	TexData::Ptr d = TexData::create();
	d->tex.loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
	d->tex.setSmooth(sampler->hasSmoothEnable());
	d->tex.setRepeated(sampler->hasRepeatedEnable());
	d->tex.setMipmap(sampler->hasMipmapEnable(), sampler->getMaxMipmapLvl());
	// sampler->_d = d;
	
	return d;
}

//--------------------------------------------------------------
GlPlugin::Data::Ptr GlPlugin::load(const std::string& vert, const std::string& frag)
{
	bool alt = vert.empty();
	ProgData::Ptr d = ProgData::create();
	if(alt) d->sha.load(basicVert.c_str(),basicFrag.c_str());
	else d->sha.load(vert.c_str(),frag.c_str());
	return d;
}

//--------------------------------------------------------------
void GlPlugin::update(Data::Ptr data, const Sampler* sampler)
{
	if(data->ty == Ty_Tex)
	{
		Image::Ptr img = sampler->getSource();
		TexData::Ptr d = std::dynamic_pointer_cast<TexData>(data);
		d->tex.loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
	}
}

//--------------------------------------------------------------
void GlPlugin::bind(Data::Ptr data)
{
	if(data->ty == Ty_Tgt)
	{
		// _target->bind();
		FboData::Ptr d = std::dynamic_pointer_cast<FboData>( data );
		d->frames.bind();
	}
	else if(data->ty == Ty_Shader)
	{
		ProgData::Ptr d = std::dynamic_pointer_cast<ProgData>( data );
		d->sha.use();
		// _shader->enable();
		// _shader->updateAllUniforms();	
	}
	else if(data->ty == Ty_Vbo)
	{
	}
	else if(data->ty == Ty_Tex)
	{
		TexData::Ptr d = std::dynamic_pointer_cast<TexData>( data );
		d->tex.bind();
	}
}

//--------------------------------------------------------------
void GlPlugin::init(Target* target)
{
	FboData::Ptr d = FboData::create();
	d->frames.create(target->width(), target->height(), target->count(), target->hasDepth());
	target->_d = d;
}

//--------------------------------------------------------------
void GlPlugin::unbind(Target* target)
{
	FboData::Ptr d = std::dynamic_pointer_cast<FboData>( target->_d );
	d->frames.unbind();
}

//--------------------------------------------------------------
void GlPlugin::bringBack(Image::Ptr img, Data::Ptr data, int n)
{
	if(data->ty == Ty_Tex)
	{
		TexData::Ptr d = std::dynamic_pointer_cast<TexData>( data );
		d->tex.saveToImage(img);
	}
	
	if(data->ty == Ty_Tgt && n>=0)
	{
		FboData::Ptr d = std::dynamic_pointer_cast<FboData>( data );
		Texture::Ptr tex = d->frames.getTexture(n);
		tex->saveToImage(img);
	}
	
}

//--------------------------------------------------------------
void GlPlugin::draw(Data::Ptr data)
{
	if(data->ty == Ty_Vbo)
	{
		VboData::Ptr d = std::dynamic_pointer_cast<VboData>(data);
		d->vbo.draw();
	}
}

//--------------------------------------------------------------
void GlPlugin::update(Data::Ptr data, const Uniform* uniform)
{
	std::string uId = uniform->getID();
	Uniform::Type type = uniform->getType();
	Uniform::Value value = uniform->getValue();
	ProgData::Ptr sha = std::dynamic_pointer_cast<ProgData>(data);
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
	else if(type == Uniform::Type_Sampler)
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + value.value_1i);
		TexData::Ptr d = std::dynamic_pointer_cast<TexData>(uniform->getSampler()->_d);
		glBindTexture(GL_TEXTURE_2D, d->tex.getVideoID());
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
		std::cout << "impError : Send param (" << uId << ") to shader (error " 
		<< errorState << " : " << strErr << ")" << std::endl;
		std::cout << "loc " << uniformLocation << " in " << sha->sha.id() << std::endl;
	}
	
    GL_CHECKERROR("end of uniform setting");
}

IMPGEARS_END
