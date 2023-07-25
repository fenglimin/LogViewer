// LogViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogViewer.h"
#include "LogViewerDlg.h"
#include "StringEx.h"
#include <fstream>
#include <afxtempl.h>
#include "WaitDialog.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT LoadCurrentHourThread(LPVOID pParam)
{
	CLogViewerDlg* pParent = static_cast<CLogViewerDlg*>(pParam);
	pParent->OnBnClickedButtonReload();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewerDlg dialog

CLogViewerDlg::CLogViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogViewerDlg)
	m_tEndDay = COleDateTime::GetCurrentTime();
	m_tStartDay = COleDateTime::GetCurrentTime();
	m_strErrorCode = _T("");
	m_strLogContains = _T("");
	m_bCurrentHour = FALSE;
	m_bToday = FALSE;
	m_nEndHour = m_tStartDay.GetHour();
	m_nStartHour = m_nEndHour;
	m_bExclude = FALSE;
	m_nItemForLastSelectedRawLog = -1;
	m_bWorking = FALSE;
	m_bLatestConsoleStartupOnly = FALSE;
	m_bUserActions = FALSE;
	m_bAcqEvents = FALSE;
	m_bDipCom = FALSE;
	m_bDipLog = FALSE;
	m_bDipBeamSenseCom = FALSE;
	m_bPocVita = FALSE;
	m_bScannerState = FALSE;
	m_bPerformance = FALSE;
	m_bStarting = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLogViewerDlg::~CLogViewerDlg()
{
	delete m_pDlgWait;
	CleanMemory();
}

void CLogViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogViewerDlg)
	DDX_Control(pDX, IDC_COMBO_SEVERITY, m_comboLogSeverity);
	DDX_Control(pDX, IDC_SPIN_START_HOUR, m_spinStartHour);
	DDX_Control(pDX, IDC_SPIN_END_HOUR, m_spinEndHour);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_tEndDay);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_tStartDay);
	DDX_Text(pDX, IDC_EDIT_ERROR_CODE, m_strErrorCode);
	DDX_Text(pDX, IDC_EDIT_LOG_CONTAINS, m_strLogContains);
	DDX_Text(pDX, IDC_EDIT_PROCESS_ID, m_strProcessId);
	DDX_Text(pDX, IDC_EDIT_THREAD_ID, m_strThreadId);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_strSearch);
	DDX_Check(pDX, IDC_CHECK_CURRENT_HOUR, m_bCurrentHour);
	DDX_Check(pDX, IDC_CHECK_TODAY, m_bToday);
	DDX_Check(pDX, IDC_CHECK_LATEST_CONSOLE_STARTUP, m_bLatestConsoleStartupOnly);
	DDX_Check(pDX, IDC_CHECK_USER_ACTIONS, m_bUserActions);
	DDX_Check(pDX, IDC_CHECK_ACQ_EVENT, m_bAcqEvents);
	DDX_Check(pDX, IDC_CHECK_DIP_COM, m_bDipCom);
	DDX_Check(pDX, IDC_CHECK_DIP_LOG, m_bDipLog);
	DDX_Check(pDX, IDC_CHECK_DIP_BEAM_SENSE_COM, m_bDipBeamSenseCom);
	DDX_Check(pDX, IDC_CHECK_POC_VITA, m_bPocVita);
	DDX_Check(pDX, IDC_CHECK_SCANNER_STATE, m_bScannerState);
	DDX_Check(pDX, IDC_CHECK_PERFORMANCE, m_bPerformance);
	DDX_Text(pDX, IDC_EDIT_END_HOUR, m_nEndHour);
	DDV_MinMaxInt(pDX, m_nEndHour, 0, 23);
	DDX_Text(pDX, IDC_EDIT_START_HOUR, m_nStartHour);
	DDV_MinMaxInt(pDX, m_nStartHour, 0, 23);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CLogViewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_BN_CLICKED(IDC_CHECK_TODAY, OnCheckToday)
	ON_BN_CLICKED(IDC_CHECK_CURRENT_HOUR, OnCheckCurrentHour)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, OnButtonSelectAll)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_NONE, OnButtonSelectNone)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_LOG, OnCustomDrawListCtrl)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MODULE_LIST, &CLogViewerDlg::OnItemClickModuleList)
	//}}AFX_MSG_MAP
	
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &CLogViewerDlg::OnBnClickedButtonReload)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LOG, &CLogViewerDlg::OnLvnItemchangedListLog)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CLogViewerDlg::OnEnChangeEditSearch)
	ON_BN_CLICKED(IDC_BUTTON_HIGHLIGHT_FIRST, &CLogViewerDlg::OnBnClickedButtonHighlightFirst)
	ON_BN_CLICKED(IDC_BUTTON_HIGHLIGHT_PREV, &CLogViewerDlg::OnBnClickedButtonHighlightPrev)
	ON_BN_CLICKED(IDC_BUTTON_HIGHLIGHT_NEXT, &CLogViewerDlg::OnBnClickedButtonHighlightNext)
	ON_BN_CLICKED(IDC_BUTTON_HIGHLIGHT_LAST, &CLogViewerDlg::OnBnClickedButtonHighlightLast)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CLogViewerDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_CHECK_LATEST_CONSOLE_STARTUP, &CLogViewerDlg::OnBnClickedCheckLatestConsoleStartup)
	ON_BN_CLICKED(IDC_CHECK_USER_ACTIONS, &CLogViewerDlg::OnBnClickedCheckUserActions)
	ON_BN_CLICKED(IDC_CHECK_ACQ_EVENT, &CLogViewerDlg::OnBnClickedCheckAcqEvent)
	ON_BN_CLICKED(IDC_CHECK_DIP_COM, &CLogViewerDlg::OnBnClickedCheckDipCom)
	ON_BN_CLICKED(IDC_CHECK_DIP_LOG, &CLogViewerDlg::OnBnClickedCheckDipLog)
	ON_BN_CLICKED(IDC_CHECK_DIP_BEAM_SENSE_COM, &CLogViewerDlg::OnBnClickedCheckDipBeamSenseCom)
	ON_BN_CLICKED(IDC_CHECK_POC_VITA, &CLogViewerDlg::OnBnClickedCheckPocVita)
	ON_BN_CLICKED(IDC_CHECK_SCANNER_STATE, &CLogViewerDlg::OnBnClickedCheckScannerState)
	ON_BN_CLICKED(IDC_CHECK_PERFORMANCE, &CLogViewerDlg::OnBnClickedCheckPerformance)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, &CLogViewerDlg::OnDtnDatetimechangeDatetimepickerStart)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL_FILE, &CLogViewerDlg::OnBnClickedButtonSelectAllFile)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, &CLogViewerDlg::OnDtnDatetimechangeDatetimepickerEnd)
	ON_EN_CHANGE(IDC_EDIT_START_HOUR, &CLogViewerDlg::OnEnChangeEditStartHour)
	ON_EN_CHANGE(IDC_EDIT_END_HOUR, &CLogViewerDlg::OnEnChangeEditEndHour)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_NONE_FILE, &CLogViewerDlg::OnBnClickedButtonSelectNoneFile)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILE, &CLogViewerDlg::OnNMClickListFile)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOG, &CLogViewerDlg::OnNMDblclkListLog)
