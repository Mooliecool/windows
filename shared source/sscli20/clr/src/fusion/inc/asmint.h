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
#ifndef _ASMINT_H_
#define _ASMINT_H_

#include <windows.h>
#include <winerror.h>
#include <wincrypt.h>
#include "fusionp.h"
#include "fusion.h"

#define MAX_HASH_LEN  100

class CModuleHashNode
{

public :
    CModuleHashNode ();
    ~CModuleHashNode ( );
    void AddToList(CModuleHashNode **pHead);
    void DestroyList();
    static HRESULT DoIntegrityCheck( CModuleHashNode *pStreamList, IAssemblyManifestImport *pImport, BOOL *pbDownLoadComplete);
    HRESULT RectifyFileName( LPCTSTR pszPath, DWORD cbPath);
    void Init ( LPCTSTR szPath, ALG_ID aAlgId, DWORD cbHash, BYTE *pHash);
    static HRESULT FindMatchingHashInList ( CModuleHashNode *pStreamList, DWORD cbHash, PBYTE pHash, ALG_ID aAlgId, CModuleHashNode **ppMachingModNode );
    static HRESULT HashesForAllModulesFound(CModuleHashNode *pStreamList);

private :
    DWORD   _dwSig;
    TCHAR   _szPath[MAX_PATH +1 ];
    DWORD   _cbHash;
    BYTE    _bHash[MAX_HASH_LEN];
    BOOL    _bHashGen;
    BOOL    _bHashFound;
    ALG_ID  _aAlgId;
    CModuleHashNode *_pNext;


};

HRESULT
DoesThisFileExistInManifest( IAssemblyManifestImport *pManifestImport, 
                             LPCTSTR szFilePath, 
                             BOOL *pbFileExistsInManifest );

HRESULT GetHash(LPCTSTR szFileName, ALG_ID iHashAlg, PBYTE pbHash, DWORD *pdwHash);
BOOL CompareHashs(DWORD cbHash, PBYTE pHash1, PBYTE pHash2);
#endif // _ASMINT_H_

