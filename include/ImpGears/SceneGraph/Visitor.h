#ifndef IMP_VISITOR_H
#define IMP_VISITOR_H

#include <Core/Object.h>

#include <vector>

IMPGEARS_BEGIN

template<typename Ty>
class IMP_API Visitor : public Object
{
public:

	Meta_Class(Visitor)

	Visitor() {}
	virtual ~Visitor() {}

	virtual void apply( Ty node ) = 0;
	
	virtual void push( Ty node ) = 0;
	virtual void pop() = 0;
};

IMPGEARS_END

#endif // IMP_VISITOR_H
