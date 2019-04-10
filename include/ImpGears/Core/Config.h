#ifndef IMPCONFIG_H
#define IMPCONFIG_H

/// IMPGEARS_BUILD_DLL must be defined for dll building
/// IMPGEARS_STATIC must be defined for static link

/// use IMP_API for public element (struct, class, enum, union, etc...) of the API.
/// use IMP_LOCAL for private element.


/// IMPGEARS_DEBUG - Debug option for ImpGears
#if defined(IMPGEARS_DEBUG)
	#define IMP_DEBUG_ONLY( arg ) arg
#else
	#define IMP_DEBUG_ONLY( arg )
#endif

/// IMP_API/IMP_LOCAL - Visibility macros for ImpGears API (used with the gcc option -fvisibility)
#if defined(IMPGEARS_BUILD_DLL)
    #if defined(__GNUC__)
        #define IMP_API __attribute__((visibility("default")))
        #define IMP_LOCAL __attribute__((visibility("hidden")))
    #elif defined(_WIN32)
        #define IMP_API _declspec(dllexport)
        #define IMP_LOCAL
    #endif
#elif !defined(IMPGEARS_STATIC)
    #if defined(__GNUC__)
        #define IMP_API
        #define IMP_LOCAL
    #elif defined(_WIN32)
        #define IMP_API _declspec(dllimport)
        #define IMP_LOCAL
    #endif
#else
    #define IMP_API
    #define IMP_LOCAL
#endif

/// IMP_EXTERN_* for C code, plugin, etc...
#if defined(__cplusplus)
    #define IMP_EXTERN extern "C"
    #define IMP_EXTERN_BEGIN IMP_EXTERN {
    #define IMP_EXTERN_END };
#else
	#define IMP_EXTERN
    #define IMP_EXTERN_BEGIN
    #define IMP_EXTERN_END
#endif


// define namespace
#define IMPGEARS_NS imp
#define IMPGEARS_BEGIN namespace IMPGEARS_NS {
#define IMPGEARS_END };

#endif // IMPCONFIG_H
