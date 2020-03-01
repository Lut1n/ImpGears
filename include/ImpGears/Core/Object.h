#ifndef IMP_OBJECT_H
#define IMP_OBJECT_H

#include <cmath>
#include <cstdint>

#include <memory>
#include <algorithm>

#include <sstream>
#include <iostream>
#include <string>

#include <Core/Config.h>

#define Meta_Class(name) \
	typedef std::shared_ptr<name> Ptr;\
    static name::Ptr dMorph(Object::Ptr obj){return std::dynamic_pointer_cast<name>(obj);}\
    static name::Ptr sMorph(Object::Ptr obj){return std::static_pointer_cast<name>(obj);}\
	static std::string getClassName() {return std::string( #name );}\
	virtual std::string getObjectID() const {std::stringstream ss; ss << getClassName() << "@" << (this); return ss.str();}\
	template<class... Args> static Ptr create(Args&&... args) {return std::make_shared<name>(std::forward<Args>(args)...);}

IMPGEARS_BEGIN
	
class IMP_API Object
{
	public:
	
	Meta_Class(Object)
	
	Object(){}
	virtual ~Object(){}
    
    Object::Ptr getRenderData(){return _renderData;}
    void setRenderData(Object::Ptr obj){_renderData=obj;}
    
    private:
    
    Object::Ptr _renderData;
};

IMPGEARS_END

#endif // IMP_OBJECT_H
