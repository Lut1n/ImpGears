#ifndef IMPBUILDCONFIG_HPP_INCLUDED
#define IMPBUILDCONFIG_HPP_INCLUDED

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

/// IMP_API_C_CODE_BEGIN/END - Block for C code
#if defined(__cplusplus)
    #define IMP_API_C_CODE_BEGIN extern "C" {
    #define IMP_API_C_CODE_END }
#else
    #define IMP_API_C_CODE_BEGIN
    #define IMP_API_C_CODE_END
#endif

#endif // IMPBUILDCONFIG_HPP_INCLUDED
