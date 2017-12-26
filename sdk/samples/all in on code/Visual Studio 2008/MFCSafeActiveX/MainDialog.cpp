#include "stdafx.h"
#include "MFCSafeActiveX.h"
#include "MFCSafeActiveXCtrl.h"
#include "MainDialog.h"


IMPLEMENT_DYNAMIC(CMainDialog, CDialog)

CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/) : CDialog(CMainDialog::IDD, pParent)
{
}

CMainDialog::~CMainDialog()
{
}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSGBOX_EDIT, m_EditMessage);
	DDX_Control(pDX, IDC_SCRIPT_EDIT, m_EditScript);
	DDX_Control(pDX, IDC_FLOATPROP_STATIC, m_StaticFloatProperty);
}


BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	ON_BN_CLICKED(IDC_MSGBOX_BN, &CMainDialog::OnBnClickedMsgBoxBn)
	ON_BN_CLICKED(IDC_SCRIPT_BN, &CMainDialog::OnBnClickedScriptBn)
END_MESSAGE_MAP()


// CMainDialog message handlers

void CMainDialog::OnBnClickedMsgBoxBn()
{
	CString strMessage;
	m_EditMessage.GetWindowText(strMessage);
	MessageBox(strMessage, TEXT("HelloWorld"), MB_ICONINFORMATION | MB_OK);
}

void CMainDialog::OnBnClickedScriptBn()
{
	// m_pParentWnd should point to the ActiveX control
	ASSERT(this->m_pParentWnd);

	// Get the script to call
	TCHAR szScript[256];
	m_EditScript.GetWindowText(szScript, 256);

	// Call the HTML script through the control
	((CMFCSafeActiveXCtrl*)m_pParentWnd)->CallHtmlScript(szScript);
}