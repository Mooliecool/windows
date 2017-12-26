
// MFCRestartManagerSupportView.cpp : implementation of the CMFCRestartManagerSupportView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCRestartManagerSupport.h"
#endif

#include "MFCRestartManagerSupportDoc.h"
#include "MFCRestartManagerSupportView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCRestartManagerSupportView

IMPLEMENT_DYNCREATE(CMFCRestartManagerSupportView, CView)

BEGIN_MESSAGE_MAP(CMFCRestartManagerSupportView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCRestartManagerSupportView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMFCRestartManagerSupportView construction/destruction

CMFCRestartManagerSupportView::CMFCRestartManagerSupportView()
{
	// TODO: add construction code here

}

CMFCRestartManagerSupportView::~CMFCRestartManagerSupportView()
{
}

BOOL CMFCRestartManagerSupportView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMFCRestartManagerSupportView drawing

void CMFCRestartManagerSupportView::OnDraw(CDC* /*pDC*/)
{
	CMFCRestartManagerSupportDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CMFCRestartManagerSupportView printing


void CMFCRestartManagerSupportView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCRestartManagerSupportView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCRestartManagerSupportView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCRestartManagerSupportView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMFCRestartManagerSupportView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCRestartManagerSupportView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCRestartManagerSupportView diagnostics

#ifdef _DEBUG
void CMFCRestartManagerSupportView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCRestartManagerSupportView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCRestartManagerSupportDoc* CMFCRestartManagerSupportView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCRestartManagerSupportDoc)));
	return (CMFCRestartManagerSupportDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCRestartManagerSupportView message handlers