//	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CLogViewerDlg::OnNMDblclkListFile)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_FILE, &CLogViewerDlg::OnNMRDblclkListFile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewerDlg message handlers

BOOL CLogViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_logConfig.bIgnoreKnownRepeatedLog = TRUE;
	m_logConfig.strSourceRoot = "D:\\Work\\Git\\ImageSuite\\GCPACS\\Src";
	m_logConfig.strNotepadPathName = "D:\\Work\\GreenTools\\NotePad++\\NotePad++.EXE";

	m_dtNow = COleDateTime::GetCurrentTime();
	InitLogFileList();
	InitLogList();
	InitModuleList();
	InitFilter();

	GetLogDir();

	OnButtonSelectAll();
	m_pDlgWait = new CWaitDialog(this);

	ShowAllLogFiles();
	OnBnClickedButtonSelectAllFile();
	//AfxBeginThread(LoadCurrentHourThread, this);

	m_bStarting = FALSE;
	OnBnClickedButtonReload();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLogViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

BOOL CLogViewerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;

		BOOL bCtrlPressed = GetKeyState(VK_CONTROL) & 0x8000;
		BOOL bShiftPressed = GetKeyState(VK_SHIFT) & 0x8000;
		if (pMsg->wParam == VK_RETURN)
		{
			OnReturnPressed(bCtrlPressed, bShiftPressed);
			return TRUE;
		}

		if (GetFocus() == m_pLogList)
		{
			if (pMsg->wParam == 69)
			{
				VisibleKeyLog(m_nCurrentErrorItemIndex, m_vecErrorLog, bCtrlPressed, bShiftPressed);
			}
			else if (pMsg->wParam == 87)
			{
				VisibleKeyLog(m_nCurrentWarningItemIndex, m_vecWarningLog, bCtrlPressed, bShiftPressed);
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
}




// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLogViewerDlg::OnCustomDrawListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	// Check the stage of custom drawing
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// Request item-specific notifications for each subitem
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		// Determine the item and subitem index
		int itemIndex = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		LVITEM lvItem;
		lvItem.mask = LVIF_IMAGE;
		lvItem.iItem = itemIndex;
		lvItem.iSubItem = 0;

		if (m_pLogList->GetItem(&lvItem))
		{
			int imageIndex = lvItem.iImage;
			if (imageIndex >= 3)
			{
				pLVCD->clrText = RGB(255, 0, 0);
			}
			else if (imageIndex == 2)
			{
				pLVCD->clrText = RGB(153, 102, 0);
			}

			LogStatus* pLogStatus = (LogStatus*)m_pLogList->GetItemData(itemIndex);
			if (pLogStatus->bQueried)
			{
				pLVCD->clrTextBk = pLogStatus->bHighlighted? RGB(0, 240, 240) : RGB(0, 190, 190);
			}
			else
			{
				pLVCD->clrTextBk = RGB(255, 255, 255);
			}

			if (pLogStatus->bSelected)
			{
				pLVCD->clrTextBk = RGB(0, 0, 255);
			}
		}		

		*pResult = CDRF_NEWFONT;
		break;
	}
}


void CLogViewerDlg::InitLogList()
{
	m_pLogList = (CListCtrl*)GetDlgItem(IDC_LIST_LOG);

	// Initial extended style for the list control on this dialog
	DWORD dwStyle = m_pLogList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_pLogList->SetExtendedStyle(dwStyle);

	int nCol = 0;
	m_pLogList->InsertColumn(nCol++, " ", LVCFMT_LEFT, 20);
	m_pLogList->InsertColumn(nCol++, "Log Time", LVCFMT_LEFT, 150);
	m_pLogList->InsertColumn(nCol++, "ModuleName", LVCFMT_LEFT, 90);
	m_pLogList->InsertColumn(nCol++, "PID", LVCFMT_LEFT, 50);
	m_pLogList->InsertColumn(nCol++, "TID", LVCFMT_LEFT, 50);
	m_pLogList->InsertColumn(nCol++, "Log Content", LVCFMT_LEFT, 580);
	m_pLogList->InsertColumn(nCol++, "Code", LVCFMT_LEFT, 50);
	m_pLogList->InsertColumn(nCol++, "File", LVCFMT_LEFT, 60);
	m_pLogList->InsertColumn(nCol++, "LineNo", LVCFMT_LEFT, 50);
	m_pLogList->InsertColumn(nCol++, "MN", LVCFMT_LEFT, 30);

	// Create 256 color image lists
	HIMAGELIST hList = ImageList_Create(16, 18, ILC_COLOR8 | ILC_MASK, 8, 1);
	m_ImageListSmall.Attach(hList);
	
	// Load the small icons
	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_BITMAP_LOG_SEVERITY);
	m_ImageListSmall.Add(&cBmp, RGB(255,0, 255));

	m_pLogList->SetImageList(&m_ImageListSmall, LVSIL_SMALL);


//	LoadLogFile("C:\\Image Suite\\Log\\2010-08-18\\2010-08-18-09.log");

}

void CLogViewerDlg::InsertLog(const LogDetail& logDetail)
{
	int nItem = m_pLogList->GetItemCount();
	m_pLogList->InsertItem(nItem, "", logDetail.nLogSeverity);
	if (logDetail.nLogSeverity >= 3)
		m_vecErrorLog.push_back(nItem);
	else if (logDetail.nLogSeverity == 2)
		m_vecWarningLog.push_back(nItem);

	int nSubItem = 1;
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strDateTime);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strModuleName);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strProcessId);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strThreadId);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strLogContent);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strCode);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strSourceFileName);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strSourceFileLineNumber);
	m_pLogList->SetItemText(nItem, nSubItem++, CStringEx(logDetail.nModuleNumber));

	m_pLogList->SetItemData(nItem, (DWORD_PTR)logDetail.pLogStatus);

	if (logDetail.strRawLog == m_strLastSelectedRawLog)
	{
		m_nItemForLastSelectedRawLog = nItem;
	}
}

