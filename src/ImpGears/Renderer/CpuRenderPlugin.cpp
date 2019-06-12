#include <SceneGraph/ClearNode.h>
#include <SceneGraph/State.h>
#include <Renderer/CpuRenderPlugin.h>
#include <Renderer/Uniform.h>

#include <Graphics/Rasterizer.h>
#include <Renderer/GeometryRenderer.h>
#include <Renderer/CpuBlinnPhong.h>

#include "LightModelImpl.h"

#include <iostream>

/*IMP_EXTERN IMP_API imp::RenderPlugin::Ptr loadRenderPlugin()
{
	static imp::CpuRenderPlugin::Ptr singleton = NULL;
	if(singleton == NULL) singleton = imp::CpuRenderPlugin::create();
	return singleton;
}*/


IMPGEARS_BEGIN

//--------------------------------------------------------------
struct GeoData : public RenderPlugin::Data
{
	Meta_Class(GeoData)
	GeoData() { ty=RenderPlugin::Ty_Vbo; }
	
	Geometry::Ptr geo;
};

//--------------------------------------------------------------
struct ImgData : public RenderPlugin::Data
{
	Meta_Class(ImgData)
	ImgData() { ty=RenderPlugin::Ty_Tex; }
	
	ImageSampler::Ptr sampler;
};

//--------------------------------------------------------------
struct ShaData : public RenderPlugin::Data
{
	Meta_Class(ShaData)
	ShaData() { ty=RenderPlugin::Ty_Shader; }
	
	GeometryRenderer::VertCallback::Ptr vertCb;
	FragCallback::Ptr fragCb;
};

//--------------------------------------------------------------
struct TgtData : public RenderPlugin::Data
{
	Meta_Class(TgtData)
	TgtData() { ty=RenderPlugin::Ty_Tgt; }
	
	RenderTarget::Ptr tgt;
};

//--------------------------------------------------------------
struct CpuRenderPlugin::Priv
{
	Priv() : _targetOverride(false) {}
	
	std::map<Geometry::Ptr,GeoData::Ptr> vertexBuffers;
	std::map<ImageSampler::Ptr,ImgData::Ptr> samplers;
	std::map<ReflexionModel::Ptr,ShaData::Ptr> callbacks;
	std::map<RenderTarget::Ptr,TgtData::Ptr> renderTargets;
	
	// Rasterizer rast;
	GeometryRenderer geoRenderer;
	RenderTarget::Ptr defaultRenderTarget;
	RenderTarget::Ptr target;
	bool _targetOverride;
};

//--------------------------------------------------------------
CpuRenderPlugin::Priv* CpuRenderPlugin::s_internalState = nullptr;

