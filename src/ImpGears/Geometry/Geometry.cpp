#include <Geometry/Geometry.h>
#include <Core/Matrix3.h>
#include <Core/Perlin.h>

#include <cmath>

IMPGEARS_BEGIN


inline double frac(double x)
{
	return x - std::floor(x);
}

/*
inline Vec3 mix(const Vec3& v1, const Vec3& v2, double f)
{
	return (v2-v1)*f + v1;
}
*/

Geometry::Geometry()
	: _prim(Primitive_Lines)
	, _hasTexCoords(false)
	, _hasColors(false)
	, _hasNormals(false)
	{}

Geometry::Geometry(const Geometry& other)
	: _prim(other._prim)
	, _hasTexCoords(other._hasTexCoords)
	, _hasColors(other._hasColors)
	, _hasNormals(other._hasNormals)
{
	_vertices = other._vertices;
	_texCoords = other._texCoords;
	_normals = other._normals;
	_colors = other._colors;
	
	//_indices = other._indices;
}

void Geometry::operator=(const Geometry& other)
{
	_prim = other._prim;
	_vertices = other._vertices;
	
	_hasTexCoords = other._hasTexCoords;
	_hasColors = other._hasColors;
	_hasNormals = other._hasNormals;
	
	_normals = other._normals;
	_colors = other._colors;
	_texCoords = other._texCoords;
	
	//_indices = other._indices;
}

void Geometry::operator+=(const Geometry& other)
{
	add(other._vertices);
	addColors(other._colors);
	addTexCoords(other._texCoords);
	addNormals(other._normals);
}

Geometry Geometry::operator+(const Geometry& other)
{
	Geometry geo(*this);
	geo += other;
	return geo;
}

void Geometry::origin(const Vec3& origin)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		_vertices[i] -= origin;
	}
}

void Geometry::scale(const Vec3& vec)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i].x() = ( _vertices[i].x() * vec.x() );
		 _vertices[i].y() = ( _vertices[i].y() * vec.y() );
		 _vertices[i].z() = ( _vertices[i].z() * vec.z() );
	}
}

void Geometry::sphericalNormalization(float factor)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		Vec3 vn = _vertices[i];
		 vn.normalize();
		 
		 _vertices[i] = mix(_vertices[i], vn, factor);
	}
}

void Geometry::rotX(float a)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i]*=Matrix3::rotationX(a);
	}
}

void Geometry::rotY(float a)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i]*=Matrix3::rotationY(a);
	}
}

void Geometry::rotZ(float a)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i]*=Matrix3::rotationZ(a);
	}
}

void Geometry::optimize()
{
}

void Geometry::fillBuffer(std::vector<float>& buffer) const
{
	buffer.resize(_vertices.size() * 3);
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		buffer[3*i+0] = _vertices[i].x();
		buffer[3*i+1] = _vertices[i].y();
		buffer[3*i+2] = _vertices[i].z();
	}
}

void Geometry::noiseBump(unsigned int octaveCount, double persistence, double freq, float force)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		// double noise = simplexOctave(_vertices[i].x(), _vertices[i].y(), _vertices[i].z(), octaveCount, persistence, freq, 1.0);
		 double noise = perlinOctave(_vertices[i].x(), _vertices[i].y(), _vertices[i].z(), octaveCount, persistence, freq, 1.0);
		//double noise = (frac( std::sin( _vertices[i].dot( Vec3(84.135815, 12.64412, 2.38741) ) ) * 1354.1468 ) -0.5)*2.0;
		Vec3 n(_vertices[i]);
		n.normalize();
		_vertices[i] += n * (noise * force);
	}
}

void Geometry::bump(SignalFunctor* functor, float force)
{
       for(unsigned int i=0; i<_vertices.size();++i)
       {
        double noise = functor->apply(_vertices[i].x(), _vertices[i].y(), _vertices[i].z());
               Vec3 n(_vertices[i]);
               n.normalize();
               _vertices[i] += n * (noise * force);
    }
}