BOOL CLogViewerDlg::LoadLogFile(char* strLogFile, CString strDate, BOOL bUpdateSize)
{
	char sCurLine[1024];
	char sNextLine[1024];

	ifstream ifs(strLogFile);
	if ( !ifs.good())
	{
		AfxMessageBox("Error reading log file " + CString(strLogFile));
		return FALSE;
	}
		
	CString strShow = "Loading log file ";
	strShow += strLogFile;
	m_pDlgWait->UpdateText( strShow, bUpdateSize);
	
	LogFile logFile;
	logFile.strLogFileName = strLogFile;

	int nFileIndex = m_vecLogFile.size();
	int nLineNum = 0;
	ifs.getline(sCurLine, 1023);
	CStringEx strCurLine = sCurLine;

	do
	{
		ifs.getline(sNextLine, 1023);
		nLineNum++;
		CString strNextLine = sNextLine;
		if ( strNextLine.Left(10) != strDate)
		{
			strCurLine += strNextLine;
			if (ifs.good())
				continue;
		}
		
		if ( strCurLine == "")
			continue;

		CStringEx strSave = strCurLine;
		strCurLine = strNextLine;

		int nSize = logFile.vecLog.size();
		if (m_logConfig.bIgnoreKnownRepeatedLog && nSize > 2)
		{
			CString aa = strCurLine.Mid(28, 86);
			if (logFile.vecLog[nSize - 2].strLogContent == "[DIP COM] CDRManager::GetNotReadyReason success: Exposures Disabled" &&
				logFile.vecLog[nSize - 1].strLogContent == "[DIP Event] DeviceManagerStateChanged: State changed to Not ready.")
			{
				if (strCurLine.Mid(28, 86) == "[DIP COM] CDRManager::GetNotReadyReason success: Exposures Disabled``DRManager.cpp`778" ||
					strCurLine.Mid(28, 90) == "[DIP Event] DeviceManagerStateChanged: State changed to Not ready.``DREventHandler.cpp`341")
					continue;
			}			
		}

		LogDetail logDetail;

		logDetail.nRawLogLineNumber = nLineNum;

		logDetail.strRawLog = strSave;
		if (strSave.GetAt(19) == '`')
			logDetail.strDateTime = strSave.Left(19);
		else
			logDetail.strDateTime = strSave.Left(23);
		
		CStringEx sModuleNumber = strSave.GetField("`", 1);
		int nModuleNumber = sModuleNumber.AsInt();
		if (nModuleNumber >= 1000)
		{
			sModuleNumber = "0";
			nModuleNumber = 0;
		}
		logDetail.nModuleNumber = nModuleNumber;
		
		CString strModuleName = strSave.GetField("`", 2);
		logDetail.strModuleName = m_saModules[nModuleNumber];
		if (logDetail.strModuleName == "")
			logDetail.strModuleName = strModuleName;

		logDetail.nLogSeverity = strSave.GetField("`", 3).AsInt();
		logDetail.strCode = strSave.GetField("`", 4);
		logDetail.strProcessId = strSave.GetDelimitedField("PID:", ",", 0);
		logDetail.strThreadId = strSave.GetDelimitedField("TID:", ",", 0);
		logDetail.strLogContent = strSave.GetField("`", 5);
		if (!logDetail.strProcessId.IsEmpty())
		{
			logDetail.strLogContent = logDetail.strLogContent.Right(logDetail.strLogContent.GetLength() - logDetail.strProcessId.GetLength() - logDetail.strThreadId.GetLength() - 11);
		}
		logDetail.strSourceFileName = strSave.GetField("`", 7);
		logDetail.strSourceFileLineNumber = strSave.GetField("`", 8);

		logDetail.pLogStatus = new LogStatus();
		logDetail.pLogStatus->nLogFileIndex = nFileIndex;
		logDetail.pLogStatus->nLogContentIndex = logFile.vecLog.size();

		logFile.vecLog.push_back(logDetail);

		if (!m_bLatestConsoleStartupOnly)
			ProcessLog(logDetail);
	} while (ifs.good());
	
	ifs.close();

	m_vecLogFile.push_back(logFile);
	return TRUE;
}

void CLogViewerDlg::ProcessLog(const LogDetail& logDetail)
{
	logDetail.pLogStatus->bFiltered = FilterLog(logDetail);
	logDetail.pLogStatus->bQueried = FALSE;
	logDetail.pLogStatus->bHighlighted = FALSE;
	logDetail.pLogStatus->bSelected = FALSE;

	if (logDetail.pLogStatus->bFiltered)
	{
		InsertLog(logDetail);
	}
}

BOOL CLogViewerDlg::FilterLog(const LogDetail& logDetail)
{
	if (!m_bExclude)
	{
		if (m_bCheckModule && m_saModules[logDetail.nModuleNumber] == "")
			return FALSE;
	}
	else
	{
		if (m_saModules[logDetail.nModuleNumber] != "")
			return FALSE;
	}

	if (logDetail.nLogSeverity < m_nSeverity)
		return FALSE;

	if (m_strErrorCode != "" && m_strErrorCode != logDetail.strCode)
		return FALSE;

	if (m_strLogContains != "")
	{
		CString strTemp = logDetail.strLogContent;
		strTemp.MakeUpper();
		if (strTemp.Find(m_strLogContains) == -1)
			return FALSE;
	}

	if (m_strProcessId != "")
	{
		if (logDetail.strProcessId.Find(m_strProcessId) == -1)
			return FALSE;
	}
	
	if (m_strThreadId != "")
	{
		if (logDetail.strThreadId.Find(m_strThreadId) == -1)
			return FALSE;
	}

	return CheckKeyWord(logDetail.strLogContent);
}

BOOL CLogViewerDlg::CheckKeyWord(const CString& strContent)
{
	if (m_vecFilterKeyword.size() == 0)
		return TRUE;
	
	CString strTemp = strContent;
	strTemp.MakeUpper();
	for(int i = 0; i < m_vecFilterKeyword.size(); i++)
	{
		if (strTemp.Find(m_vecFilterKeyword[i]) != -1)
			return TRUE;

	}
	
	return FALSE;
}

