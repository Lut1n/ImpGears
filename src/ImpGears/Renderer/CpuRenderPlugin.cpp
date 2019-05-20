#include <SceneGraph/ClearNode.h>
#include <SceneGraph/State.h>
#include <SceneGraph/TextureSampler.h>
#include <SceneGraph/Target.h>
#include <Renderer/CpuRenderPlugin.h>
#include <Graphics/Uniform.h>
#include <Geometry/Geometry.h>

#include <Graphics/Rasterizer.h>
#include <Graphics/GeometryRenderer.h>
#include <Graphics/CpuBlinnPhong.h>

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
struct CpuRenderPlugin::Priv
{
	Priv() : _targetOverride(false) {}
	
	// Rasterizer rast;
	GeometryRenderer geoRenderer;
	Target defaultTarget;
	Target* target;
	bool _targetOverride;
};

//--------------------------------------------------------------
CpuRenderPlugin::Priv* CpuRenderPlugin::s_internalState = nullptr;


//--------------------------------------------------------------
struct GeoData : public RenderPlugin::Data
{
	Meta_Class(GeoData)
	GeoData() { ty=RenderPlugin::Ty_Vbo; }
	
	const Geometry* geo;
};

//--------------------------------------------------------------
struct ImgData : public RenderPlugin::Data
{
	Meta_Class(ImgData)
	ImgData() { ty=RenderPlugin::Ty_Tex; }
	
	const TextureSampler* sampler;
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
	
	Target* tgt;
};

