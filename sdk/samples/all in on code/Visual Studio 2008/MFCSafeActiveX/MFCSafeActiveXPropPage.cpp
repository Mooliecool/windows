#include "stdafx.h"
#include "MFCSafeActiveX.h"
#include "MFCSafeActiveXPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMFCSafeActiveXPropPage, COlePropertyPage)


// Message map
BEGIN_MESSAGE_MAP(CMFCSafeActiveXPropPage, COlePropertyPage)
END_MESSAGE_MAP()


// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX(CMFCSafeActiveXPropPage, "MFCSAFEACTIVEX.MFCSafeActiveXPropPage.1",
	0xc8f3d57c, 0x42eb, 0x41f8, 0xb1, 0x2b, 0x56, 0xe5, 0x9e, 0xd0, 0x52, 0x43)


// CMFCSafeActiveXPropPage::CMFCSafeActiveXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CMFCSafeActiveXPropPage
BOOL CMFCSafeActiveXPropPage::CMFCSafeActiveXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MFCSAFEACTIVEX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


// CMFCSafeActiveXPropPage::CMFCSafeActiveXPropPage - Constructor
CMFCSafeActiveXPropPage::CMFCSafeActiveXPropPage() :
	COlePropertyPage(IDD, IDS_MFCSAFEACTIVEX_PPG_CAPTION)
{
}


// CMFCSafeActiveXPropPage::DoDataExchange - Moves data between page and properties
void CMFCSafeActiveXPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}


// CMFCSafeActiveXPropPage message handlers