void CLogViewerDlg::InitModuleList()
{
	m_pModuleList = (CListCtrl*)GetDlgItem(IDC_LIST_MODULE_LIST);
	DWORD dwStyle = m_pModuleList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_CHECKBOXES;
	m_pModuleList->SetExtendedStyle(dwStyle);

	m_pModuleList->InsertColumn(0, " ", LVCFMT_LEFT, 20);
	m_pModuleList->InsertColumn(1, "No", LVCFMT_LEFT, 30);
	m_pModuleList->InsertColumn(2, "Name", LVCFMT_LEFT, 120);

	AddModule("11", "SMS Server", "Server\\SMS", "SMS.vcxproj");
	AddModule("12", "Print Server", "Server\\PrintServer", "PrintServer.vcxproj");
	AddModule("16", "Service Manager", "Server\\ServiceManager", "ServiceManager.vcxproj");
	AddModule("30", "Task Manager", "Server\\TaskManager", "TaskManager.vcxproj");
	AddModule("33", "Oam Server", "Server\\OamServer", "OamServer.vcxproj");
	AddModule("34", "OAM Panel", "Client\\OAMPANEL", "OamPanel.vcxproj");
	AddModule("35", "Rule Server", "Server\\RuleServer", "RuleServer.vcxproj");
	AddModule("36", "Eclipse", "Client\\EclipseTool", "EclipseTool.vcxproj");
	AddModule("47", "PDC Sender Server", "Server\\PdcSender\\PdcSenderServer", "PdcSenderServer.vcxproj");
	AddModule("52", "SSCU", "Server\\SSCU", "SSCU.vcxproj");
	AddModule("53", "SSCP Server", "Server\\SSCPServer", "SscpServer.vcxproj");
	AddModule("56", "Acq Server", "Server\\AcquisitionServer", "AcquisitionServer.vcxproj");
	AddModule("59", "Multi Recordset", "Server\\QRSCU\\MultiRecordSet", "MRS.vcxproj");
	AddModule("65", "Framework", "Client\\FRAMEWORK", "framework.vcxproj");
	AddModule("66", "Audit Server", "Server\\AuditServer", "AuditServer.vcxproj");
	AddModule("77", "MWL Server", "Server\\MWL", "mwl.vcxproj");	
	AddModule("93", "Device Manager", "Client\\DeviceManager\\DeviceManager", "DeviceManager.vcxproj");	
	AddModule("94", "Acq Panel", "Client\\AcqPanel", "AcqPanel.vcxproj");	
}

void CLogViewerDlg::InitLogFileList()
{
	m_pLogFileList = (CListCtrl*)GetDlgItem(IDC_LIST_FILE);
	DWORD dwStyle = m_pLogFileList->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_CHECKBOXES;
	m_pLogFileList->SetExtendedStyle(dwStyle);

	m_pLogFileList->InsertColumn(0, " ", LVCFMT_LEFT, 20);
	m_pLogFileList->InsertColumn(1, "Name", LVCFMT_LEFT, 150);
}

void CLogViewerDlg::OnCheckToday() 
{
	m_bToday = !m_bToday;
	EnableDateControl(!m_bToday);

	COleDateTime dt = COleDateTime::GetCurrentTime();
	int nYear = dt.GetYear();
	int nMonth = dt.GetMonth();
	int nStartDay = dt.GetDay();
	int nEndDay = nStartDay;
	if ( !m_bToday )
	{
		nStartDay--;
		
		m_bCurrentHour = FALSE;
		m_nStartHour = 0;
		m_nEndHour = 23;	
	}

	m_tStartDay.SetDate(nYear, nMonth, nStartDay);
	m_tEndDay.SetDate(nYear, nMonth, nEndDay);
	UpdateData(FALSE);
	
	ShowAllLogFiles();
}

void CLogViewerDlg::OnCheckCurrentHour() 
{
	m_bCurrentHour = !m_bCurrentHour;
	EnableHourControl(!m_bCurrentHour);

	COleDateTime dt = COleDateTime::GetCurrentTime();
	int nHour = dt.GetHour();
	m_nEndHour = nHour;

	if ( !m_bCurrentHour )
	{
		if ( nHour > 0 )
			nHour--;
	}

	m_nStartHour = nHour;
	UpdateData(FALSE);

	ShowAllLogFiles();
}

void CLogViewerDlg::OnButtonSelectAll() 
{
	int nCount = m_pModuleList->GetItemCount();
	for ( int i = 0; i < nCount; i ++)
	{
		m_pModuleList->SetCheck(i, TRUE);
	}
	
}

void CLogViewerDlg::OnButtonSelectNone() 
{
	int nCount = m_pModuleList->GetItemCount();
	for ( int i = 0; i < nCount; i ++)
	{
		m_pModuleList->SetCheck(i, FALSE);
	}	
}

void CLogViewerDlg::InitFilter()
{
	m_comboLogSeverity.SetCurSel(4);

	m_spinStartHour.SetRange(0,23);
	m_spinEndHour.SetRange(0,23);

	m_bToday = TRUE;
	EnableDateControl(!m_bToday);

	m_bCurrentHour = TRUE;
	EnableHourControl(!m_bCurrentHour);

	m_bExclude = FALSE;

	UpdateData(FALSE);

}

void CLogViewerDlg::EnableDateControl(BOOL bEnable)
{
	GetDlgItem(IDC_DATETIMEPICKER_START)->EnableWindow(bEnable);
	GetDlgItem(IDC_DATETIMEPICKER_END)->EnableWindow(bEnable);
}

void CLogViewerDlg::EnableHourControl(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_START_HOUR)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_END_HOUR)->EnableWindow(bEnable);
}

void CLogViewerDlg::AddModule(const CString& strModuleId, const CString& strModuleName, const CString& strProjectDir, const CString& strProjectFile)
{
	ModuleDetail moduleDetail;
	moduleDetail.strModuleId = strModuleId;
	moduleDetail.strModuleName = strModuleName;
	moduleDetail.strProjectDir = strProjectDir;
	moduleDetail.strProjectFile = strProjectFile;

	m_vecModule.push_back(moduleDetail);
	
	int nPos = m_pModuleList->InsertItem(m_pModuleList->GetItemCount(), "");
	m_pModuleList->SetItemText(nPos, 1, strModuleId);
	m_pModuleList->SetItemText(nPos, 2, strModuleName);
}

void CLogViewerDlg::AddLogFile(const CString& strLogFileName)
{
	CString strLogFile = m_strLogHome + "\\" + strLogFileName.Left(10) + "\\" + strLogFileName;
	ifstream ifs(strLogFile);
	if (!ifs.good())
	{
		return;
	}

	ifs.seekg(0, std::ios::end);
	std::streampos fileSize = ifs.tellg();
	ifs.close();

	int i = (int)fileSize;
	
	int nPos = m_pLogFileList->InsertItem(m_pLogFileList->GetItemCount(), "");
	m_pLogFileList->SetItemText(nPos, 1, strLogFileName);
}

