// LogViewer.h : main header file for the LOGVIEWER application
//

#if !defined(AFX_LOGVIEWER_H__7B52A5E0_C52E_4684_A6D3_DDFBD00EAFD5__INCLUDED_)
#define AFX_LOGVIEWER_H__7B52A5E0_C52E_4684_A6D3_DDFBD00EAFD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogViewerApp:
// See LogViewer.cpp for the implementation of this class
//

class CLogViewerApp : public CWinApp
{
public:
	CLogViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLogViewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEWER_H__7B52A5E0_C52E_4684_A6D3_DDFBD00EAFD5__INCLUDED_)