//--------------------------------------------------------------
void CpuRenderPlugin::init()
{
	std::cout << "CPU Renderer init" << std::endl;
	if(s_internalState == nullptr)
	{
		imp::Vec4 vp(0.0,0.0,512.0,512.0);
		s_internalState = new CpuRenderPlugin::Priv();
		s_internalState->geoRenderer.setViewport( vp );
		
		s_internalState->defaultTarget.create( (int)vp[2] , (int)vp[3], 1, true);
		s_internalState->target = &s_internalState->defaultTarget;
		
		TextureSampler::Ptr tgt = s_internalState->defaultTarget.get(0);
		Image::Ptr sourceTgt = tgt->getSource();
		s_internalState->geoRenderer.setTarget(0,sourceTgt, Vec4(0.0));
		// s_internalState->geoRenderer.setTarget(1,s_internalState->depth, Vec4(255.0));
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::apply(const ClearNode* clear)
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
CpuRenderPlugin::Data::Ptr CpuRenderPlugin::load(const Geometry* geo)
{
	GeoData::Ptr d = GeoData::create();
	d->geo = geo;
	return d;
}

//--------------------------------------------------------------
CpuRenderPlugin::Data::Ptr CpuRenderPlugin::load(const TextureSampler* sampler)
{
	Image::Ptr img = sampler->getSource();
	ImgData::Ptr d = ImgData::create();
	d->sampler = sampler;
	
	return d;
}

//--------------------------------------------------------------
CpuRenderPlugin::Data::Ptr CpuRenderPlugin::load(const LightModel* program)
{
	LightModel::AbstractFrag::Ptr frag = LightModel::AbstractFrag::create();
	
	ShaData::Ptr d = ShaData::create();
	d->vertCb = DefaultVertCb::create();
	d->fragCb = frag;
	
	LightModel::Lighting li = program->getLighting();
	if(li == LightModel::Lighting_None)
	{
		frag->_lighting = NoLighting::create();
	}
	else if(li == LightModel::Lighting_Phong)
	{
		frag->_lighting = PhongLighting::create();
	}
	else
	{
		frag->_lighting = program->_lightingCb;
	}
	
	LightModel::Texturing te = program->getTexturing();
	if(te == LightModel::Texturing_PlainColor)
	{
		frag->_texturing = PlainColorCb::create();
	}
	else if(te == LightModel::Texturing_Samplers_CN)
	{
		frag->_texturing = SamplerCbCN::create();
	}
	else if(te == LightModel::Texturing_Samplers_CNE)
	{
		frag->_texturing = SamplerCbCNE::create();
	}
	else
	{
		frag->_texturing = program->_texturingCb;
	}
	
	LightModel::MRT mrt = program->getMRT();
	if(mrt == LightModel::MRT_1_Col)
	{
		frag->_mrt = Mrt1ColorCb::create();
	}
	else if(mrt == LightModel::MRT_2_Col_Emi)
	{
		frag->_mrt = Mrt1ColorCb::create();
	}
	else
	{
		frag->_mrt = program->_mrtCb;
	}
	
	
	return d;
}

//--------------------------------------------------------------
void CpuRenderPlugin::update(Data::Ptr data, const TextureSampler* sampler)
{
	if(data->ty == Ty_Tex)
	{
		// Image::Ptr img = sampler->getSource();
		ImgData::Ptr d = std::dynamic_pointer_cast<ImgData>(data);
		d->sampler = sampler;
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::bind(Data::Ptr data)
{
	if(data->ty == Ty_Tgt)
	{
		// _target->bind();
		TgtData::Ptr d = std::dynamic_pointer_cast<TgtData>( data );
		
		if(d->tgt != nullptr)
			s_internalState->target = d->tgt;
		else
			s_internalState->target = &s_internalState->defaultTarget;
		
		TextureSampler::Ptr tgt = s_internalState->target->get(0);
		Image::Ptr sourceTgt = tgt->getSource();
		s_internalState->geoRenderer.setTarget(0,sourceTgt,Vec4(0.0));
	}
	else if(data->ty == Ty_Shader)
	{
		ShaData::Ptr d = std::dynamic_pointer_cast<ShaData>( data );
		if(d.get() != nullptr)
		{
			GeometryRenderer& georender = s_internalState->geoRenderer;
			if(d->vertCb == nullptr) georender.setDefaultVertCallback();
			else georender.setVertCallback(d->vertCb);
			if(d->fragCb == nullptr) georender.setDefaultFragCallback();
			else georender.setFragCallback(d->fragCb);
		}
	}
	else if(data->ty == Ty_Vbo)
	{
	}
	else if(data->ty == Ty_Tex)
	{
		ImgData::Ptr d = std::dynamic_pointer_cast<ImgData>( data );
		// d->tex.bind();
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::init(Target* target)
{
	TgtData::Ptr d = TgtData::create();
	d->tgt = target;
	target->_d = d;
}

//--------------------------------------------------------------
void CpuRenderPlugin::unbind(Target* target)
{
	s_internalState->target = &s_internalState->defaultTarget;

	TextureSampler::Ptr tgt = s_internalState->target->get(0);
	Image::Ptr sourceTgt = tgt->getSource();
	s_internalState->geoRenderer.setTarget(0,sourceTgt,Vec4(0.0));
}

//--------------------------------------------------------------
void CpuRenderPlugin::bringBack(Image::Ptr img, Data::Ptr data, int n)
{
	/*if(data->ty == Ty_Tex)
	{
		ImgData::Ptr d = static_cast<ImgData::Ptr>( data );
		d->tex.saveToImage(img);
	}
	
	if(data->ty == Ty_Tgt && n>=0)
	{
		TgtData::Ptr d = static_cast<TgtData::Ptr>( data );
		Texture::Ptr tex = d->frames.getTexture(n);
		tex->saveToImage(img);
	}*/
	
}

//--------------------------------------------------------------
void CpuRenderPlugin::draw(Data::Ptr data)
{
	if(data->ty == Ty_Vbo)
	{
		GeoData::Ptr d = std::dynamic_pointer_cast<GeoData>(data);
		s_internalState->geoRenderer.render( *(d->geo) );
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::update(Data::Ptr data, const Uniform::Ptr& uniform)
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
