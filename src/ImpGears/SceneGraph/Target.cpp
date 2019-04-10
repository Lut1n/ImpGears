#include <Core/Object.h>
#include <SceneGraph/Target.h>

#include <SceneGraph/GraphRenderer.h>

IMPGEARS_BEGIN


//--------------------------------------------------------------
Target::Target()
{
	_d = NULL;
	_hasChanged = false;
	_hasDepthBuffer = false;
}

//--------------------------------------------------------------
Target::~Target()
{
	
}

//--------------------------------------------------------------
void Target::create(int w, int h, int count, bool hasDepth)
{
	_targets.resize(count);
	for(int i=0;i<count;++i) _targets[i] = Image::create(w,h,4);
	
	_hasDepthBuffer = hasDepth;
}

//--------------------------------------------------------------
void Target::destroy()
{
	_targets.clear();
	_hasDepthBuffer = false;
}

//--------------------------------------------------------------
Image::Ptr Target::get(int n)
{
	update();
	return _targets[n];
}

//--------------------------------------------------------------
int Target::width() const
{
	int w = 0;
	if(_targets.size() > 0) w = _targets[0]->width();
	return w;
}

//--------------------------------------------------------------
int Target::height() const
{
	int h = 0;
	if(_targets.size() > 0) h = _targets[0]->height();
	return h;
}

//--------------------------------------------------------------
int Target::count() const
{
	return _targets.size();
}

//--------------------------------------------------------------
bool Target::hasDepth() const
{
	return _hasDepthBuffer;
}

//--------------------------------------------------------------
void Target::change()
{
	_hasChanged = true;
}

//--------------------------------------------------------------
void Target::update()
{
	if(_hasChanged)
	{
		for(int i=0;i<(int)_targets.size();++i)
		{
			Image::Ptr& img = _targets[i];
			imp::GraphRenderer::s_interface->bringBack(img,_d,i);
		}
	}
	
	_hasChanged = false;
}


IMPGEARS_END
