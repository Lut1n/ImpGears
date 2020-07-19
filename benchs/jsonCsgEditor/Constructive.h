
#include <ImpGears/Geometry/Polyhedron.h>

static const bool DEBUG_LINES = true;



void getLines(Polyhedron poly, std::vector<Vec3>& vertices, std::vector<Vec3>& colors)
{
    vertices.clear();
    for(int f=0;f<poly.faceCount();++f)
    {
        std::vector<Vec3> verts = poly[f].vertices;

        Vec3 last = verts.back();
        for(auto p : verts)
        {
            vertices.push_back(last);
            vertices.push_back(p);
            colors.push_back(Vec3(0.0,0.0,1.0));
            colors.push_back(Vec3(0.0,0.0,1.0));
            last = p;
        }
    }
}

Geometry transformGeo(const Geometry& geo, const Vec3& p, const Vec3& r, const Vec3& s)
{
    Geometry copy = geo;
    copy *= s;
    copy.rotX(r[0]); copy.rotY(r[1]); copy.rotZ(r[2]);
    copy += p;
    return copy;
}


GeoNode::Ptr mergeGeodes(GeoNode::Ptr& geode1, GeoNode::Ptr& geode2)
{
    Geometry::Ptr geomerged = Geometry::create();
    Polyhedron merged;
    {
        // get geometry copies
        Geometry geo1 = transformGeo(*(geode1->_geo), geode1->getPosition(), geode1->getRotation(), geode1->getScale());
        Geometry geo2 = transformGeo(*(geode2->_geo), geode2->getPosition(), geode2->getRotation(), geode2->getScale());

        Polyhedron poly1, poly2;
        poly1.set(geo1._vertices);
        poly2.set(geo2._vertices);
        merged = poly1 + poly2;
        // merged = override_plus(poly1,poly2);
    }

    geomerged->setPrimitive(Geometry::Primitive_Triangles);
    merged.getTriangles(geomerged->_vertices);
    // getTriangles(merged, geomerged->_vertices);

    geomerged->generateColors( Vec3(1.0) );
    geomerged->generateNormals( Geometry::NormalGenMode_PerFace );
    geomerged->generateTexCoords( Geometry::TexGenMode_Cubic, 1.0);

    GeoNode::Ptr result = GeoNode::create(geomerged);
    result->setReflexion( geode1->getState()->getReflexion() );
    result->setRenderPass( geode1->getState()->getRenderPass() );
    // result->setMaterial( geo1->_material );

    if(DEBUG_LINES)
    {
        Geometry::Ptr geoline = Geometry::create();
        geoline->setPrimitive(Geometry::Primitive_Lines);
        getLines(merged, geoline->_vertices, geoline->_colors);
        geoline->_hasColors = true;
        GeoNode::Ptr linenode = GeoNode::create(geoline);
        linenode->setReflexion( geode1->getState()->getReflexion() );
        linenode->setRenderPass( geode1->getState()->getRenderPass() );
        result->addNode(linenode);
    }


    return result;
}


GeoNode::Ptr diffGeodes(GeoNode::Ptr& geode1, GeoNode::Ptr& geode2)
{
    Geometry::Ptr geomerged = Geometry::create();
    Polyhedron merged;
    {
        // get geometry copies
        Geometry geo1 = transformGeo(*(geode1->_geo), geode1->getPosition(), geode1->getRotation(), geode1->getScale());
        Geometry geo2 = transformGeo(*(geode2->_geo), geode2->getPosition(), geode2->getRotation(), geode2->getScale());

        Polyhedron poly1, poly2;
        poly1.set(geo1._vertices);
        poly2.set(geo2._vertices);
        merged = poly1 - poly2;
        // merged = override_plus(poly1,poly2);
    }

    geomerged->setPrimitive(Geometry::Primitive_Triangles);
    merged.getTriangles(geomerged->_vertices);
    // getTriangles(merged, geomerged->_vertices);

    geomerged->generateColors( Vec3(1.0) );
    geomerged->generateNormals( Geometry::NormalGenMode_PerFace );
    geomerged->generateTexCoords( Geometry::TexGenMode_Cubic, 1.0);

    GeoNode::Ptr result = GeoNode::create(geomerged);
    result->setReflexion( geode1->getState()->getReflexion() );
    result->setRenderPass( geode1->getState()->getRenderPass() );
    // result->setMaterial( geode1->_material );

    if(DEBUG_LINES)
    {
        Geometry::Ptr geoline = Geometry::create();
        geoline->setPrimitive(Geometry::Primitive_Lines);
        getLines(merged, geoline->_vertices, geoline->_colors);
        geoline->_hasColors = true;
        GeoNode::Ptr linenode = GeoNode::create(geoline);
        linenode->setReflexion( geode1->getState()->getReflexion() );
        linenode->setRenderPass( geode1->getState()->getRenderPass() );
        result->addNode(linenode);
    }


    return result;
}


GeoNode::Ptr interGeodes(GeoNode::Ptr& geode1, GeoNode::Ptr& geode2)
{
    Geometry::Ptr geomerged = Geometry::create();
    Polyhedron merged;
    {
        // get geometry copies
        Geometry geo1 = transformGeo(*(geode1->_geo), geode1->getPosition(), geode1->getRotation(), geode1->getScale());
        Geometry geo2 = transformGeo(*(geode2->_geo), geode2->getPosition(), geode2->getRotation(), geode2->getScale());

        Polyhedron poly1, poly2;
        poly1.set(geo1._vertices);
        poly2.set(geo2._vertices);
        merged = poly1 * poly2;
        // merged = override_plus(poly1,poly2);
    }

    geomerged->setPrimitive(Geometry::Primitive_Triangles);
    merged.getTriangles(geomerged->_vertices);
    // getTriangles(merged, geomerged->_vertices);

    geomerged->generateColors( Vec3(1.0) );
    geomerged->generateNormals( Geometry::NormalGenMode_PerFace );
    geomerged->generateTexCoords( Geometry::TexGenMode_Cubic, 1.0);

    GeoNode::Ptr result = GeoNode::create(geomerged);
    result->setReflexion( geode1->getState()->getReflexion() );
    result->setRenderPass( geode1->getState()->getRenderPass() );
    // result->setMaterial( geode1->_material );

    if(DEBUG_LINES)
    {
        Geometry::Ptr geoline = Geometry::create();
        geoline->setPrimitive(Geometry::Primitive_Lines);
        getLines(merged, geoline->_vertices, geoline->_colors);
        geoline->_hasColors = true;
        GeoNode::Ptr linenode = GeoNode::create(geoline);
        linenode->setReflexion( geode1->getState()->getReflexion() );
        linenode->setRenderPass( geode1->getState()->getRenderPass() );
        result->addNode(linenode);
    }


    return result;
}



GeoNode::Ptr merge2first(std::vector<GeoNode::Ptr>& nodes)
{
  if(nodes.size() <= 1) return nullptr;
  return mergeGeodes(nodes[0],nodes[1]);
}



GeoNode::Ptr diff2first(std::vector<GeoNode::Ptr>& nodes)
{
  if(nodes.size() <= 1) return nullptr;
  return diffGeodes(nodes[0],nodes[1]);
}


GeoNode::Ptr inter2first(std::vector<GeoNode::Ptr>& nodes)
{
  if(nodes.size() <= 1) return nullptr;
  return interGeodes(nodes[0],nodes[1]);
}
