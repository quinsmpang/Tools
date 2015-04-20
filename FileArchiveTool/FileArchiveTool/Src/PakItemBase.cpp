#include "PakItemBase.h"
#include "FileArchiveToolSys.h"
#include "Util.h"
#include "FileInfo.h"
#include "Version.h"
#include "SystemEndian.h"
#include "MByteBuffer.h"
#include "BufferDefaultValue.h"
#include "ArchiveTask.h"
#include "TaskQueue.h"
#include "ArchiveParam.h"
#include "UnArchiveParam.h"
#include "UnArchiveTask.h"
#include "Config.h"
#include "ArchiveHeader.h"
#include "PakPathSplitInfo.h"
#include "PakTask.h"

#include <sstream>

BEGIN_NAMESPACE_FILEARCHIVETOOL

PakItemBase::PakItemBase()
{

}

PakItemBase::~PakItemBase()
{

}

void PakItemBase::ArchiveDir()
{
	PakTask* pPakTask = new PakTask(this);
	FileArchiveToolSysDef->getTaskQueuePtr()->addTask(pPakTask);
}

void PakItemBase::asyncArchiveDir(ArchiveParam* pArchiveParam)
{
	
}

void PakItemBase::unArchiveFile()
{
	UnArchiveTask* pUnArchiveTask = new UnArchiveTask(FileArchiveToolSysDef->getUnArchiveParamPtr());
	FileArchiveToolSysDef->getTaskQueuePtr()->addTask(pUnArchiveTask);
}

void PakItemBase::asyncUnArchiveFile(UnArchiveParam* pUnArchiveParam)
{
	
}

void PakItemBase::adjustHeaderOffset()
{

}

void PakItemBase::calcHeaderSize(uint32& headerSize)
{

}

bool PakItemBase::canAddFile(PakPathSplitInfo* pPakPathSplitInfo)
{
	return false;
}

void PakItemBase::addFileHeader(FileHeader* pFileHeader)
{
	
}

void PakItemBase::initByPakPathSplitInfo(PakPathSplitInfo* pPakPathSplitInfo, uint32 packIdx)
{
	
}

void PakItemBase::readArchiveFileHeader(const char* pFileName)
{

}

bool PakItemBase::isPakNameEqual(std::string& pakName)
{
	return false;
}

END_NAMESPACE_FILEARCHIVETOOL