BOOL CLogViewerDlg::GetLogDir()
{
	char	szConfigFile[128], szExePath[255];

	char	Temp[128];
	
	char *pDest;
	::GetModuleFileName(NULL, szExePath, 255);	
	pDest = strrchr(szExePath, '\\');
	*(++pDest) = '\0';
	
	strcpy(szConfigFile, szExePath);
	strcat(szConfigFile, "config\\Client.ini");


	GetPrivateProfileString("LOG","LogHomeDir","C:\\Image Suite\\Log",(char*)Temp,128,szConfigFile);

	m_strLogHome = Temp;
	SetDlgItemText(IDC_EDIT_LOG_DIR, m_strLogHome);

	return TRUE;
}

void CLogViewerDlg::LoadDayLog(CString strDate, BOOL bUpdateSize)
{
	if ( m_bCurrentHour )
	{
		CString strHour = m_dtNow.Format("%H");
		AddLogFile(strDate + "-" + strHour + ".log");
	}
	else
	{
		for ( int i = m_nStartHour; i <= m_nEndHour; i++ )
		{
			CString strHour;
			if ( i < 10 )
				strHour.Format("0%d", i );
			else
				strHour.Format("%d", i );

			AddLogFile(strDate + "-" + strHour + ".log");
		}
	}
}

int CLogViewerDlg::GetSelectedModules()
{
	int nTotal = 0;
	int i;
	for ( i = 0; i < 1000; i ++ )
		m_saModules[i] = "";

	int nCount = m_pModuleList->GetItemCount();
	for ( i = 0; i < nCount; i ++)
	{
		if ( m_pModuleList->GetCheck(i) )
		{
			nTotal++;
			CStringEx strModuleNumber = m_pModuleList->GetItemText(i, 1);
			m_saModules[strModuleNumber.AsInt()] = m_pModuleList->GetItemText(i, 2);
		}
	}
	
	if ( nTotal == nCount )
		m_bCheckModule = FALSE;
	else
		m_bCheckModule = TRUE;

	return nTotal;
}


void CLogViewerDlg::OnItemClickModuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->iItem >= 0 && pNMLV->iSubItem > 0)
	{
		UINT state = m_pModuleList->GetItemState(pNMLV->iItem, LVIS_STATEIMAGEMASK);

		// Toggle checkbox state
		if (state == INDEXTOSTATEIMAGEMASK(1)) // Unchecked
		{
			m_pModuleList->SetCheck(pNMLV->iItem, TRUE);
		}
		else if (state == INDEXTOSTATEIMAGEMASK(2)) // Checked
		{
			m_pModuleList->SetCheck(pNMLV->iItem, FALSE);
		}
	}
	
	*pResult = 0;
}

void CLogViewerDlg::OnButtonRefresh()
{
	if (!BeforeLoad())
		return;

	if (m_bLatestConsoleStartupOnly)
	{
		FilterLatestConsoleStartup();
	}
	else
	{
		for (int i = 0; i < m_vecLogFile.size(); i++)
		{
			CString strShow = "Filtering log file " + m_vecLogFile[i].strLogFileName;
			m_pDlgWait->UpdateText(strShow, i == 0);

			for (int j = 0; j < m_vecLogFile[i].vecLog.size(); j++)
			{
				ProcessLog(m_vecLogFile[i].vecLog[j]);
			}
		}
	}	

	AfterLoad();
	m_pLogList->SetFocus();
}

void CLogViewerDlg::CleanMemory()
{
	for (int i = 0; i < m_vecLogFile.size(); i++)
	{
		for (int j = 0; j < m_vecLogFile[i].vecLog.size(); j++)
		{
			delete m_vecLogFile[i].vecLog[j].pLogStatus;
		}
	}
}

void CLogViewerDlg::ShowAllLogFiles()
{
	UpdateData();
	m_pLogFileList->DeleteAllItems();
	if (m_bToday)
	{
		CString strDate = m_dtNow.Format("%Y-%m-%d");
		LoadDayLog(strDate, TRUE);
	}
	else
	{
		for (COleDateTime dtDay = m_tStartDay; dtDay <= m_tEndDay; dtDay += 1)
		{
			CString strDate = dtDay.Format("%Y-%m-%d");
			LoadDayLog(strDate, dtDay == m_tStartDay);
		}
	}

	OnBnClickedButtonSelectAllFile();
	ShowLogFileCount();
}

void CLogViewerDlg::ShowLogFileCount()
{
	CString strShow;
	m_nLogFileCount = 0;
	for (int i = 0; i < m_pLogFileList->GetItemCount(); i++)
	{
		if (m_pLogFileList->GetCheck(i))
			m_nLogFileCount++;
	}

	strShow.Format("File(%d)", m_nLogFileCount);
	SetDlgItemText(IDC_STATIC_LOG_FILE, strShow);
}


void CLogViewerDlg::OnBnClickedButtonReload()
{
	if (!BeforeLoad())
		return;

	CleanMemory();
	
	m_vecLogFile.clear();

	BOOL bUpdateSize = TRUE;
	int nCount = m_pLogFileList->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		if (!m_pLogFileList->GetCheck(i))
			continue;

		CString strFileName = m_pLogFileList->GetItemText(i, 1);
		CString strDate = strFileName.Left(10);
		CString strFilePath = m_strLogHome + "\\" + strDate + "\\" + strFileName;
		LoadLogFile((LPTSTR)(LPCTSTR)strFilePath, strDate, bUpdateSize);
		bUpdateSize = FALSE;
	}

	if (m_bLatestConsoleStartupOnly)
	{
		FilterLatestConsoleStartup();
	}
	
	AfterLoad();
	m_pLogList->SetFocus();
}


void CLogViewerDlg::OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if (m_bWorking)
		return;
	
	LogStatus* pLogStatus = (LogStatus*)m_pLogList->GetItemData(pNMLV->iItem);
	pLogStatus->bSelected = pNMLV->uNewState & LVIS_SELECTED == LVIS_SELECTED;

	if (pNMLV->uNewState & LVIS_FOCUSED)
	{
		SetRawLogContent(pNMLV->iItem);
		ShowStatistics(pNMLV->iItem);
	}	
}

