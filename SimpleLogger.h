#pragma once

#include <stdlib.h>
#include <fstream>
using namespace std;

#define SIMPLE_LOG_LEVEL_SYSTEM					0
#define SIMPLE_LOG_LEVEL_DEBUG					1
#define SIMPLE_LOG_LEVEL_PERFORMANCE			2
#define SIMPLE_LOG_LEVEL_INFO					3
#define SIMPLE_LOG_LEVEL_WARN					4
#define SIMPLE_LOG_LEVEL_ERROR					5

#define SIMPLE_LOG_START(cfgFile)				(CSimpleLogger::StartLogger(cfgFile))
#define SIMPLE_LOG_END							(CSimpleLogger::EndLogger())

#define SIMPLE_LOG_DEBUG(fmt,...)				(CSimpleLogger::Log(SIMPLE_LOG_LEVEL_DEBUG,(CSimpleLogger::m_strSourceInfo+fmt).GetBuffer(0),__FILE__, __LINE__,__VA_ARGS__))
#define SIMPLE_LOG_INFO(fmt,...)				(CSimpleLogger::Log(SIMPLE_LOG_LEVEL_INFO, (CSimpleLogger::m_strSourceInfo+fmt).GetBuffer(0),__FILE__, __LINE__,__VA_ARGS__))
#define SIMPLE_LOG_WARN(fmt,...)				(CSimpleLogger::Log(SIMPLE_LOG_LEVEL_WARN, (CSimpleLogger::m_strSourceInfo+fmt).GetBuffer(0),__FILE__, __LINE__,__VA_ARGS__))
#define SIMPLE_LOG_ERR(fmt,...)					(CSimpleLogger::Log(SIMPLE_LOG_LEVEL_ERROR,(CSimpleLogger::m_strSourceInfo+fmt).GetBuffer(0),__FILE__, __LINE__,__VA_ARGS__))

#define SIMPLE_LOG_START_PERFORMANCE			(CSimpleLogger::StartPerformance())
#define SIMPLE_LOG_PERFORMANCE(start, action)	(CSimpleLogger::LogPerformance(start, action, __FILE__, __LINE__))

class CSimpleLogger
{
public:
	CSimpleLogger(void);
	virtual ~CSimpleLogger(void);

public:
	static BOOL StartLogger(const CString& strConfigFile);
	static BOOL EndLogger();
	static void Log(int nLevel, char *fmt, ...);
	static DWORD StartPerformance();
	static DWORD LogPerformance(DWORD dwActionID, const CString& strActionName, const CString& strFileName, int nLineNo);


private:
	static CString	GetCurrentFormattedTime(BOOL bForFileName);
	static CString  FormatMessage(const CString& strMessage);
	static void  LogInternal(int nLevel, const CString& strMessage);

public:
	static char*	m_strLogLevelString;
	static int	m_nLogLevel;
	static ofstream* m_pOfsLogger;
	static CString  m_strSourceInfo;
	static BOOL m_bEnable;
	static BOOL m_bShowSourceFileNameAndLineNo;
};

