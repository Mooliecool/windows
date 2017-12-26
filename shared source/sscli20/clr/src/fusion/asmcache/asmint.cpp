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


#include "asmint.h"
#include "helpers.h"
#include "lock.h"

CModuleHashNode::CModuleHashNode ( )
{
    _dwSig = 0x4e444f4d; /* 'NDOM' */
    _szPath[0] = L'\0';
    _cbHash = MAX_HASH_LEN;
    _bHashGen = 0;
    _bHashFound = 0;
    _aAlgId   = 0;
}

CModuleHashNode::~CModuleHashNode ( )
{
}

void CModuleHashNode::Init ( LPCTSTR szPath, ALG_ID aAlgId, DWORD cbHash, BYTE *pHash)
{
    _aAlgId   = aAlgId;
    if(_aAlgId && (cbHash <= MAX_HASH_LEN ) )
    {
        memcpy(_bHash, pHash, cbHash);
        _cbHash = cbHash;
        _bHashGen = TRUE;
    }
    else
    {
        _bHashGen = FALSE;
    }

    if(lstrlenW(szPath) <= MAX_PATH) {
        HRESULT hr = StringCbCopy(_szPath, sizeof(_szPath), szPath);
        if (FAILED(hr)) {
            _ASSERTE(!"StringCopy should not fail.");
            return;
        }
    }
}


void
CModuleHashNode::AddToList(CModuleHashNode **pHead)
{
    // Is it possible that a node for this file already exists ??
    // If yes check szPath to avoid duplicates !!

    _pNext = *pHead;
    *pHead = this;
}


void
CModuleHashNode::DestroyList()
{
    CModuleHashNode *pModList = this, *pTemp;

    while ( pModList ) 
    {
        pTemp = pModList;
        pModList = pModList->_pNext;
        SAFEDELETE(pTemp);
    }

}


BOOL 
CompareHashs(DWORD cbHash, PBYTE pHash1, PBYTE pHash2)
{
    DWORD * pdwHash1 = (DWORD *)pHash1, *pdwHash2 = (DWORD *)pHash2;

    // Here the assumption is cbHash will always be a multiple of sizeof(DWORD).
    while (cbHash > 0 )
    {
        if( *pdwHash1 != *pdwHash2 )
        {
            return FALSE;
        }

        pdwHash1++; pdwHash2++; cbHash -= sizeof (DWORD);
    }

    return TRUE;
}


