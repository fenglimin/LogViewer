#if !defined(AFX_WAITDIALOG_H__49E26200_83EC_4A43_8B65_88A5FF2B0506__INCLUDED_)
#define AFX_WAITDIALOG_H__49E26200_83EC_4A43_8B65_88A5FF2B0506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaitDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaitDialog dialog
#include "Resource.h"

class CWaitDialog : public CDialog
{
// Construction
public:
	void Close();
	void Show();
	void UpdateText(CString strShowContent, BOOL bUpdateSize);
	CWaitDialog(CWnd* pParent = NULL);   // standard constructor
	~CWaitDialog();
// Dialog Data
	//{{AFX_DATA(CWaitDialog)
	enum { IDD = IDD_DIALOG_WAIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAITDIALOG_H__49E26200_83EC_4A43_8B65_88A5FF2B0506__INCLUDED_)
