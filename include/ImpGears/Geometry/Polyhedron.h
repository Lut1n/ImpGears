#ifndef IMP_POLYHEDRON_H
#define IMP_POLYHEDRON_H

#include <Geometry/Geometry.h>
#include <Geometry/Triangulator.h>
#include <Geometry/Path.h>


IMPGEARS_BEGIN

class IMP_API Polyhedron : public Object
{
public:

    Meta_Class(Polyhedron)

    using PathSet = std::vector<Path>;
    PathSet faces;

    Polyhedron();
    virtual ~Polyhedron();

    void set(const Path::BufType& triangleBuf);
    void getTriangles(Path::BufType& triangleBuf) const;
    int faceCount() const;
    Path& operator[](int i);
    Path operator[](int i) const;
    void clear();
    void addFace(const Path& face);
    bool contains(const Vec3& p) const;
    Vec3 externalPoint() const;

    static bool iSegFace( const Edge& seg, const Path& face, Vec3& iPoint, bool strict = true);
    static bool iFacePh(const Path& face, const Polyhedron& ph, std::vector<Edge>& eout);
    static bool iFace2(const Path& f1, const Path& f2, Path::BufType& pts);
    static void cutPolyhedron(const Polyhedron& ph1, const Polyhedron& ph2, Polyhedron& phOut, bool inverse = false);
    static PathSet cutFace(const Path& face, std::vector<Edge> cutters);
    static PathSet cutFace(const Path& face, Edge cutter);

    Polyhedron operator-(const Polyhedron& ph) const;
    Polyhedron operator+(const Polyhedron& ph) const;
    Polyhedron operator*(const Polyhedron& ph) const;
};

IMPGEARS_END


#endif // IMP_POLYHEDRON_H

