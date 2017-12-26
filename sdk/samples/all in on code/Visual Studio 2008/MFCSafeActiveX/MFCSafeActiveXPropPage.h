#pragma once


class CMFCSafeActiveXPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMFCSafeActiveXPropPage)
	DECLARE_OLECREATE_EX(CMFCSafeActiveXPropPage)

// Constructor
public:
	CMFCSafeActiveXPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MFCSAFEACTIVEX };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};