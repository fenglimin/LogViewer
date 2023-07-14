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
	m_nEndHour = 0;
	m_nStartHour = 0;
	m_bExclude = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	DDX_Check(pDX, IDC_CHECK_CURRENT_HOUR, m_bCurrentHour);
	DDX_Check(pDX, IDC_CHECK_TODAY, m_bToday);
	DDX_Text(pDX, IDC_EDIT_END_HOUR, m_nEndHour);
	DDV_MinMaxInt(pDX, m_nEndHour, 0, 23);
	DDX_Text(pDX, IDC_EDIT_START_HOUR, m_nStartHour);
	DDV_MinMaxInt(pDX, m_nStartHour, 0, 23);
	DDX_Check(pDX, IDC_CHECK_EXCLUDE, m_bExclude);
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
	
	InitLogList();
	InitModuleList();
	InitFilter();

	GetLogDir();

	OnButtonSelectAll();
	
	//OnBnClickedButtonReload();
	
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
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
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
	m_pLogList->InsertColumn(nCol++, "Raw Log", LVCFMT_LEFT, 0);

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
		m_nErrorCount++;
	else if (logDetail.nLogSeverity == 2)
		m_nWarningCount++;

	int nSubItem = 1;
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strDateTime);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strModuleName);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strProcessId);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strThreadId);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strLogContent);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strCode);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strSourceFileName);
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strLineNumber);
	m_pLogList->SetItemText(nItem, nSubItem++, CStringEx(logDetail.nModuleNumber));
	m_pLogList->SetItemText(nItem, nSubItem++, logDetail.strRawLog);
}

BOOL CLogViewerDlg::LoadLogFile(char* strLogFile, CString strDate, BOOL bUpdateSize)
{
	char sCurLine[1024];
	char sNextLine[1024];

	ifstream ifs(strLogFile);
	if ( !ifs.good())
	{
//		AfxMessageBox("Error reading log file " + CString(strLogFile));
		return FALSE;
	}

	
	CString strShow = "Loading log file ";
	strShow += strLogFile;
	m_dlgWait.UpdateText( strShow, bUpdateSize);
	
	LogFile logFile;

	logFile.strLogFileName = strLogFile;

	ifs.getline(sCurLine, 1023);
	CStringEx strCurLine = sCurLine;
	
	while (ifs.good())
	{
		ifs.getline(sNextLine, 1023);
		CString strNextLine = sNextLine;
		if ( strNextLine.Left(10) != strDate )
		{
			strCurLine += strNextLine;
			continue;
		}
		
		if ( strCurLine == "")
			continue;

		CStringEx strSave = strCurLine;
		strCurLine = strNextLine;

		//SetWindowText(strSave);
		LogDetail logDetail;

		logDetail.strRawLog = strSave;
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
		logDetail.strLineNumber = strSave.GetField("`", 8);

		logFile.vecLog.push_back(logDetail);

		if (FilterLog(logDetail))
		{
			InsertLog(logDetail);
		}
	}

	ifs.close();

	m_vecLogFile.push_back(logFile);
	return TRUE;
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

	return TRUE;
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

	AddModule("11", "SMS Server");
	AddModule("12", "Print Server");
	AddModule("14", "Worklist Panel");
	AddModule("16", "Service Manager");
	AddModule("30", "Task Manager");
	AddModule("33", "Oam Server");
	AddModule("34", "OAM Panel");
	AddModule("35", "Rule Server");
	AddModule("36", "Eclipse");
	AddModule("47", "PDC Sender Server");
	AddModule("52", "SSCU");
	AddModule("53", "SSCP Server");
	AddModule("56", "Acq Server");
	AddModule("59", "Multi Recordset");
	AddModule("65", "Framework");
	AddModule("66", "Audit Server");
	AddModule("77", "MWL Server");	
	AddModule("93", "Device Manager");	
	AddModule("94", "Acq Panel");	
}

