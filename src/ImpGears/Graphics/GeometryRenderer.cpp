#include <Graphics/GeometryRenderer.h>

IMPGEARS_BEGIN


//--------------------------------------------------------------
struct DefaultFragCallback : public FragCallback
{
	Meta_Class(DefaultFragCallback)

	virtual void exec(ImageBuf& targets, const Vec3& pt, Uniforms* uniforms = nullptr)
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

	virtual void exec(const Vec3& vert_in, const Vec3& vert_in2, Uniforms& out_uniforms, const GeometryRenderer* rder)
	{
		Vec4 vp = rder->getViewport();
		Vec4 win1(vp[2]*0.5, vp[3]*0.5, 1.0, 1.0);
		Vec4 win2(vp[2]*0.5, vp[3]*0.5, 0.0, 0.0);
		
		Vec4 vertex = vert_in;
		Vec4 mvertex = vertex * rder->getModel();
		Vec4 mvvertex = mvertex * rder->getView();
		Vec4 mvpvertex = mvvertex * rder->getProj();
		
		mvertex /= mvertex.w();
		mvvertex /= mvvertex.w();
		mvpvertex /= mvpvertex.w();
		
		mvpvertex *= win1; mvpvertex += win2;
		
		out_uniforms.set("v_vert",vert_in);
		out_uniforms.set("m_vert",mvertex);
		out_uniforms.set("mv_vert",mvvertex);
		out_uniforms.set("mvp_vert",mvpvertex);
		out_uniforms.set("col_vert",vert_in2);
    }
 };

//--------------------------------------------------------------
GeometryRenderer::GeometryRenderer()
{
	_proj = Matrix4::perspectiveProj(60.0, 1.0, 0.01, 100.0);
	_view = Matrix4::view( Vec3(1,0,0), Vec3(0.0), Vec3(0,0,1) );
	_model = Matrix4::translation(0.0, 0.0, 0.0);
	_viewport = Vec4(0,0,512,512);
	
	_vertCallback = DefaultVertCallback::create();
	_fragCallback = DefaultFragCallback::create();
	_cull = Cull_Back;
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
	
	Uniforms uniforms[3];
	Vec3 mvpVertex[3];
	Vec3 col(1.0);

	// int i = 0;
	for(int i=0;i<(int)geo.size();i+=3)
	{
		for(int k=0;k<3;++k)
		{
			_vertCallback->exec(geo[i+k],col,uniforms[k],this);
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
}

//--------------------------------------------------------------
void GeometryRenderer::setView(const Matrix4& view)
{
	_view = view;
}

//--------------------------------------------------------------
void GeometryRenderer::setModel(const Matrix4& model)
{
	_model = model;
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
