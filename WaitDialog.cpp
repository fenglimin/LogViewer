// WaitDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WaitDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitDialog dialog


CWaitDialog::CWaitDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	Create ( IDD_DIALOG_WAIT );
}

CWaitDialog::~CWaitDialog()
{
}

void CWaitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitDialog)
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitDialog, CDialog)
	//{{AFX_MSG_MAP(CWaitDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitDialog message handlers

void CWaitDialog::Show()
{
	CenterWindow();
	ShowWindow ( SW_SHOW );
	UpdateWindow ();
}

void CWaitDialog::UpdateText(CString strShowContent, BOOL bUpdateSize)
{
	SetDlgItemText(IDC_STATIC_SHOW, strShowContent);

	if (bUpdateSize)
	{
		CPaintDC dc(this);
		CSize Size = dc.GetTextExtent(strShowContent);
		SetWindowPos(NULL, 0, 0, Size.cx + 70, 60, SWP_NOZORDER);

		Show();
	}
}

void CWaitDialog::Close()
{
	ShowWindow(SW_HIDE);
}

