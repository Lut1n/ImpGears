
#ifndef SCENE_EXPORT_H
#define SCENE_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_SCENE_API
#define IG_SCENE_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_scenegraph_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_SCENE_API LIB_HELPER_DLL_EXPORT
#else
#define IG_SCENE_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_SCENE_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // SCENE_EXPORT_H