Geometry Geometry::quad(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p4)
{
	Geometry quad;
	quad.add( {p1,p2,p3, p4,p3,p2} );
	quad.setPrimitive(Primitive_Triangles);
	return quad;
}

Geometry Geometry::cube()
{
	Vec3 p1(-1.0,-1.0,-1.0);
	Vec3 p2(-1.0,-1.0,1.0);
	Vec3 p3(-1.0,1.0,-1.0);
	Vec3 p4(-1.0,1.0,1.0);
	Vec3 p5(1.0,-1.0,-1.0);
	Vec3 p6(1.0,-1.0,1.0);
	Vec3 p7(1.0,1.0,-1.0);
	Vec3 p8(1.0,1.0,1.0);
	
	Geometry cube =
		quad(p1,p2,p3,p4) + quad(p5,p6,p7,p8) + quad(p1,p2,p5,p6) +
		quad(p3,p4,p7,p8) + quad(p1,p3,p5,p7) + quad(p2,p4,p6,p8);
	return cube;
}

Geometry subdivTriangle(const Geometry& buf, int count)
{
	Geometry tri;
	int ite = count+1;
	
	for(int it = 0; it<buf.size(); it+=3)
	{
		Vec3 u = (buf[it+1]-buf[it+0]) / ite;
		Vec3 v = (buf[it+2]-buf[it+0]) / ite;
		
		for(int j=0; j<ite; ++j)
		{
			int maxOnRaw = (ite-j);
			for(int i=0; i<maxOnRaw; ++i)
			{
				Vec3 t1 = buf[it+0] + u*Vec3(i+0) + v*Vec3(j+0);
				Vec3 t2 = buf[it+0] + u*Vec3(i+0) + v*Vec3(j+1);
				Vec3 t3 = buf[it+0] + u*Vec3(i+1) + v*Vec3(j+0);
				Vec3 t4 = buf[it+0] + u*Vec3(i+1) + v*Vec3(j+1);
				
				tri.add({t1,t2,t3});
				if(i < maxOnRaw-1) tri.add({t2,t4,t3});
			}
		}
	}
	
	return tri;
}

Geometry subdivLine(const Geometry& buf, int count)
{
	Geometry segs;
	int ite = count+1;
	
	for(int it = 0; it<buf.size(); it+=2)
	{
		Vec3 u = (buf[it+1]-buf[it+0]) / ite;
		
		for(int i=0; i<ite; ++i)
		{
			Vec3 l1 = buf[it+0] + u*Vec3(i+0);
			Vec3 l2 = buf[it+0] + u*Vec3(i+1);
			segs.add({l1,l2});
		}
	}
	
	return segs;
}

Geometry Geometry::subdivise(int count)
{
	Geometry res;
	if(_prim == Primitive_Lines)
		res = subdivLine(*this, count);
	else if(_prim == Primitive_Triangles)
		res = subdivTriangle(*this, count);
	
	res.setPrimitive(getPrimitive());
	return res;
}

void Geometry::setVertices(const BufType& vertices)
{
	_vertices = vertices;
}

void Geometry::setColors(const BufType& colors)
{
	_colors = colors;
	_hasColors = true;
}

void Geometry::setNormals(const BufType& normals)
{
	_normals = normals;
	_hasNormals = true;
}

Geometry Geometry::tetrahedron()
{
	const double PI = 3.141592;
	const double rad120 = (120.0 * PI / 180.0);
	
	Vec3 p1(0.0,0.0,1.0);
	Vec3 p2 = p1 * Matrix3::rotationY(rad120);
	Vec3 p3 = p2 * Matrix3::rotationZ(rad120);
	Vec3 p4 = p2 * Matrix3::rotationZ(-rad120);
	
	Geometry geo;
	geo.add({p1,p2,p3, p1,p3,p4, p1,p4,p2, p2,p4,p3});
	geo.setPrimitive(Primitive_Triangles);
	
	return geo;
}

void Geometry::setPrimitive(Primitive p) {_prim=p;}
Geometry::Primitive Geometry::getPrimitive() const {return _prim;}

