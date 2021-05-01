#ifndef IMP_VISITOR_H
#define IMP_VISITOR_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/SceneGraph/Node.h>
#include <ImpGears/SceneGraph/Export.h>

#include <vector>

IMPGEARS_BEGIN

class IG_SCENE_API Visitor : public Object
{
public:

    Meta_Class(Visitor)

    Visitor();
    virtual ~Visitor();

    virtual bool apply( Node::Ptr ) = 0;

    virtual void push( Node::Ptr ) = 0;
    virtual void pop() = 0;
};

IMPGEARS_END

#endif // IMP_VISITOR_H
