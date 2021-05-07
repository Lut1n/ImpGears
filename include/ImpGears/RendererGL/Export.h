
#ifndef BACKEND_GL_EXPORT_H
#define BACKEND_GL_EXPORT_H

#include <ImpGears/Core/Config.h>

#ifdef IMPGEARS_STATIC
#define IG_BKND_GL_API
#define IG_BKND_GL_LOCAL
#else // as DLL
#if defined IMPGEARS_BUILD_DLL && defined impgears_renderer_gl_EXPORTS // Cmake default DEFINE_SYMBOL when compiling sources in a shared library
#define IG_BKND_GL_API LIB_HELPER_DLL_EXPORT
#else
#define IG_BKND_GL_API LIB_HELPER_DLL_IMPORT
#endif
#define IG_BKND_GL_LOCAL LIB_HELPER_DLL_LOCAL
#endif

#endif // BACKEND_GL_EXPORT_H