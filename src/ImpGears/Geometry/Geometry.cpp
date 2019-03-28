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
inline imp::Vec3 mix(const imp::Vec3& v1, const imp::Vec3& v2, double f)
{
	return (v2-v1)*f + v1;
}
*/

Geometry::Geometry()
	: _prim(Primitive_Lines)
	{}

Geometry::Geometry(const Geometry& other)
	: _prim(other._prim)
{
	_vertices = other._vertices;
	// _normals = other._normals;
	//_indices = other._indices;
}

void Geometry::operator=(const Geometry& other)
{
	_vertices = other._vertices;
	_prim = other._prim;
	// _normals = other._normals;
	//_indices = other._indices;
}

void Geometry::operator+=(const Geometry& other)
{
	add(other._vertices);
	//for(unsigned int i=0; i<other._vertices.size();++i)
	{
		// _vertices.push_back(other._vertices[i]);
		// _normals.push_back(other._normals[i]);
	}
	/*int indiceCount = _indices.size();
	for(int i=0; i<other._indices.size();++i)
	{
		_indices.push_back( other._indices[i] + indiceCount) );
	}*/
}

Geometry Geometry::operator+(const Geometry& other)
{
	Geometry geo(*this);
	geo += other;
	return geo;
}

void Geometry::origin(const imp::Vec3& origin)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		_vertices[i] -= origin;
	}
}

void Geometry::scale(const imp::Vec3& vec)
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
		imp::Vec3 vn = _vertices[i];
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

void Geometry::fillBuffer(std::vector<float>& buffer)
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
		// double noise = imp::simplexOctave(_vertices[i].x(), _vertices[i].y(), _vertices[i].z(), octaveCount, persistence, freq, 1.0);
		 double noise = imp::perlinOctave(_vertices[i].x(), _vertices[i].y(), _vertices[i].z(), octaveCount, persistence, freq, 1.0);
		//double noise = (frac( std::sin( _vertices[i].dot( imp::Vec3(84.135815, 12.64412, 2.38741) ) ) * 1354.1468 ) -0.5)*2.0;
		imp::Vec3 n(_vertices[i]);
		n.normalize();
		_vertices[i] += n * (noise * force);
	}
}

void Geometry::bump(SignalFunctor* functor, float force)
{
       for(unsigned int i=0; i<_vertices.size();++i)
       {
        double noise = functor->apply(_vertices[i].x(), _vertices[i].y(), _vertices[i].z());
               imp::Vec3 n(_vertices[i]);
               n.normalize();
               _vertices[i] += n * (noise * force);
    }
}

Geometry Geometry::createQuad(unsigned int subdivisionCount, const imp::Vec3& xvec, const imp::Vec3& yvec, const imp::Vec3& zvec, float size)
{
	const float center = (subdivisionCount+1.f)/2.f;
	const float scaling = size/center;
	
	Geometry quad;
	for(unsigned int u=0;u<subdivisionCount+1;++u)
	{
		for(unsigned int v=0;v<subdivisionCount+1;++v)
		{
			float u1 = (u-center)*scaling ;
			float u2 = (u-center+1.0)*scaling;
			float v1 = (v-center)*scaling;
			float v2 = (v-center+1.0)*scaling;
			quad._vertices.push_back( xvec*u1 + yvec*v1 );
			quad._vertices.push_back( xvec*u2 + yvec*v1 );
			quad._vertices.push_back( xvec*u2 + yvec*v2 );
			quad._vertices.push_back( xvec*u1 + yvec*v2 );
		}
	}
	
	return quad;
}

Geometry Geometry::createCube(unsigned int subdivisionCount)
{
	imp::Vec3 v_x(1.0,0.0,0.0);
	imp::Vec3 v_y(0.0,1.0,0.0);
	imp::Vec3 v_z(0.0,0.0,1.0);
	imp::Vec3 v_ox(-1.0,0.0,0.0);
	imp::Vec3 v_oy(0.0,-1.0,0.0);
	imp::Vec3 v_oz(0.0,0.0,-1.0);
	
	Geometry up = createQuad(subdivisionCount, v_x, v_y, v_z);
	up.origin( v_oz );
	Geometry dw = createQuad(subdivisionCount, v_y, v_x, v_oz);
	dw.origin(v_z );
	Geometry lf = createQuad(subdivisionCount, v_z, v_y, v_ox);
	lf.origin( v_x );
	Geometry rg = createQuad(subdivisionCount, v_y, v_z, v_x);
	rg.origin( v_ox );
	Geometry bc = createQuad(subdivisionCount, v_x, v_z, v_oy);
	bc.origin( v_y );
	Geometry fc = createQuad(subdivisionCount, v_z, v_x, v_y);
	fc.origin( v_oy );
	
	Geometry cube = up + dw + lf + rg + bc + fc;
	
	return cube;
}

