#pragma once
#include "afxwin.h"


class CMainDialog : public CDialog
{
	DECLARE_DYNAMIC(CMainDialog)

public:
	CMainDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDialog();

// Dialog Data
	enum { IDD = IDD_MAINDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMsgBoxBn();
	CEdit m_EditMessage;
	CStatic m_StaticFloatProperty;
	afx_msg void OnBnClickedScriptBn();
	CEdit m_EditScript;
};