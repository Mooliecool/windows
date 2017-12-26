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

#ifndef _DLWRAP_H
#define _DLWRAP_H

//include this file if you get contract violation because of delayload

//nothrow implementations


#if defined(VER_H) && !defined (GetFileVersionInfoSizeW_NoThrow)
inline DWORD 
GetFileVersionInfoSizeW_NoThrow(
        LPCWSTR lptstrFilename, /* Filename of version stamped file */
        LPDWORD lpdwHandle
        )
{
    WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    DWORD dwRet=0;
    EX_TRY
    {
         dwRet=GetFileVersionInfoSizeW( lptstrFilename,  lpdwHandle );  
    }
    EX_CATCH_HRESULT(hr);
    if (hr!=S_OK)
        SetLastError(hr);
    return dwRet;
    
};
#else
#endif

#if defined(VER_H) && !defined (GetFileVersionInfoW_NoThrow)
inline BOOL
GetFileVersionInfoW_NoThrow(
        LPCWSTR lptstrFilename, /* Filename of version stamped file */
        DWORD dwHandle,         /* Information from GetFileVersionSize */
        DWORD dwLen,            /* Length of buffer for info */
        LPVOID lpData
        )         
{
    WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    BOOL bRet=FALSE;
    EX_TRY
    {
         bRet=GetFileVersionInfoW( lptstrFilename, dwHandle,dwLen,lpData );  
    }
    EX_CATCH_HRESULT(hr);
    if (hr!=S_OK)
        SetLastError(hr);
    return bRet;
    
};
#endif

#if defined(VER_H) && !defined (VerQueryValueW_NoThrow)
inline BOOL
VerQueryValueW_NoThrow(
        const LPVOID pBlock,
        LPCWSTR lpSubBlock,
        LPVOID * lplpBuffer,
        PUINT puLen
        )     
{
    WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    BOOL bRet=FALSE;
    EX_TRY
    {
         bRet=VerQueryValueW( pBlock, (LPWSTR)lpSubBlock,lplpBuffer,puLen );  
    }
    EX_CATCH_HRESULT(hr);
    if (hr!=S_OK)
        SetLastError(hr);
    return bRet;
    
};
#endif

#if defined(VER_H) && !defined (GetFileVersionInfoSizeA_NoThrow)
inline DWORD 
GetFileVersionInfoSizeA_NoThrow(
        LPCSTR lptstrFilename, /* Filename of version stamped file */
        LPDWORD lpdwHandle
        )
{
    WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    DWORD dwRet=0;
    EX_TRY
    {
         dwRet=GetFileVersionInfoSizeA( lptstrFilename,  lpdwHandle );  
    }
    EX_CATCH_HRESULT(hr);
    if (hr!=S_OK)
        SetLastError(hr);
    return dwRet;
    
};
#else
#endif

#if defined(VER_H) && !defined (GetFileVersionInfoW_NoThrow)
inline BOOL
GetFileVersionInfoA_NoThrow(
        LPCSTR lptstrFilename, /* Filename of version stamped file */
        DWORD dwHandle,         /* Information from GetFileVersionSize */
        DWORD dwLen,            /* Length of buffer for info */
        LPVOID lpData
        )         
{
    WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    BOOL bRet=FALSE;
    EX_TRY
    {
         bRet=GetFileVersionInfoA( lptstrFilename, dwHandle,dwLen,lpData );  
    }
    EX_CATCH_HRESULT(hr);
    if (hr!=S_OK)
        SetLastError(hr);
    return bRet;
    
};
#endif

#if defined(VER_H) && !defined (VerQueryValueW_NoThrow)
inline BOOL
VerQueryValueA_NoThrow(
        const LPVOID pBlock,
        LPCSTR lpSubBlock,
        LPVOID * lplpBuffer,
        PUINT puLen
        )     
{
    WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    BOOL bRet=FALSE;
    EX_TRY
    {
         bRet=VerQueryValueA( pBlock, (LPSTR)lpSubBlock,lplpBuffer,puLen );  
    }
    EX_CATCH_HRESULT(hr);
    if (hr!=S_OK)
        SetLastError(hr);
    return bRet;
    
};
#endif




