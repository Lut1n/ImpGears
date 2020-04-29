#include <ImpGears/Geometry/Polyhedron.h>
#include <map>


IMPGEARS_BEGIN

void processBar(float p)
{
    std::cout << '\r';
    std::cout << "[";
    for(int i=0;i<p*100;++i) std::cout << "x";
    for(int i=p*100;i<100;++i) std::cout << ".";
    std::cout << "] " << (int)(p*100) << "%";
}

/*std::string asstring(const Vec3& v)
{
    std::stringstream ss;
    ss << "[" << v[0] << ";" << v[1] << ";" << v[2] << "]";
    return ss.str();
}*/

//--------------------------------------------------------------
float signOf(float f) {return f>0.0?1.0:-1.0;}

//--------------------------------------------------------------
Polyhedron::Polyhedron()
{
}

//--------------------------------------------------------------
Polyhedron::~Polyhedron()
{
}

//--------------------------------------------------------------
void Polyhedron::set(const Path::BufType& triangleBuf)
{
    for(int i=0;i<(int)triangleBuf.size();i+=3)
        addFace(Path({triangleBuf[i+0],triangleBuf[i+1],triangleBuf[i+2]}));
}
//--------------------------------------------------------------
void Polyhedron::getTriangles(Path::BufType& triangleBuf) const
{
    triangleBuf.clear();
    for(int i=0;i<faceCount();++i)
    {
        Path::BufType triangles = Triangulation::triangulate(faces[i]);
        for(auto v : triangles) triangleBuf.push_back(v);
    }
}
int Polyhedron::faceCount() const {return faces.size();}
Path& Polyhedron::operator[](int i) {return faces[i];}
Path Polyhedron::operator[](int i) const {return faces[i];}
void Polyhedron::clear() {faces.clear();}
void Polyhedron::addFace(const Path& face) {faces.push_back(face);}
//--------------------------------------------------------------
bool Polyhedron::contains(const Vec3& p) const
{
    int cpt = 0;
    Vec3 ipoint;
    Edge seg(externalPoint(),p);
    for(int i=0;i<faceCount();++i)
    {
        if( iSegFace(seg,faces[i],ipoint,false)) cpt++;
    }
    return cpt%2 != 0;
}
//--------------------------------------------------------------
Vec3 Polyhedron::externalPoint() const
{
    Vec3 e;
    for(auto f:faces)
    {
        for(auto p:f.data())
        {
            if(e[0] > p[0])e[0]=p[0];
            if(e[1] > p[1])e[1]=p[1];
            if(e[2] > p[2])e[2]=p[2];
        }
    }

    e -= Vec3(10.3875452,10.7656347,10.0143432);

    return e;
}

