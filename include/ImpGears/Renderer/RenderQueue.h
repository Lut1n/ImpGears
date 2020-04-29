#ifndef IMP_RENDER_QUEUE_H
#define IMP_RENDER_QUEUE_H

#include <ImpGears/SceneGraph/Node.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/LightNode.h>
#include <ImpGears/SceneGraph/State.h>

#include <vector>

IMPGEARS_BEGIN

struct IMP_API RenderState
{
    State::Ptr state;
    Node::Ptr node;
    Matrix4 proj;
    Matrix4 model;
    Matrix3 normal;
    
    static State::Ptr s_defaultState;
    
    RenderState();
    
    void reset();
};

struct IMP_API StateBin
{
    std::vector<RenderState> items;
    int position;
    
    StateBin();
    
    void push(const State::Ptr state, Node::Ptr node, const Matrix4& model);
    void sortByDistance(const Vec3& camPos);
    
    int size();
    void reset();
    
    State::Ptr stateAt(int index);
    Node::Ptr nodeAt(int index);
    
    RenderState at(int index);
};

class IMP_API RenderQueue : public Object
{
public:

    Meta_Class(RenderQueue)

    RenderQueue();
    virtual ~RenderQueue();

    const Camera* _camera;
    std::vector<LightNode*> _lights;
    StateBin _renderBin;
    StateBin _transparentRenderBin;
};

IMPGEARS_END

#endif // IMP_RENDER_QUEUE_H
