#include <Plugins/RenderPlugin.h>

#include <dlfcn.h>

IMPGEARS_BEGIN

std::map<RenderPlugin::Ptr,void*> PluginManager::_handlers;

//--------------------------------------------------------------
RenderPlugin::Ptr PluginManager::open(const std::string& name)
{
	RenderPlugin::Ptr result;
	
	const std::string plugin_prefix("./Plugins/");
	const std::string plugin_postfix(".so");
	
	const std::string path = plugin_prefix + name + plugin_postfix;
	
	void* handler = dlopen(path.c_str(),RTLD_LAZY);
	if(handler == nullptr)
	{
		std::cout << "error dlopen : " << dlerror() << std::endl;
		return result;
	}
 
	LoadFunc func = (LoadFunc)dlsym(handler, "loadRenderPlugin");
	if (func == NULL)
	{
		std::cout << "error dlsym : " << dlerror() << std::endl;
		dlclose(handler);
		return result;
	}
 
	result = func();
	
	_handlers[result] = handler;
	return result;
}

//--------------------------------------------------------------
void PluginManager::close(RenderPlugin::Ptr& plugin)
{
	void* handler = _handlers[plugin];
	dlclose(handler);
	_handlers.erase(plugin);
}

IMPGEARS_END
