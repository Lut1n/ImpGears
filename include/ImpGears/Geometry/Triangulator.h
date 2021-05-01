#include <ImpGears/Geometry/Polygon.h>
#include <ImpGears/Geometry/Export.h>

IMPGEARS_BEGIN

class IG_GEO_API Triangulation : public Object
{
public:

    Meta_Class(Triangulation)

    static imp::Polygon triangulate(const imp::Polygon& poly);

    static Path::BufType triangulate(const Path& shape);
};

IMPGEARS_END