Geometry Geometry::generateTriangle(unsigned int subdivisionCount, const imp::Vec3& p1, const imp::Vec3& p2, const imp::Vec3& p3)
{
	Geometry tri;
	
	imp::Vec3 u = (p2-p1) / (subdivisionCount+1);
	imp::Vec3 v = (p3-p1) / (subdivisionCount+1);
	
	for(unsigned int j=0; j<(subdivisionCount+1); ++j)
	{
		unsigned int maxOnRaw = (subdivisionCount-j);
		for(unsigned int i=0; i<maxOnRaw+1; ++i)
		{
			tri._vertices.push_back( p1 + u*(float)i 			+ v*(float)j );
			tri._vertices.push_back( p1 + u*(float)i 			+ v*(float)(j+1) );
			tri._vertices.push_back( p1 + u*(float)(i+1.0)	+ v*(float)j );
			
			if(i < maxOnRaw)
			{
				tri._vertices.push_back( p1 + u*(float)i 			+ v*(float)(j+1.0) );
				tri._vertices.push_back( p1 + u*(float)(i+1.0) 	+ v*(float)(j+1.0) );
				tri._vertices.push_back( p1 + u*(float)(i+1.0) 	+ v*(float)j  );
			}
		}
	}
	
	return tri;
}

Geometry Geometry::createTetrahedron(unsigned int subdivisionCount)
{
	const double PI = 3.141592;
	const double rad120 = (120.0 * PI / 180.0);
	
	imp::Vec3 p1(0.0,0.0,-1.0);
	imp::Vec3 p2(0.0,0.0,-1.0);
	imp::Vec3 p3(0.0,0.0,-1.0);
	imp::Vec3 p4(0.0,0.0,-1.0);
	
	p1*=Matrix3::rotationY(rad120);
	
	p2*=Matrix3::rotationY(rad120);
	p2*=Matrix3::rotationZ(-rad120);
	
	p3*=Matrix3::rotationY(rad120);
	p3*=Matrix3::rotationZ(rad120);
	
	Geometry pir = generateTriangle(subdivisionCount, p1, p2, p3) + generateTriangle(subdivisionCount, p1, p4, p2) + generateTriangle(subdivisionCount, p2, p4, p3) + generateTriangle(subdivisionCount, p3, p4, p1);
	
	return pir;
}

Geometry Geometry::createPyramid(unsigned int baseDivision, unsigned int subdivisionCount)
{
	const double PI = 3.141592;
	const double rad120 = (120.0 * PI / 180.0);
	const double radStep = (2.0*PI)/baseDivision;
	
	Geometry pir;
	
	for(double step = 0; step < 2.0*PI; step+=radStep)
	{
		
		imp::Vec3 p1(0.0,0.0,-1.0);
		
		imp::Vec3 p2 = p1;
		imp::Vec3 p3 = p1;
		imp::Vec3 p4 = p1;
		
		p2*=Matrix3::rotationY(rad120);
		p3*=Matrix3::rotationY(rad120);
		p4*=Matrix3::rotationY(rad120);
		p4.set( 0.0, 0.0, p4.z() );
		
		p2*=Matrix3::rotationZ(step);
		p3*=Matrix3::rotationZ(step+radStep);
		
		pir += generateTriangle(subdivisionCount, p1, p2, p3);
		pir += generateTriangle(subdivisionCount, p3, p2, p4);
	}
	
	return pir;
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
	Geometry geo = createTetrahedron(sub);
	geo.sphericalNormalization(1.0);
	geo*=size;
	geo.setPrimitive(Primitive_Triangles);
	return geo;
}

Geometry Geometry::extrude(const Path& base, float len, float ratioTop, int sub)
{
	Geometry res;
	
	int size = base.count();
	int i2 = size-1;
	for(int i=0;i<size;++i)
	{
		for(int j=0;j<sub+1;++j)
		{
			float f1 = j/(float)(sub+1);
			float f2 = (j+1)/(float)(sub+1);
			
			float h1 = f1*len;
			float h2 = f2*len;
			
			float r1 = mix(1.0f,ratioTop,f1);
			float r2 = mix(1.0f,ratioTop,f2);
			
			Vec3 bt1 = base[i]*r1 + Vec3(0.0,0.0,h1);
			Vec3 bt2 = base[i2]*r1 + Vec3(0.0,0.0,h1);
			Vec3 tp1 = base[i]*r2 + Vec3(0.0,0.0,h2);
			Vec3 tp2 = base[i2]*r2 + Vec3(0.0,0.0,h2);
			
			res.add( {bt2,bt1,tp2} );
			res.add( {tp1,tp2,bt1} );
		}
		
		// bottom
		res.add( {Vec3(0.0),base[i2],base[i]} );
			
		// top
		if(ratioTop > 0.0)
		{
			res.add(Vec3(0.0,0.0,len));
			res.add(base[i2]*ratioTop + Vec3(0.0,0.0,len));
			res.add(base[i]*ratioTop + Vec3(0.0,0.0,len));
		}
		
		i2=i;
	}
	
	// res+=base;
	// res+=top;
	res.setPrimitive(Primitive_Triangles);
	return res;
}

Geometry Geometry::cylinder(int sub, float len, float radius)
{
	
	Path base = Path::circle(sub,radius);
	Geometry res = extrude(base, len,1.0,0);
	return res;
}


Geometry Geometry::cone(int sub, float len, float radius1, float radius2)
{
	
	Path base = Path::circle(sub,radius1);
	Geometry res = extrude(base, len,radius2/radius1,0);
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


IMPGEARS_END
