#include "Config.h"

BEGIN_NAMESPACE_FILEARCHIVETOOL

Config::Config()
{
	m_pRootPath = new std::string("E:\\");
}

Config::~Config()
{
	delete m_pRootPath;
}

std::string& Config::getRootPath()
{
	return *m_pRootPath;
}

END_NAMESPACE_FILEARCHIVETOOL