void CLogViewerDlg::SetRawLogContent(int nItem)
{
	if (!m_bWorking)
	{
		if (nItem == -1)
		{
			m_strLastSelectedRawLog = "";			
		}
		else
		{
			LogStatus* pLogStatus = (LogStatus*)m_pLogList->GetItemData(nItem);
			LogDetail logDetail = m_vecLogFile[pLogStatus->nLogFileIndex].vecLog[pLogStatus->nLogContentIndex];
			m_strLastSelectedRawLog = logDetail.strRawLog;
		}
		SetDlgItemText(IDC_EDIT_RAW_LOG, m_strLastSelectedRawLog);
	}	
}


BOOL CLogViewerDlg::BeforeLoad()
{
	if (GetSelectedModules() == 0)
	{
		AfxMessageBox("Please select at least one Module!");
		return FALSE;
	}

	if (m_nLogFileCount == 0)
	{
		AfxMessageBox("Please select at least one log file!");
		return FALSE;
	}

	m_nCurrentErrorItemIndex = -1;
	m_nCurrentWarningItemIndex = -1;
	m_bWorking = TRUE;
	m_vecErrorLog.clear();
	m_vecWarningLog.clear();
	m_dtNow = COleDateTime::GetCurrentTime();
	m_pDlgWait->Show();
	UpdateData();

	m_strLogContains.MakeUpper();

	m_pLogList->SetRedraw(FALSE);// LockWindowUpdate();



	m_nSeverity = 4 - m_comboLogSeverity.GetCurSel();
	m_pLogList->DeleteAllItems();

	m_vecFilterKeyword.clear();
	if (m_bUserActions)
		m_vecFilterKeyword.push_back("[USERACTION]");
	if (m_bAcqEvents)
		m_vecFilterKeyword.push_back("[ACQ EVENT]");
	if (m_bDipCom)
		m_vecFilterKeyword.push_back("[DIP COM]");
	if (m_bDipLog)
		m_vecFilterKeyword.push_back("[DIP LOG]");
	if (m_bDipBeamSenseCom)
		m_vecFilterKeyword.push_back("[DIP  BEAM SENSE COM]");
	if (m_bPocVita)
		m_vecFilterKeyword.push_back("[POC VITA]");
	if (m_bScannerState)
		m_vecFilterKeyword.push_back("[SCANNER STATE]");
	if (m_bPerformance)
		m_vecFilterKeyword.push_back("PERFORMANCE");

	return TRUE;
}


void CLogViewerDlg::AfterLoad()
{
	m_pDlgWait->Close();
	m_pLogList->SetRedraw(TRUE);//UnlockWindowUpdate();

	OnEnChangeEditSearch();
	
	COleDateTimeSpan timeSpan = COleDateTime::GetCurrentTime() - m_dtNow;
	CString strTitle;
	strTitle.Format("LogViewer - Total %d logs, %d Error logs, %d Warning logs, takes %d seconds.", m_pLogList->GetItemCount(), 
		(int)m_vecErrorLog.size(), (int)m_vecWarningLog.size(), (int)timeSpan.GetTotalSeconds());
	SetWindowText(strTitle);

	m_bWorking = FALSE;
	if (m_nItemForLastSelectedRawLog != -1 && m_nItemForLastSelectedRawLog < m_pLogList->GetItemCount())
	{
		CenterLogItem(m_nItemForLastSelectedRawLog);
		m_pLogList->SetItemState(m_nItemForLastSelectedRawLog, LVIS_SELECTED, LVIS_SELECTED);
		ShowStatistics(m_nItemForLastSelectedRawLog);
	}
	else
		ShowStatistics(0);
}

void CLogViewerDlg::OnEnChangeEditSearch()
{
	GetDlgItemText(IDC_EDIT_SEARCH, m_strSearch);
	m_strSearch.MakeUpper();
	m_strSearch.Trim();

	m_vecHitedLine.clear();
	
	int nCount = m_pLogList->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		BOOL bNeedRedraw;
		LogStatus* pLogStatus = (LogStatus*)m_pLogList->GetItemData(i);
		if (!m_strSearch.IsEmpty() && m_pLogList->GetItemText(i, 5).MakeUpper().Find(m_strSearch) != -1)
		{
			bNeedRedraw = !pLogStatus->bQueried;
			pLogStatus->bQueried = TRUE;
			m_vecHitedLine.push_back(i);
		}
		else
		{
			bNeedRedraw = pLogStatus->bQueried;
			pLogStatus->bQueried = FALSE;
		}

		if (bNeedRedraw || pLogStatus->bHighlighted)
		{
			pLogStatus->bHighlighted = FALSE;
			m_pLogList->RedrawItems(i, i);
		}
	}

	int nCurrentHighlightedItemIndex = m_vecHitedLine.size() > 0 ? 0 : -1;
	ChangeHighlightedItem(nCurrentHighlightedItemIndex);	
}

void CLogViewerDlg::OnReturnPressed(BOOL bCtrlPressed, BOOL bShiftPressed)
{
	CWnd* pFocusCtrl = GetFocus();
	
	if ( pFocusCtrl == GetDlgItem(IDC_EDIT_SEARCH) || pFocusCtrl == m_pLogList )
	{
		if (bShiftPressed)
		{
			if (bCtrlPressed)
				OnBnClickedButtonHighlightFirst();
			else
				OnBnClickedButtonHighlightPrev();
		}
		else
		{
			if (bCtrlPressed)
				OnBnClickedButtonHighlightLast();
			else
				OnBnClickedButtonHighlightNext();
		}		
	}
	else if (pFocusCtrl == GetDlgItem(IDC_EDIT_ERROR_CODE) ||
		pFocusCtrl == GetDlgItem(IDC_EDIT_LOG_CONTAINS) ||
		pFocusCtrl == GetDlgItem(IDC_EDIT_PROCESS_ID) ||
		pFocusCtrl == GetDlgItem(IDC_EDIT_THREAD_ID))
	{
		OnButtonRefresh();
	}
}

void CLogViewerDlg::VisibleKeyLog(int& nCurrentIndex, vector<int>& vecData, BOOL bCtrlPressed, BOOL bShiftPressed)
{
	if (vecData.size() == 0)
		return;
	
	if (bShiftPressed)
	{
		if (nCurrentIndex == 0)
			return;
		
		if (bCtrlPressed)
			nCurrentIndex = 0;
		else
		{
			nCurrentIndex--;
		}
	}
	else
	{
		if (nCurrentIndex >= (int)vecData.size() - 1)
			return;
		
		if (bCtrlPressed)
		{
			nCurrentIndex = (int)vecData.size() - 1;
		}
		else
		{
			nCurrentIndex++;
		}			
	}

	POSITION pos = m_pLogList->GetFirstSelectedItemPosition();
	for(;;)
	{
		int nItem = m_pLogList->GetNextSelectedItem(pos);
		if (nItem == -1)
			break;
		m_pLogList->SetItemState(nItem, 0, LVIS_SELECTED);
		if (pos == NULL)
			break;
	}
	
	CenterLogItem(vecData[nCurrentIndex]);
	m_pLogList->SetItemState(vecData[nCurrentIndex], LVIS_SELECTED, LVIS_SELECTED);

	ShowStatistics(vecData[nCurrentIndex]);
}

