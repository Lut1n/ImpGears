#ifndef IMP_NODE_H
#define IMP_NODE_H

#include <Core/Object.h>
#include <Core/Vec3.h>
#include <Core/Matrix4.h>
#include <SceneGraph/Visitor.h>
#include <SceneGraph/State.h>

#include <list>

IMPGEARS_BEGIN

class IMP_API Node : public Object
{
public:

    Meta_Class(Node)

    using NodeList = std::list<Node::Ptr>;

    Node();
    virtual ~Node();

    virtual void update() {}
    virtual void render() {}

    void setPosition(const Vec3& position);
    void setRotation(const Vec3& rotation) ;
    void setScale(const Vec3& scale);

    const Vec3& getPosition() const {return _position;}
    const Vec3& getRotation() const {return _rotation;}
    const Vec3& getScale() const {return _scale;}

    void addNode(const Node::Ptr& sceneNode);
    void remNode(const Node::Ptr& sceneNode);

    void accept( Visitor::Ptr& visitor );

    void computeMatrices();

    const Matrix4& getModelMatrix() const { return _modelMatrix; }

    virtual State::Ptr getState() { return _state; }

protected:

    State::Ptr _state;
    Vec3 _position;
    Vec3 _rotation;
    Vec3 _scale;

    Matrix4 _positionMat, _rotationMat, _scaleMat;
    Matrix4 _modelMatrix;

    NodeList _children;
};

IMPGEARS_END

#endif // IMP_NODE_H
