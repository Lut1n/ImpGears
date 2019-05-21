#ifndef IMP_CLEARNODE_H
#define IMP_CLEARNODE_H

#include <SceneGraph/Node.h>

IMPGEARS_BEGIN

class IMP_API ClearNode : public Node
{
public:
	
	Meta_Class(ClearNode)
	
	ClearNode();
	virtual ~ClearNode();

	virtual void render();
	virtual void update();
	
	void setColor(const Vec4& color) { _color=color; }
	const Vec4& getColor() const { return _color; }
	
	void setDepth(float depth) { _depth=depth; }
	float getDepth() const { return _depth; }
	
	void enableColor(bool enable) { _clearColor=enable; }
	bool isColorEnable() const { return _clearColor; }
	void enableDepth(bool enable) { _clearDepth=enable; }
	bool isDepthEnable() const { return _clearDepth; }
	
	void setTarget(const Target::Ptr& targets);
	void setDefaultTarget();
	Target::Ptr getTarget() const;
	

private:

	Vec4 _color;
	float _depth;
	bool _clearColor;
	bool _clearDepth;
};

IMPGEARS_END

#endif // IMP_CLEARNODE_H