#if defined(__urlmon_h__) && !defined(CoInternetCreateSecurityManager_NoThrow)
inline HRESULT CoInternetCreateSecurityManager_NoThrow(IServiceProvider *pSP,
                                                                             IInternetSecurityManager **ppSM, 
                                                                             DWORD dwReserved)
{
   WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    EX_TRY
    {
         hr=CoInternetCreateSecurityManager(pSP,ppSM, dwReserved);
    }
    EX_CATCH_HRESULT(hr);
    return hr;
};
#endif


#if defined(__urlmon_h__) && !defined(URLDownloadToCacheFileW_NoThrow)
inline HRESULT URLDownloadToCacheFileW_NoThrow( LPUNKNOWN lpUnkcaller,
                                                                    LPCWSTR szURL,
                                                                    __out_ecount(dwBufLength) LPWSTR szFileName,
                                                                    DWORD dwBufLength,
                                                                    DWORD dwReserved,
                                                                    IBindStatusCallback *pBSC
                                                                )
{
   WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    EX_TRY
    {
         hr=URLDownloadToCacheFileW(lpUnkcaller,szURL,szFileName,dwBufLength,dwReserved,pBSC);
    }
    EX_CATCH_HRESULT(hr);
    return hr;
};
#endif


#if defined(__urlmon_h__) && !defined(CoInternetGetSession_NoThrow)
inline HRESULT CoInternetGetSession_NoThrow( WORD dwSessionMode,
                                                                                        IInternetSession **ppIInternetSession,
                                                                                        DWORD dwReserved
                                                                                    )
{
   WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    EX_TRY
    {
         hr=CoInternetGetSession(dwSessionMode,ppIInternetSession,dwReserved);
    }
    EX_CATCH_HRESULT(hr);
    return hr;
};
#endif

#if defined(__urlmon_h__) && !defined(CopyBindInfo_NoThrow)
inline HRESULT CopyBindInfo_NoThrow( const BINDINFO * pcbiSrc, BINDINFO * pbiDest )
{
   WRAPPER_CONTRACT;
    HRESULT hr=S_OK;
    EX_TRY
    {
         hr=CopyBindInfo(pcbiSrc,pbiDest );
    }
    EX_CATCH_HRESULT(hr);
    return hr;
};
#endif







//overrides
#undef InternetTimeToSystemTimeA           
#undef CommitUrlCacheEntryW                    
#undef HttpQueryInfoA                                 
#undef InternetCloseHandle                         
#undef HttpSendRequestA                            
#undef HttpOpenRequestA                            
#undef InternetConnectA                              
#undef InternetOpenA                                  
#undef InternetReadFile                               
#undef CreateUrlCacheEntryW                     
#undef CoInternetGetSession                       
#undef CopyBindInfo                                     
#undef CoInternetCreateSecurityManager   
#undef URLDownloadToCacheFileW              
#undef FDICreate                                          
#undef FDIIsCabinet                                     
#undef FDICopy                                             
#undef FDIDestroy                                                            
#undef VerQueryValueW                               
#undef GetFileVersionInfoW                         
#undef GetFileVersionInfoSizeW                  
#undef VerQueryValueA                                
#undef GetFileVersionInfoA                          
#undef GetFileVersionInfoSizeA                   


#define InternetTimeToSystemTimeA               InternetTimeToSystemTimeA_NoThrow    
#define CommitUrlCacheEntryW                        CommitUrlCacheEntryW_NoThrow                                  
#define CreateUrlCacheEntryW                        CreateUrlCacheEntryW_NoThrow               
#define CoInternetGetSession                          CoInternetGetSession_NoThrow                
#define CopyBindInfo                                        CopyBindInfo_NoThrow                              
#define CoInternetCreateSecurityManager      CoInternetCreateSecurityManager_NoThrow  
#define URLDownloadToCacheFileW                 URLDownloadToCacheFileW_NoThrow                                         
#define VerQueryValueW                                  VerQueryValueW_NoThrow                  
#define GetFileVersionInfoW                            GetFileVersionInfoW_NoThrow                 
#define GetFileVersionInfoSizeW                     GetFileVersionInfoSizeW_NoThrow                 
#define VerQueryValueA                                  VerQueryValueA_NoThrow              
#define GetFileVersionInfoA                             GetFileVersionInfoA_NoThrow             
#define GetFileVersionInfoSizeA                     GetFileVersionInfoSizeA_NoThrow     

#endif
