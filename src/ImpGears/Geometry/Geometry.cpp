#include <Geometry/Geometry.h>
#include <Core/Matrix3.h>
#include <Core/Noise.h>
#include <Core/Math.h>

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
    , _hasIndices(false)
    , _hasTexCoords(false)
    , _hasColors(false)
    , _hasNormals(false)
{}

Geometry::Geometry(const Geometry& other)
    : _prim(other._prim)
    , _hasIndices(other._hasIndices)
    , _hasTexCoords(other._hasTexCoords)
    , _hasColors(other._hasColors)
    , _hasNormals(other._hasNormals)
{
    _vertices = other._vertices;
    _indices = other._indices;
    _texCoords = other._texCoords;
    _normals = other._normals;
    _colors = other._colors;

    //_indices = other._indices;
}

void Geometry::operator=(const Geometry& other)
{
    _prim = other._prim;
    _vertices = other._vertices;

    _hasIndices = other._hasIndices;
    _hasTexCoords = other._hasTexCoords;
    _hasColors = other._hasColors;
    _hasNormals = other._hasNormals;

    _indices = other._indices;
    _normals = other._normals;
    _colors = other._colors;
    _texCoords = other._texCoords;

    //_indices = other._indices;
}

void Geometry::operator+=(const Geometry& other)
{
    add(other._vertices);
    // add(other._indices); // indices will be not consistent
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
        // double noise = fbmSimplex(_vertices[i].x(), _vertices[i].y(), _vertices[i].z(), octaveCount, persistence, freq, 1.0);
        double noise = fbmPerlin(_vertices[i].x(), _vertices[i].y(), _vertices[i].z(), octaveCount, persistence, freq, 1.0);
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

void Geometry::intoCCW( Geometry& buf, bool toExterior )
{
    for(int i=0;i<(int)buf.size();i+=3)
    {
        Vec3 p1 = buf[i+0];
        Vec3 p2 = buf[i+1];
        Vec3 p3 = buf[i+2];

        Vec3 t12=p2-p1, t13=p3-p1;
        Vec3 dir = p1 * (toExterior?1.0:-1.0);
        bool ccw = t13.angleFrom(t12,dir) > 0; // ok for convex shape
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
    if(buf._hasIndices)
    {
        Geometry res = buf;
        res.setPrimitive(Primitive_Lines);
        res._indices.resize( buf._indices.size() * 2 );

        int j=0;
        for(int i=0;i<(int)buf._indices.size(); i+=3)
        {
            res._indices[j+0] = buf._indices[i+0];
            res._indices[j+1] = buf._indices[i+1];

            res._indices[j+2] = buf._indices[i+1];
            res._indices[j+3] = buf._indices[i+2];

            res._indices[j+4] = buf._indices[i+2];
            res._indices[j+5] = buf._indices[i+0];
            j += 6;
        }

        return res;
    }


    BufType colors;
    BufType normals;
    TexCoordBuf coords;
    Geometry res;
    for(int i=0;i<(int)buf.size();i+=3)
    {
        Vec3 p1 = buf[i+0];
        Vec3 p2 = buf[i+1];
        Vec3 p3 = buf[i+2];

        res.add( {p1,p2} );
        res.add( {p1,p3} );
        res.add( {p2,p3} );

        if(buf._hasColors)
        {
            Vec3 c1 = buf._colors[i+0];
            Vec3 c2 = buf._colors[i+1];
            Vec3 c3 = buf._colors[i+2];

            colors.push_back(c1); colors.push_back(c2);
            colors.push_back(c1); colors.push_back(c3);
            colors.push_back(c2); colors.push_back(c3);
        }

        if(buf._hasNormals)
        {
            Vec3 n1 = buf._normals[i+0];
            Vec3 n2 = buf._normals[i+1];
            Vec3 n3 = buf._normals[i+2];

            normals.push_back(n1); normals.push_back(n2);
            normals.push_back(n1); normals.push_back(n3);
            normals.push_back(n2); normals.push_back(n3);
        }

        if(buf._hasTexCoords)
        {
            TexCoord t1 = buf._texCoords[i+0];
            TexCoord t2 = buf._texCoords[i+1];
            TexCoord t3 = buf._texCoords[i+2];

            coords.push_back(t1); coords.push_back(t2);
            coords.push_back(t1); coords.push_back(t3);
            coords.push_back(t2); coords.push_back(t3);
        }
    }

    if(buf._hasColors) res.setColors( colors );
    if(buf._hasNormals) res.setNormals( normals );
    if(buf._hasTexCoords) res.setTexCoords( coords );

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

void Geometry::generateNormals( NormalGenMode genMode )
{
    _normals = BufType(size());
    _hasNormals = true;
    if(_prim != Primitive_Triangles) return;

    Vec3 grav_cent;
    for(auto v : _vertices) grav_cent += v;
    grav_cent /= size();

    for(int i=0;i<size();i+=3)
    {
        Vec3 n1,n2,n3;

        if( genMode == NormalGenMode_PerFace )
        {
            //Vec3 dir = at(i) - grav_cent;
            Vec3 dx = at(i+1) - at(i); // dx.normalize();
            Vec3 dy = at(i+2) - at(i); // dy.normalize();

            Vec3 n = dx.cross(dy); n.normalize();
            // if( n.dot(dir) < 0.0 ) n *= -1.0;

            n1 = n;
            n2 = n;
            n3 = n;
        }

        if( genMode == NormalGenMode_Spheric )
        {
            n1 = at(i) - grav_cent; n1.normalize();
            n2 = at(i+1) - grav_cent; n2.normalize();
            n3 = at(i+2) - grav_cent; n3.normalize();
        }
        _normals[i] = n1;
        _normals[i+1] = n2;
        _normals[i+2] = n3;
    }
}

void Geometry::generateIndices()
{
    // naive method, not optimized
    _indices.resize(_vertices.size());
    for(std::uint32_t i=0;i<_indices.size();++i) _indices[i]=i;

    _hasIndices = true;
}

void Geometry::interpolateNormals()
{
    std::vector<int> done;
    for(int i=0;i<(int)_vertices.size();++i)
    {
        if(std::find(done.begin(),done.end(),i) != done.end()) continue;
        Vec3 n(0.0);
        std::vector<int> indexes;
        for(int j=0;j<(int)_vertices.size();++j) if(_vertices[j]==_vertices[i])
        {
            n += _normals[j];
            indexes.push_back(j);
        }
        n /= indexes.size();
        for(auto it : indexes)
        {
            _normals[it] = n;
            done.push_back(it);
        }
    }
}

void Geometry::generateColors(const Vec3& color)
{
    _colors = BufType(size(), color);
    _hasColors = true;
}

void getUV_spheric(Vec3& triDir, Vec3& d, float& u, float& v)
{
    // const float EPSILON = 0.01;
    const float PI = 3.141592;

    u = std::atan2( d.x(), d.z() ) / PI;
    float eq = std::sqrt(d.x()*d.x() + d.z()*d.z());
    v = std::atan2( d.y(), eq ) / (PI*0.5);

    u = u * 0.5 + 0.5;
    v = v * 0.5 + 0.5;
}


void getUV_cubemap(Vec3& triDir, Vec3& d, float& u, float& v)
{
    enum TargetFace { POSITIVE_X = 0, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z };
    Vec3 majorAxisDirection[6] = {Vec3::X,-Vec3::X,Vec3::Y,-Vec3::Y,Vec3::Z,-Vec3::Z};

    // selection of cube face based on major axis direction
    TargetFace face = POSITIVE_X;
    float ma_rel = 0.0;
    for(int i=0;i<6;++i)
    {
        float curr = majorAxisDirection[i].dot(triDir);
        if(curr > ma_rel) { face=(TargetFace)i; ma_rel=curr; }
    }

    // selection of 2d coordinates and magnitude
    float sc, tc, ma;
    if(face == POSITIVE_X) { sc=-d.z(); tc=-d.y(); ma=d.x(); }
    if(face == NEGATIVE_X) { sc=+d.z(); tc=-d.y(); ma=d.x(); }
    if(face == POSITIVE_Y) { sc=+d.x(); tc=+d.z(); ma=d.y(); }
    if(face == NEGATIVE_Y) { sc=+d.x(); tc=-d.z(); ma=d.y(); }
    if(face == POSITIVE_Z) { sc=+d.x(); tc=-d.y(); ma=d.z(); }
    if(face == NEGATIVE_Z) { sc=-d.x(); tc=-d.y(); ma=d.z(); }

    // using sc, tc and ma for computing u and v
    ma = std::abs(ma);
    u = 0.5 * ( sc/ma + 1.0 );
    v = 0.5 * ( tc/ma + 1.0 );
}

void Geometry::generateTexCoords( TexGenMode genMode, float resFactor)
{	
    _texCoords = TexCoordBuf(size());
    _hasTexCoords = true;

    Vec3 grav_cent;
    for(auto v : _vertices) grav_cent += v;
    grav_cent /= size();

    for(int i=0;i<size();i+=3)
    {
        Vec3 triDir = (at(i)+at(i+1)+at(i+2))/3.0 - grav_cent;
        triDir.normalize();

        for(int j=0;j<3;++j)
        {
            float u,v;
            Vec3 dir = at(i+j) - grav_cent; dir.normalize();
            if(genMode == TexGenMode_Spheric) getUV_spheric(triDir, dir, u, v);
            else if(genMode == TexGenMode_Cubic) getUV_cubemap(triDir, dir, u, v);
            _texCoords[i+j] = Geometry::TexCoord(u*resFactor,v*resFactor);
        }
    }
}


IMPGEARS_END
