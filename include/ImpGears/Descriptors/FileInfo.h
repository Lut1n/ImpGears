#ifndef IMP_FILE_INFO_H
#define IMP_FILE_INFO_H

#include <ImpGears/Core/Object.h>

IMPGEARS_BEGIN

//-------------------------------------------------------
std::string IMP_API loadText(const std::string& filename);

//-------------------------------------------------------
bool IMP_API fileExists(const std::string& filename);

//-------------------------------------------------------
void IMP_API getTimeInfo(const std::string& filename, long& access, long& modif, long& status);

IMPGEARS_END

#endif // IMP_FILE_INFO_H
