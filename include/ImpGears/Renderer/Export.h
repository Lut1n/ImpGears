
#ifndef RENDERER_EXPORT_H
#define RENDERER_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_RENDERER_API
#define IG_RENDERER_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_renderer_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_RENDERER_API LIB_HELPER_DLL_EXPORT
#else
#define IG_RENDERER_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_RENDERER_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // RENDERER_EXPORT_H