
#ifndef CORE_EXPORT_H
#define CORE_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_CORE_API
#define IG_CORE_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_core_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_CORE_API LIB_HELPER_DLL_EXPORT
#else
#define IG_CORE_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_CORE_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // CORE_EXPORT_H