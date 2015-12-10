// LogViewerDlg.h : header file
//

#if !defined(AFX_LOGVIEWERDLG_H__58682DE1_0802_4C44_BC07_84A0211F0C9A__INCLUDED_)
#define AFX_LOGVIEWERDLG_H__58682DE1_0802_4C44_BC07_84A0211F0C9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLogViewerDlg dialog

class CLogViewerDlg : public CDialog
{
// Construction
public:
	BOOL	m_bCheckModule;
	int GetSelectedModules();
	CString m_saModules[1000];
	
	int		m_nSeverity;
	COleDateTime m_dtNow;
	void LoadDayLog(CString strDate);
	CString m_strLogHome;
	BOOL GetLogDir();
	void AddModule(CString strModuleNumber, CString strModuleName);

	void EnableHourControl(BOOL bEnable);
	void EnableDateControl(BOOL bEnable);
	void InitFilter();
	void InitModuleList();
	BOOL LoadLogFile(char* strLogFile, CString strDate);
	void InsertLog(char* sDate, char* sTime, char* sModuleNumber, char* sModuleName, int nLogSeverity, char* sErrorCode, char* sLogInfo,  char* sFile, char* sLineNo);
	CImageList	m_ImageListSmall, m_ImageListNormal;
	CListCtrl*	m_pLogList;
	CListCtrl*	m_pModuleList;
	void InitLogList();
	CLogViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLogViewerDlg)
	enum { IDD = IDD_LOGVIEWER_DIALOG };
	CComboBox	m_comboLogSeverity;
	CSpinButtonCtrl	m_spinStartHour;
	CSpinButtonCtrl	m_spinEndHour;
	COleDateTime	m_tEndDay;
	COleDateTime	m_tStartDay;
	CString	m_strErrorCode;
	CString	m_strLogContains;
	BOOL	m_bCurrentHour;
	BOOL	m_bToday;
	int		m_nEndHour;
	int		m_nStartHour;
	BOOL	m_bExclude;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLogViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonRefresh();
	afx_msg void OnCheckToday();
	afx_msg void OnCheckCurrentHour();
	afx_msg void OnButtonSelectAll();
	afx_msg void OnButtonSelectNone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEWERDLG_H__58682DE1_0802_4C44_BC07_84A0211F0C9A__INCLUDED_)
