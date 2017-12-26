/****************************** Module Header ******************************\
* Module Name:  CommandHandler.h
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
#include "stdafx.h"

class CommandHandler : public IUICommandHandler
{
public:
	CommandHandler(void);
	~CommandHandler(void);

	// Standard COM (IUnknown) methods.
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());
	STDMETHOD(QueryInterface(REFIID iid, void** ppv));

    // IUICommandHandler methods
    STDMETHOD(UpdateProperty)(UINT nCmdID,
        __in REFPROPERTYKEY key,
        __in_opt const PROPVARIANT* ppropvarCurrentValue,
        __out PROPVARIANT* ppropvarNewValue);

    STDMETHOD(Execute)(UINT nCmdID,
        UI_EXECUTIONVERB verb, 
        __in_opt const PROPERTYKEY* key,
        __in_opt const PROPVARIANT* ppropvarValue,
        __in_opt IUISimplePropertySet* pCommandExecutionProperties);

private:
	LONG m_cRef;                            // Reference count.
};

