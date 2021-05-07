#include <ImpGears/Plugins/RenderPlugin.h>

#if defined _WIN32 || defined __CYGWIN__
#include <windows.h>
#else
#include <dlfcn.h>
#endif

IMPGEARS_BEGIN

std::map<RenderPlugin::Ptr,void*> PluginManager::_handlers;

//--------------------------------------------------------------
RenderPlugin::Ptr PluginManager::open(const std::string& name)
{
    RenderPlugin::Ptr result;

    void* handler = nullptr;

#if defined _WIN32 || defined __CYGWIN__
    handler = LoadLibrary(name.c_str());
    if (handler == nullptr)
    {
        auto errCode = GetLastError();
        const char* errStr = "Unknown error";
        if (errCode == ERROR_MOD_NOT_FOUND) errStr = "ERROR_MOD_NOT_FOUND";
        if (errCode == ERROR_BAD_EXE_FORMAT) errStr = "ERROR_BAD_EXE_FORMAT";
        std::cout << "error LoadLibrary : " << errStr << std::endl;
        return result;
    }

    LoadFunc func = (LoadFunc) GetProcAddress((HMODULE)handler, "loadRenderPlugin");
    if (func == NULL)
    {
        std::cout << "error GetProcAddress : Unknown" << std::endl;
        FreeLibrary((HMODULE) handler);
        return result;
    }

    result = func();
#else
    // void* handler = dlopen(name.c_str(),RTLD_LAZY);
    handler = dlopen(name.c_str(),RTLD_NOW);
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
#endif

    _handlers[result] = handler;
    return result;
}

//--------------------------------------------------------------
void PluginManager::close(RenderPlugin::Ptr& plugin)
{
    void* handler = _handlers[plugin];
#if defined _WIN32 || defined __CYGWIN__
    FreeLibrary((HMODULE) handler);
#else
    dlclose(handler);
#endif
    _handlers.erase(plugin);
}

IMPGEARS_END
