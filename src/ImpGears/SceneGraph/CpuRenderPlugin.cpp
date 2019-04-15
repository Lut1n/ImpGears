#include <SceneGraph/ClearNode.h>
#include <SceneGraph/State.h>
#include <SceneGraph/Sampler.h>
#include <SceneGraph/Target.h>
#include <SceneGraph/CpuRenderPlugin.h>
#include <SceneGraph/Uniform.h>
#include <Geometry/Geometry.h>

#include <Graphics/Rasterizer.h>
#include <Graphics/GeometryRenderer.h>

#include <iostream>

/*IMP_EXTERN IMP_API imp::RenderPlugin::Ptr loadRenderPlugin()
{
	static imp::CpuRenderPlugin::Ptr singleton = NULL;
	if(singleton == NULL) singleton = imp::CpuRenderPlugin::create();
	return singleton;
}*/


IMPGEARS_BEGIN

/// =========== VERTEX SHADER SOURCE =====================
/*static std::string basicVert = IMP_GLSL_SRC(
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
void main() { gl_Position = u_projection * u_view * u_model * gl_Vertex; }
);

/// =========== FRAGMENT SHADER SOURCE =====================
static std::string basicFrag = IMP_GLSL_SRC(
uniform vec3 u_color;
void main() { gl_FragData[0] = vec4(u_color, 1.0); }
);*/

//--------------------------------------------------------------
struct CpuRenderPlugin::Priv
{
	Priv() : _targetOverride(false) {}
	
	// Rasterizer rast;
	GeometryRenderer geoRenderer;
	Image::Ptr depth;
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
	
	const Sampler* sampler;
};

//--------------------------------------------------------------
struct ShaData : public RenderPlugin::Data
{
	Meta_Class(ShaData)
	ShaData() { ty=RenderPlugin::Ty_Shader; }
	
	GeometryRenderer::VertCallback::Ptr vert;
	FragCallback::Ptr frag;
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
		
		s_internalState->depth = Image::create( vp[2] , vp[3], 1 );
		
		s_internalState->defaultTarget.create( (int)vp[2] , (int)vp[3], 1, true);
		s_internalState->target = &s_internalState->defaultTarget;
		
		Image::Ptr tgt = s_internalState->defaultTarget.get(0);
		s_internalState->geoRenderer.setTarget(0,tgt, Vec4(0.0));
		s_internalState->geoRenderer.setTarget(1,s_internalState->depth, Vec4(255.0));
	}
}

//--------------------------------------------------------------
void CpuRenderPlugin::apply(const ClearNode* clear)
{
	GeometryRenderer& r = s_internalState->geoRenderer;
	
	r.init();
	
	/*if(clear->isColorEnable()) r.setClearColor(0, clear->getColor() * 255.0);
	if(clear->isDepthEnable()) r.setClearColor(1, clear->getDepth() * 255.0);
	r.clearTargets();*/
	
	if(clear->isColorEnable()) r.getTarget(0)->fill(clear->getColor() * 255.0);
	if(clear->isDepthEnable()) r.getTarget(1)->fill(clear->getDepth() * 255.0);
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
	
	Image::Ptr depth = s_internalState->depth;
	if( vp[2] != depth->width() || vp[3] != depth->height() )
	{
		depth->resize( vp[2] , vp[3], 1 );
		s_internalState->geoRenderer.setTarget(1,s_internalState->depth, Vec4(255.0));
	}
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
CpuRenderPlugin::Data::Ptr CpuRenderPlugin::load(const Sampler* sampler)
{
	Image::Ptr img = sampler->getSource();
	ImgData::Ptr d = ImgData::create();
	d->sampler = sampler;
	
	return d;
}

//--------------------------------------------------------------
CpuRenderPlugin::Data::Ptr CpuRenderPlugin::load(const ShaderDsc* program)
{
	ShaData::Ptr d = ShaData::create();
	return d;
}

//--------------------------------------------------------------
void CpuRenderPlugin::update(Data::Ptr data, const Sampler* sampler)
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
		
		Image::Ptr tgt = s_internalState->target->get(0);
		s_internalState->geoRenderer.setTarget(0,tgt,Vec4(0.0));
	}
	else if(data->ty == Ty_Shader)
	{
		ShaData::Ptr d = std::dynamic_pointer_cast<ShaData>( data );
		// d->sha.use();
		// _shader->enable();
		// _shader->updateAllUniforms();
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

	Image::Ptr tgt = s_internalState->target->get(0);
	s_internalState->geoRenderer.setTarget(0,tgt,Vec4(0.0));
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
void CpuRenderPlugin::update(Data::Ptr data, const Uniform* uniform)
{
	GeometryRenderer& r = s_internalState->geoRenderer;
	
	std::string name = uniform->getID();
	Uniform::Type type = uniform->getType();
	Uniform::Value value = uniform->getValue();
	
	if(type == Uniform::Type_Mat4v)
	{
		if(name == "u_projection") r.setProj( *value.value_mat4v );
		else if(name == "u_view") r.setView( *value.value_mat4v );
		else if(name == "u_model") r.setModel( *value.value_mat4v );
		// todo
	}
	else if(type == Uniform::Type_3f)
	{
		// if(name == "u_color") // set color
	}
}

IMPGEARS_END
