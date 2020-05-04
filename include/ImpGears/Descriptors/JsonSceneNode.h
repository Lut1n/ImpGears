#ifndef IMP_JSON_SCENE_NODE_H
#define IMP_JSON_SCENE_NODE_H

#include <ImpGears/SceneGraph/GeoNode.h>
// #include <ImpGears/Core/Vec3.h>
// #include <ImpGears/Core/Matrix4.h>
// #include <ImpGears/SceneGraph/State.h>

IMPGEARS_BEGIN

class IMP_API JsonSceneNode : public Object
{
public:

    enum GeometricPrimitive
    {
        Undefined,
        Cube,
        Sphere,
        Cylinder,
        Capsule,
        Crystal,
        Torus
    };

    struct Item
    {
        Vec3 position;
        Vec3 rotation;
        Vec3 scale;
        Vec3 color;
        GeometricPrimitive primitive;

        Item()
            : position(0.0,0.0,0.0)
            , rotation(0.0,0.0,0.0)
            , scale(1.0,1.0,1.0)
            , color(1.0,1.0,1.0)
            , primitive(Undefined)
        {}
    };

    Meta_Class(JsonSceneNode);

    JsonSceneNode();
    virtual ~JsonSceneNode();

    void parseJsonData(const std::string& json);
    std::string writeIntoJson();

    Vec3& favoriteViewOri() {return _favoriteView;}
    int itemCount() {return _items.size();}
    Item& getItem(int index) {return _items[index];}
    void addItem(const Item& item) {_items.push_back(item);}
    void remItem(int index) {_items.erase(_items.begin()+index);}
    void clearItems() {_items.clear();}

    std::vector<Item>& items() {return _items;}

    static Node::Ptr buildSceneNode(const JsonSceneNode& jnode, ReflexionModel::Ptr refl, RenderPass::Ptr flags);
    static GeoNode::Ptr buildGeometryNode(const Item& item, ReflexionModel::Ptr refl, RenderPass::Ptr flags);

protected:
    Vec3 _favoriteView;
    std::vector<Item> _items;
};

IMPGEARS_END

#endif // IMP_JSON_SCENE_NODE_H
