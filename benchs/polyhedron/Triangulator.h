#include <Geometry/Polygon.h>

IMPGEARS_BEGIN

class Triangulation : public Object
{
public:
	
	Meta_Class(Triangulation)
	
	static imp::Polygon triangulate(const imp::Polygon& poly);
	
	static std::vector<Vec3> triangulate(const std::vector<Vec3>& buf, Vec3 n,const std::vector<Edge> constrained = {});
	
	static std::string s_lastlog;
};

IMPGEARS_END