void Geometry::operator+=(const Vec3& v){for(int i=0;i<size();++i)at(i)+=v;}
void Geometry::operator-=(const Vec3& v){for(int i=0;i<size();++i)at(i)-=v;}
void Geometry::operator*=(const Vec3& v){for(int i=0;i<size();++i)at(i)*=v;}
void Geometry::operator*=(float f){for(int i=0;i<size();++i)at(i)*=f;}

int Geometry::size() const {return _vertices.size();}
Vec3& Geometry::at(int i){if(i<0)i=0;if(i>=size())i=size()-1; return _vertices[i];}
const Vec3& Geometry::at(int i) const {if(i<0)i=0;if(i>=size())i=size()-1; return _vertices[i];}
Vec3& Geometry::operator[](int i){return at(i);}
const Vec3& Geometry::operator[](int i) const {return at(i);}

void Geometry::add(const Vec3& v){_vertices.push_back(v);}
void Geometry::add(const BufType& buf){for(auto v:buf)_vertices.push_back(v);}

void Geometry::addNormals(const BufType& buf)
{
	if(buf.size() > 0)_hasNormals = true;
	for(auto v:buf)_normals.push_back(v);
}

void Geometry::addColors(const BufType& buf)
{
	if(buf.size() > 0)_hasColors = true;
	for(auto v:buf)_colors.push_back(v);
}

void Geometry::addTexCoords(const TexCoordBuf& buf)
{
	if(buf.size() > 0)_hasTexCoords = true;
	for(auto v:buf)_texCoords.push_back(v);
}

void Geometry::intoCCW( Geometry& buf )
{
	for(int i=0;i<(int)buf.size();i+=3)
	{
		Vec3 p1 = buf[i+0];
		Vec3 p2 = buf[i+1];
		Vec3 p3 = buf[i+2];
		
		Vec3 t12=p2-p1, t13=p3-p1;
		bool ccw = t13.angleFrom(t12,p1) > 0; // ok for convex shape
		if(!ccw) 
		{
			buf[i+1]=p3;
			buf[i+2]=p2;
			
			if(buf._hasTexCoords)
			{
				TexCoord t1 = buf._texCoords[i+0];
				TexCoord t2 = buf._texCoords[i+1];
				TexCoord t3 = buf._texCoords[i+2];
				buf._texCoords[i+1] = t3;
				buf._texCoords[i+2] = t2;
			}
			
			if(buf._hasNormals)
			{
				Vec3 n1 = buf._normals[i+0];
				Vec3 n2 = buf._normals[i+1];
				Vec3 n3 = buf._normals[i+2];
				buf._normals[i+1] = n3;
				buf._normals[i+2] = n2;
			}
			
			if(buf._hasColors)
			{
				Vec3 c1 = buf._colors[i+0];
				Vec3 c2 = buf._colors[i+1];
				Vec3 c3 = buf._colors[i+2];
				buf._colors[i+1] = c3;
				buf._colors[i+2] = c2;
			}
		}
	}
}

Geometry Geometry::intoLineBuf(const Geometry& buf)
{
	Geometry res;
	for(int i=0;i<(int)buf.size();i+=3)
	{
		Vec3 p1 = buf[i+0];
		Vec3 p2 = buf[i+1];
		Vec3 p3 = buf[i+2];
		
		res.add( {p1,p2} );
		res.add( {p1,p3} );
		res.add( {p2,p3} );
	}
	return res;
}

Geometry Geometry::sphere(int sub, float size)
{
	Geometry geo = tetrahedron();
	geo = geo.subdivise(sub);
	geo.sphericalNormalization(1.0);
	geo*=size;
	return geo;
}