void CLogViewerDlg::ShowStatistics(int nItem)
{
	CString strMessage;
	strMessage.Format("Log List - Total(%d/%d), Error(%d/%d), Warning(%d/%d)", nItem+1, m_pLogList->GetItemCount(),
		m_nCurrentErrorItemIndex + 1, (int)m_vecErrorLog.size(), m_nCurrentWarningItemIndex + 1, (int)m_vecWarningLog.size());
	SetDlgItemText(IDC_STATIC_LOG_GROUP, strMessage);
}

BOOL CLogViewerDlg::IsLogItemVisible(int nItem)
{
	CRect itemRect;
	if (m_pLogList->GetItemRect(nItem, &itemRect, LVIR_BOUNDS))
	{
		CRect clientRect;
		m_pLogList->GetClientRect(&clientRect);

		return clientRect.PtInRect(itemRect.TopLeft()) || clientRect.PtInRect(itemRect.BottomRight());
	}

	return FALSE;
}

int CLogViewerDlg::FindLastVisibleItem(const vector<int>& vecData)
{
	CRect itemRect;
	for (int i = vecData.size() - 1; i >= 0; i--)
	{
		if (IsLogItemVisible(vecData[i]))
			return vecData[i];
	}

	return -1;
}

void CLogViewerDlg::ChangeHighlightedItem(int nNewItemIndex)
{
	if (m_vecHitedLine.size() == 0)
	{
		SetDlgItemText(IDC_STATIC_SEARCH_RESULT, "0/0");
	}
	
	if (nNewItemIndex < 0 || nNewItemIndex >= m_vecHitedLine.size())
	{
		return;
	}

	if (m_nCurrentHighlightedItemIndex >= 0 && m_nCurrentHighlightedItemIndex < m_vecHitedLine.size())
	{
		LogStatus* pLogStatus = (LogStatus*)m_pLogList->GetItemData(m_vecHitedLine[m_nCurrentHighlightedItemIndex]);
		pLogStatus->bHighlighted = FALSE;
		m_pLogList->RedrawItems(m_vecHitedLine[m_nCurrentHighlightedItemIndex], m_vecHitedLine[m_nCurrentHighlightedItemIndex]);
	}

	m_nCurrentHighlightedItemIndex = nNewItemIndex;
	LogStatus* pLogStatus = (LogStatus*)m_pLogList->GetItemData(m_vecHitedLine[m_nCurrentHighlightedItemIndex]);
	pLogStatus->bHighlighted = TRUE;
	CenterLogItem(m_vecHitedLine[m_nCurrentHighlightedItemIndex]);
	
	SetRawLogContent(m_vecHitedLine[m_nCurrentHighlightedItemIndex]);

	CString strResult;
	strResult.Format("%d/%d", m_nCurrentHighlightedItemIndex + 1, (int)m_vecHitedLine.size());
	SetDlgItemText(IDC_STATIC_SEARCH_RESULT, strResult);
}

void CLogViewerDlg::CenterLogItem(int nItem)
{
	m_pLogList->RedrawItems(nItem, nItem);

	CRect clientRect;
	m_pLogList->GetClientRect(&clientRect);

	CRect itemRect;
	m_pLogList->GetItemRect(nItem, &itemRect, LVIR_BOUNDS);

	// Calculate the scroll position to center the item
	int scrollPos = itemRect.top + (itemRect.Height() / 2) - (clientRect.Height() / 2);

	m_pLogList->Scroll(CSize(0, scrollPos));
}

void CLogViewerDlg::OnBnClickedButtonHighlightFirst()
{
	ChangeHighlightedItem(0);
}


void CLogViewerDlg::OnBnClickedButtonHighlightPrev()
{
	ChangeHighlightedItem(m_nCurrentHighlightedItemIndex - 1);
}


void CLogViewerDlg::OnBnClickedButtonHighlightNext()
{
	ChangeHighlightedItem(m_nCurrentHighlightedItemIndex + 1);
}


void CLogViewerDlg::OnBnClickedButtonHighlightLast()
{
	ChangeHighlightedItem(m_vecHitedLine.size() - 1);
}


void CLogViewerDlg::OnBnClickedButtonClear()
{
	m_comboLogSeverity.SetCurSel(4);
	OnButtonSelectAll();
	m_strErrorCode = "";
	m_strLogContains = "";
	m_strProcessId = "";
	m_strThreadId = "";
	m_bLatestConsoleStartupOnly = FALSE;
	UpdateData(FALSE);
}


void CLogViewerDlg::FilterLatestConsoleStartup()
{
	BOOL bFound = FALSE;
	int nStartFileIndex;
	int nStartLineIndex;
	for (nStartFileIndex = m_vecLogFile.size() - 1; nStartFileIndex >= 0; nStartFileIndex--)
	{
		for (nStartLineIndex = m_vecLogFile[nStartFileIndex].vecLog.size() - 1; nStartLineIndex >= 0; nStartLineIndex--)
		{
			if (m_vecLogFile[nStartFileIndex].vecLog[nStartLineIndex].strLogContent == "lcpacs, start to check Win10 system and kill the background instances.")
			{
				bFound = TRUE;
				break;
			}
		}

		if (bFound)
			break;
	}

	for (int i = nStartFileIndex; i < m_vecLogFile.size(); i++)
	{
		CString strShow = "Filtering log file " + m_vecLogFile[i].strLogFileName;
		m_pDlgWait->UpdateText(strShow, i == 0);

		int j = i == nStartFileIndex ? nStartLineIndex : 0;
		for (; j < m_vecLogFile[i].vecLog.size(); j++)
		{
			ProcessLog(m_vecLogFile[i].vecLog[j]);
		}
	}
}

void CLogViewerDlg::OnBnClickedCheckLatestConsoleStartup()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckUserActions()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckAcqEvent()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckDipCom()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckDipLog()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckDipBeamSenseCom()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckPocVita()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckScannerState()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnBnClickedCheckPerformance()
{
	OnButtonRefresh();
}


void CLogViewerDlg::OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	ShowAllLogFiles();
	*pResult = 0;
}


