#include <Descriptors/FileInfo.h>

// struct stat is defined in <sys/stat.h>
#include <sys/stat.h>
// time_t is defined in <sys/types.h>

//#include <time.h>

#include <fstream>

IMPGEARS_BEGIN

//-------------------------------------------------------
std::string loadText(const std::string& filename)
{
	std::ifstream ifs(filename);
	std::string buffer( (std::istreambuf_iterator<char>(ifs) ),
						(std::istreambuf_iterator<char>()    ) );
	return buffer;
}

//-------------------------------------------------------
bool fileExists(const std::string& filename)
{
	std::ifstream f(filename.c_str());
	return f.good();
}

//-------------------------------------------------------
void getTimeInfo(const std::string& filename, long& access, long& modif, long& status)
{
	struct stat t_stat;
	stat(filename.c_str(), &t_stat);

	access = (long)t_stat.st_atime; //time of last access
	modif = (long)t_stat.st_mtime; //time of last data modification
	status = (long)t_stat.st_ctime; //time of last status change
}

IMPGEARS_END
