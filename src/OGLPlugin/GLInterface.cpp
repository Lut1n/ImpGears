#include <ImpGears/SceneGraph/ClearNode.h>
#include <ImpGears/SceneGraph/State.h>
#include <ImpGears/Graphics/Sampler.h>
#include <ImpGears/Renderer/RenderTarget.h>
#include <ImpGears/Graphics/Uniform.h>
#include <ImpGears/Geometry/Geometry.h>

#include <OGLPlugin/GlError.h>
#include <OGLPlugin/Texture.h>
#include <OGLPlugin/GLInterface.h>
#include <OGLPlugin/VertexArray.h>
#include <OGLPlugin/FrameBuffer.h>
#include <OGLPlugin/CubeMap.h>
#include <OGLPlugin/Program.h>
#include <OGLPlugin/GlRenderer.h>

#include <iostream>

IMP_EXTERN IMP_API imp::RenderPlugin::Ptr loadRenderPlugin()
{
    static imp::GlPlugin::Ptr singleton = NULL;
    if(singleton == NULL) singleton = imp::GlPlugin::create();
    return singleton;
}

#include <OGLPlugin/GlslCode.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
struct GlPlugin::Priv
{
    bool _needInverseMat;
};

//--------------------------------------------------------------
GlPlugin::Priv* GlPlugin::s_internalState = nullptr;

//--------------------------------------------------------------
void GlPlugin::init()
{
    if(s_internalState == nullptr)
    {
        std::cout << "GPU Renderer init" << std::endl;
        s_internalState = new Priv();
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
        
        // debug info
        // int maxVertUniformsVect;
        // glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertUniformsVect);
        // std::cout << "OpenGL GL_MAX_VERTEX_UNIFORM_VECTORS = " << maxVertUniformsVect << std::endl;

        // inverse(mat) is available from gl version 3.1 .
        // Here, glsl version is forced to 1.30 (needInverseMat has te be alway true)
        s_internalState->_needInverseMat = true; // (major<3) || (major==3 && minor==0);
    }
}

