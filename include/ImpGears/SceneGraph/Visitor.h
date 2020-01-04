#ifndef IMP_VISITOR_H
#define IMP_VISITOR_H

#include <Core/Object.h>

#include <vector>

IMPGEARS_BEGIN

class Node;
class IMP_API Visitor : public Object
{
public:

    Meta_Class(Visitor)

    Visitor();
    virtual ~Visitor();

    virtual bool apply( Node* ) = 0;

    virtual void push( Node* ) = 0;
    virtual void pop() = 0;
};

IMPGEARS_END

#endif // IMP_VISITOR_H
