#include <Graphics/GeometryRenderer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
struct DefaultPlain : public FragCallback
{
	Meta_Class(DefaultPlain)
	
	virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings)
    {
		Vec3 color = varyings.get("color");
		targets[0]->setPixel(pt[0],pt[1], Vec4(color,1.0) * 255.0 );
    }
};

//--------------------------------------------------------------
struct DepthTestFragCallback : public FragCallback
{
	Image::Ptr _depthBuffer;
	FragCallback::Ptr _subCallback;
	bool _depthTestEnabled;
	float _near;
	float _far;
	
	Meta_Class(DepthTestFragCallback)
	
	DepthTestFragCallback()
		: _depthTestEnabled(true)
		, _near(0.1)
		, _far(100.0)
	{
		_subCallback = DefaultPlain::create();
	}
	
	void setDepthBuffer(Image::Ptr depthBuffer)
	{
		_depthBuffer = depthBuffer;
	}
	
	void setSubFragCallback(const FragCallback::Ptr& subcb)
	{
		_subCallback = subcb;
	}
	
	void enableDepthTest(bool b) { _depthTestEnabled = b; }
	
	virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings)
    {
		if(_depthTestEnabled == false)
		{
			if(_subCallback) _subCallback->exec(targets, pt, uniforms, varyings);
			return;
		}
		
		// depth test
		float depth = -varyings.get("mv_vert").z();
		
		float depthPx = clamp( linearstep(_near, _far, depth )  )* 255.0;
		Vec4 depthV = _depthBuffer->getPixel(pt[0], pt[1]);
		float curr_depth = depthV[0];
		if( depthPx < curr_depth)
		{
			Vec4 depth(depthPx);
			_depthBuffer->setPixel(pt[0],pt[1],depth);
			if(_subCallback) _subCallback->exec(targets, pt, uniforms, varyings);
		}
    }
};

//--------------------------------------------------------------
 struct DefaultVertCallback : public GeometryRenderer::VertCallback
 {
	Meta_Class(DefaultVertCallback)

	virtual void exec(const Vec3& vert, GeometryRenderer::Attributes& att, const UniformMap& uniforms, Varyings& out_varyings)
	{
		const Matrix4& model = uniforms.getMat4("u_model");
		const Matrix4& view = uniforms.getMat4("u_view");
		const Matrix4& proj = uniforms.getMat4("u_proj");
		const Matrix3& normalMat = uniforms.getMat3("u_normal");
		
		Vec4 vertex = vert;
		Vec4 mvertex = vertex * model;
		Vec4 mvvertex = mvertex * view;
		Vec4 mvpvertex = mvvertex * proj;
		Vec3 normal = att.normal * normalMat;
		
		mvertex /= mvertex.w();
		mvvertex /= mvvertex.w();
		mvpvertex /= mvpvertex.w();
		
		out_varyings.set("vert",vertex);
		out_varyings.set("m_vert",mvertex);
		out_varyings.set("mv_vert",mvvertex);
		out_varyings.set("mvp_vert",mvpvertex);
		out_varyings.set("color",att.color);
		out_varyings.set("normal",normal);
		out_varyings.set("texUV",att.texUV);
    }
 };

//--------------------------------------------------------------
GeometryRenderer::GeometryRenderer()
{
	_fragCallback = DepthTestFragCallback::create();
	_depthBuffer = Image::create(32,32,1);
	setDefaultVertCallback();
	setDefaultFragCallback();
	_cull = Cull_Back;
	
	// default uniforms
	_uniforms.set( Uniform::create("u_proj",Uniform::Type_Mat4) );
	_uniforms.set( Uniform::create("u_view",Uniform::Type_Mat4) );
	_uniforms.set( Uniform::create("u_model",Uniform::Type_Mat4) );
	
	setProj( Matrix4::perspectiveProj(60.0, 1.0, 0.01, 100.0) );
	setView( Matrix4::view( Vec3(1,0,0), Vec3(0.0), Vec3(0,0,1)) );
	setModel( Matrix4::translation(0.0, 0.0, 0.0) );
	setViewport( Vec4(0,0,512,512) );
	enableDepthTest(true);
}

//--------------------------------------------------------------
GeometryRenderer::~GeometryRenderer()
{
}

//--------------------------------------------------------------
void GeometryRenderer::init()
{
	_rasterizer.clearTarget();
	for(auto t : _targets) _rasterizer.setTarget(t.first, t.second);
	_rasterizer.setFragCallback(_fragCallback);
}

//--------------------------------------------------------------
void GeometryRenderer::clearTargets()
{
	if(_depthTestEnabled) _depthBuffer->fill(Vec4(255.0));
	
	for(int i=0;i<(int)_targets.size();++i) _targets[i]->fill(_clearColors[i]);
}