//--------------------------------------------------------------
bool Polyhedron::iSegFace( const Edge& seg, const Path& face, Vec3& iPoint, bool strict)
{
    Vec3 n = face.normal();

    float d1 = n.dot(seg._p1-face[0]);
    float d2 = n.dot(seg._p2-face[0]);

    bool sideTest = signOf(d1)!=signOf(d2) && (d1!=0.0 && d2!=0.0);
    if(!strict) sideTest = d1==0.0 || d2==0.0 || signOf(d1)!=signOf(d2);

    // check sides
    if (sideTest)
    {
        float l = std::abs(d1) + std::abs(d2);
        float t = l>0.0 ? (std::abs(d1) / l) : 0.0;
        iPoint = mix(seg._p1,seg._p2,t);

        // return face.inside(iPoint);

        // optimized algorithm for convex shape
        Vec3 last = face[-1];
        for(int i=0;i<face.count();++i)
        {
            Vec3 curr = face[i];
            // test demi-plane
            Vec3 dp = n.cross( curr-last );
            float sideOri = dp.dot(iPoint-last);
            if(sideOri<0 || (strict && sideOri==0.0))return false;
            last = curr;
        }

        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool Polyhedron::iFace2(const Path& f1, const Path& f2, Path::BufType& pts)
{
    Vec3 ip;
    int is = pts.size();
    Edge seg(f1[-1],f1[0]);
    for(int i=0;i<f1.count();++i)
    {
        seg._p2 = f1[i];
        if(iSegFace(seg,f2,ip,false)) pts.push_back(ip);
        seg._p1 = seg._p2;
    }
    return ((int)pts.size() > is);
}

//--------------------------------------------------------------
bool Polyhedron::iFacePh(const Path& face, const Polyhedron& ph, std::vector<Edge>& eout)
{
    bool res = false;

    for(int j=0;j<(int)ph.faceCount();++j)
    {
        Path::BufType o;
        bool r = iFace2(face,ph[j],o);
        r |= iFace2(ph[j],face,o);
        res = res || r;
        if(r)eout.push_back( Edge(o[0],o[1]) );
    }

    return res;
}

std::vector<Path> Polyhedron::cutFace(const Path& face, Edge cutter)
{
    std::vector<Path> res;

    std::map<int,Vec3> toInsert;
    Vec3 n = face.normal();

    Vec3 tan = cutter._p2 - cutter._p1; tan.normalize();
    Vec3 bitan = n.cross(tan); bitan.normalize();

    Vec3 prev = face[-1];
    float dprev = bitan.dot(prev-cutter._p1);
    for(int i=0;i<face.count();++i)
    {
        Vec3 curr = face[i];
        float dcurr = bitan.dot(curr-cutter._p1);

        if( (dprev==0.0 || dcurr==0.0) || signOf(dprev)!=signOf(dcurr) )
        {
            float d = std::abs(dprev) + std::abs(dcurr);
            float t = d>0.0 ? (std::abs(dprev) / d) : 0.0;
            toInsert[i] = mix(prev,curr,t);
        }

        prev = curr;
        dprev = dcurr;
    }

    // exception
    if( toInsert.size() < 2 )
    {
        // jump
        res.push_back(face);
        return res;
    }

    auto it = toInsert.begin();
    int k1 = it->first;
    it++;
    int k2 = it->first;

    Path geoA;
    for(int i=0;i<k1;++i) geoA.addVertex(face[i]);
    geoA.addVertex(toInsert[k1]); geoA.addVertex(toInsert[k2]);
    for(int i=k2;i<face.count();++i) geoA.addVertex(face[i]);

    Path geoB;
    for(int i=k1;i<k2;++i) geoB.addVertex(face[i]);
    geoB.addVertex(toInsert[k2]); geoB.addVertex(toInsert[k1]);

    res.push_back(geoA);
    res.push_back(geoB);

    return res;
}

//--------------------------------------------------------------
std::vector<Path> Polyhedron::cutFace(const Path& face, std::vector<Edge> cutters)
{
    PathSet res; res.push_back(face);

    // cut face into sub face using straight lines
    for(int i=0;i<(int)cutters.size();++i)
    {
        PathSet swapped;
        for(int j=0;j<(int)res.size();++j)
        {
            PathSet r = cutFace(res[j],cutters[i]);
            for(auto g : r)swapped.push_back(g);
        }
        res = swapped;
    }

    return res;
}

//--------------------------------------------------------------
void addFaceIf(const Path& face, const Polyhedron& testPh, bool inverse, Polyhedron& outPh)
{
    Vec3 mid = face[0] + face[1] + face[2]; mid *= (1.0/3.0);
    if(inverse == testPh.contains(mid)) outPh.addFace(face);
}

//--------------------------------------------------------------
void Polyhedron::cutPolyhedron(const Polyhedron& ph1, const Polyhedron& ph2, Polyhedron& phOut, bool inverse)
{
    processBar( 0.0 );

    for(int i=0;i<(int)ph1.faceCount();++i)
    {
        Path face = ph1[i];
        Vec3 n = face.normal();

        std::vector<Edge> eout;
        if( iFacePh(face, ph2, eout) )
        {
            PathSet subfaces = cutFace(face, eout);
            for(auto g : subfaces) addFaceIf(g, ph2, inverse, phOut);
        }
        else
        {
            addFaceIf(face, ph2, inverse, phOut);
        }
        processBar( (float)i/(float)ph1.faceCount() );
    }

    processBar( 1.0 );
    std::cout << std::endl;
}

//--------------------------------------------------------------
Polyhedron Polyhedron::operator-(const Polyhedron& ph) const
{
    Polyhedron r1,r2;
    cutPolyhedron(*this, ph, r1, false);
    cutPolyhedron(ph, *this, r2, true);

    for(auto f : r2.faces) r1.addFace(f);
    return r1;
}

//--------------------------------------------------------------
Polyhedron Polyhedron::operator+(const Polyhedron& ph) const
{
    Polyhedron r1,r2;
    cutPolyhedron(*this, ph, r1, false);
    cutPolyhedron(ph, *this, r2, false);

    for(auto f : r2.faces) r1.addFace(f);
    return r1;
}

//--------------------------------------------------------------
Polyhedron Polyhedron::operator*(const Polyhedron& ph) const
{
    Polyhedron r1,r2;
    cutPolyhedron(*this, ph, r1, true);
    cutPolyhedron(ph, *this, r2, true);

    for(auto f : r2.faces) r1.addFace(f);
    return r1;
}

IMPGEARS_END

