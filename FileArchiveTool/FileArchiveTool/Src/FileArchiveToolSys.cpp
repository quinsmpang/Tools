#include "FileArchiveToolSys.h"
#include "Config.h"
#include "ArchiveData.h"
#include "CharsetConv.h"
#include "Util.h"
#include "TaskQueue.h"
#include "LogSys.h"

BEGIN_NAMESPACE_FILEARCHIVETOOL

template <> FileArchiveToolSys* Singleton<FileArchiveToolSys>::m_sSingleton = 0;

FileArchiveToolSys::FileArchiveToolSys()
{
	m_pConfig = new Config();
	m_pArchiveData = new ArchiveData();
	m_pCharsetConv = new CharsetConv();
	m_pUtil = new Util();
	m_pTaskQueue = new TaskQueue();
	m_pLogSys = new LogSys();
}

FileArchiveToolSys::~FileArchiveToolSys()
{
	delete m_pConfig;
	delete m_pArchiveData;
}

Config* FileArchiveToolSys::getConfigPtr()
{
	return m_pConfig;
}

ArchiveData* FileArchiveToolSys::getArchiveDataPtr()
{
	return m_pArchiveData;
}

CharsetConv* FileArchiveToolSys::getCharsetConvPtr()
{
	return m_pCharsetConv;
}

Util* FileArchiveToolSys::getUtilPtr()
{
	return m_pUtil;
}

TaskQueue* FileArchiveToolSys::getTaskQueuePtr()
{
	return m_pTaskQueue;
}

LogSys* FileArchiveToolSys::getLogSysPtr()
{
	return m_pLogSys;
}

void FileArchiveToolSys::onTick()
{
	m_pLogSys->onTick();
}

END_NAMESPACE_FILEARCHIVETOOL