#include "PakItem.h"
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

BEGIN_NAMESPACE_FILEARCHIVETOOL

PakItem::PakItem()
	:m_fileSize(0)
{
	m_pArchiveHeader = new ArchiveHeader;
	m_pFileVec = new FileHeaderVec();
	m_pakName = new std::string;
	m_fullPath = new std::string;
}

PakItem::~PakItem()
{
	delete m_pArchiveHeader;
	clearFileVec();
	delete m_pFileVec;
	delete m_pakName;
	delete m_fullPath;
}

void PakItem::ArchiveDir()
{
	PakTask* pPakTask = new PakTask(this);
	FileArchiveToolSysDef->getTaskQueuePtr()->addTask(pPakTask);
}

void PakItem::asyncArchiveDir(ArchiveParam* pArchiveParam)
{
	writeFile2ArchiveFile(pArchiveParam);
}

void PakItem::unArchiveFile()
{
	UnArchiveTask* pUnArchiveTask = new UnArchiveTask(FileArchiveToolSysDef->getUnArchiveParamPtr());
	FileArchiveToolSysDef->getTaskQueuePtr()->addTask(pUnArchiveTask);
}

void PakItem::asyncUnArchiveFile(UnArchiveParam* pUnArchiveParam)
{
	clearFileVec();
	writeArchiveFile2File(pUnArchiveParam);
}

void PakItem::adjustHeaderOffset()
{
	calcHeaderSize(m_pArchiveHeader->m_headerSize);

	FileHeaderVecIt itBegin;
	FileHeaderVecIt itEnd;

	itBegin = m_pFileVec->begin();
	itEnd = m_pFileVec->end();

	uint32 curFileOffset = m_pArchiveHeader->m_headerSize;

	for (; itBegin != itEnd; ++itBegin)
	{
		(*itBegin)->adjustHeaderOffset(curFileOffset);
		curFileOffset += (*itBegin)->getFileSize();
	}
}

void PakItem::calcHeaderSize(uint32& headerSize)
{
	headerSize = m_pArchiveHeader->calcArchiveHeaderSizeNoFileHeader();

	FileHeaderVecIt itBegin;
	FileHeaderVecIt itEnd;

	itBegin = m_pFileVec->begin();
	itEnd = m_pFileVec->end();

	for (; itBegin != itEnd; ++itBegin)
	{
		headerSize += (*itBegin)->calcHeaderSize();
	}
}

void PakItem::clearFileVec()
{
	m_fileSize = 0;
	m_pArchiveHeader->clear();

	FileHeaderVecIt itBegin = m_pFileVec->begin();
	FileHeaderVecIt itEnd = m_pFileVec->end();
	for (; itBegin != itEnd; ++itBegin)
	{
		delete (*itBegin);
	}

	m_pFileVec->clear();
}

// 写入文件
void PakItem::writeFile2ArchiveFile(ArchiveParam* pArchiveParam)
{
	FILE* fileHandle = fopen(m_fullPath->c_str(), "wb");

	if (fileHandle != nullptr)
	{
		calcHeaderSize(m_pArchiveHeader->m_headerSize);
		m_pArchiveHeader->writeArchiveFileHeader(fileHandle);

		// 移动文件指针
		fseek(fileHandle, m_pArchiveHeader->m_headerSize, SEEK_SET);	// 移动到文件开始位置

		FileHeaderVecIt itBegin;
		FileHeaderVecIt itEnd;

		itEnd = m_pFileVec->end();

		// 写文件内容
		itBegin = m_pFileVec->begin();
		for (; itBegin != itEnd; ++itBegin)
		{
			(*itBegin)->writeFile2ArchiveFile(fileHandle);
		}

		// 移动文件指针
		fseek(fileHandle, m_pArchiveHeader->calcArchiveHeaderSizeNoFileHeader(), SEEK_SET);	// 移动到文件开始位置

		// 修正文件偏移
		adjustHeaderOffset();

		// 写入头部
		itBegin = m_pFileVec->begin();

		for (; itBegin != itEnd; ++itBegin)
		{
			(*itBegin)->writeHeader2ArchiveFile(fileHandle);
		}

		fflush(fileHandle);
		fclose(fileHandle);
	}
}

void PakItem::readArchiveFileHeader(const char* pFileName)
{
	FILE* fileHandle = fopen(pFileName, "rb");

	if (fileHandle != nullptr)
	{
		readArchiveFileHeader(fileHandle);
		fclose(fileHandle);
	}
}

void PakItem::readArchiveFileHeader(FILE* fileHandle)
{
	MByteBuffer* pMByteBuffer = new MByteBuffer(INIT_CAPACITY);

	if (m_pArchiveHeader->readArchiveFileHeader(fileHandle, pMByteBuffer))		// 检查 magic
	{
		FileHeader* pFileHeader;
		for (uint32 idx = 0; idx < m_pArchiveHeader->m_fileCount; ++idx)
		{
			pFileHeader = new FileHeader();
			m_pFileVec->push_back(pFileHeader);
			pFileHeader->readHeaderFromArchiveFile(pMByteBuffer);
		}
	}
}

void PakItem::writeArchiveFile2File(UnArchiveParam* pUnArchiveParam)
{
	FILE* fileHandle = fopen(pUnArchiveParam->getUnArchiveFilePath(), "rb");

	if (fileHandle != nullptr)
	{
		readArchiveFileHeader(fileHandle);

		FileHeaderVecIt itBegin;
		FileHeaderVecIt itEnd;

		// 写入头部
		itBegin = m_pFileVec->begin();
		itEnd = m_pFileVec->end();

		for (; itBegin != itEnd; ++itBegin)
		{
			(*itBegin)->writeArchiveFile2File(fileHandle, pUnArchiveParam);
		}

		fclose(fileHandle);
	}
}

bool PakItem::canAddFile(PakPathSplitInfo* pPakPathSplitInfo)
{
	// 检查包名字是否相同
	if (pPakPathSplitInfo->getPakName() == *m_pakName)
	{
		// 如果没有超过最大大小
		if (m_fileSize + pPakPathSplitInfo->getFileOrigSize() <= FileArchiveToolSysDef->getConfigPtr()->getMaxSizePerPak())
		{
			return true;
		}
	}

	return false;
}

void PakItem::addFileHeader(FileHeader* pFileHeader)
{
	m_pFileVec->push_back(pFileHeader);
}

void PakItem::initByPakPathSplitInfo(PakPathSplitInfo* m_pPakPathSplitInfo, uint32 packIdx)
{
	*m_pakName = m_pPakPathSplitInfo->getPakName();
	m_pakIdx = packIdx;

	*m_fullPath += FileArchiveToolSysDef->getConfigPtr()->getOutputRootPath();
	*m_fullPath += "/";
	*m_fullPath += *m_pakName;
	*m_fullPath += "_";
	*m_fullPath += m_pakIdx;
	*m_fullPath += ".unity3d";
}

END_NAMESPACE_FILEARCHIVETOOL