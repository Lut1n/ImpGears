#include <SceneGraph/QuadNode.h>
#include <Geometry/Geometry.h>

IMPGEARS_BEGIN

QuadNode::QuadNode()
{
	using FloatBuf = std::vector<float>;
	FloatBuf vertex, texCoords;

	// quad generation
	Vec3 p1 = -Vec3::X-Vec3::Y;
	Vec3 p2 = -Vec3::X+Vec3::Y;
	Vec3 p3 = Vec3::X-Vec3::Y;
	Vec3 p4 = Vec3::X+Vec3::Y;
	Geometry geo = Geometry::quad(p1,p2,p3,p4); Geometry::intoCCW( geo );

	// texture coords generation
	geo.fillBuffer(vertex);
	for(auto v : geo._vertices) {texCoords.push_back(v[0]); texCoords.push_back(v[1]);}

	// gpu load
	int vertSize = vertex.size()*sizeof(float);
	int texSize = texCoords.size()*sizeof(float);
	_gData.requestVBO(vertSize+texSize);
	_gData.setPrimitive(Primitive_Triangles);
	_gData.setVertices(vertex.data(), vertSize);
	_gData.setData(texCoords.data(), texSize, vertSize);
}

QuadNode::~QuadNode()
{
	_gData.releaseVBO();
}


void QuadNode::render()
{
	_gData.drawVBO();
}

IMPGEARS_END
