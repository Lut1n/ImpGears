#include "RTSceneRenderer.h"

#include <cstdlib>

IMPGEARS_BEGIN





//--------------------------------------------------------------
// Möller–Trumbore intersection algorithm (wikipedia)
bool rayIntersectsTriangle(Vec3 rayOrigin, 
						Vec3 rayVector, 
						Vec3* inTriangle,
						Vec3& outIntersectionPoint)
{
	const float EPSILON = 0.0000001;
	Vec3 vertex0 = inTriangle[0];
	Vec3 vertex1 = inTriangle[1];  
	Vec3 vertex2 = inTriangle[2];
	Vec3 edge1, edge2, h, s, q;
	float a,f,u,v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = rayVector.cross(edge2);
	a = edge1.dot(h);
	if (a > -EPSILON && a < EPSILON)
		return false;    // This ray is parallel to this triangle.
	f = 1.0/a;
	s = rayOrigin - vertex0;
	u = f * (s.dot(h));
	if (u < 0.0 || u > 1.0)
		return false;
	q = s.cross(edge1);
	v = f * rayVector.dot(q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * edge2.dot(q);
	if (t > EPSILON) // ray intersection
	{
		outIntersectionPoint = rayOrigin + rayVector * t;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
}











//--------------------------------------------------------------
RTSceneRenderer::RTSceneRenderer()
	: SceneRenderer()
{
}

//--------------------------------------------------------------
RTSceneRenderer::~RTSceneRenderer()
{
}

//---------------------------------------------------------------
LightNode* closest(Node* node, const std::vector<LightNode*>& ls)
{
	Matrix4 m = node->getModelMatrix();
	Vec3 wPos(m(3,0),m(3,1),m(3,2));
	
	LightNode* res = nullptr;
	float dist = -1;
	
	for(auto l : ls)
	{
		float d = (l->_worldPosition - wPos).length2();
		if(dist == -1 || d<dist) { res=l;dist=d; }
	}
	
	return res;
}

//---------------------------------------------------------------
void RTSceneRenderer::render(const Graph::Ptr& scene)
{	
	Visitor::Ptr visitor = _visitor;
	_visitor->reset();
	scene->accept(visitor);
	
	RenderQueue::Ptr queue = _visitor->getQueue();
	Matrix4 view;
	if(queue->_camera)
	{
		view = queue->_camera->getViewMatrix();
		_camPos = queue->_camera->getAbsolutePosition();
		_view = view;
	}
	
	Vec3 lightPos(0.0);
	Vec3 lightCol(1.0);
	Vec3 color(1.0);
	Vec3 latt(0.0);
	
	for(int i=0;i<(int)queue->_nodes.size();++i)
	{
		LightNode* light = closest(queue->_nodes[i], queue->_lights);
		if(light)
		{
			lightPos = light->_worldPosition;
			lightCol = light->_color;
			latt[0] = light->_power;
		}
		
		GeoNode* geode = dynamic_cast<GeoNode*>( queue->_nodes[i] );
		ClearNode* clear = dynamic_cast<ClearNode*>( queue->_nodes[i] );
		if(geode)
		{
			Material::Ptr mat = geode->_material;
			latt[1] = mat->_shininess;
			color = mat->_color;
			
			if(mat->_baseColor)
				queue->_states[i]->setUniform("u_sampler_color", mat->_baseColor, 0);
			if(mat->_normalmap)
				queue->_states[i]->setUniform("u_sampler_normal", mat->_normalmap, 1);
			if(mat->_emissive)
				queue->_states[i]->setUniform("u_sampler_emissive", mat->_emissive, 2);
		
			queue->_states[i]->setUniform("u_view", view);
			queue->_states[i]->setUniform("u_lightPos", lightPos);
			queue->_states[i]->setUniform("u_lightCol", lightCol);
			queue->_states[i]->setUniform("u_lightAtt", latt);
			queue->_states[i]->setUniform("u_color", color);
			applyState(queue->_states[i]);
			drawGeometry(geode);
		}
		else if(clear)
		{
			applyClear(clear);
		}
	}
}


//---------------------------------------------------------------
void RTSceneRenderer::applyState(const State::Ptr& state)
{
	const std::map<std::string,Uniform::Ptr>& uniforms = state->getUniforms();	
	_proj = uniforms.at("u_proj")->getMat4();
}

//---------------------------------------------------------------
void RTSceneRenderer::applyClear(ClearNode* clearNode)
{
	_target->fill(Vec3(0.0));
}

//---------------------------------------------------------------
Vec2 getTexCoord(const Vec3& vx, int tri, Vec3* arr, Geometry::Ptr& geo)
{
	Vec3 t1 = arr[ 0 ];
	Vec3 t2 = arr[ 1 ];
	Vec3 t3 = arr[ 2 ];
	
	/*
	// compute barycentric coordinate (vector representation)
	Vec3 n1 = t1-t3; n1.normalize();
	Vec3 n2 = t2-t3; n2.normalize();
	Vec3 nx = vx-t3; nx.normalize();
	float u = n1.dot(nx);
	float v = n2.dot(nx);
	float w = 1.0-u-v;*/
	
	// compute barycentric coordinate (matrix representation)
	// T*k = vx-t3
	// with :
	// k = vec(u,v,w)
	// T = matrix(t1-t3|t2-t3)
	//
	// k = inv(T) * (vx-t3)
	float detT = ( (t2.y()-t3.y())*(t1.x()-t3.x()) ) + ( (t3.x()-t2.x())*(t1.y()-t3.y()) );
	float u = ( (t2.y()-t3.y())*(vx.x()-t3.x()) ) + ( (t3.x()-t2.x())*(vx.y()-t3.y()) ); u/=detT;
	float v = ( (t3.y()-t1.y())*(vx.x()-t3.x()) ) + ( (t1.x()-t3.x())*(vx.y()-t3.y()) ); v/=detT;
	float w = 1.0-u-v;
	
	
	Vec2 tx1 = geo->_texCoords[ tri ];
	Vec2 tx2 = geo->_texCoords[ tri+1 ];
	Vec2 tx3 = geo->_texCoords[ tri+2 ];
	
	return tx1*u + tx2*v + tx3*w;
}

//---------------------------------------------------------------
void rayl(const Vec2& uv, const Vec3& r, GeoNode* geoNode, Image::Ptr& _target, const Matrix4& _view)
{
	Geometry::Ptr& geo = geoNode->_geo;
	Matrix3 model = geoNode->getModelMatrix();
	for(int i=0;i<(int)geo->_vertices.size();i+=3)
	{
		Vec3 arr[3];
		arr[0] = Vec4(geo->_vertices[i+0] * model,1.0) * _view;
		arr[1] = Vec4(geo->_vertices[i+1] * model,1.0) * _view;
		arr[2] = Vec4(geo->_vertices[i+2] * model,1.0) * _view;
		
		Vec3 p1p2 = arr[1] - arr[0];
		Vec3 p1p3 = arr[2] - arr[0];
		Vec3 dir = p1p2.cross(p1p3);
		
		bool cullTest = dir[2]>0;
		if(!cullTest) continue;
		
		Vec3 out;
		bool res = rayIntersectsTriangle(Vec3(0.0),r,arr,out);
		if(res)
		{
			Vec2 tex = getTexCoord(out,i,arr,geo);
			
			Material::Ptr mat = geoNode->_material;
			ImageSampler::Ptr sampler = mat->_baseColor;
			Vec4 color = sampler->get(tex); color[3] = 1.0;
			color *= Vec4(0.0,1.0,0.0,1.0);
			const float RES = 32.0;
			_target->setPixel(uv*RES,color * 255.0);
		}
	}
}

//---------------------------------------------------------------
void RTSceneRenderer::drawGeometry(GeoNode* geoNode)
{
	float near = 0.1;
	float fov = 60.0;
	float rad_fov = fov * 3.141592 / 180.0;
	float n_tan = std::tan( rad_fov*0.5 ) * near;
	
	Vec3 camTarget(0.0,0.0,0.0);
	Vec3 camOri = camTarget - _camPos; camOri.normalize();
	Vec3 z = Vec3::Z;
	
	Vec3 y = z.cross(camOri); y.normalize();
	z = camOri.cross(y);
	
	int RES = 32;
	
	for(int i=0;i<RES;++i) for(int j=0;j<RES;++j)
	{
		Vec2 uv = Vec2(i,j) / Vec2(RES);
		Vec2 cuv = uv * 2.0 - 1.0;
		Vec3 ray = Vec3(cuv.x()*n_tan,cuv.y()*n_tan,-near);
		ray.normalize();
		rayl(uv,ray,geoNode,_target,_view);
	}
}

IMPGEARS_END
