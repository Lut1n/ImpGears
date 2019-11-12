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


// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
    #define LIB_HELPER_DLL_IMPORT __declspec(dllimport)
    #define LIB_HELPER_DLL_EXPORT __declspec(dllexport)
    #define LIB_HELPER_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define LIB_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
        #define LIB_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
        #define LIB_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
    #else
        #define LIB_HELPER_DLL_IMPORT
        #define LIB_HELPER_DLL_EXPORT
        #define LIB_HELPER_DLL_LOCAL
   #endif
#endif


// Use the generic helper definitions above to define IMP_API and IMP_LOCAL
#ifdef IMPGEARS_STATIC
    #define IMP_API
    #define IMP_LOCAL
#else // as DLL
    #ifdef IMPGEARS_BUILD_DLL
        #define IMP_API LIB_HELPER_DLL_EXPORT
    #else
        #define IMP_API LIB_HELPER_DLL_IMPORT
    #endif
    #define IMP_LOCAL LIB_HELPER_DLL_LOCAL
#endif


// IMP_EXTERN_* for C code, plugin, etc...
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


// library extension using for plugin loading
#if defined _WIN32 || defined __CYGWIN__
    #define LIB_EXT "dll"
#else
    #define LIB_EXT "so"
#endif


#endif // IMPCONFIG_H
