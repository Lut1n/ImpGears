
#ifndef DESC_EXPORT_H
#define DESC_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_DESC_API
#define IG_DESC_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_descriptors_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_DESC_API LIB_HELPER_DLL_EXPORT
#else
#define IG_DESC_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_DESC_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // DESC_EXPORT_H