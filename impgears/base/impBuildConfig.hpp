#ifndef IMPBUILDCONFIG_HPP_INCLUDED
#define IMPBUILDCONFIG_HPP_INCLUDED

#if defined(IMPGEARS_DEBUG)
	#define IMP_DEBUG_ONLY( arg ) arg
#else
	#define IMP_DEBUG_ONLY( arg )
#endif // defined


#endif // IMPBUILDCONFIG_HPP_INCLUDED
