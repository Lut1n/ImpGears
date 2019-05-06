#ifndef IMP_TARGET_H
#define IMP_TARGET_H

#include <Core/Object.h>
#include <Graphics/Image.h>

#include <SceneGraph/TextureSampler.h>
#include <SceneGraph/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API Target : public Object
{
public:
	
	Meta_Class(Target);
	
	Target();
	virtual ~Target();

	void create(int w, int h, int count = 1, bool hasDepth = false);
	void create(const std::vector<TextureSampler::Ptr>& textures, bool hasDepth = false);

	void destroy();

	bool hasDepth() const;

	TextureSampler::Ptr get(int n);
	
	int width() const;
	int height() const;
	int count() const;
	
	void change();
	void update();
	
	RenderPlugin::Data::Ptr _d;
	
protected:
	
	std::vector<TextureSampler::Ptr> _targets;
	bool _hasDepthBuffer;
	bool _hasChanged;
};

IMPGEARS_END

#endif // IMP_TARGET_H