//--------------------------------------------------------------
void CpuRenderPlugin::init()
{
	if(s_internalState == nullptr)
	{
		std::cout << "CPU Renderer init" << std::endl;
		imp::Vec4 vp(0.0,0.0,512.0,512.0);
		s_internalState = new CpuRenderPlugin::Priv();
		s_internalState->geoRenderer.setViewport( vp );
		
		s_internalState->defaultRenderTarget = RenderTarget::create();
		s_internalState->defaultRenderTarget->create( (int)vp[2] , (int)vp[3], 1, true);
		s_internalState->target = s_internalState->defaultRenderTarget;
		
		ImageSampler::Ptr tgt = s_internalState->defaultRenderTarget->get(0);
		Image::Ptr sourceTgt = tgt->getSource();
		s_internalState->geoRenderer.setTarget(0,sourceTgt, Vec4(0.0));
		// s_internalState->geoRenderer.setTarget(1,s_internalState->depth, Vec4(255.0));
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::apply(ClearNode::Ptr& clear)
{
	GeometryRenderer& r = s_internalState->geoRenderer;
	
	r.init();
	r.clearTargets();
}

//--------------------------------------------------------------
void CpuRenderPlugin::setCulling(int mode)
{
	GeometryRenderer& r = s_internalState->geoRenderer;
	
	if(mode == 0) r.setCullMode(GeometryRenderer::Cull_None);
	else if(mode == 1) r.setCullMode(GeometryRenderer::Cull_Back);
	else if(mode == 2) r.setCullMode(GeometryRenderer::Cull_Front);
}

//--------------------------------------------------------------
void CpuRenderPlugin::setBlend(int mode)
{
	if(mode == 0)
	{
		// glDisable(GL_BLEND);
	}
	else if(mode == 1)
	{
		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::setLineW(float lw)
{
	// glLineWidth(lw);
}

//--------------------------------------------------------------
void CpuRenderPlugin::setViewport(Vec4 vp)
{
	s_internalState->geoRenderer.setViewport( vp );
}

//--------------------------------------------------------------
void CpuRenderPlugin::setDepthTest(int mode)
{
	if(mode == 1)
	{
		// glEnable(GL_DEPTH_TEST);
		// glDepthFunc(GL_LEQUAL);
		// glDepthMask(GL_TRUE);
	}
	else
	{
		// glDisable(GL_DEPTH_TEST);
	}
}

//--------------------------------------------------------------
int CpuRenderPlugin::load(Geometry::Ptr& geo)
{
	if(s_internalState->vertexBuffers.find(geo) == s_internalState->vertexBuffers.end())
	{
		GeoData::Ptr d = GeoData::create();
		d->geo = geo;
		s_internalState->vertexBuffers[geo] = d;
	}
	return 0;
}

//--------------------------------------------------------------
int CpuRenderPlugin::load(ImageSampler::Ptr& sampler)
{
	if(s_internalState->samplers.find(sampler) == s_internalState->samplers.end())
	{
		ImgData::Ptr d = ImgData::create();
		d->sampler = sampler;
		s_internalState->samplers[sampler] = d;
	}
	return 0;
}

//--------------------------------------------------------------
int CpuRenderPlugin::load(ReflexionModel::Ptr& program)
{
	if(s_internalState->callbacks.find(program) != s_internalState->callbacks.end())
		return 0;
	
	ReflexionModel::AbstractFrag::Ptr frag = ReflexionModel::AbstractFrag::create();
	
	ShaData::Ptr d = ShaData::create();
	d->vertCb = DefaultVertCb::create();
	d->fragCb = frag;
	
	ReflexionModel::Lighting li = program->getLighting();
	if(li == ReflexionModel::Lighting_None)
	{
		frag->_lighting = NoLighting::create();
	}
	else if(li == ReflexionModel::Lighting_Phong)
	{
		frag->_lighting = PhongLighting::create();
	}
	else
	{
		frag->_lighting = program->_lightingCb;
	}
	
	ReflexionModel::Texturing te = program->getTexturing();
	if(te == ReflexionModel::Texturing_PlainColor)
	{
		frag->_texturing = PlainColorCb::create();
	}
	else if(te == ReflexionModel::Texturing_Samplers_CN)
	{
		frag->_texturing = SamplerCbCN::create();
	}
	else if(te == ReflexionModel::Texturing_Samplers_CNE)
	{
		frag->_texturing = SamplerCbCNE::create();
	}
	else
	{
		frag->_texturing = program->_texturingCb;
	}
	
	ReflexionModel::MRT mrt = program->getMRT();
	if(mrt == ReflexionModel::MRT_1_Col)
	{
		frag->_mrt = Mrt1ColorCb::create();
	}
	else if(mrt == ReflexionModel::MRT_2_Col_Emi)
	{
		frag->_mrt = Mrt1ColorCb::create();
	}
	else
	{
		frag->_mrt = program->_mrtCb;
	}
	
	s_internalState->callbacks[program] = d;
	return 0;
}

//--------------------------------------------------------------
void CpuRenderPlugin::update(ImageSampler::Ptr& sampler)
{
	ImgData::Ptr d = s_internalState->samplers.at(sampler);
	if(d) d->sampler = sampler;
}

//--------------------------------------------------------------
void CpuRenderPlugin::bind(RenderTarget::Ptr& target)
{
	TgtData::Ptr d = s_internalState->renderTargets.at(target);
	if(d)
		s_internalState->target = d->tgt;
	else
		s_internalState->target = s_internalState->defaultRenderTarget;
	
	ImageSampler::Ptr tgt = s_internalState->target->get(0);
	Image::Ptr sourceTgt = tgt->getSource();
	s_internalState->geoRenderer.setTarget(0,sourceTgt,Vec4(0.0));
}

//--------------------------------------------------------------
void CpuRenderPlugin::bind(ReflexionModel::Ptr& reflexion)
{
	ShaData::Ptr d = s_internalState->callbacks.at(reflexion);
	if(d)
	{
		GeometryRenderer& georender = s_internalState->geoRenderer;
		if(d->vertCb == nullptr) georender.setDefaultVertCallback();
		else georender.setVertCallback(d->vertCb);
		if(d->fragCb == nullptr) georender.setDefaultFragCallback();
		else georender.setFragCallback(d->fragCb);
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::bind(Geometry::Ptr& geo)
{
}

//--------------------------------------------------------------
void CpuRenderPlugin::bind(ImageSampler::Ptr& sampler)
{
	ImgData::Ptr d = s_internalState->samplers.at(sampler);
}

//--------------------------------------------------------------
void CpuRenderPlugin::init(RenderTarget::Ptr& target)
{
	if(s_internalState->renderTargets.find(target) == s_internalState->renderTargets.end())
	{
		TgtData::Ptr d = TgtData::create();
		d->tgt = target;
		s_internalState->renderTargets[target] = d;
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::unbind()
{
	s_internalState->target = s_internalState->defaultRenderTarget;

	ImageSampler::Ptr tgt = s_internalState->target->get(0);
	Image::Ptr sourceTgt = tgt->getSource();
	s_internalState->geoRenderer.setTarget(0,sourceTgt,Vec4(0.0));
}

//--------------------------------------------------------------
void CpuRenderPlugin::bringBack(ImageSampler::Ptr& sampler)
{
}

//--------------------------------------------------------------
void CpuRenderPlugin::draw(Geometry::Ptr& geo)
{
	GeoData::Ptr d = s_internalState->vertexBuffers.at(geo);
	if(d)
	{
		s_internalState->geoRenderer.render( *(d->geo) );
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::update(ReflexionModel::Ptr& reflexion, Uniform::Ptr& uniform)
{
	GeometryRenderer& r = s_internalState->geoRenderer;
	
	std::string name = uniform->getID();
	Uniform::Type type = uniform->getType();
	
	if(type == Uniform::Type_Mat4)
	{
		if(name == "u_proj") r.setProj( uniform->getMat4() );
		else if(name == "u_view") r.setView( uniform->getMat4() );
		else if(name == "u_model") r.setModel( uniform->getMat4() );
		// todo
	}
	else
	{
		r.setUniform( uniform );
	}
}

IMPGEARS_END
