#include <Graphics/GeometryRenderer.h>

IMPGEARS_BEGIN


//--------------------------------------------------------------
struct DefaultFragCallback : public FragCallback
{
	Meta_Class(DefaultFragCallback)

	virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr)
	{
		int rgb_target = 0, depth_target = 1;
		// depth test
		float depth = -uniforms->get("mv_vert").z();
		
		float depthPx = clamp( linearstep(0.1f, 20.f, depth )  )* 255.0;
		Vec4 depthV = targets[depth_target]->getPixel(pt[0], pt[1]);
		float curr_depth = depthV[0];
		if( depthPx < curr_depth)
		{
			Vec4 color(depthPx); color[3] = 255;
			Vec4 depth(depthPx);
			targets[rgb_target]->setPixel(pt[0],pt[1],color);
			targets[depth_target]->setPixel(pt[0],pt[1],depth);
		}
	}
};

//--------------------------------------------------------------
 struct DefaultVertCallback : public GeometryRenderer::VertCallback
 {
	Meta_Class(DefaultVertCallback)

	virtual void exec(const Vec3& vert, GeometryRenderer::Attributes& att, const CnstUniforms& cu, Uniforms& out_uniforms)
	{
		const Matrix4& model = *(cu.at("u_model")->getValue().value_mat4v);
		const Matrix4& view = *(cu.at("u_view")->getValue().value_mat4v);
		const Matrix4& proj = *(cu.at("u_proj")->getValue().value_mat4v);
		const Vec4& vp = *(cu.at("u_vp")->getValue().value_4f);
		
		Vec4 win1(vp[2]*0.5, vp[3]*0.5, 1.0, 1.0);
		Vec4 win2(vp[2]*0.5, vp[3]*0.5, 0.0, 0.0);
		
		Vec4 vertex = vert;
		Vec4 mvertex = vertex * model;
		Vec4 mvvertex = mvertex * view;
		Vec4 mvpvertex = mvvertex * proj;
		
		mvertex /= mvertex.w();
		mvvertex /= mvvertex.w();
		mvpvertex /= mvpvertex.w();
		
		mvpvertex *= win1; mvpvertex += win2;
		
		out_uniforms.set("vert",vertex);
		out_uniforms.set("m_vert",mvertex);
		out_uniforms.set("mv_vert",mvvertex);
		out_uniforms.set("mvp_vert",mvpvertex);
		out_uniforms.set("color",att.color);
		out_uniforms.set("normal",att.normal);
		out_uniforms.set("texUV",att.texUV);
    }
 };

//--------------------------------------------------------------
GeometryRenderer::GeometryRenderer()
{	
	setDefaultVertCallback();
	setDefaultFragCallback();
	_cull = Cull_Back;
	
	// default uniforms
	_uniforms["u_proj"] = Uniform::create("u_proj",Uniform::Type_Mat4v);
	_uniforms["u_view"] = Uniform::create("u_view",Uniform::Type_Mat4v);
	_uniforms["u_model"] = Uniform::create("u_model",Uniform::Type_Mat4v);
	_uniforms["u_vp"] = Uniform::create("u_vp",Uniform::Type_4f);
	
	setProj( Matrix4::perspectiveProj(60.0, 1.0, 0.01, 100.0) );
	setView( Matrix4::view( Vec3(1,0,0), Vec3(0.0), Vec3(0,0,1)) );
	setModel( Matrix4::translation(0.0, 0.0, 0.0) );
	setViewport( Vec4(0,0,512,512) );
}

//--------------------------------------------------------------
GeometryRenderer::~GeometryRenderer()
{
}

//--------------------------------------------------------------
void GeometryRenderer::init()
{
	_rasterizer.clearTarget();
	for(auto t : _targets) _rasterizer.addTarget(t);
	_rasterizer.setFragCallback(_fragCallback);
}

//--------------------------------------------------------------
void GeometryRenderer::clearTargets()
{
	for(int i=0;i<(int)_targets.size();++i) _targets[i]->fill(_clearColors[i]);
}

