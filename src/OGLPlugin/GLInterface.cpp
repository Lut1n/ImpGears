#include <SceneGraph/ClearNode.h>
#include <SceneGraph/State.h>
#include <Graphics/Sampler.h>
#include <Renderer/RenderTarget.h>
#include <Graphics/Uniform.h>
#include <Geometry/Geometry.h>

#include <OGLPlugin/GlError.h>
#include <OGLPlugin/Texture.h>
#include <OGLPlugin/GLInterface.h>
#include <OGLPlugin/BufferObject.h>
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

    Texture::Ptr tex;
    CubeMap::Ptr cubetex;
    std::string info;
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
struct GlPlugin::Priv
{
    std::map<Geometry::Ptr,VboData::Ptr> vertexBuffers;
    std::map<ImageSampler::Ptr,TexData::Ptr> samplers;
    std::map<CubeMapSampler::Ptr,TexData::Ptr> cubesamplers;
    std::map<ReflexionModel::Ptr,ProgData::Ptr> callbacks;
    std::map<RenderTarget::Ptr,FboData::Ptr> renderTargets;

    VboData::Ptr getVbo(Geometry::Ptr& g)
    {
        auto it=vertexBuffers.find(g);
        if(it==vertexBuffers.end())return nullptr;
        else return it->second;
    }
    TexData::Ptr getTex(ImageSampler::Ptr& is)
    {
        auto it=samplers.find(is);
        if(it==samplers.end())return nullptr;
        else return it->second;
    }
    TexData::Ptr getTex(CubeMapSampler::Ptr& is)
    {
        auto it=cubesamplers.find(is);
        if(it==cubesamplers.end())return nullptr;
        else return it->second;
    }
    ProgData::Ptr getProg(ReflexionModel::Ptr& cb)
    {
        auto it=callbacks.find(cb);
        if(it==callbacks.end())return nullptr;
        else return it->second;
    }
    FboData::Ptr getFbo(RenderTarget::Ptr& t)
    {
        auto it=renderTargets.find(t);
        if(it==renderTargets.end())return nullptr;
        else return it->second;
    }
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
    }
}

//--------------------------------------------------------------
void GlPlugin::apply(ClearNode::Ptr& clear, int bufferIndex)
{
    if(bufferIndex >= 0) glDrawBuffer(GL_COLOR_ATTACHMENT0+bufferIndex);


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

    // unbind
    if(bufferIndex >= 0) glDrawBuffer(GL_COLOR_ATTACHMENT0);
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
    VboData::Ptr d = s_internalState->getVbo(geo);
    if(d == nullptr)
    {
        d = VboData::create();
        d->vbo.load(*geo);
        s_internalState->vertexBuffers[geo] = d;
    }
    return 0;
}

//--------------------------------------------------------------
int GlPlugin::load(ImageSampler::Ptr& sampler)
{
    TexData::Ptr d = s_internalState->getTex(sampler);
    if(d == nullptr)
    {
        d = TexData::create();
        d->tex = Texture::create();
        Image::Ptr img = sampler->getSource();
        if(img == nullptr)
            std::cout << "error no img in sampler" << std::endl;
        else
            d->tex->loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
        d->tex->setSmooth( sampler->getFiltering() != ImageSampler::Filtering_Nearest );
        d->tex->setRepeated( sampler->getWrapping() == ImageSampler::Wrapping_Repeat );
        // d->tex->setMipmap(sampler->hasMipmapEnable(), sampler->getMaxMipmapLvl());
        s_internalState->samplers[sampler] = d;
    }
    return 0;
}

//--------------------------------------------------------------
int GlPlugin::load(CubeMapSampler::Ptr& sampler)
{
    TexData::Ptr d = s_internalState->getTex(sampler);
    if(d == nullptr)
    {
        d = TexData::create();
        d->cubetex = CubeMap::create();
        d->cubetex->loadFromImage(sampler->getSource());
        s_internalState->cubesamplers[sampler] = d;
    }
    return 0;
}

//--------------------------------------------------------------
int GlPlugin::load(ReflexionModel::Ptr& program)
{
    static const std::string glsl_version = ""; // "#version 330 core\n";

    ProgData::Ptr d = s_internalState->getProg(program);
    if(d != nullptr) return 0;

    d = ProgData::create();

    std::string fullVertCode = basicVert;
    std::string fullFragCode = glsl_version;

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
        fullFragCode = fullFragCode + glsl_invMat3 + glsl_phong;
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

    d->sha.load(fullVertCode.c_str(),fullFragCode.c_str());
        s_internalState->callbacks[program] = d;
    return 0;
}

//--------------------------------------------------------------
void GlPlugin::update(ImageSampler::Ptr& sampler)
{
    TexData::Ptr d = s_internalState->getTex(sampler);
    if(d)
    {
        Image::Ptr img = sampler->getSource();
        d->tex->loadFromMemory(img->asGrid()->data(), img->width(),img->height(),img->channels());
    }
}

