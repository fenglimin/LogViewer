#include "StdAfx.h"
#include "SimpleLogger.h"

ofstream* CSimpleLogger::m_pOfsLogger = NULL;
CString  CSimpleLogger::m_strSourceInfo = _T("File:%s,Line:%d ");
BOOL CSimpleLogger::m_bShowSourceFileNameAndLineNo = FALSE;
BOOL CSimpleLogger::m_bEnable = TRUE;
int  CSimpleLogger::m_nLogLevel = SIMPLE_LOG_LEVEL_WARN;
char* CSimpleLogger::m_strLogLevelString = NULL;


CSimpleLogger::CSimpleLogger( void )
{
}


CSimpleLogger::~CSimpleLogger(void)
{
}

BOOL CSimpleLogger::StartLogger(const CString& strConfigFile)
{
	m_strLogLevelString = new char[7];
	strcpy_s(m_strLogLevelString, 7, _T(" DPIWE"));

	char cCurDir[1024];
	::GetModuleFileName(NULL,cCurDir, 1024);
	CString strExeFullPathName = cCurDir;

	int nToken = strExeFullPathName.ReverseFind('\\');
	CString strOnlyName = strExeFullPathName.Mid(nToken+1, strExeFullPathName.GetLength()-nToken-5);
	CString strDefaultConfigFile = strExeFullPathName + _T(".logconfig");

	if ( !strConfigFile.IsEmpty())
		strDefaultConfigFile = strConfigFile;

	TCHAR sValue[128];
	GetPrivateProfileString("SimpleLogger", "Enable", "0", sValue, 128, strDefaultConfigFile);
	m_bEnable = strcmp(sValue,"0");
	if ( !m_bEnable )
	{
		return FALSE;
	}

	GetPrivateProfileString("SimpleLogger", "ShowSourceFileNameAndLineNo", "0", sValue, 128, strConfigFile);
	m_bShowSourceFileNameAndLineNo = strcmp(sValue,"0");

	GetPrivateProfileString("SimpleLogger", "Level", "3", sValue, 128, strConfigFile);
	m_nLogLevel = atoi(sValue);

	GetPrivateProfileString("SimpleLogger", "LogFilePath", "C:\\", sValue, 128, strConfigFile);

	m_pOfsLogger = new ofstream();
	locale loc = locale::global(locale(""));
	CString strLogFileName = sValue + strOnlyName + " " + GetCurrentFormattedTime(TRUE)+ ".log";
	m_pOfsLogger->open(strLogFileName, ios_base::out);
	locale::global(loc);

	CString strMessage = "Logger started for application " + strExeFullPathName;
	if ( m_bShowSourceFileNameAndLineNo )
	{
		strMessage += " ,showing source file name and line number.";
	}
	strMessage += "\n";
	LogInternal( SIMPLE_LOG_LEVEL_SYSTEM, strMessage.GetBuffer(0));

	return TRUE;
}

BOOL CSimpleLogger::EndLogger()
{
	if ( !m_bEnable )
	{
		return FALSE;
	}

	LogInternal(SIMPLE_LOG_LEVEL_SYSTEM, "Logger ended");
	m_pOfsLogger->close();
	delete m_pOfsLogger;
	return FALSE;
}

void CSimpleLogger::Log(int nLevel, char *fmt, ... )
{
	if ( !m_bEnable ) return;

	if ( m_pOfsLogger == NULL ) return;

	if ( nLevel < m_nLogLevel) return;


	va_list  argptr ;				/* Argument list pointer	*/
	char str[10000];				/* Buffer to build sting into	*/

	va_start( argptr, fmt );		/* Initialize va_ functions	*/
	vsnprintf_s (str, 10000, _TRUNCATE, fmt, argptr);
	va_end( argptr );	

	LogInternal(nLevel, FormatMessage(str));
}

CString CSimpleLogger::GetCurrentFormattedTime( BOOL bForFileName )
{
	CString strFormat = "%04d-%02d-%02d %02d-%02d-%02d";
	if ( !bForFileName )
	{
		strFormat = "%04d-%02d-%02d %02d:%02d:%02d";
	}

	COleDateTime dt = COleDateTime::GetCurrentTime();
	CString strRet;
	strRet.Format(strFormat, dt.GetYear(), dt.GetMonth(), dt.GetDay(),
		dt.GetHour(), dt.GetMinute(), dt.GetSecond());

	return strRet;
}

CString CSimpleLogger::FormatMessage( const CString& strMessage )
{
	if ( m_bShowSourceFileNameAndLineNo )
		return strMessage;

	int nIndex = strMessage.Find(_T("Line:"));
	ASSERT(nIndex!=-1);

	nIndex += 5;
	int nLen = strMessage.GetLength();
	while ( nIndex < nLen && strMessage.GetAt(nIndex) != ' ' )
		nIndex++;

	return strMessage.Right(nLen-nIndex-1);
}

void CSimpleLogger::LogInternal( int nLevel, const CString& strMessage )
{
	CString strLine;
	strLine.Format("%s %c %d %s\n", GetCurrentFormattedTime(FALSE), m_strLogLevelString[nLevel], GetCurrentThreadId(), strMessage);
	(*m_pOfsLogger) << strLine;
	m_pOfsLogger->flush();
}

DWORD CSimpleLogger::StartPerformance()
{
	return GetTickCount();
}

DWORD CSimpleLogger::LogPerformance( DWORD dwStart, const CString& strActionName, const CString& strFileName, int nLineNo )
{
	DWORD dwEnd = GetTickCount();
	if ( m_nLogLevel > SIMPLE_LOG_LEVEL_PERFORMANCE)
		return dwEnd;

	CString strMessage;
	strMessage.Format("Running %s takes %d micro seconds.", strActionName, dwEnd-dwStart);

	if ( m_bShowSourceFileNameAndLineNo )
	{
		CString strTemp;
		strTemp.Format(m_strSourceInfo, strFileName, nLineNo);
		strMessage = strTemp + " " + strMessage;
	}

	LogInternal(SIMPLE_LOG_LEVEL_PERFORMANCE, strMessage);

	return dwEnd;
}
