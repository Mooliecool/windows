//--------------------------------------------------------------------------
// Microsoft Visual Basic
//
// Copyright (c) 1994-2002 Microsoft Corporation Inc.
// All rights reserved
//
// CryptHash.cpp
//      Implement CryptHash class methods.
//      This file is used by the compiler to calculate the hash of source code to 
//      properly identify the right source files for retrieval and resolving which 
//      files to place bp's in.It might be used by other security check.
//      How it works for managed PDB check sum:
//      1.Compiler spits the hash value into PDB by calling 
//        ISymUnmanagedDocumentWriter::SetCheckSum()
//      2.Debugger reads it back from the PDB by calling 
//        ISymUnmanagedDocument::GetCheckSum(), computes the current hash, and
//        compare them by call CompareCryptHash() below.
//-----------------------------------------------------------------------------

#include <malloc.h>
#include <windows.h>
#include <wincrypt.h>
#include "CryptHash.h"
#include "vsassert.h"

//
// CryptHash::CreateCryptHash
//
bool CryptHash::CreateCryptHash(OUT CryptHash **ppsh)
{
    return CreateCryptHashAlgorithm(ppsh, CALG_SHA_256);
}

//
// CryptHash::CreateCryptHashAlgorithm
//
bool CryptHash::CreateCryptHashAlgorithm(OUT CryptHash **ppsh, ALG_ID AlgorithmId, HCRYPTPROV hProv /* NULL */)
{
    bool fSucceed = false;
    if (ppsh)
    {
        *ppsh = NULL;
        CryptHash *psh = new CryptHash(AlgorithmId, hProv);
        if (psh) 
        {
            if (psh->Init()) 
            {
                *ppsh = psh;
                fSucceed = true;
            }
            else
            {
                delete psh;
            }
        }
    }
    return fSucceed;
}

//
// CryptHash::SetCryptHashData
//
bool CryptHash::SetCryptHashData(LPCVOID pvBuf, size_t cbBuf) 
{
    return ::CryptHashData(m_hHash, PBYTE(pvBuf), static_cast<DWORD>(cbBuf), 0);
}

//
// CryptHash::GetCryptHashSize
//
DWORD CryptHash::GetCryptHashSize() const 
{
    if (m_hHash && m_hProv) 
    {        
        DWORD cbHash = 0;
        DWORD cbT = sizeof(cbHash);
        if (::CryptGetHashParam(m_hHash, HP_HASHSIZE,  PBYTE(&cbHash), &cbT, 0))
        {
            return cbHash;
        }
    }
    return 0;
}

//
// CryptHash::GetCryptHash()
//      caller needs to allocate memory for pvHash based on GetHashSize()
//
bool CryptHash::GetCryptHash(void *pvHash, DWORD cbHash) const 
{
    VSASSERT(cbHash == GetCryptHashSize(),"Wrong hash size");
    DWORD cbT = cbHash;
    bool fSucceed = ::CryptGetHashParam(m_hHash, HP_HASHVAL, PBYTE(pvHash), &cbT, 0);
    
    if(!fSucceed)
    {
        ::memset(pvHash, 0, cbHash);
    }
    
    return fSucceed;
}

//
// CryptHash::CompareCryptHash
//  return 1 if they are equal;0 if they are not equal;-1 for maybe 
//
int CryptHash::CompareCryptHash
(
    DWORD       cbHash,         //hash size
    LPCVOID     pvHash,         //hash value
    size_t      cbBuf,          //hash data size
    LPCVOID     pvBuf           //hash data
)
{
    int iRet = -1;

    if (GetCryptHashSize() == cbHash)
    {
        // set the hash data
        if (SetCryptHashData(pvBuf, cbBuf)) 
        {            
            BYTE* pHashValue = new BYTE[cbHash];

            if (pHashValue)
            {
                // get the hash value 
                GetCryptHash(pHashValue, cbHash);

                // if the hash values are equal
                if (0 == memcmp(pvHash, pHashValue, cbHash))
                {
                    iRet = 1;
                }
                else
                {
                    iRet = 0;
                }

                delete[] pHashValue;
            }
        }
    }

    return iRet;
}

bool CryptHash::Reset()
{
    UnInit();
    return Init();
}

//
// CryptHash::Close
//
void CryptHash::Close() 
{
    UnInit();
    delete this;
}

//
// CryptHash::Init
//
bool CryptHash::Init() 
{
    bool fSucceed = false;
    UnInit();

    // If the provider was not explicitly initialized before constructing this class (and we own it), attempt to initialize it now 
    if (m_hProv == NULL)
    {
        // Initilize with CRYPT_VERIFYCONTEXT  because CRYPT_NEWKEYSET will fail for
        // users in the Guest or Domain Guests group, or for a user with a roaming profile.
        // See Knowledge Base articles "Q265357 - Roaming Profiles Cannot Create Key Containers"
        // and "Q238187 - INFO CryptAcquireContext Use and Troubleshooting".
        // The temporary, in memory context obtained by CRYPT_VERIFYCONTEXT cannot be
        // used for anything that requires a secret key. It can be used for generating
        // hash values, which is what we use it for, and indeed this is the recommended
        // context to use for that purpose.
        BOOL dwFlags = CRYPT_VERIFYCONTEXT;
        
        if (m_AlgorithmId == CALG_SHA_256 || m_AlgorithmId == CALG_SHA_384 || m_AlgorithmId == CALG_SHA_512)
        {
            // SHA2 algorithms are supported by "Microsoft Enhanced RSA and AES Cryptographic Provider"
            // However, on WinXP the provider supporting SHA2 algorithms has a different name ("Microsoft Enhanced RSA and AES Cryptographic Provider (Prototype)")
            // To make things worse, WinXP 64bit has the same version number as Win2003 (5.2)
            // Therefore, instead of testing for the OS version, we'll test for the feature's presence 
            // by attempting to initialize the Vista-style provider, and falling back to XP-style provider if that failed
            if (!::CryptAcquireContext(&m_hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, dwFlags))
            {
                ::CryptAcquireContext(&m_hProv, NULL, MS_ENH_RSA_AES_PROV_XP, PROV_RSA_AES, dwFlags);
            }
        }
        else
        {
            // We'll use the default provider
            // If the provider created here doesn't support the requested algorithm, please initialize the appropriate provider yourself and pass the provider handle in the constructor.
            ::CryptAcquireContext(&m_hProv, NULL, NULL, PROV_RSA_FULL, dwFlags);
        }
    }

    if (m_hProv != NULL) 
    {
        if (::CryptCreateHash(m_hProv, m_AlgorithmId, 0, 0, &m_hHash)) 
        {
            fSucceed = true;
        }
    }

    return fSucceed;
}

//
// CryptHash::UnInit
//
void CryptHash::UnInit() 
{
    if (m_hHash) 
    {
        ::CryptDestroyHash(m_hHash);
    }
    if (m_hProv && m_bOwnsProv) 
    {
        ::CryptReleaseContext(m_hProv, 0);
        m_hProv = 0;
    }
    m_hHash = 0;
}
