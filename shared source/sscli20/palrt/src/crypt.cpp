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
// ===========================================================================
// File: crypt.cpp
// 
// PAL RT Crypto APIs
// ===========================================================================

#include "rotor_palrt.h"

#include "crypt.h"

/***************** Crypto base class ****************************/

class CryptProvider;
class CryptKey;
class CryptHash;

#define ADDREF(x)   ((CryptBase*)x)->AddRef()
#define RELEASE(x)  ((CryptBase*)x)->Release()

class CryptBase
{
    LONG m_RefCount;
    CryptProvider* m_pProvider;

public:
    CryptBase()
    {
        m_RefCount = 1;
        m_pProvider = NULL;
    }

    virtual ~CryptBase()
    {
        if (m_pProvider != NULL)
            RELEASE(m_pProvider);
    }

    void AddRef()
    {
        m_RefCount++;
    }

    void Release()
    {
        if (--m_RefCount == 0)
            delete this;
    }

    CryptProvider* GetProvider()
    {
        _ASSERTE(m_pProvider != NULL);
        return m_pProvider;
    }

    void SetProvider(CryptProvider* pProvider)
    {
        _ASSERTE(m_pProvider == NULL);
        m_pProvider = pProvider;
        ADDREF(m_pProvider);
    }
};

/***************** Crypto provider ******************************/

class CryptProvider : public CryptBase
{
    CryptKey* m_pSignatureKey;

public:
    CryptProvider()
    {
        m_pSignatureKey = NULL;
    }

    virtual ~CryptProvider()
    {
        if (m_pSignatureKey != NULL)
            RELEASE(m_pSignatureKey);
    }

    CryptKey* GetSignatureKey()
    {
        return m_pSignatureKey;
    }

    void SetSignatureKey(CryptKey *pKey)
    {
        CryptKey* pOldKey;

        if (pKey != NULL)
            ADDREF(pKey);

        pOldKey = m_pSignatureKey;
        m_pSignatureKey = pKey;

        if (pOldKey != NULL)
            RELEASE(pOldKey);
    }
};

/***************** virtual Hash base ****************************/

class CryptHash : public CryptBase
{
public:
    CryptHash()
    {
    }

    virtual ~CryptHash()
    {
    }

    virtual BOOL Initialize() = 0;

    virtual BOOL HashData(
        CONST BYTE *pbData,
        DWORD dwDataLen,
        DWORD dwFlags) = 0;

    virtual BOOL GetHashParam(
        DWORD dwParam,
        BYTE *pbData,
        DWORD *pdwDataLen,
        DWORD dwFlags) = 0;

    virtual BOOL GetSignatureMagic(
        BYTE *pbData,
        DWORD *pdwDataLen) = 0;
};

/***************** SHA-1 message digest *************************/

class SHA1Hash : public CryptHash
{
    SHA1_CTX    m_Context;
    BYTE        m_Value[SHA1DIGESTLEN];
    bool        m_fFinalized;

public:
    SHA1Hash()
    {
    }

    virtual ~SHA1Hash()
    {
    }

    virtual BOOL Initialize();

    virtual BOOL HashData(
        CONST BYTE *pbData,
        DWORD dwDataLen,
        DWORD dwFlags);

    virtual BOOL GetHashParam(
        DWORD dwParam,
        BYTE *pbData,
        DWORD *pdwDataLen,
        DWORD dwFlags);

    virtual BOOL GetSignatureMagic(
        BYTE *pbData,
        DWORD *pdwDataLen);
};

///////////////////////////////// SHA1Hash methods /////////////////////////////

BOOL SHA1Hash::Initialize()
{
    m_fFinalized = false;
    return SHA1Init(&m_Context);
}

BOOL SHA1Hash::HashData(
    CONST BYTE *pbData,
    DWORD dwDataLen,
    DWORD dwFlags)
{
    return SHA1Update(&m_Context, pbData, dwDataLen);
}

BOOL SHA1Hash::GetHashParam(
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags)
{
    DWORD dwDataLen = (pbData != NULL) ? *pdwDataLen : NULL;

    switch (dwParam)
    {
    case HP_ALGID:
        *pdwDataLen = sizeof(ALG_ID);
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        *(ALG_ID*)pbData = CALG_SHA1;
        return TRUE;

    case HP_HASHSIZE:
        *pdwDataLen = sizeof(DWORD);
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        *(DWORD*)pbData = SHA1DIGESTLEN;
        return TRUE;

    case HP_HASHVAL:
        *pdwDataLen = SHA1DIGESTLEN;
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        if (!m_fFinalized)
        {
            if (!SHA1Final(&m_Context, m_Value))
                return FALSE;
            m_fFinalized = true;
        }

        memcpy(pbData, m_Value, SHA1DIGESTLEN);
        return TRUE;

    default:
        SetLastError(NTE_BAD_TYPE);
        return FALSE;
    }

MoreData:
    if (pbData != NULL)
    {
        SetLastError(ERROR_MORE_DATA);
        return FALSE;
    }
    return TRUE;
}

BOOL SHA1Hash::GetSignatureMagic(
    BYTE *pbData,
    DWORD *pdwDataLen)
{
    // OID: 1.3.14.3.2.26
    static const BYTE data[] = 
	    {0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e, 0x03,0x02,0x1a,0x05,0x00,0x04,0x14};
    DWORD dwDataLen = *pdwDataLen;
    *pdwDataLen = sizeof(data);
    if (dwDataLen < *pdwDataLen)
        return FALSE;

    memcpy(pbData, data, sizeof(data));
    return TRUE;
}


