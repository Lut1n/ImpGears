#ifndef IMP_FILEPATH_H
#define IMP_FILEPATH_H

#include <Core/impBase.h>
#include <Core/String.h>

IMPGEARS_BEGIN

class IMP_API FilePath : public String
{
	public:

		enum PathType
		{
			PathType_Undefined = 0,
			PathType_File,
			PathType_Folder
		};

		FilePath();
		FilePath(const char* value);
		FilePath(const String& other);
		virtual ~FilePath();

		String getFilename();
		String getFullFilename();
		String getPath();
		String getFullPath();
		String getExtensionType();

		PathType getPathType();
		FilePath getParentFolder();

		void setFilename(const char* filename);
		void setExtensionType(const char* extension);
		void setPath(const char* path);
		void createChildFolder(const char* foldername);
		void createChildFile(const char* filename);

		void evaluatePath();

	protected:

	private:

		PathType m_pathtype;

		Uint32 m_pathPos;
		Uint32 m_pathSize;
		Uint32 m_parentPos;
		Uint32 m_parentSize;
		Uint32 m_filenamePos;
		Uint32 m_filenameSize;
		Uint32 m_extensionPos;
		Uint32 m_extensionSize;
};

IMPGEARS_END

#endif // IMP_FILEPATH_H
