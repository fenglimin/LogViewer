// LogViewerDlg.h : header file
//

#if !defined(AFX_LOGVIEWERDLG_H__58682DE1_0802_4C44_BC07_84A0211F0C9A__INCLUDED_)
#define AFX_LOGVIEWERDLG_H__58682DE1_0802_4C44_BC07_84A0211F0C9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "WaitDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CLogViewerDlg dialog

struct LogStatus
{
	BOOL bFiltered;		// This log will be shown in Log list
	BOOL bQueried;		// This log will be queried in the log list(Change background color)
	BOOL bHighlighted;	// This log will be highlighted in the log list(Change background color and font color)
	BOOL bSelected;
};

struct LogDetail
{
	int		nLogSeverity;
	CString strDateTime;
	CString strModuleName;
	int		nModuleNumber;
	CString	strProcessId;
	CString	strThreadId;
	CString	strCode;
	CString strLogContent;
	CString strSourceFileName;
	CString	strLineNumber;

	CString strRawLog;
	LogStatus* pLogStatus;
};

struct LogFile
{
	CString strLogFileName;
	BOOL	bNeedRefresh;
	std::vector<LogDetail> vecLog;
};


class CLogViewerDlg : public CDialog
{
// Construction
public:
	BOOL	m_bCheckModule;
	int GetSelectedModules();
	CString m_saModules[1000];
	
	int		m_nSeverity;
	COleDateTime m_dtNow;
	void LoadDayLog(CString strDate, BOOL bUpdateSize);
	CString m_strLogHome;
	BOOL GetLogDir();
	void AddModule(CString strModuleNumber, CString strModuleName);
	void BeforeLoad();
	void AfterLoad();
	
	void EnableHourControl(BOOL bEnable);
	void EnableDateControl(BOOL bEnable);
	void InitFilter();
	void InitModuleList();
	BOOL LoadLogFile(char* strLogFile, CString strDate, BOOL bUpdateSize);
	void ProcessLog(const LogDetail & logDetail);
	void InsertLog(const LogDetail& logDetail);
	CImageList	m_ImageListSmall, m_ImageListNormal;
	CListCtrl*	m_pLogList;
	CListCtrl*	m_pModuleList;
	CListCtrl*	m_pLogFileList;
	void InitLogList();
	CLogViewerDlg(CWnd* pParent = NULL);	// standard constructor
	~CLogViewerDlg();

	std::vector<CString> m_vecFilterKeyword;
	std::vector<LogFile> m_vecLogFile;
	std::vector<int> m_vecHitedLine;
	int		m_nCurrentHighlightedItemIndex;
	
	BOOL FilterLog(const LogDetail& logDetail);
	void SetRawLogContent(int nItem);
	CWaitDialog* m_pDlgWait;
	BOOL CheckKeyWord(const CString& strContent);
	void AddLogFile(const CString& strLogFileName);
	void InitLogFileList();
	void ShowAllLogFiles();
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
	CString	m_strProcessId;
	CString	m_strThreadId;
	BOOL	m_bCurrentHour;
	BOOL	m_bToday;
	int		m_nEndHour;
	int		m_nStartHour;
	BOOL	m_bExclude;
	int		m_nErrorCount;
	int		m_nWarningCount;
	CString m_strSearch;
	CString	m_strLastSelectedRawLog;
	int		m_nItemForLastSelectedRawLog;
	BOOL	m_bWorking;
	BOOL	m_bLatestConsoleStartupOnly;
	BOOL	m_bUserActions;
	BOOL	m_bAcqEvents;
	BOOL	m_bDipCom;
	BOOL	m_bDipLog;
	BOOL	m_bDipBeamSenseCom;
	BOOL	m_bPocVita;
	BOOL	m_bScannerState;
	BOOL	m_bPerformance;

	BOOL	m_bStarting;
	int		m_nLogFileCount;

	void	ShowLogFileCount();
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
	afx_msg void OnItemClickModuleList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void FilterLatestConsoleStartup();
	void CleanMemory();
	void OnBnClickedButtonReload();
	afx_msg void OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult);
	void OnReturnPressed(BOOL bCtrlPressed, BOOL bShiftPressed);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ChangeHighlightedItem(int nNewItem);
	void OnEnChangeEditSearch();
	afx_msg void OnBnClickedButtonHighlightFirst();
	afx_msg void OnBnClickedButtonHighlightPrev();
	afx_msg void OnBnClickedButtonHighlightNext();
	afx_msg void OnBnClickedButtonHighlightLast();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedCheckLatestConsoleStartup();
	afx_msg void OnBnClickedCheckUserActions();
//	afx_msg void OnBnClickedCheckUserActions2();
	afx_msg void OnBnClickedCheckAcqEvent();
	afx_msg void OnBnClickedCheckDipCom();
	afx_msg void OnBnClickedCheckDipLog();
	afx_msg void OnBnClickedCheckDipBeamSenseCom();
	afx_msg void OnBnClickedCheckPocVita();
	afx_msg void OnBnClickedCheckScannerState();
	afx_msg void OnBnClickedCheckPerformance();
	afx_msg void OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSelectAllFile();
	afx_msg void OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnEnChangeEditStartHour();
//	afx_msg void OnEnChangeEditEndHour();
//	afx_msg void OnDeltaposSpinStartHour(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditStartHour();
	afx_msg void OnEnChangeEditEndHour();
	afx_msg void OnBnClickedButtonSelectNoneFile();
//	afx_msg void OnLvnItemchangedListModuleList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListFile(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEWERDLG_H__58682DE1_0802_4C44_BC07_84A0211F0C9A__INCLUDED_)