Geometry Geometry::extrude(const Path& base, float len, float ratioTop)
{
	Geometry res;
	
	int size = base.count();
	int i2 = size-1;
	for(int i=0;i<size;++i)
	{
		// side
		Vec3 bt1 = base[i2];
		Vec3 bt2 = base[i];
		Vec3 tp1 = base[i2]*ratioTop + Vec3(0.0,0.0,len);
		Vec3 tp2 = base[i]*ratioTop + Vec3(0.0,0.0,len);
		
		res.add( {bt2,bt1,tp2} );
		res.add( {tp1,tp2,bt1} );
		
		// bottom
		res.add( {Vec3(0.0),bt1,bt2} );

		// top
		if(ratioTop > 0.0) res.add( {Vec3(0.0,0.0,len),tp1,tp2} );
		
		i2=i;
	}
	
	res.setPrimitive(Primitive_Triangles);
	return res;
}

Geometry Geometry::cylinder(int sub, float len, float radius)
{
	
	Path base = Path::circle(sub,radius);
	Geometry res = extrude(base, len,1.0);
	return res;
}


Geometry Geometry::cone(int sub, float len, float radius1, float radius2)
{
	Path base = Path::circle(sub,radius1);
	Geometry res = extrude(base, len,radius2/radius1);
	return res;
}

void Geometry::reduceTriangles(float f)
{
	if(_prim == Primitive_Triangles)
	{
		for(int i=0;i<size();i+=3)
		{
			Vec3 mid = at(i) + at(i+1) + at(i+2);
			mid *= 1.0/3.0;
			at(i) = ((at(i) - mid) * f) + mid;
			at(i+1) = ((at(i+1) - mid) * f) + mid;
			at(i+2) = ((at(i+2) - mid) * f) + mid;
		}
	}
}

void Geometry::setTexCoords(const TexCoordBuf& coords)
{
	_texCoords = coords;
	_hasTexCoords = true;
}

void Geometry::generateNormals()
{
	_normals = BufType(size());
	_hasNormals = true;
	if(_prim != Primitive_Triangles) return;
	
	Vec3 grav_cent;
	for(auto v : _vertices) grav_cent += v;
	grav_cent /= size();
	
	for(int i=0;i<size();i+=3)
	{
		/*Vec3 dir = at(i) - grav_cent;
		Vec3 dx = at(i+1) - at(i); dx.normalize();
		Vec3 dy = at(i+2) - at(i); dy.normalize();
		
		Vec3 n = dx.cross(dy);
		if( n.dot(dir) < 0.0 ) n *= -1.0;*/
		
		Vec3 n1 = at(i) - grav_cent; n1.normalize();
		Vec3 n2 = at(i+1) - grav_cent; n2.normalize();
		Vec3 n3 = at(i+2) - grav_cent; n3.normalize();
		
		_normals[i] = n1;
		_normals[i+1] = n2;
		_normals[i+2] = n3;
	}
}

void Geometry::generateColors(const Vec3& color)
{
	_colors = BufType(size(), color);
	_hasColors = true;
}

void getUV(Vec3& d, float& u, float& v)
{
	Vec3 fs[6] = {-Vec3::X,-Vec3::Y,-Vec3::Z,Vec3::X,Vec3::Y,Vec3::Z};
	float p[6]; int j=0;
	for(int i=0;i<6;++i) { p[i] = fs[i].dot(d); if(p[j]>p[i])j=i; }
	
	Vec3 x, y;
	if(j==0 || j==3) { x=Vec3::Y; y=Vec3::Z; }
	if(j==1 || j==4) { x=Vec3::Z; y=Vec3::X; }
	if(j==2 || j==5) { x=Vec3::X; y=Vec3::Y; }
	
	u = x.dot(d);
	v = y.dot(d);
}

void Geometry::generateTexCoords(float resFactor)
{	
	_texCoords = TexCoordBuf(size());
	_hasTexCoords = true;
	
	Vec3 grav_cent;
	for(auto v : _vertices) grav_cent += v;
	grav_cent /= size();
	
	for(int i=0;i<size();++i)
	{
		Vec3 dir = at(i) - grav_cent; dir.normalize();
		float u,v;
		getUV(dir, u, v);
		_texCoords[i] = Geometry::TexCoord(u*resFactor,v*resFactor);
	}
}


IMPGEARS_END
