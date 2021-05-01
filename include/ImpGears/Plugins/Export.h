
#ifndef PLUGIN_EXPORT_H
#define PLUGIN_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_PLUGIN_API
#define IG_PLUGIN_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_plugins_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_PLUGIN_API LIB_HELPER_DLL_EXPORT
#else
#define IG_PLUGIN_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_PLUGIN_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // PLUGIN_EXPORT_H