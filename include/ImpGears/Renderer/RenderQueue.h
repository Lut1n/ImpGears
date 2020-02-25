#ifndef IMP_RENDER_QUEUE_H
#define IMP_RENDER_QUEUE_H

#include <SceneGraph/Node.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/LightNode.h>
#include <SceneGraph/State.h>

#include <vector>

IMPGEARS_BEGIN

struct IMP_API RenderState
{
    State::Ptr state;
    Node* node;
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
    
    void push(const State::Ptr state, Node* node, const Matrix4& model);
    
    int size();
    void reset();
    
    State::Ptr stateAt(int index);
    Node* nodeAt(int index);
    
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
};

IMPGEARS_END

#endif // IMP_RENDER_QUEUE_H
