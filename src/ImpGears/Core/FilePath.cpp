#include <Core/FilePath.h>

IMPGEARS_BEGIN

FilePath::FilePath():
	String(),
	m_pathtype(PathType_Undefined),
	m_pathPos(0),
	m_pathSize(0),
	m_parentPos(0),
	m_parentSize(0),
	m_filenamePos(0),
	m_filenameSize(0),
	m_extensionPos(0),
	m_extensionSize(0)
{
}

FilePath::FilePath(const char* value):
	String(value)
{
	evaluatePath();
}

FilePath::FilePath(const String& other):
	String(other)
{
	evaluatePath();
}

FilePath::~FilePath()
{
}

void FilePath::evaluatePath()
{
    /*m_pathtype = PathType_Undefined;

    const char* value = getValue();

    m_pathPos = 0;
    m_pathSize = getSize();
    m_parentPos = 0;
    m_parentSize = findLast('/');
    m_filenamePos = m_parentSize+1;
    m_filenameSize = m_pathSize - m_filenamePos;
    m_extensionPos = findLast('.');
    m_extensionSize = m_pathSize - m_extensionPos;*/
}

String FilePath::getFilename()
{
    return substring(m_filenamePos, m_filenamePos+m_filenameSize);
}

String FilePath::getFullFilename()
{
    return substring(m_filenamePos, m_pathSize);
}

String FilePath::getPath()
{
    return getParentFolder();
}

String FilePath::getFullPath()
{
    return getValue();
}

String FilePath::getExtensionType()
{
    return substring(m_extensionPos, m_extensionPos+m_extensionSize);
}

FilePath::PathType FilePath::getPathType()
{
	return PathType_Undefined;
}

FilePath FilePath::getParentFolder()
{
    return substring(m_parentPos, m_parentSize);
}

void FilePath::setFilename(const char* filename)
{
    // need replace/insert
}

void FilePath::setExtensionType(const char* extension)
{
    //need replace/insert
}

void FilePath::setPath(const char* path)
{
    //need replace/insert
}

void FilePath::createChildFolder(const char* foldername)
{
    // need append
}

void FilePath::createChildFile(const char* filename)
{
    // need append
}

IMPGEARS_END