//--------------------------------------------------------------
void GlPlugin::bind(RenderTarget::Ptr& target)
{
    FboData::Ptr d = s_internalState->getFbo(target);
    if(d) d->frames.bind();
}

//--------------------------------------------------------------
void GlPlugin::bind(ReflexionModel::Ptr& reflexion)
{
    ProgData::Ptr d = s_internalState->getProg(reflexion);
    if(d) d->sha.use();
}

//--------------------------------------------------------------
void GlPlugin::bind(Geometry::Ptr& geo)
{
}

//--------------------------------------------------------------
void GlPlugin::bind(ImageSampler::Ptr& sampler)
{
    TexData::Ptr d = s_internalState->getTex(sampler);
    if(d) d->tex->bind();
}

//--------------------------------------------------------------
void GlPlugin::bind(CubeMapSampler::Ptr& sampler)
{
    TexData::Ptr d = s_internalState->getTex(sampler);
    if(d) d->cubetex->bind();
}

//--------------------------------------------------------------
void GlPlugin::init(RenderTarget::Ptr& target)
{
    FboData::Ptr d = s_internalState->getFbo(target);
    if(d == nullptr)
    {
        d = FboData::create();

        if(target->useFaceSampler())
        {
            RenderTarget::FaceSampler faceSampler = target->getFace();
            CubeMapSampler::Ptr sampler = faceSampler.first;
            int faceID = faceSampler.second;
            TexData::Ptr dtex = s_internalState->getTex(sampler);
            if(dtex == nullptr)
            {
                load(sampler);
                dtex = s_internalState->getTex(sampler);
                if(dtex == nullptr) std::cout << "error loading cubemap" << std::endl;
                else dtex->info = "loaded from renderTarget (cubemap)";
            }
            CubeMap::Ptr cm = dtex->cubetex;
            d->frames.build(cm, faceID, target->hasDepth());
        }
        else
        {
            std::vector<Texture::Ptr> textures;
            for(int i=0;i<target->count();++i)
            {
                ImageSampler::Ptr s = target->get(i);
                TexData::Ptr dtex = s_internalState->getTex(s);
                if(dtex == nullptr)
                {
                    load(s);
                    dtex = s_internalState->getTex(s);
                    dtex->info = "loaded from renderTarget";
                }
                Texture::Ptr t = dtex->tex;
                textures.push_back(t);
            }
            d->frames.build(textures, target->hasDepth());
        }

        s_internalState->renderTargets[target] = d;
    }
}

//--------------------------------------------------------------
void GlPlugin::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//--------------------------------------------------------------
void GlPlugin::bringBack(ImageSampler::Ptr& sampler)
{
    TexData::Ptr d = s_internalState->getTex(sampler);
    if(d)
    {
        Image::Ptr img = sampler->getSource();
        d->tex->saveToImage(img);
    }
}

//--------------------------------------------------------------
void GlPlugin::draw(Geometry::Ptr& geo)
{
    VboData::Ptr d = s_internalState->getVbo(geo);
    if(d) d->vbo.draw();
}

//--------------------------------------------------------------
void GlPlugin::update(ReflexionModel::Ptr& reflexion, Uniform::Ptr& uniform)
{
    ProgData::Ptr sha = s_internalState->getProg(reflexion);
    if(sha == nullptr) return;

    std::string uId = uniform->getID();
    Uniform::Type type = uniform->getType();
    std::int32_t uniformLocation = sha->sha.locate(uId);
    if(uniformLocation == -1) return;

    if(type == Uniform::Type_1f)
    {
        glUniform1f(uniformLocation, uniform->getFloat1());
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
        TexData::Ptr d = s_internalState->getTex(sampler);
        if(d == nullptr)
        {
            load(sampler);
            d = s_internalState->getTex(sampler);
            if(d == nullptr) std::cout << "error loading texture" << std::endl;
            else d->info = "loaded from uniform sampler";
        }
        glActiveTexture(GL_TEXTURE0 + uniform->getInt1());
        glBindTexture(GL_TEXTURE_2D, d->tex->getVideoID());
        glUniform1i(uniformLocation, uniform->getInt1());
    }
    else if(type == Uniform::Type_CubeMap)
    {
        CubeMapSampler::Ptr sampler = uniform->getCubeMap();
        TexData::Ptr d = s_internalState->getTex(sampler);
        if(d == nullptr)
        {
            load(sampler);
            d = s_internalState->getTex(sampler);
            if(d == nullptr) std::cout << "error loading cubemap" << std::endl;
            else d->info = "loaded from uniform sampler";
        }
        glActiveTexture(GL_TEXTURE0 + uniform->getInt1());
        glBindTexture(GL_TEXTURE_CUBE_MAP, d->cubetex->getVideoID());
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
        std::cout << "loc " << uniformLocation << " in " << sha->sha.id() << std::endl;
    }

    GL_CHECKERROR("end of uniform setting");
}


//--------------------------------------------------------------
SceneRenderer::Ptr GlPlugin::getRenderer()
{
    GlRenderer::Ptr r = GlRenderer::create();
    r->setRenderPlugin(this);
    return r;
}

IMPGEARS_END
