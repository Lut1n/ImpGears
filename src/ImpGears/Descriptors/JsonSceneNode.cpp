#include <ImpGears/Descriptors/JsonSceneNode.h>
#include <ImpGears/Descriptors/JsonReaderWriter.h>
#include <ImpGears/Geometry/GeometryBuilder.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
#define CASE_TO_STRING(case_name,in_case,out_string) if(in_case==case_name)out_string=#case_name;
#define STRING_TO_CASE(case_name,in_string,out_case) if(in_string==#case_name)out_case=case_name;



//--------------------------------------------------------------
std::ostream& operator<<(std::ostream& ss, const Vec3& v)
{
    ss << "[" << v[0]  << "," << v[1]  << "," << v[2] << "]";
    return ss;
}

//--------------------------------------------------------------
std::string tab(int n)
{
    return std::string(n,'\t');
}

//--------------------------------------------------------------
void wrap(std::string& s, char c)
{
    s.insert(0,1,c); s.push_back(c);
}

//--------------------------------------------------------------
Vec3 parseVec3(JsonArray* json)
{
    Vec3 v;
    v[0] = json->getNumeric(0)->value;
    v[1] = json->getNumeric(1)->value;
    v[2] = json->getNumeric(2)->value;
    return v;
}


//--------------------------------------------------------------
JsonSceneNode::JsonSceneNode()
    : _favoriteView(0.25,0.25,10)
{
}

//--------------------------------------------------------------
JsonSceneNode::~JsonSceneNode()
{
}

//--------------------------------------------------------------
void JsonSceneNode::parseJsonData(const std::string& json)
{
    std::string jsonWithoutSpace = removeSpace(json);

    JsonObject* root = dynamic_cast<JsonObject*>( parse(jsonWithoutSpace) );
    JsonObject* editorview = root->getObject("editor_preference");
    _favoriteView = parseVec3(editorview->getArray("favorite_view"));

    JsonArray* array = root->getArray("nodes");

    int s = array->value.size();
    // _items.resize(s);

    for(int i=0;i<s;++i)
    {
        Item item;
        JsonObject* statejson = array->getObject(i);

        std::string p = statejson->getString("primitive")->value;
        STRING_TO_CASE(Cube,p,item.primitive);
        STRING_TO_CASE(Sphere,p,item.primitive);
        STRING_TO_CASE(Cylinder,p,item.primitive);
        STRING_TO_CASE(Capsule,p,item.primitive);
        STRING_TO_CASE(Crystal,p,item.primitive);
        STRING_TO_CASE(Torus,p,item.primitive);

        item.position = parseVec3(statejson->getArray("position"));
        item.rotation = parseVec3(statejson->getArray("rotation"));
        item.scale = parseVec3(statejson->getArray("scale"));
        item.color = parseVec3(statejson->getArray("color"));

        _items.push_back(item);
    }
}

//--------------------------------------------------------------
std::string JsonSceneNode::writeIntoJson()
{
    std::stringstream ss;
    ss << "{" << std::endl;

    ss << tab(1) << "\"editor_preference\":"<< std::endl;
    ss << tab(1) << "{" << std::endl;
    ss << tab(2) << "\"favorite_view\":" << _favoriteView << std::endl;
    ss << tab(1) << "}," << std::endl;

    ss  << tab(1)<< "\"nodes\":" << std::endl;
    ss  << tab(1) << "["  << std::endl;
    for(auto item : _items)
    {
        ss  << tab(2) << "{" ;
        std::string p;
        CASE_TO_STRING(Cube,item.primitive,p);
        CASE_TO_STRING(Sphere,item.primitive,p);
        CASE_TO_STRING(Cylinder,item.primitive,p);
        CASE_TO_STRING(Capsule,item.primitive,p);
        CASE_TO_STRING(Crystal,item.primitive,p);
        CASE_TO_STRING(Torus,item.primitive,p);
        wrap(p,'"');

        // = m.primitive==Cube ? "\"cube\"" : "\"sphere\"";
        ss << "\"primitive\":" << p << ",";
        ss << "\"position\":" << item.position << ",";
        ss << "\"rotation\":" << item.rotation << ",";
        ss << "\"scale\":" << item.scale << ",";
        ss << "\"color\":" << item.color << ",";
        ss << "},";
        ss << std::endl;
    }
    ss << tab(1) << "]" << std::endl;
    ss << "}";
    ss << std::endl;

    std::string res = ss.str();
    return res;
}

//--------------------------------------------------------------
GeoNode::Ptr JsonSceneNode::buildGeometryNode( const JsonSceneNode::Item& item, ReflexionModel::Ptr refl, RenderPass::Ptr flags )
{
    Geometry::Ptr newGeo;

    if(item.primitive == JsonSceneNode::Sphere)
    {
        newGeo = GeometryBuilder::createSphere(16.0,1.0);
    }
    else if(item.primitive == JsonSceneNode::Cube)
    {
        newGeo = GeometryBuilder::createCube(1.0);
    }
    else if(item.primitive == JsonSceneNode::Torus)
    {
        newGeo = GeometryBuilder::createTorus();
    }
    else if(item.primitive == JsonSceneNode::Cylinder)
    {
        newGeo = GeometryBuilder::createCylinder();
    }
    else if(item.primitive == JsonSceneNode::Capsule)
    {
        newGeo = GeometryBuilder::createCapsule();
    }
    else if(item.primitive == JsonSceneNode::Crystal)
    {
        newGeo = GeometryBuilder::createCrystal(5);
    }

    GeoNode::Ptr newNode = GeoNode::create(newGeo, false);
    if(refl) newNode->setReflexion(refl);
    if(flags) newNode->getState()->setRenderPass(flags);
    Material::Ptr newmat = Material::create(item.color, 64.0);
    newNode->setMaterial(newmat);

    newNode->setPosition(item.position);
    newNode->setRotation(item.rotation);
    newNode->setScale(item.scale);

    return newNode;
}

//--------------------------------------------------------------
Node::Ptr JsonSceneNode::buildSceneNode(const JsonSceneNode& jnode, ReflexionModel::Ptr refl, RenderPass::Ptr flags)
{
    Node::Ptr node = Node::create();
    for(auto& item : jnode._items)
    {
        Node::Ptr geoNode = buildGeometryNode(item, refl, flags);
        node->addNode(geoNode);
    }
    return node;
}

IMPGEARS_END
