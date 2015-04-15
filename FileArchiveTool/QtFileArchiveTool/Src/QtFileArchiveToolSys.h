#ifndef __QtFileArchiveToolSys_H_
#define __QtFileArchiveToolSys_H_

#include "FileArchiveTool.h"

class QtFileArchiveToolSys : public FileArchiveToolSys
{
public:
	QtFileArchiveToolSys();
	virtual ~QtFileArchiveToolSys();

	virtual void init();	// 构造函数调用完成立马调用这个
};

#define QtFileArchiveToolSysDef QtFileArchiveToolSys::getSingletonPtr()

#endif		// __QTAIEDITORSYS_H_