// LogViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LogViewer.h"
#include "LogViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogViewerApp

BEGIN_MESSAGE_MAP(CLogViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CLogViewerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewerApp construction

CLogViewerApp::CLogViewerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLogViewerApp object

CLogViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLogViewerApp initialization

BOOL CLogViewerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CRegKey partnerKey;
	HKEY hKey;
	CString subKey = L"Folder\\shell\\LogViewer\\command";
	::RegCreateKeyEx(HKEY_CLASSES_ROOT, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

	char szExePath[255];
	::GetModuleFileName(NULL, szExePath, 255);
	CString strValue;
	strValue.Format("\"%s\" \"%%1\"", szExePath);
	::RegSetValueEx(hKey, "", 0, REG_SZ, reinterpret_cast<const BYTE*>(strValue.GetBuffer()), strValue.GetLength() * sizeof(TCHAR));

	subKey = L"*\\shell\\LogViewer\\command";
	::RegCreateKeyEx(HKEY_CLASSES_ROOT, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
	::RegSetValueEx(hKey, "", 0, REG_SZ, reinterpret_cast<const BYTE*>(strValue.GetBuffer()), strValue.GetLength() * sizeof(TCHAR));

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	CLogViewerDlg dlg;
	dlg.m_strCommandInput = cmdInfo.m_strFileName;
	
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
