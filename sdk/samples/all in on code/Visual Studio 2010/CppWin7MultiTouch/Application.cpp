/****************************** Module Header ******************************\
* Module Name:  Application.cpp
* Project:      CppWin7MultiTouch
* Copyright (c) Microsoft Corporation.
* 
* A class implemeting IUIApplication. The ribbon framework will callback into this class.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "StdAfx.h"
#include "Application.h"
#include "MainWindow.h"

extern MainWindow *g_pMainWindow;

UINT g_RibbonHeight = 0;

// Standard COM (IUnknown) method implementations.
STDMETHODIMP_(ULONG) CApplication::AddRef()
{
    return InterlockedIncrement(&this->m_cRef);
}

STDMETHODIMP_(ULONG) CApplication::Release()
{
    LONG cRef = InterlockedDecrement(&this->m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

STDMETHODIMP CApplication::QueryInterface(REFIID iid, void** ppv)
{
    if (iid == __uuidof(IUnknown))
    {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (iid == __uuidof(IUIApplication))
    {
        *ppv = static_cast<IUIApplication*>(this);
    }
    else 
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    this->AddRef();
    return S_OK;
}

// Called by the ribbon framework for each command specified in markup,
// to allow the host application to bind a command handler to that command.
STDMETHODIMP CApplication::OnCreateUICommand(
    UINT nCmdID,
    __in UI_COMMANDTYPE typeID,
    __deref_out IUICommandHandler** ppCommandHandler)
{
	if (this->m_pCommandHandler == NULL)
	{
		this->m_pCommandHandler = static_cast<IUICommandHandler*>(new CommandHandler());
	}
	HRESULT hr = this->m_pCommandHandler->QueryInterface(IID_PPV_ARGS(ppCommandHandler));
	return hr;
}

// Called when the state of the ribbon changes, for example, created, destroyed, or resized.
STDMETHODIMP CApplication::OnViewChanged(
    UINT viewId,
    __in UI_VIEWTYPE typeId,
    __in IUnknown* pView,
    UI_VIEWVERB verb,
    INT uReasonCode)
{
    HRESULT hr = E_NOTIMPL;

	if (UI_VIEWTYPE_RIBBON == typeId)
    {
        switch (verb)
        {           
        case UI_VIEWVERB_CREATE:
        case UI_VIEWVERB_SIZE:
            {
                IUIRibbon* pRibbon = NULL;
                hr = pView->QueryInterface(IID_PPV_ARGS(&pRibbon));
                if (SUCCEEDED(hr))
                {
                    // Call to the framework to determine the desired height of the ribbon.
                    hr = pRibbon->GetHeight(&g_RibbonHeight);
					// Resize the Direct2D window if it has already been created.
					if (g_pMainWindow != NULL)
					{
						if (g_pMainWindow->m_pD2DWindow != NULL)
						{
							g_pMainWindow->m_pD2DWindow->OnResize();
						}
					}
                    pRibbon->Release();
                }
            }
            break;
        case UI_VIEWVERB_DESTROY:
            hr = S_OK;
            break;
        }
    }
    return hr;
}

// Called by the ribbon framework for each command at the time of ribbon destruction.
STDMETHODIMP CApplication::OnDestroyUICommand(
    UINT32 nCmdID,
    __in UI_COMMANDTYPE typeID,
    __in_opt IUICommandHandler* commandHandler)
{
    return E_NOTIMPL;
}
