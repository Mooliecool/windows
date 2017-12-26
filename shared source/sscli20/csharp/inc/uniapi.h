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

// UniApi.h - Unicode API wrappers

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _UNIAPI_H_
#define _UNIAPI_H_

#ifndef UNIAPI_NOATL
#include "atlbase.h"
#endif

//---------------------------------------------------------------
// Initialization
BOOL    WINAPI W_IsUnicodeSystem();

//---------------------------------------------------------------
// API Wrappers
//---------------------------------------------------------------

#define W_FindFirstFile FindFirstFileW
#define W_FindNextFile          FindNextFileW
#define W_GetFileAttributes     GetFileAttributesW
#define W_SetFileAttributes     SetFileAttributesW
#define W_GetFullPathName       GetFullPathNameW
#define W_CreateFile            CreateFileW
#define W_GetCurrentDirectory   GetCurrentDirectoryW
#define W_GetTempPath           GetTempPathW
#define W_GetTempFileName       GetTempFileNameW
#define W_DeleteFile            DeleteFileW
#define W_GetWindowTextLength   GetWindowTextLengthW
#define W_GetWindowText         GetWindowTextW
#define W_RegisterClass         RegisterClassW
#define W_CreateWindowEx        CreateWindowExW
#define W_CreateWindow(C,W,S,x,y,w,h,p,m,i,L) CreateWindowExW(0L,C,W,S,x,y,w,h,p,m,i,L)
#define W_CreateDialogParam     CreateDialogParamW
#define W_CreateDialog(i,t,p,f) CreateDialogParamW(i,t,p,f,NULL)
#define W_CreateDialogIndirect(i,t,p,f) CreateDialogIndirectParamW(i,t,p,f,NULL)
#define W_CopyFile(s,d,f)       CopyFileW(s,d,f)


//---------------------------------------------------------------
// Additional utility functions
//---------------------------------------------------------------

// These resource string load funcs never convert the string, even on Win9x
// -- you get resource data verbatim.
int     WINAPI W_LoadString   (HINSTANCE hinst, UINT id, __out_ecount(cch) PWSTR psz, int cch);
BSTR    WINAPI W_LoadBSTR     (HINSTANCE hInst, UINT id); // Free with SysFreeString
PWSTR   WINAPI W_LoadVSString (HINSTANCE hInst, UINT id); // Free with VSFree


#if !defined(NUMBER_OF)
#define NUMBER_OF(x) (sizeof(x) / sizeof((x)[0]))
#endif // !defined(NUMBER_OF)

// Returns length-prefixed non-null-terminated resource image for string
PCWSTR  WINAPI W_GetStringResourceImage (HINSTANCE hinst, UINT id);

// file/directory functions
int WINAPI W_Access(PCWSTR pPathName, int mode);                // _wchdir/_chdir
int WINAPI W_Rename(PCWSTR pOldPathName, PCWSTR pNewPathName);  // _wrename/_rename
int WINAPI W_Remove(PCWSTR pPathName);                          // _wremove/_remove
int WINAPI W_ChDir(PCWSTR pPathName);                           // _wchdir/_chdir
int WINAPI W_MkDir(PCWSTR pPathName);                           // _wmkdir/_mkdir
int WINAPI W_RmDir(PCWSTR pPathName);                           // _wrmdir/_rmdir

// Walk a file's path and get its actual letter case; also expand all short names
// to long names.  Returns non-zero on success, 0 on failure (output buffer too small
// to hold the result, error accessing the path, or out of memory).  In case of failure 
// the contents of the pszOut buffer are undefined.  cchOut specifies the size of the 
// pszOut buffer in characters.
BOOL    WINAPI W_GetActualFileCase(__in_z PCWSTR pszName, __out_ecount(cchOut) PWSTR pszOut, DWORD cchOut);

//---------------------------------------------------------
void inline W_ReplaceChar(__inout_opt __inout_z PWSTR psz, WCHAR chOld, WCHAR chNew)
{
    if (psz)
    {
        WCHAR ch;
        while (0 != (ch = *psz))
        {
            if (ch == chOld)
                *psz = chNew;
            psz++;
        }
    }
}

//--------------

#ifndef UNIAPI_NOATL
void inline W_AddTrailingChar(ATL::CComBSTR &bstr, WCHAR wch)
{
  int len = bstr.Length();
  if(len > 0)
  {
    if(bstr[len - 1] != wch)
    {
      WCHAR wchBuf[2] = {wch, 0};
      bstr += wchBuf;
    }
  }
}

void inline W_AddTrailingSlash(ATL::CComBSTR &bstrFolder)
{
  W_AddTrailingChar(bstrFolder, L'\\');
}

void inline W_RemoveTrailingSlash(ATL::CComBSTR &bstrFolder)
{
  int len = bstrFolder.Length();
  if(len > 0)
  {
    if(bstrFolder[len - 1] == L'\\')
    {
	  ATL::CComBSTR bstrTmp(bstrFolder);
      bstrTmp[len - 1] = 0;
      bstrFolder = bstrTmp;
    }
  }
}

//---------------------------------------------------------

int inline BSTRcmp(BSTR bstr1, BSTR bstr2)
{
  if ((bstr1 == NULL) && (bstr2 == NULL))
    return 0;
  else if ((bstr1 == NULL) && (bstr2 != NULL))
    return -1;
  else if ((bstr1 != NULL) && (bstr2 == NULL))
    return 1;
  else 
    return wcscmp(bstr1, bstr2);
}

int inline BSTRicmp(BSTR bstr1, BSTR bstr2)
{
  if ((bstr1 == NULL) && (bstr2 == NULL))
    return 0;
  else if ((bstr1 == NULL) && (bstr2 != NULL))
    return -1;
  else if ((bstr1 != NULL) && (bstr2 == NULL))
    return 1;
  else 
    return _wcsicmp(bstr1, bstr2);
}

//------------------------------------------

// GetWindowText
BOOL W_GetWindowText(HWND hwnd, ATL::CComBSTR &bstrText);
BOOL WINAPI W_GetWindowTextTrimSpaces(HWND hwnd, ATL::CComBSTR &bstrText);

BOOL inline W_GetDlgItemText(HWND hwnd, UINT uiID, ATL::CComBSTR &bstrText)
{
  HWND hwndItem = GetDlgItem(hwnd, uiID);
  //_ASSERTE(hwndItem); // commented because we can't rely on _ASSERTE being defined

  return W_GetWindowText(hwndItem, bstrText);
}

BOOL inline W_GetDlgItemTextTrimSpaces(HWND hwnd, UINT uiID, ATL::CComBSTR &bstrText)
{
  HWND hwndItem = GetDlgItem(hwnd, uiID);
  //_ASSERTE(hwndItem); // commented because we can't rely on _ASSERTE being defined

  return W_GetWindowTextTrimSpaces(hwndItem, bstrText);
}

#endif // UNIAPI_NOATL

//----------------------------------

#endif // _UNIAPI_H_
