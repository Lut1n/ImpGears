#ifndef IMP_POLYGON_H
#define IMP_POLYGON_H

#include <Geometry/Path.h>

#include <Core/Object.h>
#include <Core/Math.h>
#include <Core/Vec3.h>

#include <vector>

IMPGEARS_BEGIN

using PathSet = std::vector<Path>;

class IMP_API Polygon : public Object
{
public:

    Meta_Class(Polygon)

    enum OpType
    {
        OpType_OR,
                OpType_AND,
                OpType_A_NOT_B,
                OpType_B_NOT_A
    };

    enum PathType
    {
        PathType_Unchange,
        PathType_Shape,
        PathType_Hole
    };

    PathSet _paths;

    Polygon();
    Polygon(const Path& shape);
    Polygon(const PathSet& shapes);
    Polygon(const PathSet& shapes, const PathSet& holes);

    virtual ~Polygon();

    void operator+=(const Vec3& vec);
    void operator-=(const Vec3& vec);
    void operator*=(const Vec3& vec);
    void operator/=(const Vec3& vec);
    void rotation(float rad);

    void addPath(const Path& path, PathType ptype = PathType_Unchange);
    void addPaths(const PathSet& set);

    static bool contains(const PathSet& set, const Vec3& v);

    static Polygon applyOp2(const Polygon& setA, const Polygon& setB, OpType op);

    Polygon operator-(const Polygon& other) const;
    Polygon operator+(const Polygon& other) const;
    Polygon operator*(const Polygon& other) const;

    bool inside(const Vec3& v) const;
    bool inside(const Path& path) const;
    bool inside(const Polygon& poly) const;

    // keep only the surrounding shape of the polygons
    // Polygon boundary() const;
    Polygon simplifyAll() const;
    // Polygon rmSelfIntersects() const;

    // bool isRegular() const;
    bool isSimple() const;
    bool isConvex() const;
};

IMPGEARS_END


#endif // IMP_POLYGON_H

