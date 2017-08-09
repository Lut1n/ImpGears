#ifndef IMPBASE_HPP_INCLUDED
#define IMPBASE_HPP_INCLUDED

#include "impBuildConfig.h"

// Hack (avoid a compilation bug with mingw...)
#ifndef __NO_INLINE__
#define __NO_INLINE__
#endif

/// Impgears namespace
#define IMPGEARS_BEGIN namespace imp{
#define IMPGEARS_END };

IMPGEARS_BEGIN

/// NULL value
#define IMP_NULL 0

/// Fundamental types
#if defined(_MSV_S)
	/// MC Visual Studio
	typedef signed __int8 Int8;
	typedef unsigned __int8 Uint8;
	typedef signed __int16 Int16;
	typedef unsigned __int16 Uint16;
	typedef signed __int32 Int32;
	typedef unsigned __int32 Uint32;
	typedef signed __int64 Int64;
	typedef unsigned __int64 Uint64;
#else
	/// GCC/minGW
	typedef signed char Int8;
	typedef unsigned char Uint8;
	typedef signed short Int16;
	typedef unsigned short Uint16;
	typedef signed int Int32;
	typedef unsigned int Uint32;
	typedef signed long long Int64;
	typedef unsigned long long Uint64;
#endif // defined

IMPGEARS_END

#endif // IMPBASE_HPP_INCLUDED
