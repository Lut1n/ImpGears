#ifndef IMP_RENDER_QUEUE_H
#define IMP_RENDER_QUEUE_H

#include <SceneGraph/Node.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/LightNode.h>
#include <SceneGraph/State.h>

#include <vector>

IMPGEARS_BEGIN

class IMP_API RenderQueue : public Object
{
public:

	Meta_Class(RenderQueue)

	RenderQueue();
	virtual ~RenderQueue();
	
	const Camera* _camera;
	std::vector<LightNode*> _lights;
	
	std::vector<State::Ptr> _states;
	std::vector<Node*> _nodes;
	// std::vector<Matrix4> _modelMats;
};

IMPGEARS_END

#endif // IMP_RENDER_QUEUE_H
