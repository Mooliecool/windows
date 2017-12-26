// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>

#include <specstrings.h>
#include "culture.h"
#include "culturedata.h"

#ifndef LOCALE_CUSTOM_DEFAULT
#define LOCALE_CUSTOM_DEFAULT 0x0c00
#endif

////////////////////////////////////////////////////////////////////////////
//
//    Compare LangID with the LangID in a CultureDataType item.
//    Parameters:
//        arg1: pointer to a LangID
//        arg2: pointer to a CultureDataType item.
//
////////////////////////////////////////////////////////////////////////////

int __cdecl CompareLangIDs(const void* *arg1, const void *arg2)
{
    LANGID langID = *((LANGID*)arg1);
    CultureDataType* item = (CultureDataType*)arg2;
    if (langID == item->langID)
        return 0;
    if (langID >  item->langID)
        return 1;
    return -1;
}



//
//    Convert 16-bit language ID from Windows to RFC 1766 culture name, such as "en-US" for US English.
//    
//    Parameters:
//        [IN] langID: The 16-bit language ID.  E.g. it is 0x0409 for US English
//        [OUT] pwzCultureName: The output buffer for the culture name.  If this is NULL and pcchCultureName is not NULL, the WCHAR count needed by
//            pwzCultureName will be put in *pcchCultureName.  
//        [IN/OUT] pcchCultureName: The pointer to the size of output buffer.  It will contain the real WCHAR count copied if the function succeeds.
//            This value can not be null.
//        [OUT] pwzParentName: The output buffer for the parent culture name.  
//            A parent culture is the fallback culture to search for the resource.  For example, the parent culture name for "en-US" is "en".
//            If this is NULL and pcchCultureName is not NULL, the WCHAR count needed by
//            pcchParentName will be put in *pcchCultureName.
//        [IN/OUT] pcchParentName: The pointer to the size of output buffer for parent culture.  It will contain the real WCHAR count copied if the function succeeds.
//            This value and pwzParentName can both be null so that the information for parent culture is not retrieved.
//    Returns:
//        TRUE if the language ID is converted to culture name and/or parent culture name successfully.  Otherwise, FALSE is returned.
//        If the output buffer is not enough, GetLastError() will return ERROR_INSUFFICIENT_BUFFER.  
//        If the languaged ID is not valid, GetLastError() will return ERROR_INVALID_PARAMETER.
//        If the pcchCultureName is NULL, but pwzCultureName is not NULL, FALSE is returned, and GetLastError() will return ERROR_INVALID_PARAMETER.
//        If the pcchParentName is NULL, but pwzParentName is not NULL, FALSE is returned, and GetLastError() will return ERROR_INVALID_PARAMETER.
//        

BOOL ConvertLangIdToCultureName(                                   LANGID langID, 
                                __out_ecount_opt(*pcchCultureName) LPWSTR pwzCultureName, 
                                                                   SIZE_T* pcchCultureName, 
                                __out_ecount_opt(*pcchParentName)  LPWSTR pwzParentName, 
                                                                   SIZE_T *pcchParentName) 
{
    const CultureDataType* pDataItem = (CultureDataType*) bsearch(&langID, g_cultureData, sizeof(g_cultureData) / sizeof(g_cultureData[0]), 
                                sizeof(g_cultureData[0]), (int (__cdecl *)(const void*, const void*))CompareLangIDs);
                                
    if (pDataItem == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (FALSE);
    }

    BOOL bResult = TRUE;
    SIZE_T size; 
    
    if (pcchCultureName != NULL) {
        size = wcslen(pDataItem->cultureName) + 1;
        
        if (pwzCultureName != NULL) {
            if ((*pcchCultureName < size) ||
                (FAILED(StringCchCopyNW(pwzCultureName, *pcchCultureName,
                                        pDataItem->cultureName, size))))
            {
                bResult = FALSE;
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
            }
        }
        // Return the size copied or needed in WCHAR.
        *pcchCultureName = size;            
    } else
    {
        if ((pwzCultureName != NULL) ||
            (pwzParentName == NULL) && (pcchParentName == NULL)) 
        {
            bResult = FALSE;
            SetLastError(ERROR_INVALID_PARAMETER);
        }
    }

    if (bResult) 
    {
        if (pcchParentName != NULL)
        {
            pDataItem = &(g_cultureData[pDataItem->parentItem]);
            size = wcslen(pDataItem->cultureName) + 1;
            if (pwzParentName != NULL) {
                if ((*pcchParentName < size) ||
                    (FAILED(StringCchCopyNW(pwzParentName, *pcchParentName, 
                                            pDataItem->cultureName, size))))
                {
                    bResult = FALSE;
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                }
            }
            // Return the size copied in WCHAR or needed in WCHAR.
            *pcchParentName = size;
        } else if (pwzParentName != NULL) 
        {
            bResult = FALSE;
            SetLastError(ERROR_INVALID_PARAMETER);
        }
    }        
    return (bResult);
}

//
//    Entry point for DLL load/unload
//
BOOL WINAPI DllMain(HANDLE hInstDLL,
                    DWORD   dwReason,
                    LPVOID  lpvReserved)
{
    BOOL    fReturn = TRUE;
    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return fReturn;
}
