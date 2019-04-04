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
	virtual void push( Ty node ) { _stack.push_back(node); };
	virtual void pop() { _stack.pop_back(); }

protected:
	
	std::vector<Ty> _stack;
};

IMPGEARS_END

#endif // IMP_VISITOR_H