/***************** MD5 message digest ***************************/

class MD5Hash : public CryptHash
{
    MD5_CTX     m_Context;
    BYTE        m_Value[MD5DIGESTLEN];
    bool        m_fFinalized;


public:
    MD5Hash()
    {
    }

    virtual ~MD5Hash()
    {
    }

    virtual BOOL Initialize();

    virtual BOOL HashData(
        CONST BYTE *pbData,
        DWORD dwDataLen,
        DWORD dwFlags);

    virtual BOOL GetHashParam(
        DWORD dwParam,
        BYTE *pbData,
        DWORD *pdwDataLen,
        DWORD dwFlags);

    virtual BOOL GetSignatureMagic(
        BYTE *pbData,
        DWORD *pdwDataLen);
};

///////////////////////// MD5Hash methods ///////////////////////////
// See comment inside SHA1Hash to understand why function definitions 
// must be outside class deifinition

BOOL MD5Hash::Initialize()
{
    m_fFinalized = false;
    return MD5Init(&m_Context);
}

BOOL MD5Hash::HashData(
    CONST BYTE *pbData,
    DWORD dwDataLen,
    DWORD dwFlags)
{
    return MD5Update(&m_Context, pbData, dwDataLen);
}

BOOL MD5Hash::GetHashParam(
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags)
{
    DWORD dwDataLen = (pbData != NULL) ? *pdwDataLen : NULL;

    switch (dwParam)
    {
    case HP_ALGID:
        *pdwDataLen = sizeof(ALG_ID);
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        *(ALG_ID*)pbData = CALG_MD5;
        return TRUE;

    case HP_HASHSIZE:
        *pdwDataLen = sizeof(DWORD);
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        *(DWORD*)pbData = MD5DIGESTLEN;
        return TRUE;

    case HP_HASHVAL:
        *pdwDataLen = MD5DIGESTLEN;
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        if (!m_fFinalized)
        {
            if (!MD5Final(&m_Context, m_Value))
                return FALSE;
            m_fFinalized = true;
        }

        memcpy(pbData, m_Value, MD5DIGESTLEN);
        return TRUE;

    default:
        SetLastError(NTE_BAD_TYPE);
        return FALSE;
    }

MoreData:
    if (pbData != NULL)
    {
        SetLastError(ERROR_MORE_DATA);
        return FALSE;
    }
    return TRUE;
}

BOOL MD5Hash::GetSignatureMagic(
    BYTE *pbData,
    DWORD *pdwDataLen)
{
    // OID: 1.2.840.0113549.2.5
    static const BYTE data[] = 
	    {0x30,0x20,0x30,0x0c,0x06,0x08,0x2a,0x86, 0x48,0x86,0xf7,0x0d,0x02,0x05,0x05,0x00,0x04,0x10};
    DWORD dwDataLen = *pdwDataLen;
    *pdwDataLen = sizeof(data);
    if (dwDataLen < *pdwDataLen)
        return FALSE;

    memcpy(pbData, data, sizeof(data));
    return TRUE;
}


/***************** virtual Key base *****************************/

struct BLOBHEADER {
    BYTE    bType;
    BYTE    bVersion;
    WORD    reserved;
    ALG_ID  aiKeyAlg;
};

class CryptKey : public CryptBase
{
    DWORD m_dwFlags; // only CRYPT_EXPORTABLE for now

public:
    CryptKey()
    {
        m_dwFlags = 0;
    }

    virtual ~CryptKey()
    {
    }

    virtual BOOL GenKey(DWORD dwFlags) = 0;

    virtual BOOL ImportKey(DWORD dwFlags, CONST BYTE *pbData, DWORD dwDataLen) = 0;
    virtual BOOL ExportKey(DWORD dwBlobType, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen) = 0;

    virtual BOOL GetKeyParam(DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags) = 0;

    virtual DWORD GetSignatureLength() = 0;

    virtual BOOL EncryptSignature(CONST BYTE *pbSrc, BYTE *pbDest) = 0;
    virtual BOOL DecryptSignature(CONST BYTE *pbSrc, BYTE *pbDest) = 0;

    void SetFlags(DWORD dwFlags)
    {
        m_dwFlags = dwFlags;
    }

    inline DWORD GetFlags()
    {
        return m_dwFlags;
    }
};

/***************** RSA crypt key ********************************/

struct RSAPUBKEY {
    DWORD   magic;
    DWORD   bitlen;
    DWORD   pubexp;
};

class RSAKey : public CryptKey
{
    PVOID   m_pBlob;
    DWORD   m_dwSize;

    DWORD   m_dwBitLen;
    BigNum* m_pExponent; // The exponent.
    BigNum* m_pModulus; // The modulus. This has a value of "prime1 * prime2" and is often known as "n". 

    // private-only fields
    BigNum* m_pPrime1; // Prime number 1, often known as "p". 
    BigNum* m_pPrime2; // Prime number 2, often known as "q". 
    BigNum* m_pExponent1; // Exponent 1. This has a numeric value of "d mod (p - 1)". 
    BigNum* m_pExponent2; // Exponent 2. This has a numeric value of "d mod (q - 1)". 
    BigNum* m_pCoefficient; // Coefficient. This has a numeric value of "(inverse of q) mod p". 
    BigNum* m_pPrivateExponent; // Private exponent, often known as "d". 

