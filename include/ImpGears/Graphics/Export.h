
#ifndef GRAPHICS_EXPORT_H
#define GRAPHICS_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_GRAPHICS_API
#define IG_GRAPHICS_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_graphics_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_GRAPHICS_API LIB_HELPER_DLL_EXPORT
#else
#define IG_GRAPHICS_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_GRAPHICS_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // GRAPHICS_EXPORT_H