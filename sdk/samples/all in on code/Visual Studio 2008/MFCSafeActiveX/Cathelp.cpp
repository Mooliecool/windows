/****************************** Module Header ******************************\
* Module Name:  Cathelp.cpp
* Project:      MFCSafeActiveX
* Copyright (c) Microsoft Corporation.
* 
* The implementation of the set of helper functions to register (and 
* unregister) a component to be safe for scripting and initialization. 
* 
* See: http://support.microsoft.com/kb/161873
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
#include "Cathelp.h"

#include <comcat.h>
#include <strsafe.h>


/*!
 * \brief
 * Used to register ActiveX control as safe.
 * 
 * \param catid
 * Category Id.
 * 
 * \param catDescription
 * Category description. It should not be too long. We only copy the first 
 * 127 characters if it is.
 * 
 * \returns
 * The HRESULT of the operation.
 */
HRESULT CreateComponentCategory(CATID catid, WCHAR *catDescription)
{
	ICatRegister *pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		return hr;

	// Make sure the HKCR\Component Categories\{..catid...} key is registered.
	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409 ; // english
	size_t len;
	// Make sure that the provided description is not too long. Only copy the 
	// first 127 characters if it is. The second parameter of StringCchLength 
	// is the maximum number of characters that may be read into 
	// catDescription. There must be room for a NULL-terminator. The third 
	// parameter is the number of characters excluding the NULL-terminator.
	hr = StringCchLength(catDescription, STRSAFE_MAX_CCH, &len);
	if (SUCCEEDED(hr))
	{
		if (len>127)
		{
			len = 127;
		}
	}   
	else
	{
		// TODO: Write an error handler;
	}
	// The second parameter of StringCchCopy is 128 because you need 
	// room for a NULL-terminator.
	hr = StringCchCopy(catinfo.szDescription, len + 1, catDescription);
	// Make sure the description is null terminated.
	catinfo.szDescription[len + 1] = '\0';

	hr = pcr->RegisterCategories(1, &catinfo);
	pcr->Release();

	return hr;
}


/*!
 * \brief
 * Register your component categories information.
 * 
 * \param clsid
 * The CLSID of the component to be registered.
 * 
 * \param catid
 * Category Id.
 * 
 * \returns
 * The HRESULT of the operation.
 */
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	// Register your component categories information.
	ICatRegister *pcr = NULL ;
	HRESULT hr = S_OK ;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Register this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}


/*!
 * \brief
 * Remove entries from the registry.
 * 
 * \param clsid
 * The CLSID of the component to be unregistered.
 * 
 * \param catid
 * Category Id.
 * 
 * \returns
 * The HRESULT of the operation.
 */
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	ICatRegister *pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Unregister this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}