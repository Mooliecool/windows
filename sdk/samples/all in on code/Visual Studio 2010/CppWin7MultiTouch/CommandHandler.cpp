/****************************** Module Header ******************************\
* Module Name:  CommandHandler.cpp
* Project:      CppWin7MultiTouch
* Copyright (c) Microsoft Corporation.
* 
* A class implemeting IUICommandHandler. Handles the ribbon commands.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once
#include "StdAfx.h"
#include "CommandHandler.h"
#include "MainWindow.h"

extern MainWindow *g_pMainWindow;

CommandHandler::CommandHandler(void)
{
}

CommandHandler::~CommandHandler(void)
{
}

// Standard COM (IUnknown) method implementations.
STDMETHODIMP_(ULONG) CommandHandler::AddRef()
{
    return InterlockedIncrement(&this->m_cRef);
}

STDMETHODIMP_(ULONG) CommandHandler::Release()
{
    LONG cRef = InterlockedDecrement(&this->m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

STDMETHODIMP CommandHandler::QueryInterface(REFIID iid, void** ppv)
{
    if (iid == __uuidof(IUnknown))
    {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (iid == __uuidof(IUICommandHandler))
    {
        *ppv = static_cast<IUICommandHandler*>(this);
    }
    else 
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    this->AddRef();
    return S_OK;
}

// IUICommandHandler methods
STDMETHODIMP CommandHandler::UpdateProperty(
    UINT nCmdID,
    __in REFPROPERTYKEY key,
    __in_opt const PROPVARIANT* ppropvarCurrentValue,
    __out PROPVARIANT* ppropvarNewValue)
{
    return E_NOTIMPL;
}

// The main method that executes all commands.
STDMETHODIMP CommandHandler::Execute(
    UINT nCmdID,
    UI_EXECUTIONVERB verb,
    __in_opt const PROPERTYKEY* key,
    __in_opt const PROPVARIANT* ppropvarValue,
    __in_opt IUISimplePropertySet* pCommandExecutionProperties)
{
	switch (nCmdID)
	{
	case cmdOpenImage:
		g_pMainWindow->m_pD2DWindow->OpenImage();
		break;
	case cmdTranslateLeft:
		g_pMainWindow->m_pD2DWindow->Translate(-100, 0);
		break;
	case cmdTranslateRight:
		g_pMainWindow->m_pD2DWindow->Translate(100, 0);
		break;
	case cmdTranslateUp:
		g_pMainWindow->m_pD2DWindow->Translate(0, -100);
		break;
	case cmdTranslateDown:
		g_pMainWindow->m_pD2DWindow->Translate(0, 100);
		break;
	case cmdScaleDown:
		g_pMainWindow->m_pD2DWindow->Scale(0.9);
		break;
	case cmdScaleUp:
		g_pMainWindow->m_pD2DWindow->Scale(1.1);
		break;
	case cmdRotateLeft:
		g_pMainWindow->m_pD2DWindow->Rotate(-90);
		break;
	case cmdRotateRight:
		g_pMainWindow->m_pD2DWindow->Rotate(90);
		break;
	default:
		break;
	}
    return S_OK;
}
