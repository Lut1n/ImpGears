
#ifndef GEO_EXPORT_H
#define GEO_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_GEO_API
#define IG_GEO_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_geometry_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_GEO_API LIB_HELPER_DLL_EXPORT
#else
#define IG_GEO_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_GEO_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // GEO_EXPORT_H