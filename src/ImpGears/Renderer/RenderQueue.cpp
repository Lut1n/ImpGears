#include <ImpGears/Renderer/RenderQueue.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
State::Ptr RenderState::s_defaultState = nullptr;


    
//--------------------------------------------------------------
RenderState::RenderState()
{
    state = State::create();
    node = nullptr;
    if(s_defaultState == nullptr) s_defaultState = State::create();  
}

//--------------------------------------------------------------
void RenderState::reset()
{
    state->clone(s_defaultState,State::CloneOpt_OverrideRef);
    node = nullptr;
}

    
//--------------------------------------------------------------
StateBin::StateBin()
    : position(-1)
{
}

//--------------------------------------------------------------
void StateBin::push(const State::Ptr state, Node::Ptr node, const Matrix4& model)
{
    position++;
    if( size()>(int)items.size() ) items.resize( size() );
    
    items[position].state->clone(state, State::CloneOpt_OverrideRef);
    items[position].node = node;
    items[position].model = model;
}

//--------------------------------------------------------------
void StateBin::sortByDistance(const Vec3& camPos)
{
    std::sort(items.begin(),items.end(), [camPos](const RenderState& a, const RenderState& b) {
        Vec3 mpos1(a.model.at(3,0),a.model.at(3,1),a.model.at(3,2));
        Vec3 mpos2(b.model.at(3,0),b.model.at(3,1),b.model.at(3,2));
        return (mpos1-camPos).length2() < (mpos2-camPos).length2();
    });
}

//--------------------------------------------------------------
int StateBin::size()
{
    return position+1;
}

//--------------------------------------------------------------
void StateBin::reset()
{
    position = -1;
}

//--------------------------------------------------------------
State::Ptr StateBin::stateAt(int index)
{
    if(index>=0 && index<=position) return items[index].state;
    return nullptr;
}

//--------------------------------------------------------------
Node::Ptr StateBin::nodeAt(int index)
{
    if(index>=0 && index<=position) return items[index].node;
    return nullptr;
}

//--------------------------------------------------------------
RenderState StateBin::at(int index)
{
    if(index>=0 && index<=position) return items[index];
    return RenderState();
}


//--------------------------------------------------------------
RenderQueue::RenderQueue()
    : _camera(nullptr)
{
}

//--------------------------------------------------------------
RenderQueue::~RenderQueue()
{
}

IMPGEARS_END
