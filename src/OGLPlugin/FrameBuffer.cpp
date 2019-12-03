
#include <SceneGraph/Camera.h>

#include <OGLPlugin/FrameBuffer.h>
#include <OGLPlugin/GlError.h>

#include <cstdio>
#include <iostream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
FrameBuffer::FrameBuffer()
	: _id(0)
	, _textureCount(0)
	, _hasDepthBuffer(false)
{
}

//--------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
	destroy();
}

//--------------------------------------------------------------
void FrameBuffer::build(const std::vector<Texture::Ptr>& textures, bool depthBuffer)
{
	destroy();
	_hasDepthBuffer = depthBuffer;
	// TODO : test  GL_MAX_COLOR_ATTACHMENTS & GL_MAX_DRAW_BUFFERS
	_textureCount = textures.size();
	_colorTextures = textures;
	
	if(_textureCount==0) return;

	GLuint id;
	glGenFramebuffers(1, &id);
	_id = (int)id;
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	GL_CHECKERROR("bind FBO");

	GLenum drawBuffers[_textureCount];
	for(int i=0; i<_textureCount; ++i)
	{
		_colorTextures[i]->bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, _colorTextures[i]->getVideoID(), 0);
		drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
		GL_CHECKERROR("set color buffer");
	}
	glDrawBuffers(_textureCount, drawBuffers);
	GL_CHECKERROR("set draw buffers");

	if(_hasDepthBuffer)
	{
		int width = _colorTextures[0]->width();
		int height = _colorTextures[0]->height();
		
		// render buffer
		GLuint rbo = _rbo;
		glGenRenderbuffers(1, &rbo); _rbo = rbo;
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);
		GL_CHECKERROR("set depth buffer");
	}

	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::string string;
		ENUM_TO_STR(error, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, string)
		ENUM_TO_STR(error, GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT, string)
		ENUM_TO_STR(error, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, string)
		ENUM_TO_STR(error, GL_FRAMEBUFFER_UNSUPPORTED, string)
		std::cout << "[impError] FrameBuffer creation failed : " << string << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//--------------------------------------------------------------
void FrameBuffer::build(int width, int height, int textureCount, bool depthBuffer)
{
	std::vector<Texture::Ptr> textures;
	textures.resize(textureCount);
	
	for(int i=0; i<textureCount; ++i)
	{
		textures[i] = Texture::create();
		textures[i]->loadFromMemory(NULL, width, height, 4);
		textures[i]->update();
		textures[i]->bind();
	}
	
	build(textures, depthBuffer);
}

//--------------------------------------------------------------
void FrameBuffer::build(const CubeMap::Ptr& cubemap, int faceID, bool depthBuffer)
{
    destroy();
    _hasDepthBuffer = depthBuffer;
    // TODO : test  GL_MAX_COLOR_ATTACHMENTS & GL_MAX_DRAW_BUFFERS
    _textureCount = 1;
    _colorTextures.clear();
    _cubemap = cubemap;
    _faceID = faceID;
    _useCubeMap = true;

    if(!_cubemap) return;

    GLuint id;
    glGenFramebuffers(1, &id);
    _id = (int)id;
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    GL_CHECKERROR("bind FBO");

    GLenum drawBuffers[_textureCount];
    for(int i=0; i<_textureCount; ++i)
    {
            _cubemap->bind();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_CUBE_MAP_POSITIVE_X+faceID, _cubemap->getVideoID(), 0);
            drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
            GL_CHECKERROR("set color buffer");
    }
    glDrawBuffers(_textureCount, drawBuffers);
    GL_CHECKERROR("set draw buffers");

    if(_hasDepthBuffer)
    {
            int width = 1024.0; // _colorTextures[0]->width();
            int height = 1024.0; // _colorTextures[0]->height();

            // render buffer
            GLuint rbo = _rbo;
            glGenRenderbuffers(1, &rbo); _rbo = rbo;
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);
            GL_CHECKERROR("set depth buffer");
    }

    GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(error != GL_FRAMEBUFFER_COMPLETE)
    {
            std::string string;
            ENUM_TO_STR(error, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, string)
            ENUM_TO_STR(error, GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT, string)
            ENUM_TO_STR(error, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, string)
            ENUM_TO_STR(error, GL_FRAMEBUFFER_UNSUPPORTED, string)
            std::cout << "[impError] FrameBuffer creation failed : " << string << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//--------------------------------------------------------------
void FrameBuffer::destroy()
{
	GLuint id = _id;
	glDeleteFramebuffers(1, &id);

	_colorTextures.clear();
	if(_hasDepthBuffer) {GLuint rbo=_rbo; glDeleteRenderbuffers(1,&rbo);_rbo=rbo;}

    _textureCount = 0;
    _hasDepthBuffer = false;
}

//--------------------------------------------------------------
Texture::Ptr FrameBuffer::getTexture(int n)
{
	return _colorTextures[n];
}

//--------------------------------------------------------------
CubeMap::Ptr FrameBuffer::getCubeMap()
{
    return _cubemap;
}

//--------------------------------------------------------------
int FrameBuffer::getFaceID()
{
    return _faceID;
}

//--------------------------------------------------------------
void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

//--------------------------------------------------------------
void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

IMPGEARS_END