//--------------------------------------------------------------
void GlPlugin::apply(ClearNode::Ptr& clear, int bufferIndex)
{
    GLbitfield bitfield = 0;

    if(clear->isColorEnable())
    {
        if( FrameBuffer::s_bound == nullptr )
        {
            Vec4 color = clear->getColor();
            glClearColor(color.x(), color.y(), color.z(), color.w());
            bitfield = bitfield | GL_COLOR_BUFFER_BIT;
        }
        else
        {
            FrameBuffer::s_clearBuffers();
        }

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
        glDepthMask(GL_TRUE); // depth write
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

//--------------------------------------------------------------
int GlPlugin::load(Geometry::Ptr& geo)
{
    if( geo->getRenderData() == nullptr )
    {
        VertexArray::Ptr vao = VertexArray::create();
        vao->load(*geo);
        geo->setRenderData(vao);
    }
    return 0;
}

//--------------------------------------------------------------
int GlPlugin::load(ImageSampler::Ptr& sampler)
{
    if( sampler->getRenderData() == nullptr )
    {
        Texture::Ptr tex = Texture::create();
        tex->setMSAA( sampler->getMSAA() );
        Image::Ptr img = sampler->getSource();
        if(img == nullptr)
            std::cout << "error no img in sampler" << std::endl;
        else
            tex->loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
        tex->setSmooth( sampler->getFiltering() != ImageSampler::Filtering_Nearest );
        tex->setRepeated( sampler->getWrapping() == ImageSampler::Wrapping_Repeat );
        tex->setMipmap(sampler->_mipmap);
        sampler->setRenderData( tex );
    }
    return 0;
}

//--------------------------------------------------------------
int GlPlugin::load(CubeMapSampler::Ptr& sampler)
{
    if(sampler->getRenderData() == nullptr)
    {
        CubeMap::Ptr cubemap = CubeMap::create();
        cubemap->loadFromImage(sampler->getSource());
        sampler->setRenderData(cubemap);
    }
    return 0;
}

//--------------------------------------------------------------
int GlPlugin::load(ReflexionModel::Ptr& program)
{
    static const std::string glsl_version = "#version 330\n"; // "#version 130\n";

    if( program->getRenderData() ) return 0;

    std::string fullVertCode = glsl_version;
    std::string fullFragCode = glsl_version;

    if( !program->_vertCode.empty() )
    {
        fullVertCode = fullVertCode + program->_vertCode;
    }
    else
    {
        fullVertCode = fullVertCode + basicVert;
    }
    
    if(s_internalState->_needInverseMat)
    {
        fullFragCode = fullFragCode + glsl_inverse_mat;
    }

    ReflexionModel::Texturing te = program->getTexturing();

    if(te == ReflexionModel::Texturing_PlainColor)
    {
        fullFragCode = fullFragCode + glsl_samplerNone;
    }
    else if(te == ReflexionModel::Texturing_Samplers_CN)
    {
        fullFragCode = fullFragCode + glsl_samplerCN;
    }
    else if(te == ReflexionModel::Texturing_Samplers_CNE)
    {
        fullFragCode = fullFragCode + glsl_samplerCNE;
    }
    else if(te == ReflexionModel::Texturing_Customized)
    {
        fullFragCode = fullFragCode + program->_fragCode_texturing;
    }

    ReflexionModel::Lighting li = program->getLighting();
    if(li == ReflexionModel::Lighting_None)
    {
        fullFragCode = fullFragCode + basicFrag;
    }
    else if(li == ReflexionModel::Lighting_Phong)
    {
        fullFragCode = fullFragCode + glsl_phong;
    }
    else if(li == ReflexionModel::Lighting_Customized)
    {
        fullFragCode = fullFragCode + program->_fragCode_lighting;
    }

    // ReflexionModel::MRT mrt = program->getMRT();
    // if(mrt == ReflexionModel::MRT_1_Col)
    // {
    //     fullFragCode = fullFragCode + glsl_mrt1;
    // }
    // else if(mrt == ReflexionModel::MRT_2_Col_Emi)
    // {
    //     fullFragCode = fullFragCode + glsl_mrt2;
    // }
    // else
    // {
    //     fullFragCode = fullFragCode + program->_fragCode_mrt;
    // }
    fullFragCode = fullFragCode + glsl_mrt_default;

    Program::Ptr shader = Program::create();
    const std::string& name = program->getName();
    if(name != REFLEXION_DEFAULT_NAME) shader->rename(name);
    shader->load(fullVertCode.c_str(),fullFragCode.c_str());
    program->setRenderData(shader);
    return 0;
}

//--------------------------------------------------------------
void GlPlugin::update(ImageSampler::Ptr& sampler)
{
    Texture::Ptr tex = Texture::dMorph( sampler->getRenderData() );
    if(tex)
    {
        Image::Ptr img = sampler->getSource();
        tex->loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
    }
}

//--------------------------------------------------------------
void GlPlugin::bind(RenderTarget::Ptr& target)
{
    FrameBuffer::Ptr fbo = FrameBuffer::dMorph( target->getRenderData() );
    if(fbo) fbo->bind();
}

//--------------------------------------------------------------
void GlPlugin::bind(ReflexionModel::Ptr& reflexion)
{
    Program::Ptr program = Program::dMorph( reflexion->getRenderData() );
    if(program) program->use();
}

//--------------------------------------------------------------
void GlPlugin::bind(Geometry::Ptr& geo)
{
}

//--------------------------------------------------------------
void GlPlugin::bind(ImageSampler::Ptr& sampler)
{
    Texture::Ptr tex = Texture::dMorph( sampler->getRenderData() );
    if(tex)
    {
        if(sampler->_needUpdate)
        {
            tex->update();
            sampler->_needUpdate = false;
        }
        tex->bind();
    }
}

//--------------------------------------------------------------
void GlPlugin::bind(CubeMapSampler::Ptr& sampler)
{
    CubeMap::Ptr cubemap = CubeMap::dMorph( sampler->getRenderData() );
    if(cubemap)
    {
        if(sampler->_needUpdate)
        {
            cubemap->update();
            sampler->_needUpdate = false;
        }
        cubemap->bind();
    }
}

//--------------------------------------------------------------
void GlPlugin::init(RenderTarget::Ptr& target)
{
    if(target->getRenderData() == nullptr)
    {
        FrameBuffer::Ptr fbo = FrameBuffer::create();

        if(target->useFaceSampler())
        {
            RenderTarget::FaceSampler faceSampler = target->getFace();
            CubeMapSampler::Ptr sampler = faceSampler.first;
            int faceID = faceSampler.second;
            CubeMap::Ptr cubemap = CubeMap::dMorph( sampler->getRenderData() );
            if(cubemap == nullptr)
            {
                load( sampler );
                cubemap = CubeMap::dMorph( sampler->getRenderData() );
                if(cubemap == nullptr) std::cout << "error loading cubemap" << std::endl;
                // else dtex->info = "loaded from renderTarget (cubemap)";
            }
            fbo->build(cubemap, faceID, target->hasDepth());
        }
        else
        {
            std::vector<Texture::Ptr> textures;
            for(int i=0;i<target->count();++i)
            {
                ImageSampler::Ptr s = target->get(i);
                Texture::Ptr tex = Texture::dMorph( s->getRenderData() );
                if(tex == nullptr)
                {
                    load(s);
                    tex = Texture::dMorph( s->getRenderData() );
                    // dtex->info = "loaded from renderTarget";
                }
                textures.push_back(tex);
            }
            fbo->build(textures, target->hasDepth());
        }

        std::vector<Vec4> clearColors = target->getClearColors();
        fbo->setClearColors( clearColors );
        target->setRenderData( fbo );
    }
}

//--------------------------------------------------------------
void GlPlugin::unbind()
{
    if(FrameBuffer::s_bound != nullptr)
        FrameBuffer::s_bound->unbind();
}

//--------------------------------------------------------------
void GlPlugin::bringBack(ImageSampler::Ptr& sampler)
{
    Texture::Ptr tex = Texture::dMorph( sampler->getRenderData() );
    if(tex)
    {
        Image::Ptr img = sampler->getSource();
        tex->saveToImage(img);
    }
}

//--------------------------------------------------------------
void GlPlugin::draw(Geometry::Ptr& geo)
{
    VertexArray::Ptr vao = VertexArray::dMorph( geo->getRenderData() );
    if(vao) vao->draw();
}

//--------------------------------------------------------------
void GlPlugin::update(ReflexionModel::Ptr& reflexion, Uniform::Ptr& uniform)
{
    Program::Ptr program = Program::dMorph( reflexion->getRenderData() );
    if(program == nullptr) return;

    std::string uId = uniform->getID();
    Uniform::Type type = uniform->getType();
    std::int32_t uniformLocation = program->locate(uId);
    if(uniformLocation == -1) return;

    if(type == Uniform::Type_1f)
    {
        glUniform1f(uniformLocation, uniform->getFloat1());
    }
    else if(type == Uniform::Type_1fv)
    {
        const std::vector<float>& fv = uniform->getFloat1v();
        glUniform1fv(uniformLocation, fv.size(), fv.data());
    }
    else if(type == Uniform::Type_2f)
    {
        const Vec2& f2 = uniform->getFloat2();
        glUniform2f(uniformLocation, f2[0], f2[1]);
    }
    else if(type == Uniform::Type_3f)
    {
        const Vec3& f3 = uniform->getFloat3();
        glUniform3f(uniformLocation, f3[0], f3[1], f3[2]);
    }
    else if(type == Uniform::Type_4f)
    {
        const Vec4& f4 = uniform->getFloat4();
        glUniform4f(uniformLocation, f4[0], f4[1], f4[2], f4[3]);
    }
    else if(type == Uniform::Type_1i)
    {
        glUniform1i(uniformLocation, uniform->getInt1());
    }
    else if(type == Uniform::Type_Mat3)
    {
        glUniformMatrix3fv(uniformLocation, 1, false, uniform->getMat3().data());
    }
    else if(type == Uniform::Type_Mat4)
    {
        glUniformMatrix4fv(uniformLocation, 1, false, uniform->getMat4().data());
    }
    else if(type == Uniform::Type_Sampler)
    {
        ImageSampler::Ptr sampler = uniform->getSampler();
        Texture::Ptr tex = Texture::dMorph( sampler->getRenderData() );
        glActiveTexture(GL_TEXTURE0 + uniform->getInt1());
        if(tex == nullptr)
        {
            load(sampler);
            tex = Texture::dMorph( sampler->getRenderData() );
            if(tex == nullptr) std::cout << "error loading texture" << std::endl;
            // else d->info = "loaded from uniform sampler";
        }
        if(sampler->_needUpdate)
        {
            tex->update();
            sampler->_needUpdate = false;
        }
        tex->bind();
        glUniform1i(uniformLocation, uniform->getInt1());
    }
    else if(type == Uniform::Type_CubeMap)
    {
        CubeMapSampler::Ptr sampler = uniform->getCubeMap();
        CubeMap::Ptr cubemap = CubeMap::dMorph( sampler->getRenderData() );
        glActiveTexture(GL_TEXTURE0 + uniform->getInt1());
        if(cubemap == nullptr)
        {
            load(sampler);
            cubemap = CubeMap::dMorph( sampler->getRenderData() );
            if(cubemap == nullptr) std::cout << "error loading cubemap" << std::endl;
            // else d->info = "loaded from uniform sampler";
        }
        if(sampler->_needUpdate)
        {
            cubemap->update();
            sampler->_needUpdate = false;
        }
        cubemap->bind();
        glUniform1i(uniformLocation, uniform->getInt1());
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
        std::cout << "loc " << uniformLocation << " in " << program->id() << std::endl;
    }

    GL_CHECKERROR("end of uniform setting");
}

//--------------------------------------------------------------
void GlPlugin::unload(Geometry::Ptr& geo)
{
    geo->setRenderData(nullptr);
}

//--------------------------------------------------------------
void GlPlugin::unload(ImageSampler::Ptr& sampler)
{
    sampler->setRenderData(nullptr);
}

//--------------------------------------------------------------
void GlPlugin::unload(CubeMapSampler::Ptr& sampler)
{
    sampler->setRenderData(nullptr);
}

//--------------------------------------------------------------
void GlPlugin::unload(RenderTarget::Ptr& target)
{
    target->setRenderData(nullptr);
}

//--------------------------------------------------------------
void GlPlugin::unload(ReflexionModel::Ptr& shader)
{
    shader->setRenderData(nullptr);
}

//--------------------------------------------------------------
void GlPlugin::unloadUnused()
{
}


//--------------------------------------------------------------
SceneRenderer::Ptr GlPlugin::getRenderer()
{
    GlRenderer::Ptr r = GlRenderer::create();
    r->setRenderPlugin(this);
    return r;
}

IMPGEARS_END