//--------------------------------------------------------------
void GeometryRenderer::render(const Geometry& geo)
{
	init();
	_rasterizer.setUniformMap(_uniforms);
	
	Varyings varyings[3];
	Vec3 coords[3];
	Attributes attribs;

	if(geo.getPrimitive() == Geometry::Primitive_Triangles)
	{
		for(int i=0;i<(int)geo.size();i+=3)
		{
			for(int k=0;k<3;++k)
			{
				if(geo._hasColors) attribs.color = geo._colors[i+k];
				if(geo._hasNormals) attribs.normal = geo._normals[i+k];
				if(geo._hasTexCoords) attribs.texUV = geo._texCoords[i+k];
				
				_vertCallback->exec(geo[i+k],attribs,_uniforms,varyings[k]);
				coords[k] = varyings[k].get("mvp_vert");
			}
			
			Vec3 p1p2 = coords[1] - coords[0];
			Vec3 p1p3 = coords[2] - coords[0];
			Vec3 dir = p1p2.cross(p1p3);
			
			bool cullTest = _cull==Cull_None;
			if(!cullTest) cullTest = (dir[2]>0) && _cull==Cull_Back;
			if(!cullTest) cullTest = (dir[2]<0) && _cull==Cull_Front;
			
			if(cullTest)
			{
				for(int k=0;k<3;++k)
					coords[k] = (coords[k]+1.0) * Vec3(_viewport[2],_viewport[3],0.0)*0.5 + Vec3(_viewport[0],_viewport[1],0.0);
				
				_rasterizer.setVaryings3(varyings[0],varyings[1],varyings[2]);
				_rasterizer.triangle(coords[0],coords[1],coords[2]);
			}
		}
	}
	else if(geo.getPrimitive() == Geometry::Primitive_Lines)
	{
		for(int i=0;i<(int)geo.size();i+=2)
		{
			for(int k=0;k<2;++k)
			{
				if(geo._hasColors) attribs.color = geo._colors[i+k];
				if(geo._hasNormals) attribs.normal = geo._normals[i+k];
				if(geo._hasTexCoords) attribs.texUV = geo._texCoords[i+k];
				
				_vertCallback->exec(geo[i+k],attribs,_uniforms,varyings[k]);
				coords[k] = varyings[k].get("mvp_vert");
				coords[k] = (coords[k]+1.0) * Vec3(_viewport[2],_viewport[3],0.0)*0.5 + Vec3(_viewport[0],_viewport[1],0.0);
			}
			
			_rasterizer.setVaryings2(varyings[0],varyings[1]);
			_rasterizer.line(coords[0],coords[1]);
		}
	}
}

//--------------------------------------------------------------
void GeometryRenderer::setTarget(int id, Image::Ptr& target, Vec4 clearValue)
{
	_targets[id] = target;
	_clearColors[id] = clearValue;
	
	if(target->width() != _depthBuffer->width()
		|| target->height() != _depthBuffer->height())
		_depthBuffer->resize(target->width(),target->height(),1);
}

//--------------------------------------------------------------
void GeometryRenderer::setClearColor(int targetId, const Vec4& clearValue)
{
	_clearColors[targetId] = clearValue;
}

//--------------------------------------------------------------
Image::Ptr GeometryRenderer::getTarget(int id)
{
	return _targets.at(id);
}

//--------------------------------------------------------------
const Image::Ptr GeometryRenderer::getTarget(int id) const
{
	return _targets.at(id);
}

//--------------------------------------------------------------
const Vec4& GeometryRenderer::getClearColor(int id)
{
	return _clearColors.at(id);
}

//--------------------------------------------------------------
void GeometryRenderer::setProj(const Matrix4& proj)
{
	_proj = proj;
	_uniforms.set("u_proj",_proj);
}

//--------------------------------------------------------------
void GeometryRenderer::setView(const Matrix4& view)
{
	_view = view;
	_uniforms.set("u_view",_view);
}

//--------------------------------------------------------------
void GeometryRenderer::setModel(const Matrix4& model)
{
	_model = model;
	_uniforms.set("u_model",_model);
}

//--------------------------------------------------------------
void GeometryRenderer::setViewport(const Vec4& viewport)
{
	_viewport = viewport;
}

//--------------------------------------------------------------
void GeometryRenderer::setCullMode(Cull mode)
{
	_cull = mode;
}

//--------------------------------------------------------------
void GeometryRenderer::setUniforms(const UniformMap& um)
{
	_uniforms = um;
}

void GeometryRenderer::setUniform(const Uniform::Ptr& uniform)
{
	_uniforms.set(uniform);
}

//--------------------------------------------------------------
void GeometryRenderer::setDefaultVertCallback()
{
	_vertCallback = DefaultVertCallback::create();
}

//--------------------------------------------------------------
void GeometryRenderer::setDefaultFragCallback()
{
	DepthTestFragCallback* ptr = dynamic_cast<DepthTestFragCallback*>(_fragCallback.get());
	DefaultPlain::Ptr fcb = DefaultPlain::create();
	ptr->setSubFragCallback( fcb );
}

//--------------------------------------------------------------
void GeometryRenderer::setVertCallback(const VertCallback::Ptr& callback)
{
	_vertCallback = callback;
}

//--------------------------------------------------------------
void GeometryRenderer::setFragCallback(const FragCallback::Ptr& callback)
{
	DepthTestFragCallback* ptr = dynamic_cast<DepthTestFragCallback*>(_fragCallback.get());
	ptr->setSubFragCallback( callback );
}

//--------------------------------------------------------------
void GeometryRenderer::enableDepthTest(bool b)
{
	DepthTestFragCallback* ptr = dynamic_cast<DepthTestFragCallback*>(_fragCallback.get());
	_depthTestEnabled = b;
	ptr->enableDepthTest(_depthTestEnabled);
	ptr->setDepthBuffer(_depthBuffer);
}

//--------------------------------------------------------------
const Matrix4& GeometryRenderer::getProj() const
{
	return _proj;
}

//--------------------------------------------------------------
const Matrix4& GeometryRenderer::getView() const
{
	return _view;
}

//--------------------------------------------------------------
const Matrix4& GeometryRenderer::getModel() const
{
	return _model;
}

//--------------------------------------------------------------
const Vec4& GeometryRenderer::getViewport() const
{
	return _viewport;
}

IMPGEARS_END
