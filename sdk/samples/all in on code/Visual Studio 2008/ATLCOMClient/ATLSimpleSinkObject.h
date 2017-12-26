/****************************** Module Header ******************************\
* Module Name:  ATLSimpleSinkObject.h
* Project:      ATLCOMClient
* Copyright (c) Microsoft Corporation.
* 
* This file contains three different sink objects all of which handle the 
* FloatPropertyChanging event which is fired by the source COM object 
* "ATLDllCOMServer.SimpleObject".
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlcom.h>

// some CString constructors will be explicit
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlstr.h>
#pragma endregion


#pragma region Import the type library

#import "ATLDllCOMServer.dll" no_namespace named_guids
// [-or-]
//#import "libid:9B23EFED-A0C1-46B6-A903-218206447F3E" no_namespace named_guids

#pragma endregion


#define IDC_ATLSIMPLEOBJECTSTA			1001


/*!
* \brief
* CATLSimpleSinkObject1 implements a sink object by deriving from 
* IDispEventImpl but not specifying the type library as a template argument. 
* Instead the type library and default source interface for the object are 
* determined using AtlGetObjectSourceInterface(). A SINK_ENTRY() macro is 
* used for each event from each source interface which is to be handled.
*/
class CATLSimpleSinkObject1 : 
	public IDispEventImpl<IDC_ATLSIMPLEOBJECTSTA, CATLSimpleSinkObject1>
{
public:

	BEGIN_SINK_MAP(CATLSimpleSinkObject1)
		// Make sure the Event Handlers have __stdcall calling convention
		SINK_ENTRY(IDC_ATLSIMPLEOBJECTSTA, 1, OnFloatPropertyChanging)
	END_SINK_MAP()

public:

	// Event handler for the 'FloatPropertyChanging' event:
	// [id(1)] void FloatPropertyChanging([in] FLOAT NewValue, 
	//         [in,out] VARIANT_BOOL* Cancel);
	void __stdcall OnFloatPropertyChanging(float NewValue, VARIANT_BOOL* Cancel)
	{
		CString strMessage;
		strMessage.Format(_T("FloatProperty is being changed to %f"), NewValue);

		// OK or cancel the change of FloatProperty
		*Cancel = (IDCANCEL == MessageBox(NULL, strMessage,
			_T("ATLDllCOMServer!FloatPropertyChanging"), MB_OKCANCEL)) 
			? VARIANT_TRUE : VARIANT_FALSE;
	}
};


static _ATL_FUNC_INFO OnFloatPropertyChangingInfo = 
{CC_STDCALL, NULL, 2, {VT_BOOL | VT_BYREF, VT_R4}};

/*!
* \brief
* CATLSimpleSinkObject2 implements a sink object by deriving from 
* IDispEventSimpleImpl. In this case the type library is either not available 
* or its more efficient not to load the type library. The source interface 
* ID is specified as an template argument. A SINK_ENTRY_INFO() macro is used 
* for each event from each source interface which is to be handled. The last 
* parameter to the macro is the _ATL_FUNC_INFO structure which provides 
* information about the event (source interface method) since the type 
* library is not available.
*/
class CATLSimpleSinkObject2 : 
	public IDispEventSimpleImpl<IDC_ATLSIMPLEOBJECTSTA, CATLSimpleSinkObject2, 
	&DIID__ISimpleObjectEvents>
{
public:

	BEGIN_SINK_MAP(CATLSimpleSinkObject2)
		// Make sure the Event Handlers have __stdcall calling convention
		SINK_ENTRY_INFO(IDC_ATLSIMPLEOBJECTSTA, DIID__ISimpleObjectEvents, 
		1, OnFloatPropertyChanging, &OnFloatPropertyChangingInfo)
	END_SINK_MAP()

public:

	// Event handler for the 'FloatPropertyChanging' event:
	// [id(1)] void FloatPropertyChanging([in] FLOAT NewValue, 
	//         [in,out] VARIANT_BOOL* Cancel);
	void __stdcall OnFloatPropertyChanging(float NewValue, VARIANT_BOOL* Cancel)
	{
		CString strMessage;
		strMessage.Format(_T("FloatProperty is being changed to %f"), NewValue);

		// OK or cancel the change of FloatProperty
		*Cancel = (IDCANCEL == MessageBox(NULL, strMessage,
			_T("ATLDllCOMServer!FloatPropertyChanging"), MB_OKCANCEL)) 
			? VARIANT_TRUE : VARIANT_FALSE;
	}
};


/*!
* \brief
* CATLSimpleSinkObject3 is essentially same as CATLSimpleSinkObject2 except 
* instead of providing the _ATL_FUNC_INFO structure statically we can fill in 
* the structure at run time. This offers a little more flexibility and is a 
* trade off of speed over size.
*/
class CATLSimpleSinkObject3 : 
	public IDispEventSimpleImpl<IDC_ATLSIMPLEOBJECTSTA, CATLSimpleSinkObject3, 
	&DIID__ISimpleObjectEvents>
{
public:

	BEGIN_SINK_MAP(CATLSimpleSinkObject3)
		// Make sure the Event Handlers have __stdcall calling convention
		SINK_ENTRY_EX(IDC_ATLSIMPLEOBJECTSTA, DIID__ISimpleObjectEvents, 
		1, OnFloatPropertyChanging)
		// [-or-]
		/*SINK_ENTRY_INFO(IDC_ATLSIMPLEOBJECTSTA, DIID__ISimpleObjectEvents, 
		1, OnFloatPropertyChanging, NULL)*/
	END_SINK_MAP()

public:

	// Fill in the _ATL_FUNC_INFO structure depending on DISPID
	HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, 
		_ATL_FUNC_INFO& info)
	{
		if (InlineIsEqualGUID(iid, DIID__ISimpleObjectEvents))
		{
			// Fill in _ATL_FUNC_INFO with attributes of the 
			// 'FloatPropertyChanging' event
			info.cc = CC_STDCALL;
			switch(dispidMember)
			{
				case 1:
					info.vtReturn = NULL;
					info.nParams = 2;
					info.pVarTypes[1] = VT_R4;
					info.pVarTypes[0] = VT_BOOL | VT_BYREF;
					return S_OK;

				default:
					return E_FAIL;
			}
		}
		return E_FAIL;
	}

	// Event handler for the 'FloatPropertyChanging' event:
	// [id(1)] void FloatPropertyChanging([in] FLOAT NewValue, 
	//         [in,out] VARIANT_BOOL* Cancel);
	void __stdcall OnFloatPropertyChanging(float NewValue, VARIANT_BOOL* Cancel)
	{
		CString strMessage;
		strMessage.Format(_T("FloatProperty is being changed to %f"), NewValue);

		// OK or cancel the change of FloatProperty
		*Cancel = (IDCANCEL == MessageBox(NULL, strMessage,
			_T("ATLDllCOMServer!FloatPropertyChanging"), MB_OKCANCEL)) 
			? VARIANT_TRUE : VARIANT_FALSE;
	}
};