// TxFmt.cpp
//------------------------------------------------------------------------------
// Copyright (c) 1999, Microsoft Corporation, All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//
// Author: Paul Chase Dempsey [paulde]
//------------------------------------------------------------------------------
#include "pch.h"
#include "vsmem.h"
#include "encoding.h"
#include "TxFmt.h"
#include "fpstfmt.h" // STG_E_...
#include "stdlib.h" // _swab
#include "dbgout.h"
#include "strsafe.h"



// ContainsNullByte - Tests if some data contains a null byte.
//
// Returns true if a null byte is found
inline bool ContainsNullByte (DWORD cb, const BYTE * pb)
{
    while (cb--)
    {
        BYTE b = *pb++;
        if (b == NULL)
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------
BOOL WINAPI CreateTempName (
    PCWSTR pszSrc,
    _Out_z_cap_(MAX_PATH) PWSTR  pszTemp
    )
{
  WCHAR szDir[MAX_PATH+1];
  if (pszSrc)
  {
    VERIFY(SUCCEEDED(StringCchCopyW(szDir, _countof(szDir), pszSrc)));
    NormalizeFileSlashes(szDir);
    PWSTR pch = FindLastChar(szDir, L'\\');
    if (pch)
      *pch = 0;
  }
  else
  {
    szDir[0] = 0;
    W_GetTempPath(MAX_PATH, szDir);
    if (!szDir[0])
      VERIFY(SUCCEEDED(StringCchCopyW(szDir, _countof(szDir), L".")));
  }
  return W_GetTempFileName(szDir, L"ve-", 0, pszTemp);
}


//------------------------------------------------------------------------------
// GetTextImageSize
//
// Get the byte size guaranteed to hold the text when converted from Unicode to
// the specified/detected file format.
//
// if fExact is true, returns the exact size required. This can be expensive to calculate.
// if fExact is false, returns the minimum size guaranteed to hold the data.
//
HRESULT WINAPI GetTextImageSize (
  LTI_INFO      lti,            // [in]   Format, detection instructions
  BOOL          fExact,         // [in]   Exact (TRUE) or minimum (FALSE) to calculate
  DWORD         cch,            // [in]   Count of characters in pwch
  _In_count_(cch) PCWSTR        pwch,           // [in]   Unicode data
  _Out_ LTI_INFO *    pInfo,          // [out]  Format used
  _Out_ DWORD *       pcb             // [out]  Count of bytes for image
  )
{
  if (!pcb || !pInfo) return E_POINTER;
  *pInfo = 0;
  *pcb   = 0;

  HRESULT   hr = S_OK;
  DWORD     dwLength = cch;
  LTI_INFO  ltiOut = 0;

  if (0 == dwLength)
  {
    ltiOut = LTI_CP(lti);
    if (LTIF_SIGNATURE & lti)
    {
      ltiOut |= LTIF_SIGNATURE;
      switch (LTI_CP(lti))
      {
      case CP_UNICODE: 
      case CP_UNICODESWAP: dwLength = sizeof(WCHAR); break;
      case CP_UTF8   : dwLength = UTF8SIGLEN; break;
      }
    }
    goto _Return;
  }

  switch (LTI_CP(lti))
  {
  case CP_UNICODE: 
  case CP_UNICODESWAP: _Unicode: 
    LTI_SETCP(ltiOut, LTI_CP(lti));
    if (LTIF_SIGNATURE & lti)
    {
      dwLength++;
      ltiOut |= LTIF_SIGNATURE;
    }
    dwLength = dwLength * sizeof(WCHAR);
    hr = S_OK;
    break;

  case CP_UTF8: _UTF8:
    LTI_SETCP(ltiOut, CP_UTF8);
    if (fExact)
    {
      if (!pwch) return E_INVALIDARG;
      dwLength = static_cast<DWORD>(UTF8LengthOfUnicode (pwch, cch));
    }
    else
      dwLength = (dwLength * 3);  // worst case: signature + 3 UTF bytes for each Unicode char

    if (LTIF_SIGNATURE & lti)
    {
      dwLength += UTF8SIGLEN;
      ltiOut |= LTIF_SIGNATURE;
    }
    hr = S_OK;
    break;

  default: //_MBCS:
    ltiOut = 0;
    if (lti & (LTIF_DETECT|LTIF_XML))
    {
      if (!pwch) return E_INVALIDARG;
      UINT  cpDetect;
      hr = ScanXMLEncodingW (pwch, cch, &cpDetect);
      if (SUCCEEDED(hr))
      {
        LTI_SETCP(lti,    cpDetect);
        ltiOut = LTIF_DETECT|LTIF_XML|cpDetect;
        goto _GetSize;
      }
    }
    if (lti & (LTIF_DETECT|LTIF_HTML))
    {
      if (!pwch) return E_INVALIDARG;
      UINT  cpDetect;
      hr = ScanHTMLCharSetW (pwch, cch, &cpDetect);
      if (S_OK == hr)
      {
        LTI_SETCP(lti,    cpDetect);
        LTI_SETCP(ltiOut, cpDetect);
        
      }
      if (SUCCEEDED(hr))
      {
        if(cpDetect == CP_UNICODE || cpDetect == CP_UNICODESWAP)
            lti |= LTIF_SIGNATURE;
        ltiOut |= (LTIF_DETECT|LTIF_HTML);
      }
    }
_GetSize:
    hr = S_OK;
    switch (LTI_CP(lti))
    {
    case CP_UTF8:    goto _UTF8;
    case CP_UNICODE: 
    case CP_UNICODESWAP: goto _Unicode;
    default:
      if(LTI_CP(lti) == 0)
            lti |= GetACP();
      if (fExact || !IsValidCodePage(LTI_CP(lti)))
      {
        if (!pwch) return E_INVALIDARG;
        hr = S_OK;

        if (IsValidCodePage(LTI_CP(lti)))
        {
            BOOL fDefaultCharUsed = FALSE;
            dwLength = WideCharToMultiByte(LTI_CP(lti), NULL, pwch, cch, NULL, 0, NULL, &fDefaultCharUsed);
            if (!dwLength)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
            else if (fDefaultCharUsed)
            {
                //SetRichErrorInfo(IDS_BUFFER_WARNING_CHAR_MAPPED);
                hr = STG_S_DATALOSS;
            }
        }
        else
        {
            hr = STG_E_INVALIDCODEPAGE;
        }

        if(ltiOut == 0)
            LTI_SETCP(ltiOut, LTI_CP(lti));
      }
      else
      {
        CPINFO cpi;
        if (GetCPInfo(LTI_CP(lti), &cpi))
          // worst case: every character is max size
          dwLength = dwLength * cpi.MaxCharSize;
        else
          // worst case: assume every character is a double-byte char
          dwLength = dwLength * 2;
        if(ltiOut == 0)
            LTI_SETCP(ltiOut, LTI_CP(lti));

        hr = S_OK;
      }
    }
    break;
  }
_Return:
  *pInfo = ltiOut;
  *pcb = dwLength;
  return hr;
}

//------------------------------------------------------------------------------
HRESULT WINAPI GetImageSignature (
  _In_                  DWORD        cbData,
  _Out_bytecap_(cbData) PCBYTE       pData,
  _Inout_               LTI_INFO *   pInfo
  )
{
  if (!pInfo||!pData) return E_INVALIDARG;
  *pInfo = 0;
  HRESULT hr = E_FAIL;
  if (cbData < 2)
  {
    // fail
  }
  else if (UCH_BOM == *(WCHAR*)pData)
  {
    *pInfo = LTIF_SIGNATURE|CP_UNICODE;
    hr = S_OK;
  }
  else if (UCH_BOMSWAP == *(WCHAR*)pData)
  {
    *pInfo = LTIF_SIGNATURE|CP_UNICODESWAP;
    hr = S_OK;
  }
  else if ((cbData >= 3) && (0xEF == pData[0]) && (0xBB == pData[1]) && (0xBF == pData[2]))
  {
    *pInfo = LTIF_SIGNATURE|CP_UTF8;
    hr = S_OK;
  }
  return hr;
}

//------------------------------------------------------------------------------
// GetImageFormat2 - detect the type of text image
// This replaces GetImageFormat which doesn't honor the LTIF_NOUTF8_NOSIG flag
//
// WARNING: If you change the detection code in this routine please update LoadTextImageFromMemoryAux
HRESULT WINAPI GetImageFormat2 (
  _In_                  LTI_INFO     Info,      // detection instructions
  _In_                  DWORD        cbData,
  _Out_bytecap_(cbData) PCBYTE       pData,
  _Inout_               LTI_INFO *   pInfo      // result
  )
{
    HRESULT hr = S_OK;
    if (!pInfo || !pData) return E_INVALIDARG;

    if ((Info & LTIF_DETECT) && (cbData > 1) && (UCH_BOM == *(WCHAR*)pData))
    {
        if((cbData > 1) && (UCH_BOM == *(WCHAR*)pData))
        {
            *pInfo = LTIF_SIGNATURE|CP_UNICODE;
        }
        else
        {
            *pInfo = CP_UNICODE;
        }

        ASSERT(0 == (cbData & 1)); // shouldn't have odd byte count for Unicode

    }
    else if ((Info & LTIF_DETECT) && (cbData > 1) && (UCH_BOMSWAP == *(WCHAR*)pData))
    {
        if((cbData > 1) && (UCH_BOMSWAP == *(WCHAR*)pData))
        {
            *pInfo = LTIF_SIGNATURE|CP_UNICODESWAP;
        }
        else
        {
            *pInfo = CP_UNICODESWAP;
        }

        ASSERT(0 == (cbData & 1)); // shouldn't have odd byte count for Unicode

    }
    else if ((Info & LTIF_DETECT) && (cbData >= 3) && (0xEF == pData[0]) && (0xBB == pData[1]) && (0xBF == pData[2]))
    {
        if((cbData >= 3) && (0xEF == pData[0]) && (0xBB == pData[1]) && (0xBF == pData[2]))
        {
            *pInfo = LTIF_SIGNATURE|CP_UTF8;
        }
        else
        {
            *pInfo = CP_UTF8;
        }

    }
    else
    {
        // binary check
        if (!IsAnsiText(min(cbData-1, 250), pData))
        { 
            *pInfo |= LTIF_BINARY;
            // if not told to allow binary, return error
            if (!(Info && (LTIF_BINARY & Info)))
            {
                return VS_E_UNSUPPORTEDFORMAT;
            }
        }

        //Use the provided CodePage if available
        //otherwise use the Active Code Page
        if (Info && LTI_CP(Info))
            LTI_SETCP(*pInfo, LTI_CP(Info));
        else
            LTI_SETCP(*pInfo, GetACP());

        switch(LTI_CP(*pInfo))
        {
            case CP_UNICODE: 
                *pInfo = CP_UNICODE;
                return hr;

            case CP_UNICODESWAP: 
                *pInfo = CP_UNICODESWAP;
                return hr;

            case CP_UTF8: 
                *pInfo = CP_UTF8;
                return hr;

            default: 
                break;
        }

        if (!Info || (Info & LTIF_XML))
        {
            UINT cp = CP_UTF8;
            if (SUCCEEDED(ScanXMLEncodingA ((PCSTR)pData, cbData, &cp)))
            {
                LTI_SETCP(*pInfo, cp);
                *pInfo |= LTIF_DETECT;
                *pInfo |= LTIF_XML;
                return hr;
            }
        }
        // detect UTF-8 w/o signature
        if (!Info || ((LTI_FLAGS(Info) & (LTIF_DETECT|LTIF_HTML)) && !(LTI_FLAGS(Info) & LTIF_NOUTF8_NOSIG)))
        {
            int cb = (int)cbData;
            DWORD dw;
            GetUTF8Info((char*)pData, &cb, &dw, FALSE);
            if (dw && !U8TU_IsError(dw))
            {
                ASSERT(cb == (int)cbData);
                LTI_SETCP(*pInfo, CP_UTF8);         // UTF-8
                *pInfo &= ~LTIF_SIGNATURE;   // no signature
                *pInfo |= LTIF_DETECT;       // detected
                return hr;
            }
        }
        // detect HTML
        if (!Info || (Info & LTIF_HTML))
        {
            UINT cp = LTI_CP(*pInfo);
            HRESULT hrScan = ScanHTMLCharSetA ((PCSTR)pData, cbData, &cp);
            if (S_OK == hrScan)
            {
                LTI_SETCP(*pInfo, cp);
                *pInfo |= LTIF_DETECT;
                *pInfo |= LTIF_HTML;
            }
            else if (S_FALSE == hrScan)
            {
                *pInfo |= LTIF_HTML;
            }
            return hr;
        }
    }

    return hr;

}

//------------------------------------------------------------------------------
// GetImageFormat - detect the type of text image
// OBSOLETE: Do not use this if your code is supposed to honor the LTIF_NOUTF8_NOSIG flag
// This flag should be set/unset based on Tools -> Options "Auto detect UTF8 without signature"
// Use GetImageFOrmat2 to get the correct detection behaviour
HRESULT WINAPI GetImageFormat (
  _In_                  LTI_INFO     Info,      // detection instructions
  _In_                  DWORD        cbData,
  _Out_bytecap_(cbData) PCBYTE       pData,
  _Inout_               LTI_INFO *   pInfo      // result
  )
{
  HRESULT hr = S_OK;
  if (!pInfo || !pData) return E_INVALIDARG;

  UINT cp = LTI_CP(Info);
  *pInfo = cp;
  // Is file too small to be able to read byte order marks?
  bool fFileIsTooSmallToReadBOM = false;

  if (cbData > 2)
  {
      if (SUCCEEDED(GetImageSignature(cbData, pData, pInfo)))
          return S_OK;

      // always check signature

      if (LTIF_BINARY & Info)
      {
          if (!IsAnsiText(min(cbData-1, 250), pData))
          {
              *pInfo |= LTIF_BINARY;
              return S_OK;
          }
      }

      if (LTIF_XML & Info)
      {
          UINT cpScan = cp;
          if (SUCCEEDED(hr = ScanXMLEncodingA ((PCSTR)pData, cbData, &cpScan)))
          {
              *pInfo = LTIF_XML | cpScan;
              return hr;
          }
      }

      if (LTIF_HTML & Info)
      {
          UINT cpScan;
          hr = ScanHTMLCharSetA((PCSTR)pData, cbData, &cpScan);
          if (S_OK == hr)
          {
              *pInfo = LTIF_HTML | cpScan;
              return S_OK;
          }
          else if (S_FALSE == hr)
          {
              // valid HTML but no charset specified
              *pInfo = LTIF_HTML | cp;
              if (!(LTIF_DETECT & Info))
                  return hr;
          }
      }
  }
  else
  {
      // The file is too small for us to detect the encoding from BOM
      fFileIsTooSmallToReadBOM = true;
  }

  // If we have been asked to detect the type, or the file is too small for BOM then
  // see if we can get UTF8 information from it. GetUTF8Info is safe even with zero sized files
  if ( fFileIsTooSmallToReadBOM  ||  LTIF_DETECT & Info)
  {
    // $
    int cb = (int)cbData;
    DWORD dw;
    GetUTF8Info((char*)pData, &cb, &dw, FALSE);
    if (dw && !U8TU_IsError(dw))
    {
      // it's UTF-8 without errors and high bits set
#ifdef _DEBUG
      {
        // If it's also valid in an MBCS codepage, the format is ambiguous
        // It's always ambiguous for SBCS, but only the KK PMs have expressed a concern:-)
        CPINFO cpi;
        UINT cpTest = (CP_INVALID == cp ? GetACP() : cp);
        memset(&cpi, 0, sizeof(cpi));
        GetCPInfo(cpTest, &cpi);
        if (cpi.MaxCharSize > 1)
        {
          int cch = MultiByteToWideChar(cpTest, MB_ERR_INVALID_CHARS, (LPCSTR)pData, cbData, NULL, 0);
          if (!cch)
          {
            DWORD dwErr = GetLastError();
            if (ERROR_NO_UNICODE_TRANSLATION == dwErr)
            {
              // we're fine
            }
            else
              ASSERT(0); // this debugging code is wrong!
          }
          else
          {
            // the data is valid UTF-8 AND valid system MBCS
            // what do we do now?
            ASSERT(0);
          }
        }
        else
        {
          // SBCS: always ambiguous
        }
      }
#endif
      ASSERT(cb == (int)cbData);
      LTI_SETCP(*pInfo, CP_UTF8);
      *pInfo |= LTIF_DETECT;
      return S_OK;
    }
    else
    {
      cp = (CP_INVALID == cp ? GetACP() : cp);
      LTI_SETCP(*pInfo, cp);
      //*pInfo |= LTIF_DETECT;
#ifdef _DEBUG
      // verify that the data is valid in the codepage
      // If this fires, then we may have to implement a search for a valid codepage
      if(cp != CP_UTF8)
        ASSERT( cb == 0  ||  0 < MultiByteToWideChar(cp, MB_ERR_INVALID_CHARS, (LPCSTR)pData, cbData, NULL, 0));
#endif
      return S_OK;
    }
  }
  return hr;
}

//------------------------------------------------------------------------------
// LoadBasicTextFile
//
// Use only for files known to be simple text files (NOT HTML/XML).
// Checks only file signatures to diagnose format, similar to Notepad.
// Does NOT recognize XML or HTML or detect UTF-8 or other codepages.
//
HRESULT WINAPI LoadBasicTextFile (
  LPCOLESTR     pszFilename, // [in ] filename
  BSTR *        pbstr        // [out] text of file's contents
  )
{
  DWORD   dwErr = 0;
  HRESULT hr = STG_E_FILENOTFOUND;
  HANDLE  hFile;
  BSTR    bstr = 0;

  hFile = W_CreateFile (pszFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    dwErr = GetLastError();
  else
  {
    DWORD dwSize = GetFileSize(hFile, NULL);
    if (dwSize)
    {
      HANDLE hMap = CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 0, NULL);
      if (hMap)
      {
        BYTE* pData = (BYTE *)MapViewOfFile (hMap, FILE_MAP_READ, 0, 0, 0);
        if (pData)
        {
            __try
            {
                LTI_INFO  lti = 0;
                DWORD     cchUni = 0;
                hr = GetImageSignature(dwSize, pData, &lti);
                hr = TextImageUnicodeSize(lti, dwSize, pData, &cchUni);
                if (SUCCEEDED(hr))
                {
                    bstr = SysAllocStringLen(NULL, cchUni);
                    if (bstr)
                    {
                        hr = TextImageToUnicode (lti, false, dwSize, pData, cchUni, bstr);
                        if (FAILED(hr))
                            SYSFREE(bstr);
                    }
                    else
                        hr = E_OUTOFMEMORY;
                }
                UnmapViewOfFile (pData);
            }
            __except(EXCEPTION_EXECUTE_HANDLER)
            {
                DWORD nRead=0;
                UnmapViewOfFile(pData);
                pData = (BYTE *) VSAlloc (sizeof(BYTE) * (dwSize));
                if(pData)
                {
                    if(ReadFile(hFile,&pData,dwSize,&nRead,NULL))
                    {
                        __try
                        {                        
                            LTI_INFO  lti = 0;
                            DWORD     cchUni = 0;
                            hr = GetImageSignature(dwSize, pData, &lti);
                            hr = TextImageUnicodeSize(lti, dwSize, pData, &cchUni);
                            if (SUCCEEDED(hr))
                            {
                                bstr = SysAllocStringLen(NULL, cchUni);
                                if (bstr)
                                {
                                    hr = TextImageToUnicode (lti, false, dwSize, pData, cchUni, bstr);
                                    if (FAILED(hr))
                                        SYSFREE(bstr);
                                }
                                else
                                    hr = E_OUTOFMEMORY;
                            }
                            UnmapViewOfFile (pData);                
                        }
                        __except(EXCEPTION_EXECUTE_HANDLER)
                        {
                            VSFree(pData);
                            hr = HRESULT_FROM_WIN32(GetLastError());;
                        }
                    }
                    else
                    {
                        VSFree(pData);
                        hr = HRESULT_FROM_WIN32(GetLastError());;
                    }
                }
            }

        }
        else
          dwErr = GetLastError();
        CloseHandle (hMap);
      }
      else
        dwErr = GetLastError();
    }
    else
      bstr = SysAllocString(L"");
    CloseHandle (hFile);
  }
  if (dwErr)
    hr = HRESULT_FROM_WIN32(dwErr);
  *pbstr = bstr;
  return hr;
}

//------------------------------------------------------------------------------
// LoadTextFileFromHandle
//
HRESULT WINAPI LoadTextFileFromHandle (
  HANDLE        hFile,          // file handle
  LTI_INFO      lti,            // Instructions for format, detection
  BSTR *        pbstr,          // [out] text of file
  LTI_INFO *    plti            // [out] format used
  )
{
  DWORD     dwErr = 0;
  HRESULT   hr = STG_E_FILENOTFOUND;
  BSTR      bstr = 0;
  LTI_INFO  ltiOut = 0;

  DWORD dwSize = GetFileSize(hFile, NULL);
  if (dwSize)
  {
      HANDLE hMap = CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 0, NULL);
      if (hMap)
      {
          BYTE* pData = (BYTE *)MapViewOfFile (hMap, FILE_MAP_READ, 0, 0, 0);
          if (pData)
          {
              __try
              {
                  hr = GetImageFormat(lti, dwSize, pData, &ltiOut);
                  DWORD cchUni = 0;
                  hr = TextImageUnicodeSize(ltiOut, dwSize, pData, &cchUni);
                  if (SUCCEEDED(hr))
                  {
                      bstr = SysAllocStringLen(NULL, cchUni);
                      if (bstr)
                      {
                          hr = TextImageToUnicode (ltiOut, false, dwSize, pData, cchUni, bstr);
                          if (FAILED(hr))
                              SYSFREE(bstr);
                      }
                      else
                          hr = E_OUTOFMEMORY;
                  }
                  UnmapViewOfFile (pData);
              }
              __except(EXCEPTION_EXECUTE_HANDLER)
              {
                  DWORD nRead=0;
                  UnmapViewOfFile(pData);
                  pData = (BYTE *) VSAlloc (sizeof(BYTE) * (dwSize));
                  if(pData)
                  {
                      SetFilePointer(hFile,0,NULL,FILE_BEGIN);
                      if(ReadFile(hFile,pData,dwSize,&nRead,NULL))
                      {
                          __try
                          {
                              hr = GetImageFormat(lti, dwSize, pData, &ltiOut);
                              DWORD cchUni = 0;
                              hr = TextImageUnicodeSize(ltiOut, dwSize, pData, &cchUni);
                              if (SUCCEEDED(hr))
                              {
                                  bstr = SysAllocStringLen(NULL, cchUni);
                                  if (bstr)
                                  {
                                      hr = TextImageToUnicode (ltiOut, false, dwSize, pData, cchUni, bstr);
                                      if (FAILED(hr))
                                          SYSFREE(bstr);
                                  }
                                  else
                                      hr = E_OUTOFMEMORY;
                              }
                              VSFree(pData);
                          }
                          __except(EXCEPTION_EXECUTE_HANDLER)
                          {
                              VSFree(pData);
                              hr=HRESULT_FROM_WIN32(GetLastError());
                          }
                      }
                      else
                      {
                          VSFree(pData);
                          hr=HRESULT_FROM_WIN32(GetLastError());;
                      }
                  }
                  
              }
          }
          else
          dwErr = GetLastError();
        CloseHandle (hMap);
      }
      else
        dwErr = GetLastError();
  }
  else
  {
      bstr = SysAllocString(L"");
      hr = S_OK;
  }

  if (dwErr)
    hr = HRESULT_FROM_WIN32(dwErr);
  
  *pbstr = bstr;

  if (plti != NULL)
    *plti = ltiOut;
  
  return hr;
}


//------------------------------------------------------------------------------
// LoadTextFile
//
HRESULT WINAPI LoadTextFile (
  LPCOLESTR     pszFilename,    // file name
  LTI_INFO      lti,            // Instructions for format, detection
  BSTR *        pbstr,          // [out] text of file
  LTI_INFO *    plti            // [out] format used
  )
{
  DWORD     dwErr = 0;
  HRESULT   hr = STG_E_FILENOTFOUND;
  HANDLE    hFile;

  *pbstr = NULL;
  
  hFile = W_CreateFile (pszFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  
  if (hFile == INVALID_HANDLE_VALUE)
  {
    dwErr = GetLastError();
  }
  else
  {
    hr = LoadTextFileFromHandle(hFile, lti, pbstr, plti);
    CloseHandle(hFile);
  }
  
  if (dwErr)
    hr = HRESULT_FROM_WIN32(dwErr);
  
  return hr;
}


//------------------------------------------------------------------------------
// TextImageUnicodeSize - get the Unicode size of a plain-text image
//
// Does not detect: ignores LTIF_DETECT, LTIF_HTML, LTIF_XML
// lti must have been created by running GetImageSignature or GetImageFormat
//
HRESULT WINAPI TextImageUnicodeSize (
  LTI_INFO    lti,      // format of image
  DWORD       cbData,   // image size
  PCBYTE      pData,    // image data
  DWORD *     pdwSize   // Unicode size
  )
{
  if (!pdwSize) return E_POINTER;
  *pdwSize = 0;
  //if (LTIF_BINARY & lti) return STG_E_NOTTEXT;

  HRESULT hr = S_OK;
  DWORD   cchRequired = 0;

  if (!cbData)
  {
    ASSERT(0 == (LTIF_SIGNATURE & lti));
    return S_OK;
  }

  ASSERT(pData);
  if (!pData) return E_INVALIDARG;

  switch (LTI_CP(lti))
  {
  case CP_UNICODE:
  case CP_UNICODESWAP:
    {
      hr = S_OK;
      if (LTIF_SIGNATURE & lti)
      {
        ASSERT((cbData > 1) && *(WCHAR*)pData == (LTI_CP(lti)==CP_UNICODESWAP ? UCH_BOMSWAP : UCH_BOM));
        cbData -= 2;
        pData += sizeof(WCHAR);
      }
      cchRequired = cbData/sizeof(WCHAR);
    }
    break;

  case CP_UTF8:
    {
      if (LTIF_SIGNATURE & lti)
      {
        ASSERT((cbData >= 3) && (0xEF == pData[0]) && (0xBB == pData[1]) && (0xBF == pData[2]));
        cbData -= 3;
        pData += 3;
      }
      cchRequired = (DWORD)UnicodeLengthOfUTF8 ((PCSTR)pData, cbData);
    }
    break;

  default:
    {
      ASSERT(0 == (LTIF_SIGNATURE & lti)); // some new signature we don't know about
      //ASSERT(IsAnsiText(min(cbData-1, 250), pData)); // LTI wasn't derived by checking the actual format
      UINT cp  = LTI_CP(lti);
      if (CP_ACP == cp) cp = GetACP();
      if (IsValidCodePage(cp))
        cchRequired = (DWORD)MultiByteToWideChar (cp, 0, (LPCSTR)pData, cbData, NULL, 0);
      else
      {
        hr = STG_E_INVALIDCODEPAGE;
      }
    }
    break;
  }
  *pdwSize = cchRequired;
  return hr;
}

//------------------------------------------------------------------------------
// TextImageToUnicode - Converts text from current format to unicode.
//
// This function converts text from unicode, UTF8, or an arbitrary  
// code page into unicode.  The code page to use for conversion is specified
// by the lti parameter.
//
// On success, the output unicode string will be null-terminated if the output
// buffer is large enough.
//
// To determine how big the output buffer needs to be, use
// TextImageUnicodeSize() prior to calling TextImageToUnicode().  If you want 
// the output string null-terminated, you should make your buffer 1 
// character bigger than the size returned by TextImageUnicodeSize().
//
// Note: This function does not perform format detection and therefore ignores 
// LTIF_DETECT, LTIF_HTML, and LTIF_XML.  lti must have been created by running 
// GetImageSignature() or GetImageFormat[2]().
//
// 








HRESULT WINAPI TextImageToUnicode (
  LTI_INFO    lti,        // [in] Load Text Image info (specifies format of text being converted)
  BOOL        fCheckSize, // [in] Fail (with ERROR_INSUFFICIENT_BUFFER) if output buffer is too small?
  DWORD       cbData,     // [in] size of text data to convert (in bytes)
  PCBYTE      pData,      // [in] text data to convert
  DWORD       cch,        // [in] size of output buffer for unicode text (in characters)
 _Out_cap_(cch) 
  PWSTR       pwch        // [out] output buffer for unicode text
  )
{
  if (!pwch) return E_POINTER;

  HRESULT hr = E_FAIL;
  DWORD   cchRequired = 0;

  if (!cbData)
  {
    ASSERT(0 == (LTIF_SIGNATURE & lti));
    if (cch)
      *pwch = 0;
    return S_OK;
  }

  ASSERT(pData);
  if (!pData) return E_INVALIDARG;

  if (fCheckSize)
  {
    IfFailRet(TextImageUnicodeSize (lti, cbData, pData, &cchRequired))
  }
  else
    cchRequired = cch;

  switch (LTI_CP(lti))
  {
  case CP_UNICODE:
  case CP_UNICODESWAP:
    {
      hr = S_OK;
      if (LTIF_SIGNATURE & lti)
      {
        ASSERT((cbData > 1) && *(WCHAR*)pData == (LTI_CP(lti)==CP_UNICODESWAP ? UCH_BOMSWAP : UCH_BOM));
        cbData -= 2;
        pData += sizeof(WCHAR);
      }
      if (cchRequired <= cch)
      {
        // Make sure we don't copy more than cch characters.
        DWORD cbConvert = min(cbData, cch * sizeof(WCHAR));

        if (LTI_CP(lti) == CP_UNICODESWAP)
          _swab((char*)pData, (char*)pwch, cbConvert);
        else
          memcpy(pwch, pData, cbConvert);

        DWORD cchConvert = cbConvert / sizeof(WCHAR);
        ASSERT(!fCheckSize || cchConvert == cchRequired);

        if (cchConvert < cch)
          pwch[cchConvert] = 0;
      }
      else
      {
        ASSERT(fCheckSize);
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
      }
    }
    break;

  case CP_UTF8:
    {
      if (LTIF_SIGNATURE & lti)
      {
        ASSERT((cbData >= 3) && (0xEF == pData[0]) && (0xBB == pData[1]) && (0xBF == pData[2]));
        cbData -= 3;
        pData += 3;
      }
      if (cchRequired <= cch)
      {
        DWORD cbConvert  = cbData;
        DWORD cchConvert = UTF8ToUnicode((PCSTR)pData, cbConvert, pwch, cch);
        // if this fires, UnicodeLengthOfUTF8 doesn't agree with UTF8ToUnicode
        ASSERT(!fCheckSize || (cchConvert == cchRequired));
        if (cchConvert < cch)
          pwch[cchConvert] = 0;
        hr = S_OK;
      }
      else
      {
        ASSERT(fCheckSize);
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
      }
    }
    break;

  default:
    {
      ASSERT(0 == (LTIF_SIGNATURE & lti)); // some new signature we don't know about

      if ((cbData > 1) && (!(LTIF_BINARY & lti) && ContainsNullByte(min(cbData-1,500), pData)))
        hr = STG_E_NOTTEXT;
      else
      {
        if (cchRequired <= cch)
        {
          UINT cp  = LTI_CP(lti);
          if (CP_ACP == cp) cp = GetACP();
          if (IsValidCodePage(cp))
          {
              DWORD cchConvert = (DWORD)MultiByteToWideChar (cp, 0, (LPCSTR)pData, cbData, pwch, cch);
              ASSERT(!fCheckSize || (cchConvert == cchRequired));
              if (cchConvert < cch)
                pwch[cchConvert] = 0;
              if ((0 == cchConvert) && (0 != cbData))
                hr = HRESULT_FROM_WIN32(GetLastError());
              else
                hr = S_OK;
          }
          else
          {
            cchRequired = 2 * cbData;
            hr = STG_E_INVALIDCODEPAGE;
          }
        }
        else
        {
          ASSERT(fCheckSize);
          hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        }
      }
    }
    break;
  }
  return hr;
}