    BOOL CheckKey();
    BOOL GenerateKey();
    void ClearStack();

public:
    RSAKey();
    virtual ~RSAKey();
    virtual BOOL GenKey(DWORD dwFlags);
    virtual BOOL ImportKey(DWORD dwFlags, CONST BYTE *pbData, DWORD dwDataLen);
    virtual BOOL ExportKey(DWORD dwBlobType, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
    virtual BOOL GetKeyParam(DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags);
    virtual DWORD GetSignatureLength();
    virtual BOOL EncryptSignature(CONST BYTE *pbSrc, BYTE *pbDest);
    virtual BOOL DecryptSignature(CONST BYTE *pbSrc, BYTE *pbDest);
};

//////////////// RSAKey private methods ////////////////
// See comment inside SHA1Hash to understand why function definitions 
// must be outside class deifinition

BOOL RSAKey::CheckKey()
{
    DWORD dwBufLen;
    BigNum* pTmp1;
    BigNum* pTmp2;
    BigNum* pTmp3;

#ifdef _DEBUG
    // check the bit lengths
    _ASSERTE(BigNum::GetBitSize(m_pExponent) <= 32);
    _ASSERTE(BigNum::GetBitSize(m_pModulus) <= m_dwBitLen);

    _ASSERTE(BigNum::GetBitSize(m_pPrime1) == m_dwBitLen/2);
    _ASSERTE(BigNum::GetBitSize(m_pPrime2) == m_dwBitLen/2);
    _ASSERTE(BigNum::GetBitSize(m_pExponent1) <= m_dwBitLen/2);
    _ASSERTE(BigNum::GetBitSize(m_pExponent2) <= m_dwBitLen/2);
    _ASSERTE(BigNum::GetBitSize(m_pCoefficient) <= m_dwBitLen/2);
    _ASSERTE(BigNum::GetBitSize(m_pPrivateExponent) <= m_dwBitLen);
#endif

    dwBufLen = BigNum::GetBufferSizeFromBits(2 * m_dwBitLen);

    pTmp1 = (BigNum*)alloca(dwBufLen);
    pTmp2 = (BigNum*)alloca(dwBufLen);
    pTmp3 = (BigNum*)alloca(dwBufLen);

    // m_pModulus == m_pPrime1 * m_pPrime2
    BigNum::Mul(pTmp1, m_pPrime1, m_pPrime2);
    if (BigNum::Cmp(pTmp1, m_pModulus) != 0)
        return FALSE;

    // m_pExponent1 == m_pPrivateExponent % (m_pPrime1 - 1)
    BigNum::Sub(pTmp1, m_pPrime1, BigNum::One());
    BigNum::Set(pTmp2, m_pPrivateExponent);
    BigNum::Modulize(pTmp2, pTmp1);
    if (BigNum::Cmp(pTmp2, m_pExponent1) != 0)
        return FALSE;

    // m_pExponent2 == m_pPrivateExponent % (m_pPrime2 - 1)
    BigNum::Sub(pTmp1, m_pPrime2, BigNum::One());
    BigNum::Set(pTmp2, m_pPrivateExponent);
    BigNum::Modulize(pTmp2, pTmp1);
    if (BigNum::Cmp(pTmp2, m_pExponent2) != 0)
        return FALSE;

    // 1 == (m_pCoefficient * m_pPrime2) % m_pPrime1
    BigNum::Mul(pTmp1, m_pCoefficient, m_pPrime2);
    BigNum::Modulize(pTmp1, m_pPrime1);
    if (!BigNum::IsOne(pTmp1))
        return FALSE;

    // 1 == (m_pExponent * m_pPrivateExponent) % ((m_pPrime1 - 1) * (m_pPrime2 - 1))
    BigNum::Sub(pTmp1, m_pPrime1, BigNum::One());
    BigNum::Sub(pTmp2, m_pPrime2, BigNum::One());
    BigNum::Mul(pTmp3, pTmp1, pTmp2);
    BigNum::Mul(pTmp1, m_pExponent, m_pPrivateExponent);
    BigNum::Modulize(pTmp1, pTmp3);
    if (!BigNum::IsOne(pTmp1))
        return FALSE;

#ifdef _DEBUG
    // Roundtrip check: 123 = (123 ^ m_pPrivateExponent) ^ m_pExponent
    BigNum::SetSimple(pTmp1, 123);
    BigNum::Set(pTmp2, pTmp1);
    BigNum::PowMod(pTmp1, pTmp1, m_pExponent, m_pModulus);
    BigNum::PowMod(pTmp1, pTmp1, m_pPrivateExponent, m_pModulus);
    _ASSERTE(BigNum::Cmp(pTmp1, pTmp2) == 0);
#endif

    return TRUE;
}

BOOL RSAKey::GenerateKey()
{
    DWORD dwBufferSize;
    DWORD dwBufferSize2;
    BigNum* pTmp1;
    BigNum* pTmp2;
    BigNum* pTmp3;

    dwBufferSize = BigNum::GetBufferSizeFromBits(m_dwBitLen);
    dwBufferSize2 = BigNum::GetBufferSizeFromBits(m_dwBitLen/2);

    pTmp1 = (BigNum*)alloca(dwBufferSize2);
    pTmp2 = (BigNum*)alloca(dwBufferSize2);
    pTmp3 = (BigNum*)alloca(dwBufferSize);

again:
    if (!BigNum::GenPrime(m_pPrime1, m_dwBitLen/2, TRUE))
        return FALSE;
    if (!BigNum::GenPrime(m_pPrime2, m_dwBitLen/2, TRUE))
        return FALSE;

    // m_pModulus = m_pPrime1 * m_pPrime2
    BigNum::Mul(m_pModulus, m_pPrime1, m_pPrime2);

    // m_pCoeffiecient = 1/m_pPrime2 % m_pPrime1
    if (!BigNum::InvMod(m_pCoefficient, m_pPrime2, m_pPrime1))
        goto again;

    BigNum::Sub(pTmp1, m_pPrime1, BigNum::One());
    BigNum::Sub(pTmp2, m_pPrime2, BigNum::One());

    // m_pPrivateExponent = 1/m_pExponent % ((m_pPrime1-1)*(m_pPrime2-1))
    BigNum::Mul(pTmp3, pTmp1, pTmp2);
    if (!BigNum::InvMod(m_pPrivateExponent, m_pExponent, pTmp3))
        goto again;

    // m_pExponent1 = m_pPrivateExponent % (m_pPrime1-1)
    BigNum::Set(pTmp3, m_pPrivateExponent);
    BigNum::Modulize(pTmp3, pTmp1);
    BigNum::Set(m_pExponent1, pTmp3);

    // m_pExponent2 = m_pPrivateExponent % (m_pPrime2-1)
    BigNum::Set(pTmp3, m_pPrivateExponent);
    BigNum::Modulize(pTmp3, pTmp2);
    BigNum::Set(m_pExponent2, pTmp3);

    return TRUE;
}

void RSAKey::ClearStack()
{
    DWORD dwSize = 8 * BigNum::GetBufferSizeFromBits(2 * m_dwBitLen) + 256 * sizeof(void*);
    void* ptr = alloca(dwSize);
    memset(ptr, 0, dwSize);
}

////////////////// RSAKey public methods ///////////////////////////

RSAKey::RSAKey()
{
    m_pBlob = NULL;

    m_pExponent = NULL;
    m_pModulus = NULL;

    m_pPrime1 = NULL;
    m_pPrime2 = NULL;
    m_pExponent1 = NULL;
    m_pExponent2 = NULL;
    m_pCoefficient = NULL;
    m_pPrivateExponent = NULL;
}

RSAKey::~RSAKey()
{
    if (m_pBlob != NULL)
    {
        // clear the blob to avoid potential exposure of sensitive bits
        memset(m_pBlob, 0, m_dwSize);
        free(m_pBlob);
    }
}

BOOL RSAKey::GenKey(DWORD dwFlags)
{
    DWORD dwBitLen;
    DWORD dwByteLen1;
    DWORD dwByteLen2;
    DWORD dwBufferSize1;
    DWORD dwBufferSize2;
    BYTE *pb;

    dwBitLen = dwFlags >> 16;

    // use the default if not specified
    if (dwBitLen == 0)
        dwBitLen = 1024;

    if ((dwBitLen % 16 != 0) || 
        (dwBitLen > 16384))
    {
        SetLastError(NTE_BAD_FLAGS);
        return FALSE;
    }
    m_dwBitLen = dwBitLen;

    dwByteLen1 = dwBitLen / 8;
    dwByteLen2 = dwByteLen1 / 2;

    dwBufferSize1 = BigNum::GetBufferSize(dwByteLen1);
    dwBufferSize2 = BigNum::GetBufferSize(dwByteLen2);

    m_dwSize = BigNum::GetBufferSize(sizeof(DWORD)) + 
            (2 * dwBufferSize1 + 5 * dwBufferSize2);
    pb = (BYTE*)malloc(m_dwSize);
    if (pb == NULL)
    {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
    }
    m_pBlob = pb;

    m_pExponent = (BigNum*)pb;
    pb += BigNum::GetBufferSize(sizeof(DWORD));

static const BYTE c_DefaultExponent[4] = { 1, 0, 1, 0 };
    BigNum::SetBytes(m_pExponent, c_DefaultExponent, sizeof(DWORD));

#define HELPER(m_pMember, size) \
    m_pMember = (BigNum*)pb; \
    pb += dwBufferSize##size;

    HELPER(m_pModulus, 1);

    HELPER(m_pPrime1, 2);
    HELPER(m_pPrime2, 2);
    HELPER(m_pExponent1, 2);
    HELPER(m_pExponent2, 2);
    HELPER(m_pCoefficient, 2);
    HELPER(m_pPrivateExponent, 1);
#undef HELPER

    if (!GenerateKey())
    {
        ClearStack();
        return FALSE;
    }

    if (!CheckKey())
    {                       
        SetLastError(E_UNEXPECTED);
        _ASSERTE(false);

        ClearStack();
        return FALSE;
    }        

    GetProvider()->SetSignatureKey(this);
    return TRUE;
}

BOOL RSAKey::ImportKey(DWORD dwFlags, CONST BYTE *pbData, DWORD dwDataLen)
{            
    BOOL bPrivate;
    DWORD dwBitLen;
    DWORD dwByteLen1;
    DWORD dwByteLen2;
    DWORD dwBufferSize1;
    DWORD dwBufferSize2;
    BYTE *pb;
    BLOBHEADER *pBlobHeader;
    RSAPUBKEY* pRSAPubKey;

    _ASSERTE(dwDataLen >= sizeof(BLOBHEADER));
    pBlobHeader = (BLOBHEADER*)pbData;

    pbData += sizeof(BLOBHEADER);
    dwDataLen -= sizeof(BLOBHEADER);

    switch (pBlobHeader->bType)
    {
    case PUBLICKEYBLOB:
        bPrivate = FALSE;
        break;
    case PRIVATEKEYBLOB:
        bPrivate = TRUE;
        break;
    default:
        goto BadKey;
    }

    if (dwDataLen < sizeof(RSAPUBKEY))
    {
        goto BadKey;
    }
    pRSAPubKey = (RSAPUBKEY*)pbData;

    pbData += sizeof(RSAPUBKEY);
    dwDataLen -= sizeof(RSAPUBKEY);

    if (GET_UNALIGNED_VAL32(&pRSAPubKey->magic) != 
        (bPrivate ? 0x32415352U : 0x31415352U)) // 'RSA2' : 'RSA1'
    {
        goto BadKey;
    }

    dwBitLen = GET_UNALIGNED_VAL32(&pRSAPubKey->bitlen);

    if ((dwBitLen == 0) ||
        (dwBitLen % 16 != 0) ||
        (dwBitLen > 16384))
    {
        goto BadKey;
    }
    m_dwBitLen = dwBitLen;

    dwByteLen1 = dwBitLen / 8;
    dwByteLen2 = dwByteLen1 / 2;

    if (dwDataLen != (bPrivate ? (9 * dwByteLen2) : dwByteLen1))
    {
        goto BadKey;
    }

    dwBufferSize1 = BigNum::GetBufferSize(dwByteLen1);
    dwBufferSize2 = BigNum::GetBufferSize(dwByteLen2);

    m_dwSize = BigNum::GetBufferSize(sizeof(DWORD)) + 
            (bPrivate ? (2 * dwBufferSize1 + 5 * dwBufferSize2) : dwBufferSize1);
    pb = (BYTE*)malloc(m_dwSize);
    if (pb == NULL)
    {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
    }
    m_pBlob = pb;

    m_pExponent = (BigNum*)pb;
    pb += BigNum::GetBufferSize(sizeof(DWORD));
    BigNum::SetBytes(m_pExponent, (BYTE*)&pRSAPubKey->pubexp, sizeof(DWORD));

#define HELPER(m_pMember, size) \
    m_pMember = (BigNum*)pb; \
    pb += dwBufferSize##size; \
    BigNum::SetBytes(m_pMember, pbData, dwByteLen##size); \
    pbData += dwByteLen##size;

    HELPER(m_pModulus, 1);

    if (!bPrivate)
        return TRUE;

    HELPER(m_pPrime1, 2);
    HELPER(m_pPrime2, 2);
    HELPER(m_pExponent1, 2);
    HELPER(m_pExponent2, 2);
    HELPER(m_pCoefficient, 2);
    HELPER(m_pPrivateExponent, 1);
#undef HELPER

    if (!CheckKey())
    {
        ClearStack();
        goto BadKey;
    }

    ClearStack();

    GetProvider()->SetSignatureKey(this);
    return TRUE;

BadKey:
    SetLastError(NTE_BAD_KEY);
    return FALSE;
}

BOOL RSAKey::ExportKey(DWORD dwBlobType, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen)
{
    DWORD dwByteLen1;
    DWORD dwByteLen2;
    DWORD dwDataLen;
    BOOL bPrivate;
    BLOBHEADER *pBlobHeader;
    RSAPUBKEY* pRSAPubKey;

    if ((dwBlobType != PUBLICKEYBLOB) && 
        ((dwBlobType != PRIVATEKEYBLOB) || (m_pPrivateExponent == NULL)))
    {
        SetLastError(NTE_BAD_TYPE);
        return FALSE;
    }

    bPrivate = (dwBlobType != PUBLICKEYBLOB);

    if (dwFlags != 0)
    {
        SetLastError(NTE_BAD_FLAGS);
        return FALSE;
    }

    dwByteLen1 = m_dwBitLen / 8;
    dwByteLen2 = dwByteLen1 / 2;

    dwDataLen = (pbData != NULL) ? *pdwDataLen : 0;
    *pdwDataLen = sizeof(BLOBHEADER) + sizeof(RSAPUBKEY) + 
        (bPrivate ? (2 * dwByteLen1 + 5 * dwByteLen2) : dwByteLen1);
    if (dwDataLen < *pdwDataLen)
    {
        if (pbData != NULL)
        {
            SetLastError(ERROR_MORE_DATA);
            return FALSE;
        }
        return TRUE;
    }

    pBlobHeader = (BLOBHEADER *)pbData;
    pbData += sizeof(BLOBHEADER);

    pBlobHeader->bType = (BYTE)dwBlobType;
    pBlobHeader->bVersion = 0x02;
    SET_UNALIGNED_VAL16(&pBlobHeader->reserved, 0x0000);
    SET_UNALIGNED_VAL32(&pBlobHeader->aiKeyAlg, CALG_RSA_SIGN);

    pRSAPubKey = (RSAPUBKEY*)pbData;
    pbData += sizeof(RSAPUBKEY);

    SET_UNALIGNED_VAL32(&pRSAPubKey->magic, bPrivate ? 0x32415352 : 0x31415352); // 'RSA2' : 'RSA1'
    SET_UNALIGNED_VAL32(&pRSAPubKey->bitlen, m_dwBitLen);

    BigNum::GetBytes(m_pExponent, (BYTE*)&pRSAPubKey->pubexp, sizeof(DWORD));

#define HELPER(m_pMember, size) \
    BigNum::GetBytes(m_pMember, pbData, dwByteLen##size); \
    pbData += dwByteLen##size;

    HELPER(m_pModulus, 1);

    if (dwBlobType == PUBLICKEYBLOB)
        return TRUE;

    HELPER(m_pPrime1, 2);
    HELPER(m_pPrime2, 2);
    HELPER(m_pExponent1, 2);
    HELPER(m_pExponent2, 2);
    HELPER(m_pCoefficient, 2);
    HELPER(m_pPrivateExponent, 1);
#undef HELPER

    return TRUE;
}

BOOL RSAKey::GetKeyParam(DWORD dwParam, BYTE *pbData, DWORD *pdwDataLen, DWORD dwFlags)
{
    DWORD dwDataLen = (pbData != NULL) ? *pdwDataLen : NULL;

    switch (dwParam)
    {
    case KP_ALGID:
        *pdwDataLen = sizeof(ALG_ID);
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        *(ALG_ID*)pbData = CALG_RSA_SIGN;
        return TRUE;

    case KP_KEYLEN:
        *pdwDataLen = sizeof(DWORD);
        if (dwDataLen < *pdwDataLen)
            goto MoreData;

        *(DWORD*)pbData = m_dwBitLen;
        return TRUE;

    default:
        SetLastError(NTE_BAD_TYPE);
        return FALSE;
    }

MoreData:
    if (pbData != NULL)
    {
        SetLastError(ERROR_MORE_DATA);
        return FALSE;
    }
    return TRUE;
}

DWORD RSAKey::GetSignatureLength()
{
    return m_dwBitLen / 8;
}

BOOL RSAKey::EncryptSignature(CONST BYTE *pbSrc, BYTE *pbDest)
{
    DWORD dwBufferSize;
    BigNum * pResult;
    DWORD dwLen;

    _ASSERTE(m_pPrivateExponent != NULL);
    
    dwBufferSize = BigNum::GetBufferSizeFromBits(m_dwBitLen);
    pResult = (BigNum*)alloca(dwBufferSize);

    dwLen = GetSignatureLength();

    BigNum::SetBytes(pResult, pbSrc, dwLen);
    BigNum::PowMod(pResult, pResult, m_pPrivateExponent, m_pModulus);
    BigNum::GetBytes(pResult, pbDest, dwLen);

    return TRUE;
}

BOOL RSAKey::DecryptSignature(CONST BYTE *pbSrc, BYTE *pbDest)
{
    DWORD dwBufferSize;
    BigNum * pResult;
    DWORD dwLen;

    dwBufferSize = BigNum::GetBufferSizeFromBits(m_dwBitLen);
    pResult = (BigNum*)alloca(dwBufferSize);

    dwLen = GetSignatureLength();

    BigNum::SetBytes(pResult, pbSrc, dwLen);
    BigNum::PowMod(pResult, pResult, m_pExponent, m_pModulus);
    BigNum::GetBytes(pResult, pbDest, dwLen);

    return TRUE;
}

/***************** Crypto APIs **********************************/

EXTERN_C
BOOL
PALAPI
CryptAcquireContextA(
    HCRYPTPROV *phProv,
    LPCSTR szContainer,
    LPCSTR szProvider,
    DWORD dwProvType,
    DWORD dwFlags)
{
    _ASSERTE(szContainer == NULL);
    _ASSERTE(szProvider == NULL);

    return CryptAcquireContextW(phProv, NULL, NULL, dwProvType, dwFlags);
}

EXTERN_C
BOOL
PALAPI
CryptAcquireContextW(
    HCRYPTPROV *phProv,
    LPCWSTR szContainer,
    LPCWSTR szProvider,
    DWORD dwProvType,
    DWORD dwFlags)
{
    CryptProvider* pProvider;

    _ASSERTE(szContainer == NULL);
    _ASSERTE(szProvider == NULL);

    pProvider = new CryptProvider();
    if (pProvider == NULL)
    {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
    }

    *phProv = (HCRYPTPROV)pProvider;
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptReleaseContext(
    HCRYPTPROV hProv,
    DWORD dwFlags)
{
    if (hProv == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    RELEASE(hProv);
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptCreateHash(
    HCRYPTPROV hProv,
    ALG_ID Algid,
    HCRYPTKEY hKey,
    DWORD dwFlags,
    HCRYPTHASH *phHash)
{
    CryptHash * pHash;

    if (hProv == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (hKey != NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (dwFlags != 0)
    {
        SetLastError(NTE_BAD_FLAGS);
        return FALSE;
    }

    switch (Algid)
    {
    case CALG_MD5:
        pHash = new MD5Hash();
        break;

    case CALG_SHA1:
        pHash = new SHA1Hash();
        break;

    default:
        SetLastError(NTE_BAD_ALGID);
        return FALSE;
    }

    if (pHash == NULL)
    {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
    }

    pHash->SetProvider((CryptProvider*)hProv);

    if (!pHash->Initialize())
    {
        RELEASE(pHash);
        return FALSE;
    }

    *phHash = (HCRYPTHASH)pHash;
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptDestroyHash(
    HCRYPTHASH hHash)
{
    if (hHash == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    RELEASE(hHash);
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptHashData(
    HCRYPTHASH hHash,
    CONST BYTE *pbData,
    DWORD dwDataLen,
    DWORD dwFlags)
{
    if (hHash == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((CryptHash*)hHash)->HashData(pbData, dwDataLen, dwFlags);
}

EXTERN_C
BOOL
PALAPI
CryptGetHashParam(
    HCRYPTHASH hHash,
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags)
{
    if (hHash == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((CryptHash*)hHash)->GetHashParam(dwParam, pbData, pdwDataLen, dwFlags);
}

static BOOL CreateKeyObject(HCRYPTPROV hProv, ALG_ID Algid, CryptKey** ppKey)
{
    CryptKey* pKey = NULL;

    switch (Algid)
    {
    case CALG_RSA_SIGN:
        pKey = new RSAKey();
        break;

   default:
        SetLastError(NTE_BAD_ALGID);
        return FALSE;
    }

    if (pKey == NULL)
    {
        SetLastError(ERROR_OUTOFMEMORY);
        return FALSE;
    }

    pKey->SetProvider((CryptProvider*)hProv);

    *ppKey = pKey;
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptGenKey(
    HCRYPTPROV hProv,
    ALG_ID Algid,
    DWORD dwFlags,
    HCRYPTKEY *phKey)
{
    CryptKey* pKey;

    if (hProv == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    // substitute the default algorithm
    if (Algid == AT_SIGNATURE)
        Algid = CALG_RSA_SIGN;

    if (!CreateKeyObject(hProv, Algid, &pKey))
    {
        return FALSE;
    }

    pKey->SetFlags(dwFlags & CRYPT_EXPORTABLE);

    if (!pKey->GenKey(dwFlags))
    {
        RELEASE(pKey);
        return FALSE;
    }

    *phKey = (HCRYPTKEY)pKey;
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptDestroyKey(
    HCRYPTKEY hKey)
{
    if (hKey == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    RELEASE(hKey);
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptImportKey(
    HCRYPTPROV hProv,
    CONST BYTE *pbData,
    DWORD dwDataLen,
    HCRYPTKEY hPubKey,
    DWORD dwFlags,
    HCRYPTKEY *phKey)
{
    CryptKey* pKey;
    BLOBHEADER *pBlobHeader;

    if (hProv == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (hPubKey != NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (dwDataLen < sizeof(BLOBHEADER))
    {
        goto BadKey;
    }
    pBlobHeader = (BLOBHEADER*)pbData;

    if ((pBlobHeader->bVersion != 0x02) || GET_UNALIGNED_VAL16(&pBlobHeader->reserved) != 0x0000)
    {
        goto BadKey;
    }

    if (!CreateKeyObject(hProv, GET_UNALIGNED_VAL32(&pBlobHeader->aiKeyAlg), &pKey))
    {
        return FALSE;
    }

    pKey->SetFlags(dwFlags & CRYPT_EXPORTABLE);

    if (!pKey->ImportKey(dwFlags, pbData, dwDataLen))
    {
        RELEASE(pKey);
        return FALSE;
    }

    *phKey = (HCRYPTKEY)pKey;
    return TRUE;

BadKey:
    SetLastError(NTE_BAD_KEY);
    return FALSE;
}

EXTERN_C
BOOL
PALAPI
CryptExportKey(
    HCRYPTKEY hKey,
    HCRYPTKEY hExpKey,
    DWORD dwBlobType,
    DWORD dwFlags,
    BYTE *pbData,
    DWORD *pdwDataLen)
{
    if (hKey == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (hExpKey != NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if ((dwBlobType == PRIVATEKEYBLOB) && !(((CryptKey*)hKey)->GetFlags() & CRYPT_EXPORTABLE))
    {
        SetLastError(NTE_BAD_KEY_STATE);
        return FALSE;
    }

    return ((CryptKey*)hKey)->ExportKey(dwBlobType, dwFlags, pbData, pdwDataLen);
}

EXTERN_C
BOOL
PALAPI
CryptGetKeyParam(
    HCRYPTKEY hKey,
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags)
{
    if (hKey == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return ((CryptKey*)hKey)->GetKeyParam(dwParam, pbData, pdwDataLen, dwFlags);
}

static void meminv(void* p, size_t len)
{
    BYTE* p1 = (BYTE*)p;
    BYTE* p2 = (BYTE*)p + (len-1);

    while (p1 < p2)
    {
        BYTE t = *p1;
        *p1 = *p2;
        *p2 = t;
        p1++; p2--;
    }
}

static BOOL GetSignature(HCRYPTHASH hHash, BYTE *pbSignature, DWORD dwSize)
{
    DWORD dwHashLen;
    DWORD dwOIDLen;

    dwHashLen = dwSize;
    if (!((CryptHash*)hHash)->GetHashParam(HP_HASHVAL, pbSignature, &dwHashLen, 0))
    {
        return FALSE;
    }

    meminv(pbSignature, dwHashLen);

    dwSize -= dwHashLen;
    pbSignature += dwHashLen;

    if (dwSize <= 3)
    {
        SetLastError(NTE_BAD_SIGNATURE);
        return FALSE;
    }

    dwOIDLen = dwSize - 3;
    if (!((CryptHash*)hHash)->GetSignatureMagic(pbSignature, &dwOIDLen))
    {
        SetLastError(NTE_BAD_SIGNATURE);
        return FALSE;
    }

    meminv(pbSignature, dwOIDLen);

    pbSignature += dwOIDLen;
    dwSize -= dwOIDLen;

    *pbSignature++ = 0x00; // reserved
    dwSize--;

    while (dwSize > 2)
    {
        *pbSignature++ = 0xFF; // padding
        dwSize--;
    }

    *pbSignature++ = 0x01; // block type
    dwSize--;
    *pbSignature++ = 0x00; // reserved
    dwSize--;

    _ASSERTE(dwSize == 0);
    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptVerifySignatureA(
    HCRYPTHASH hHash,
    CONST BYTE *pbSignature,
    DWORD dwSigLen,
    HCRYPTKEY hPubKey,
    LPCSTR szDescription,
    DWORD dwFlags)
{
    _ASSERTE(szDescription == NULL);
    return CryptVerifySignatureW(hHash, pbSignature, dwSigLen,
                                    hPubKey, NULL, dwFlags);
}

EXTERN_C
BOOL
PALAPI
CryptVerifySignatureW(
    HCRYPTHASH hHash,
    CONST BYTE *pbSignature,
    DWORD dwSigLen,
    HCRYPTKEY hPubKey,
    LPCWSTR szDescription,
    DWORD dwFlags)
{
    BYTE *pbDecryptedSignature;
    BYTE *pbMasterSignature;
    CryptKey *pKey;

    if ((hHash == NULL) || (hPubKey == NULL))
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (dwFlags != 0)
    {
        SetLastError(NTE_BAD_FLAGS);
        return FALSE;
    }

    _ASSERTE(szDescription == NULL);

    pKey = (CryptKey*)hPubKey;

    if (dwSigLen != pKey->GetSignatureLength())
    {
        SetLastError(NTE_SIGNATURE_FILE_BAD);
        return FALSE;
    }

    pbDecryptedSignature = (BYTE*)alloca(dwSigLen);
    if (!pKey->DecryptSignature(pbSignature, pbDecryptedSignature))
    {
        return FALSE;
    }

    pbMasterSignature = (BYTE*)alloca(dwSigLen);
    if (!GetSignature(hHash, pbMasterSignature, dwSigLen))
    {
        return FALSE;
    }

    if (memcmp(pbMasterSignature, pbDecryptedSignature, dwSigLen) != 0)
    {
        SetLastError(NTE_BAD_SIGNATURE);
        return FALSE;
    }

    return TRUE;
}

EXTERN_C
BOOL
PALAPI
CryptSignHashA(
    HCRYPTHASH hHash,
    DWORD dwKeySpec,
    LPCSTR szDescription,
    DWORD dwFlags,
    BYTE *pbSignature,
    DWORD *pdwSigLen)
{
    _ASSERTE(szDescription == NULL);
    return CryptSignHashW(hHash, dwKeySpec, NULL, dwFlags,
                                pbSignature, pdwSigLen);
}

EXTERN_C
BOOL
PALAPI
CryptSignHashW(
    HCRYPTHASH hHash,
    DWORD dwKeySpec,
    LPCWSTR szDescription,
    DWORD dwFlags,
    BYTE *pbSignature,
    DWORD *pdwSigLen)
{
    CryptKey* pKey;
    BYTE* pbMasterSignature;
    BYTE* pbDecryptedSignature;
    DWORD dwSigLen;
    DWORD dwDataLen;

    if (hHash == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (dwKeySpec != AT_SIGNATURE)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    _ASSERTE(szDescription == NULL);

    if (dwFlags != 0)
    {
        SetLastError(NTE_BAD_FLAGS);
        return FALSE;
    }

    pKey = ((CryptHash*)hHash)->GetProvider()->GetSignatureKey();
    if (pKey == NULL)
    {
        SetLastError(NTE_NO_KEY);
        return FALSE;
    }

    dwSigLen = pKey->GetSignatureLength();

    dwDataLen = (pbSignature != NULL) ? *pdwSigLen : 0;
    *pdwSigLen = dwSigLen;
    if (dwDataLen < *pdwSigLen)
    {
        if (pbSignature != NULL)
        {
            SetLastError(ERROR_MORE_DATA);
            return FALSE;
        }
        return TRUE;
    }

    pbMasterSignature = (BYTE*)alloca(dwSigLen);
    if (!GetSignature(hHash, pbMasterSignature, dwSigLen))
    {
        return FALSE;
    }

    if (!pKey->EncryptSignature(pbMasterSignature, pbSignature))
    {
        return FALSE;
    }

    // check the roundtrip
    pbDecryptedSignature = (BYTE*)alloca(dwSigLen);
    if (!pKey->DecryptSignature(pbSignature, pbDecryptedSignature))
    {
        return FALSE;
    }

    if (memcmp(pbMasterSignature, pbDecryptedSignature, dwSigLen) != 0)
    {
        _ASSERTE(false);
        SetLastError(NTE_FAIL);
        return FALSE;
    }

    return TRUE;
}