void CLogViewerDlg::OnCheckToday() 
{
	m_bToday = !m_bToday;
	EnableDateControl(!m_bToday);

	if ( !m_bToday )
	{
		COleDateTime dt = COleDateTime::GetCurrentTime();
		m_tStartDay.SetDate(dt.GetYear(), dt.GetMonth(), dt.GetDay()-1);

		m_bCurrentHour = FALSE;
		m_nStartHour = 0;
		m_nEndHour = 23;
		UpdateData(FALSE);
	}
	
}

void CLogViewerDlg::OnCheckCurrentHour() 
{
	m_bCurrentHour = !m_bCurrentHour;
	EnableHourControl(!m_bCurrentHour);

	if ( !m_bCurrentHour )
	{
		COleDateTime dt = COleDateTime::GetCurrentTime();
		int nHour = dt.GetHour();

		m_nEndHour= nHour;
		if ( nHour > 0 )
			nHour--;
		m_nStartHour = nHour;
		
		UpdateData(FALSE);
	}

	
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

void CLogViewerDlg::AddModule(CString strModuleNumber, CString strModuleName)
{
	int nPos = m_pModuleList->InsertItem(m_pModuleList->GetItemCount(), "");
	m_pModuleList->SetItemText(nPos, 1, strModuleNumber);
	m_pModuleList->SetItemText(nPos, 2, strModuleName);
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
		CString strLogFile = m_strLogHome + "\\" + strDate + "\\" + strDate + "-" + strHour + ".log";
		LoadLogFile((LPTSTR)(LPCTSTR)strLogFile, strDate, bUpdateSize);
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

			
			CString strLogFile = m_strLogHome + "\\" + strDate + "\\" + strDate + "-" + strHour + ".log";
			LoadLogFile((LPTSTR)(LPCTSTR)strLogFile, strDate, i == m_nStartHour && bUpdateSize);
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
	BeforeLoad();

	for (int i = 0; i < m_vecLogFile.size(); i++)
	{
		CString strShow = "Filtering log file " + m_vecLogFile[i].strLogFileName;
		m_dlgWait.UpdateText(strShow,  i == 0);
		
		for (int j = 0; j < m_vecLogFile[i].vecLog.size(); j++)
		{
			if (FilterLog(m_vecLogFile[i].vecLog[j]))
			{
				InsertLog(m_vecLogFile[i].vecLog[j]);
			}
		}
	}

	AfterLoad();
}

void CLogViewerDlg::OnBnClickedButtonReload()
{
	BeforeLoad();
	m_vecLogFile.clear();
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

	AfterLoad();
}


void CLogViewerDlg::OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CString strRawLog = m_pLogList->GetItemText(pNMLV->iItem, 10);
	SetDlgItemText(IDC_EDIT_RAW_LOG, strRawLog);
	
	*pResult = 0;
}

void CLogViewerDlg::BeforeLoad()
{
	m_nErrorCount = 0;
	m_nWarningCount = 0;
	m_dtNow = COleDateTime::GetCurrentTime();
	m_dlgWait.Show();
	UpdateData();

	m_strLogContains.MakeUpper();

	m_pLogList->SetRedraw(FALSE);// LockWindowUpdate();


	if (GetSelectedModules() == 0 && !m_bExclude)
	{
		AfxMessageBox("Please select at least one Module!");
		return;
	}

	m_nSeverity = 4 - m_comboLogSeverity.GetCurSel();
	m_pLogList->DeleteAllItems();	
}


void CLogViewerDlg::AfterLoad()
{
	m_dlgWait.Close();
	m_pLogList->SetRedraw(TRUE);//UnlockWindowUpdate();

	COleDateTimeSpan timeSpan = COleDateTime::GetCurrentTime() - m_dtNow;
	
	CString strTitle;
	strTitle.Format("LogViewer - Total %d logs, %d Error logs, %d Warning logs, takes %d seconds.", m_pLogList->GetItemCount(), m_nErrorCount, m_nWarningCount, (int)timeSpan.GetTotalSeconds());
	SetWindowText(strTitle);
}