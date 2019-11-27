#include <SceneGraph/GeoNode.h>


IMPGEARS_BEGIN

//--------------------------------------------------------------
GeoNode::GeoNode(const Polyhedron& buf, bool wireframe)
{
    _loaded = false;
    _gBuffer = -1;
    _geo->setPrimitive(Geometry::Primitive_Triangles);
    buf.getTriangles(_geo->_vertices);
    _wireframe = wireframe;

    ReflexionModel::Ptr reflexion = ReflexionModel::create();
    getState()->setReflexion(reflexion);

    _material = Material::create();
}

//--------------------------------------------------------------
GeoNode::GeoNode(const Geometry::Ptr& geo, bool wireframe)
{
    _loaded = false;
    _gBuffer = -1;
    _geo = geo;
    _wireframe = wireframe;

    ReflexionModel::Ptr reflexion = ReflexionModel::create();
    getState()->setReflexion(reflexion);

    _material = Material::create();
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

//--------------------------------------------------------------
void GeoNode::update()
{ 
    if(!_loaded)
    {
        if(_geo->getPrimitive()==Geometry::Primitive_Triangles)
        {
            if(_wireframe)
            {
                *_geo = Geometry::intoLineBuf( *_geo );
                _geo->setPrimitive(Geometry::Primitive_Lines);
            }
            else
            {
                Geometry::intoCCW(*_geo);
            }
        }
        _loaded = true;
    }
}

IMPGEARS_END
