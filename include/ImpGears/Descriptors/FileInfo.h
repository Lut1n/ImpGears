#ifndef IMP_FILE_INFO_H
#define IMP_FILE_INFO_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Descriptors/Export.h>

IMPGEARS_BEGIN

//-------------------------------------------------------
std::string IG_DESC_API loadText(const std::string& filename);

//-------------------------------------------------------
bool IG_DESC_API fileExists(const std::string& filename);

//-------------------------------------------------------
void IG_DESC_API getTimeInfo(const std::string& filename, long& access, long& modif, long& status);

IMPGEARS_END

#endif // IMP_FILE_INFO_H
