#include <Geometry/ProceduralGeometry.h>

#include <cmath>

#include <Core/Perlin.h>

IMPGEARS_BEGIN


inline double frac(double x)
{
	return x - std::floor(x);
}

inline imp::Vector3 mix(const imp::Vector3& v1, const imp::Vector3& v2, double f)
{
	return (v2-v1)*f + v1;
}


Geometry::Geometry()
{
}

Geometry::Geometry(const Geometry& other)
{
	_vertices = other._vertices;
	// _normals = other._normals;
	//_indices = other._indices;
}

void Geometry::operator=(const Geometry& other)
{
	_vertices = other._vertices;
	// _normals = other._normals;
	//_indices = other._indices;
}

void Geometry::operator+=(const Geometry& other)
{
	for(unsigned int i=0; i<other._vertices.size();++i)
	{
		_vertices.push_back(other._vertices[i]);
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

void Geometry::origin(const imp::Vector3& origin)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		_vertices[i] -= origin;
	}
}

void Geometry::scale(const imp::Vector3& vec)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i].setX( _vertices[i].getX() * vec.getX() );
		 _vertices[i].setY( _vertices[i].getY() * vec.getY() );
		 _vertices[i].setZ( _vertices[i].getZ() * vec.getZ() );
	}
}

void Geometry::sphericalNormalization(float factor)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		imp::Vector3 vn = _vertices[i];
		 vn.normalize();
		 
		 _vertices[i] = mix(_vertices[i], vn, factor);
	}
}

void Geometry::rotationX(float a)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i].rotationX(a);
	}
}

void Geometry::rotationY(float a)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i].rotationY(a);
	}
}

void Geometry::rotationZ(float a)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		 _vertices[i].rotationZ(a);
	}
}

void Geometry::optimize()
{
}

void Geometry::fillBuffer(std::vector<float>& buffer)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		buffer.push_back( _vertices[i].getX() );
		buffer.push_back( _vertices[i].getY() );
		buffer.push_back( _vertices[i].getZ() );
	}
}

void Geometry::noiseBump(unsigned int octaveCount, double persistence, double freq, float force)
{
	for(unsigned int i=0; i<_vertices.size();++i)
	{
		// double noise = imp::simplexOctave(_vertices[i].getX(), _vertices[i].getY(), _vertices[i].getZ(), octaveCount, persistence, freq, 1.0);
		 double noise = imp::perlinOctave(_vertices[i].getX(), _vertices[i].getY(), _vertices[i].getZ(), octaveCount, persistence, freq, 1.0);
		//double noise = (frac( std::sin( _vertices[i].dotProduct( imp::Vector3(84.135815, 12.64412, 2.38741) ) ) * 1354.1468 ) -0.5)*2.0;
		imp::Vector3 n(_vertices[i]);
		n.normalize();
		_vertices[i] += n * (noise * force);
	}
}

void Geometry::bump(SignalFunctor* functor, float force)
{
       for(unsigned int i=0; i<_vertices.size();++i)
       {
        double noise = functor->apply(_vertices[i].getX(), _vertices[i].getY(), _vertices[i].getZ());
               imp::Vector3 n(_vertices[i]);
               n.normalize();
               _vertices[i] += n * (noise * force);
    }
}

Geometry Geometry::createQuad(unsigned int subdivisionCount, const imp::Vector3& xvec, const imp::Vector3& yvec, const imp::Vector3& zvec, float size)
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
	imp::Vector3 v_x(1.0,0.0,0.0);
	imp::Vector3 v_y(0.0,1.0,0.0);
	imp::Vector3 v_z(0.0,0.0,1.0);
	imp::Vector3 v_ox(-1.0,0.0,0.0);
	imp::Vector3 v_oy(0.0,-1.0,0.0);
	imp::Vector3 v_oz(0.0,0.0,-1.0);
	
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

Geometry Geometry::generateTriangle(unsigned int subdivisionCount, const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3)
{
	Geometry tri;
	
	imp::Vector3 u = (p2-p1) / (subdivisionCount+1);
	imp::Vector3 v = (p3-p1) / (subdivisionCount+1);
	
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
	
	imp::Vector3 p1(0.0,0.0,-1.0);
	imp::Vector3 p2(0.0,0.0,-1.0);
	imp::Vector3 p3(0.0,0.0,-1.0);
	imp::Vector3 p4(0.0,0.0,-1.0);
	
	p1.rotationY(rad120);
	
	p2.rotationY(rad120);
	p2.rotationZ(-rad120);
	
	p3.rotationY(rad120);
	p3.rotationZ(rad120);
	
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
		
		imp::Vector3 p1(0.0,0.0,-1.0);
		
		imp::Vector3 p2 = p1;
		imp::Vector3 p3 = p1;
		imp::Vector3 p4 = p1;
		
		p2.rotationY(rad120);
		p3.rotationY(rad120);
		p4.rotationY(rad120);
		p4.setXYZ( 0.0, 0.0, p4.getZ() );
		
		p2.rotationZ(step);
		p3.rotationZ(step+radStep);
		
		pir += generateTriangle(subdivisionCount, p1, p2, p3);
		pir += generateTriangle(subdivisionCount, p3, p2, p4);
	}
	
	return pir;
}

IMPGEARS_END