void CLogViewerDlg::OnBnClickedButtonSelectAllFile()
{
	int nCount = m_pLogFileList->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		m_pLogFileList->SetCheck(i, TRUE);
	}

	ShowLogFileCount();
}


void CLogViewerDlg::OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	ShowAllLogFiles();
	*pResult = 0;
}

void CLogViewerDlg::OnEnChangeEditStartHour()
{
	if (!m_bStarting)
	{
		ShowAllLogFiles();
	}
}


void CLogViewerDlg::OnEnChangeEditEndHour()
{
	if (!m_bStarting)
	{
		ShowAllLogFiles();
	}
}


void CLogViewerDlg::OnBnClickedButtonSelectNoneFile()
{
	int nCount = m_pLogFileList->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		m_pLogFileList->SetCheck(i, FALSE);
	}

	ShowLogFileCount();
}

void CLogViewerDlg::OnNMClickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->iItem >= 0 && pNMLV->iSubItem > 0)
	{
		UINT state = m_pLogFileList->GetItemState(pNMLV->iItem, LVIS_STATEIMAGEMASK);

		// Toggle checkbox state
		if (state == INDEXTOSTATEIMAGEMASK(1)) // Unchecked
		{
			m_pLogFileList->SetCheck(pNMLV->iItem, TRUE);
		}
		else if (state == INDEXTOSTATEIMAGEMASK(2)) // Checked
		{
			m_pLogFileList->SetCheck(pNMLV->iItem, FALSE);
		}
	}

	ShowLogFileCount();
	*pResult = 0;
}

CString CLogViewerDlg::FindSourceFilePath(const CString & strModuleId, const CString & strSourceFileName)
{
	for (int i = 0; i < m_vecModule.size(); i++)
	{
		if (m_vecModule[i].strModuleId == strModuleId)
		{
			CString strProjectFilePathName = m_logConfig.strSourceRoot + "\\" + m_vecModule[i].strProjectDir + "\\" + m_vecModule[i].strProjectFile;

			char sCurLine[1024];
			ifstream ifs(strProjectFilePathName);
			while (ifs.good())
			{
				ifs.getline(sCurLine, 1023);
				CStringEx strLine(sCurLine);
				if (strLine.Find(strSourceFileName) != -1)
				{
					CString strRelativePath = strLine.GetDelimitedField("\"", strSourceFileName, 0);
					strRelativePath = strRelativePath.Right(strRelativePath.GetLength() - 1);
					return m_logConfig.strSourceRoot + "\\" + m_vecModule[i].strProjectDir + "\\" + strRelativePath + strSourceFileName;
				}
			}

			AfxMessageBox("Cannot find source file " + strSourceFileName + " in project file " + strProjectFilePathName);
			return "";
		}
	}

	AfxMessageBox("Cannot find module with id " + strModuleId);
	return "";
}

void CLogViewerDlg::OnNMDblclkListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (pNMItemActivate->iSubItem == 0)
	{
		LVITEM lvItem;
		lvItem.mask = LVIF_IMAGE;
		lvItem.iItem = pNMItemActivate->iItem;
		lvItem.iSubItem = 0;

		m_pLogList->GetItem(&lvItem);

		int imageIndex = lvItem.iImage;

		if (m_comboLogSeverity.GetCurSel() == 4 - imageIndex)
			m_comboLogSeverity.SetCurSel(4);
		else
			m_comboLogSeverity.SetCurSel(4 - imageIndex);

		OnButtonRefresh();
	}
	else if (pNMItemActivate->iSubItem == 2 || pNMItemActivate->iSubItem == 9)
	{
		CString strValue = m_pLogList->GetItemText(pNMItemActivate->iItem, 9);

		OnButtonSelectNone();
		int nCount = m_pModuleList->GetItemCount();
		for (int i = 0; i < nCount; i++)
		{
			if (m_pModuleList->GetItemText(i, 1) == strValue)
			{
				m_pModuleList->SetCheck(i);
				break;
			}
		}

		OnButtonRefresh();
	}
	else if (pNMItemActivate->iSubItem == 5)
	{
		LogStatus* pLogStatus = (LogStatus*)m_pLogList->GetItemData(pNMItemActivate->iItem);
		LogFile logFile = m_vecLogFile[pLogStatus->nLogFileIndex];
		LogDetail logDetail = logFile.vecLog[pLogStatus->nLogContentIndex];

		CString strPara;
		strPara.Format("\"%s\" -n%d", logFile.strLogFileName, logDetail.nRawLogLineNumber);
		ShellExecute(NULL, "open", m_logConfig.strNotepadPathName, strPara, NULL, SW_SHOW);
	}
	else if (pNMItemActivate->iSubItem == 7 || pNMItemActivate->iSubItem == 8)
	{
		CString strSourceFileName = m_pLogList->GetItemText(pNMItemActivate->iItem, 7);
		CString strSourceFileLineNo = m_pLogList->GetItemText(pNMItemActivate->iItem, 8);
		CString strModuleId = m_pLogList->GetItemText(pNMItemActivate->iItem, 9);
		CString strSourceFilePath = FindSourceFilePath(strModuleId, strSourceFileName);

		if (!strSourceFilePath.IsEmpty())
		{
			CString strPara;
			strPara.Format("\"%s\" -n%s", strSourceFilePath, strSourceFileLineNo);
			ShellExecute(NULL, "open", m_logConfig.strNotepadPathName, strPara, NULL, SW_SHOW);			
		}
	}
	else
	{
		int nID = -1;
		CString strValue = m_pLogList->GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);
		if (pNMItemActivate->iSubItem == 3)
			nID = IDC_EDIT_PROCESS_ID;
		else if (pNMItemActivate->iSubItem == 4)
			nID = IDC_EDIT_THREAD_ID;
		else if (pNMItemActivate->iSubItem == 6)
			nID = IDC_EDIT_ERROR_CODE;

		if (nID != -1)
		{
			CString strOldValue;
			GetDlgItemText(nID, strOldValue);
			if (strValue == strOldValue)
				strValue = "";

			SetDlgItemText(nID, strValue);

			OnButtonRefresh();
		}
	}

	
	*pResult = 0;
}

void CLogViewerDlg::OnNMRDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString strFileName = m_pLogFileList->GetItemText(pNMItemActivate->iItem, 1);
	strFileName = m_strLogHome + "\\" + strFileName.Left(10) + "\\" + strFileName;
	ShellExecute(NULL, "open", strFileName, NULL, NULL, SW_SHOW);
	
	*pResult = 0;
}