//--------------------------------------------------------------
void GeometryRenderer::render(const Geometry& geo)
{
	init();
	_rasterizer.setCnstUniforms(_uniforms);
	
	Uniforms uniforms[3];
	Vec3 mvpVertex[3];
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
				
				_vertCallback->exec(geo[i+k],attribs,_uniforms,uniforms[k]);
				mvpVertex[k] = uniforms[k].get("mvp_vert");
			}
			
			Vec3 p1p2 = mvpVertex[1] - mvpVertex[0];
			Vec3 p1p3 = mvpVertex[2] - mvpVertex[0];
			Vec3 dir = p1p2.cross(p1p3);
			
			bool cullTest = _cull==Cull_None;
			if(!cullTest) cullTest = (dir[2]>0) && _cull==Cull_Back;
			if(!cullTest) cullTest = (dir[2]<0) && _cull==Cull_Front;
			
			if(cullTest)
			{
				_rasterizer.setUniforms3(uniforms[0],uniforms[1],uniforms[2]);
				_rasterizer.triangle(mvpVertex[0],mvpVertex[1],mvpVertex[2]);
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
				
				_vertCallback->exec(geo[i+k],attribs,_uniforms,uniforms[k]);
				mvpVertex[k] = uniforms[k].get("mvp_vert");
			}
			
			_rasterizer.setUniforms2(uniforms[0],uniforms[1]);
			_rasterizer.line(mvpVertex[0],mvpVertex[1]);
		}
	}
}

//--------------------------------------------------------------
void GeometryRenderer::setTarget(int id, Image::Ptr& target, Vec4 clearValue)
{
	if(id >= (int)_targets.size())
	{
		_targets.resize(id+1);
		_clearColors.resize(id+1);
	}
	
	_targets[id] = target;
	_clearColors[id] = clearValue;
}

//--------------------------------------------------------------
void GeometryRenderer::setClearColor(int targetId, const Vec4& clearValue)
{
	if(targetId>0 && targetId < (int)_targets.size())
	{
		_clearColors[targetId] = clearValue;
	}
}

//--------------------------------------------------------------
Image::Ptr GeometryRenderer::getTarget(int id)
{
	return _targets[id];
}

//--------------------------------------------------------------
const Image::Ptr GeometryRenderer::getTarget(int id) const
{
	return _targets[id];
}

//--------------------------------------------------------------
const Vec4& GeometryRenderer::getClearColor(int id)
{
	return _clearColors[id];
}

//--------------------------------------------------------------
void GeometryRenderer::setProj(const Matrix4& proj)
{
	_proj = proj;
	if(_uniforms.find("u_proj") != _uniforms.end()) _uniforms["u_proj"]->set(&_proj);
}

//--------------------------------------------------------------
void GeometryRenderer::setView(const Matrix4& view)
{
	_view = view;
	if(_uniforms.find("u_view") != _uniforms.end()) _uniforms["u_view"]->set(&_view);
}

//--------------------------------------------------------------
void GeometryRenderer::setModel(const Matrix4& model)
{
	_model = model;
	if(_uniforms.find("u_model") != _uniforms.end()) _uniforms["u_model"]->set(&_model);
}

//--------------------------------------------------------------
void GeometryRenderer::setViewport(const Vec4& viewport)
{
	_viewport = viewport;
	if(_uniforms.find("u_vp") != _uniforms.end()) _uniforms["u_vp"]->set(&_viewport);
}

//--------------------------------------------------------------
void GeometryRenderer::setCullMode(Cull mode)
{
	_cull = mode;
}

//--------------------------------------------------------------
void GeometryRenderer::setUniforms(const CnstUniforms& cu)
{
	_uniforms = cu;
}

//--------------------------------------------------------------
void GeometryRenderer::setDefaultVertCallback()
{
	_vertCallback = DefaultVertCallback::create();
}

//--------------------------------------------------------------
void GeometryRenderer::setDefaultFragCallback()
{
	_fragCallback = DefaultFragCallback::create();
}

//--------------------------------------------------------------
void GeometryRenderer::setVertCallback(const VertCallback::Ptr& callback)
{
	_vertCallback = callback;
}

//--------------------------------------------------------------
void GeometryRenderer::setFragCallback(const FragCallback::Ptr& callback)
{
	_fragCallback = callback;
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
