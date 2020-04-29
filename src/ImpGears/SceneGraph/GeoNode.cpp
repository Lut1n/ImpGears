#include <ImpGears/SceneGraph/GeoNode.h>


IMPGEARS_BEGIN

//--------------------------------------------------------------
GeoNode::GeoNode(const Polyhedron& buf, bool wireframe)
{
    _geo->setPrimitive(Geometry::Primitive_Triangles);
    buf.getTriangles(_geo->_vertices);
    _wireframe = wireframe;

    _material = Material::create();
}

//--------------------------------------------------------------
GeoNode::GeoNode(const Geometry::Ptr& geo, bool wireframe)
{
    _geo = geo;
    _wireframe = wireframe;

    _material = Material::create();
}

//--------------------------------------------------------------
void GeoNode::setGeometry(Geometry::Ptr& geo, bool wireframe)
{
    _geo = geo;
    _wireframe = wireframe;
}

//--------------------------------------------------------------
void GeoNode::setReflexion(ReflexionModel::Ptr reflexion)
{
    getState()->setReflexion(reflexion);
}

//--------------------------------------------------------------
void GeoNode::setRenderPass(RenderPass::Ptr renderPass)
{
    getState()->setRenderPass(renderPass);
}

//--------------------------------------------------------------
void GeoNode::setMaterial(const Material::Ptr material)
{
    _material = material;
}

//--------------------------------------------------------------
GeoNode::~GeoNode()
{
}

IMPGEARS_END
