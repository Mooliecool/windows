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
// File: crypt.h
// 
// ===========================================================================
/*++

Abstract:

    PAL RT Crypto APIs

Revision History:

--*/

/***************** SHA-1 message digest *************************/

#define SHA1DIGESTLEN 20  // Number of bytes output by SHA-1

typedef struct {
        DWORD magic_sha1;    // Magic value for A_SHA_CTX
        DWORD awaiting_data[16];
                             // Data awaiting full 512-bit block.
                             // Length (nbit_total[0] % 512) bits.
                             // Unused part of buffer (at end) is zero
        DWORD partial_hash[5];
                             // Hash through last full block
        DWORD nbit_total[2];       
                             // Total length of message so far
                             // (bits, mod 2^64)
} SHA1_CTX;

EXTERN_C BOOL PALAPI SHA1Init(SHA1_CTX*);
EXTERN_C BOOL PALAPI SHA1Update(SHA1_CTX*, const BYTE*, const DWORD);
EXTERN_C BOOL PALAPI SHA1Final(SHA1_CTX*, BYTE* digest);

/***************** MD5 message digest ***************************/

#define MD5DIGESTLEN 16  // Number of bytes output by MD5

typedef struct {
        DWORD magic_md5;     // Magic value for MD5_CTX
        DWORD awaiting_data[16];
                             // Data awaiting full 512-bit block.
                             // Length (nbit_total[0] % 512) bits.
                             // Unused part of buffer (at end) is zero.
        DWORD partial_hash[4];
                             // Hash through last full block
        DWORD nbit_total[2];       
                             // Total length of message so far
                             // (bits, mod 2^64)
} MD5_CTX;

EXTERN_C BOOL PALAPI MD5Init(MD5_CTX*);
EXTERN_C BOOL PALAPI MD5Update(MD5_CTX*, const BYTE*, const DWORD);
EXTERN_C BOOL PALAPI MD5Final(MD5_CTX*, BYTE* digest);

class BigNum
{
    typedef UINT32 BASETYPE;
    typedef UINT32 BASETYPE1; // type big enough to hold BASETYPE+BASETYPE
    typedef UINT32 BASETYPE2; // type big enough to hold BASETYPE*BASETYPE

    enum {
        BASETYPE_BITS = 16,
        BASETYPE_MASK = (1 << BASETYPE_BITS) - 1,
    };

    static inline BASETYPE SHAVE1(BASETYPE1 x)
    {
        return (BASETYPE)x & BASETYPE_MASK;
    }

    static inline BASETYPE SHAVE2(BASETYPE2 x)
    {
        return (BASETYPE)x & BASETYPE_MASK;
    }

    static inline BASETYPE CARRY1(BASETYPE1 x)
    {
        return (BASETYPE)(x >> BASETYPE_BITS);
    }

    static inline BASETYPE CARRY2(BASETYPE2 x)
    {
        return (BASETYPE)(x >> BASETYPE_BITS);
    }

    INT nLength;
    BASETYPE Value[1];

    static const struct t_Zero { INT nLength; } s_Zero;
    static const struct t_One { INT nLength; UINT32 Value[1]; } s_One;
    static const struct t_Two { INT nLength; UINT32 Value[1]; } s_Two;

public:
    static inline BigNum* Zero()
    {
        return (BigNum*)&s_Zero;
    }

    static inline BigNum* One()
    {
        return (BigNum*)&s_One;
    }

    static inline BigNum* Two()
    {
        return (BigNum*)&s_Two;
    }

    static inline void SetZero(BigNum *a)
    {
        a->nLength = 0;
    }

    static inline bool IsZero(BigNum *a)
    {
        return (a->nLength == 0);
    }

    static inline void SetSimple(BigNum *a, BASETYPE b)
    {
        _ASSERTE(0 < b && b <= BASETYPE_MASK);
        a->nLength = 1; a->Value[0] = b;
    }

    static inline bool IsSimple(BigNum *a, BASETYPE b)
    {
        _ASSERTE(0 < b && b <= BASETYPE_MASK);
        return (a->nLength == 1) && (a->Value[0] == b);
    }

    static inline void SetOne(BigNum *a)
    {
        SetSimple(a, 1);
    }

    static inline bool IsOne(BigNum *a)
    {
        return IsSimple(a, 1);
    }

    static inline void Set(BigNum *a, BigNum *b)
    {
        memcpy(a, b, offsetof(BigNum, Value) + b->nLength * sizeof(BASETYPE));
    }

    static inline bool IsBit(BigNum *a, UINT bit)
    {
        return !!((a->Value[bit / BASETYPE_BITS] & (1 << (bit % BASETYPE_BITS))));
    }

    static inline UINT GetByteSize(BigNum *a)
    {
        return (GetBitSize(a) + 7) / 8;
    }

    static inline UINT GetBufferSizeFromBits(UINT nBits)
    {
        return offsetof(BigNum, Value) + 
                sizeof(BASETYPE) + // for overflow area
                sizeof(BASETYPE) + // for rounding error
            (nBits * sizeof(BASETYPE)) / BASETYPE_BITS;
    }

    // the size of the buffer to preallocate for SetBytes
    static inline UINT GetBufferSize(UINT nBytes)
    {
        return GetBufferSizeFromBits(8 * nBytes);
    }

    static inline UINT GetBufferSize(BigNum* a)
    {
        return offsetof(BigNum, Value) +
                sizeof(BASETYPE) + // for overflow area
            a->nLength * sizeof(BASETYPE);
    }

    static void SetBytes(BigNum *a, CONST BYTE *pBlock, UINT cbBlock);
    static void GetBytes(BigNum *a, BYTE *pBlock, UINT cbBlock);

    static UINT GetBitSize(BigNum *a);
    static void Normalize(BigNum *a);

    static void SetBit(BigNum *a, UINT bit);

    // result = a+b, result can overlap with a or b
    static void Add(BigNum *sum, BigNum *a, BigNum *b);

    // result = a-b, result can overlap with a or b, return TRUE on overflow
    static BOOL Sub(BigNum *diff, BigNum *a, BigNum *b);

    // result = sign(a-b)
    static int Cmp(BigNum *a, BigNum *b);

    // a = a mod module, assumes module != 0
    static void Modulize(BigNum *a, BigNum *module);

    // a = a % b, d = a / b
    static void DivMod(BigNum *a, BigNum *module, BigNum *d);

    // result = a * b, result can't overlap, the buffer must be a->nLength * b->nLength
    static void Mul(BigNum *result, BigNum *a, BigNum *b);

    // result = a << bits
    static void Shl(BigNum *result, BigNum *a, UINT bits);

    // result = a >> bits
    static void Shr(BigNum *result, BigNum *a, UINT bits);

    // return max i such that 2^i*k = a
    static UINT GetPowersOfTwo(BigNum* a);

    // result = a ^ b mod module, assumes module != 0, a < module and a to be able to hold module^2
    static void PowMod(BigNum *result, BigNum *a, BigNum *b, BigNum *module);

    // result = 1/a mod module
    static BOOL InvMod(BigNum *result, BigNum* a, BigNum *module);

    // result = random number, GetBitSize(result) <= bits    
    static BOOL GenRandom(BigNum* a, UINT bits, BOOL bStrong);

    // result = random prime, GetBitSize(result) = bits
    static BOOL GenPrime(BigNum* result, UINT bits, BOOL bStrong);

    static BOOL TrialDivisionTest(BigNum* a, INT start, INT end);
    static BOOL FermatTest(BigNum* a, INT count);
    static BOOL RabinMillerTest(BigNum* a, INT count);
};



