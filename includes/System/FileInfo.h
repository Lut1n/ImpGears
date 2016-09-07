#ifndef IMP_FILE_INFO_H
#define IMP_FILE_INFO_H

#include <Core/impBase.h>

// struct stat is defined in <sys/stat.h>
#include <sys/stat.h>
// time_t is defined in <sys/types.h>

//#include <time.h>

IMPGEARS_BEGIN

void IMP_API getTimeInfo(char *filename, time_t& access, time_t& modif, time_t& status)
{
	if(filename)
	{
		struct stat t_stat;
		stat(filename, &t_stat);

		access = t_stat.st_atime;	//time of last access
		modif = t_stat.st_mtime;	//time of last data modification
		status = t_stat.st_ctime;	//time of last status change
	}
}

IMPGEARS_END

#endif // IMP_FILE_INFO_H