HRESULT GetHash(LPCTSTR szFileName, ALG_ID iHashAlg, PBYTE pbHash, DWORD *pdwHash)
{

#define MAX_ARRAY_SIZE  16384
#define HASH_BUFFER_SIZE (MAX_ARRAY_SIZE-4)

    HRESULT    hr = E_FAIL;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    DWORD      dwBufferLen;
    BYTE      *pbBuffer = NULL;
    HANDLE     hSourceFile = INVALID_HANDLE_VALUE; 

    pbBuffer = NEW(BYTE[MAX_ARRAY_SIZE]);
    if (!pbBuffer) {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    if(!WszCryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    if(!CryptCreateHash(hProv, iHashAlg, 0, 0, &hHash)) 
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    // Open source file.
    hSourceFile = WszCreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hSourceFile == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    while (TRUE) 
    {   
        if (!ReadFile (hSourceFile, pbBuffer, HASH_BUFFER_SIZE, &dwBufferLen, NULL)) 
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }
        
        if (!dwBufferLen) {
            break;
        }
                
        // Add data to hash object.
        if(!CryptHashData(hHash, pbBuffer, dwBufferLen, 0)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }
    }

    if(!CryptGetHashParam(hHash, HP_HASHVAL, pbHash, pdwHash, 0)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    hr = S_OK;

 exit:
    SAFEDELETEARRAY(pbBuffer);
    
    if (hHash)
        CryptDestroyHash(hHash);
    if (hProv)
        CryptReleaseContext(hProv,0);
    if (hSourceFile != INVALID_HANDLE_VALUE)
        CloseHandle(hSourceFile);

    return hr;
}

// return:
//  S_OK    find it
//  S_FALSE do not find it
//  other   failure
HRESULT CModuleHashNode::FindMatchingHashInList( CModuleHashNode *pStreamList, DWORD cbHash, PBYTE pHash, ALG_ID aAlgId, CModuleHashNode **ppMachingModNode)
{
    CModuleHashNode *pModList = pStreamList;
    HRESULT hr = S_OK;

    while ( pModList ) 
    {
        // Calculate hash for this file if hash is not found or ALG_ID is diff.
        if( (!pModList->_bHashFound) && 
            ((!pModList->_bHashGen) || (pModList->_aAlgId != aAlgId)) )
        {
            pModList->_cbHash = MAX_HASH_LEN;
            hr = GetHash(pModList->_szPath, aAlgId, pModList->_bHash, &(pModList->_cbHash) ); 
            if (FAILED(hr)) {
                goto Exit;
            }
            pModList->_bHashGen = 1;
            pModList->_aAlgId = aAlgId;
        }
        
        if( (!pModList->_bHashFound) && pModList->_cbHash && 
                        (pModList->_cbHash == cbHash) )
        {
            if (CompareHashs( cbHash, pHash, pModList->_bHash ))
            {
                pModList->_bHashFound=1;

                *ppMachingModNode = pModList;
                hr = S_OK;
                goto Exit;
            }
        }
        pModList = pModList->_pNext;
    }

    hr = S_FALSE;

Exit:
    return hr;
}

// return:
//  S_OK    All modules found.
//  S_FALSE Some module missing.
//  Other   failure.
HRESULT CModuleHashNode::HashesForAllModulesFound(CModuleHashNode *pStreamList)
{
    HRESULT hr = S_OK;
    CModuleHashNode *pModList = pStreamList;
    BOOL bAllModsFound = TRUE;

    while ( pModList ) 
    {
        if( !(pModList->_bHashFound) )
        {
            WszDeleteFile(pModList->_szPath);
            bAllModsFound = FALSE;
        }

        pModList = pModList->_pNext;
    }

    if (!bAllModsFound) {
        hr = S_FALSE;
    }

    return hr;
}

HRESULT
CModuleHashNode::RectifyFileName(LPCTSTR pszPath, DWORD cbPath) 
{
    HRESULT hr = S_OK;
    TCHAR   szBuf[MAX_PATH+1], *pszTemp;

    hr = StringCbCopy(szBuf, sizeof(szBuf), _szPath);
    if (FAILED(hr)) {
        goto exit;
    }

    pszTemp = PathFindFileName (szBuf);

    if (pszTemp <= szBuf)
    {
        hr = HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME);
        goto exit;
    }

    *(pszTemp) = TEXT('\0');

    
    if( (lstrlenW(szBuf) + cbPath) > MAX_PATH )
    {
        hr =  FUSION_E_INVALID_NAME;
        goto exit;
    }

    hr = StringCbCat(szBuf, sizeof(szBuf), pszPath);
    if (FAILED(hr)) {
        goto exit;
    }

    if ( !WszMoveFile(_szPath, szBuf) )
    {
        BOOL    fExists = FALSE;
        HRESULT hrTmp = HRESULT_FROM_WIN32(GetLastError());  // Preserve the MoveFile error

        hr = CheckFileExistence(szBuf, &fExists, NULL);
        if(FAILED(hr)) {
            goto exit;
        }
        else if(!fExists) {
            hr = hrTmp;
            goto exit;
        }
        
        // someone else already downloaded this file.
        WszDeleteFile(_szPath);
        hr = S_OK;
    }

exit:

    return hr;

}


HRESULT
CModuleHashNode::DoIntegrityCheck( CModuleHashNode *pStreamList, IAssemblyManifestImport *pManifestImport, BOOL *pbDownLoadComplete )
{

    CModuleHashNode *pModList = pStreamList, *pMatchingModNode;
    HRESULT hr = S_OK;
    BYTE    bHash[MAX_HASH_LEN];
    DWORD   cbHash=0, dwIndex=0, dwAlgId=0;
    IAssemblyModuleImport *pModImport=NULL;

    if ( !pManifestImport)
    {
        hr = COR_E_MISSINGMANIFESTRESOURCE;
        goto exit;
    }

    pModList = pStreamList;
    dwIndex = 0;


    while ( SUCCEEDED(hr = pManifestImport->GetNextAssemblyModule(dwIndex, &pModImport) ))
    {
        hr = pModImport->GetHashAlgId( &dwAlgId );

        if(!SUCCEEDED(hr) )
            goto exit;

        cbHash = MAX_HASH_LEN; 
        hr = pModImport->GetHashValue(bHash, &cbHash);
        
        if(!SUCCEEDED(hr) )
            goto exit;

        hr = FindMatchingHashInList(pStreamList, cbHash, bHash, dwAlgId, &pMatchingModNode);
        if (FAILED(hr)) {
            goto exit;
        }

        if ( hr == S_OK)
        {
            TCHAR   szPath[MAX_PATH+1];
            DWORD   cbPath=MAX_PATH;

            hr = pModImport->GetModuleName(szPath, &cbPath);
            if(FAILED(hr) )
                goto exit;

            hr = pMatchingModNode->RectifyFileName(szPath, cbPath);
            if(FAILED(hr) )
                goto exit;

        }
        else
        { 
            // Atleast one module is missing !!
            *pbDownLoadComplete = FALSE;
        }

        pModImport->Release();
        pModImport = NULL;
        dwIndex++;
    }

    if (hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) {
        hr = S_OK;
    }

    if (FAILED(hr)) {
        goto exit;
    }

    hr = HashesForAllModulesFound(pStreamList);
    if (FAILED(hr)) {
        goto exit;
    }

    if (hr == S_FALSE) {
        hr = FUSION_E_UNEXPECTED_MODULE_FOUND;
    }

exit :

    if(pModImport)
        pModImport->Release();

    return hr;
}
