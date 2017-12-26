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
// File: BigNum.CPP
// 
// A simple naive portable implementation of BigNum arithmetic 
// for PALRT public/private key RSA crypto
// ===========================================================================

#include "rotor_palrt.h"

#include "crypt.h"

const BigNum::t_Zero BigNum::s_Zero = { 0 };
const BigNum::t_One BigNum::s_One = { 1, { 1 } };
const BigNum::t_Two BigNum::s_Two = { 1, { 2 } };

UINT BigNum::GetBitSize(BigNum *a)
{
    if (a->nLength == 0)
        return 0;

    UINT c = (a->nLength-1) * BASETYPE_BITS;
    BASETYPE x = a->Value[a->nLength-1];

    while (x != 0)
    {
        x >>= 1;
        c++;
    }

    return c;
}

void BigNum::SetBytes(BigNum *a, CONST BYTE *pBlock, UINT cbBlock)
{
    BASETYPE* dest = a->Value;
    BASETYPE carry = 0;
    UINT shift = 0;

    while (cbBlock > 0)
    {
        BASETYPE v = *pBlock;
        pBlock++; cbBlock--;

        carry |= v << shift;
        shift += 8;

        if (shift < BASETYPE_BITS)
            continue;

        *dest++ = carry & BASETYPE_MASK;

        shift -= BASETYPE_BITS;
        carry = v >> (8 - shift);
    }

    if (carry != 0)
        *dest++ = carry;

    a->nLength = dest - a->Value;
    Normalize(a);
}

void BigNum::GetBytes(BigNum *a, BYTE *pBlock, UINT cbBlock)
{
    UINT l = a->nLength;

    if (l > 0)
    {
        BASETYPE* src = a->Value;
        BASETYPE carry = *src++; l--;
        UINT shift = 0;

        while (cbBlock > 0)
        {
            if (shift <= BASETYPE_BITS-8)
            {
                *pBlock = (BYTE)(carry >> shift);
                pBlock++; cbBlock--;

                shift += 8;
                continue;
            }                

            if (l == 0)
            {
                *pBlock = (BYTE)(carry >> shift);
                pBlock++; cbBlock--;
                break;
            }

            BASETYPE v = *src++; l--;

            *pBlock = (BYTE)((carry >> shift) | (v << (BASETYPE_BITS-shift)));                                         
            pBlock++; cbBlock--;

            shift -= BASETYPE_BITS-8;
            carry = v;
        }
    }

    memset(pBlock, 0, cbBlock);
}

void BigNum::Normalize(BigNum *a)
{
    while (a->nLength > 0 && a->Value[a->nLength-1] == 0) 
        a->nLength--;
}

void BigNum::SetBit(BigNum *a, UINT bit)
{
    UINT offset = bit / BASETYPE_BITS;
    UINT shift = bit % BASETYPE_BITS;

    if ((INT)offset >= a->nLength)
    {
        memset(&(a->Value[a->nLength]), 0, sizeof(BASETYPE) * ((offset+1) - a->nLength));
        a->nLength = offset+1;
    }       
    a->Value[offset] |= 1 << shift;
}

void BigNum::Add(BigNum *result, BigNum *a, BigNum *b)
{
    INT i, l;
    BASETYPE carry = 0;

    if (a->nLength < b->nLength)
    {
        BigNum *t = a;
        a = b;
        b = t;
    }

    l = b->nLength;
    for (i = 0; i < l; i++)
    {
        BASETYPE1 x = (BASETYPE1)a->Value[i] + (BASETYPE1)b->Value[i] + (BASETYPE1)carry;
        result->Value[i] = SHAVE1(x);
        carry = CARRY1(x);
    }

    l = a->nLength;
    for ( ; carry && (i < l); i++)
    {
        BASETYPE1 x = (BASETYPE1)a->Value[i] + (BASETYPE1)carry;
        result->Value[i] = SHAVE1(x);
        carry = CARRY1(x);
    }

    if (a != result)
    {
        for ( ; i < l; i++)
        {
            result->Value[i] = a->Value[i];
        }
    }

    if (carry != 0)
    {
        result->Value[l] = carry;
        result->nLength = l + 1;
    }
    else
    {
        result->nLength = l;
    }
}

BOOL BigNum::Sub(BigNum *result, BigNum *a, BigNum *b)
{
    INT i, l;
    BASETYPE carry = 0;

    l = min(a->nLength, b->nLength);
    for (i = 0; i < l; i++)
    {
        BASETYPE1 x = (BASETYPE1)a->Value[i] - (BASETYPE1)b->Value[i] - (BASETYPE1)carry;
        result->Value[i] = SHAVE1(x);
        carry = CARRY1(x) & 1;
    }

    if (a->nLength > b->nLength)
    {
        l = a->nLength;
        for ( ; carry && (i < l); i++)
        {
            BASETYPE1 x = (BASETYPE1)a->Value[i] - 0 - (BASETYPE1)carry;
            result->Value[i] = SHAVE1(x);
            carry = CARRY1(x) & 1;
        }

        if (a != result)
        {
            for ( ; i < l; i++)
            {
                result->Value[i] = a->Value[i];
            }
        }
    }
    else
    {
        l = b->nLength;
        for ( ; i < l; i++)
        {
            BASETYPE1 x = 0 - (BASETYPE1)b->Value[i] - (BASETYPE1)carry;
            result->Value[i] = SHAVE1(x);
            carry = CARRY1(x) & 1;
        }
    }

    result->nLength = l;
    Normalize(result);

    return (carry != 0);
}

int BigNum::Cmp(BigNum *a, BigNum *b)
{
    INT i;

    if (a->nLength != b->nLength)
    {
        return (a->nLength > b->nLength) ? 1 : -1;
    }

    for (i = a->nLength - 1; i >= 0; i--)
    {
        if (a->Value[i] != b->Value[i])
        {
            return (a->Value[i] > b->Value[i]) ? 1 : -1;
        }
    }

    return 0;
}

void BigNum::Modulize(BigNum *a, BigNum *module)
{
    DivMod(a, module, NULL);
}

void BigNum::DivMod(BigNum *a, BigNum *module, BigNum *d)
{
    UINT module_bits = GetBitSize(module);
    UINT a_bits = GetBitSize(a);
    INT i, l;

    if (a_bits < module_bits)
    {
        if (d != NULL)
            SetZero(d);
        return;
    }

    if (d != NULL)
    {
        d->nLength = 1 + (a_bits - module_bits) / BASETYPE_BITS;
        memset(&(d->Value[0]), 0, d->nLength * sizeof(BASETYPE));
    }

    l = module->nLength;
    _ASSERTE(l > 0);

    // to avoid range checks later
    a->Value[a->nLength] = 0;

    for ( ; a_bits >= module_bits; a_bits--)
    {
        UINT offset = (a_bits - module_bits) / BASETYPE_BITS;
        UINT shift = (a_bits - module_bits) % BASETYPE_BITS;

        BASETYPE m, v, carry;
        BASETYPE1 x;

        // compare
        m = module->Value[l-1] >> (BASETYPE_BITS-shift);
        v = a->Value[offset+l];
        if (m > v) goto skip_it;
        if (m < v) goto subtract_it;

        for (i = l - 1; i > 0; i--)
        {
            m = ((module->Value[i] << shift) & BASETYPE_MASK) | (module->Value[i-1] >> (BASETYPE_BITS-shift));
            v = a->Value[offset+i];            
            if (m > v) goto skip_it;
            if (m < v) goto subtract_it;
        }

        m = (module->Value[0] << shift) & BASETYPE_MASK;
        v = a->Value[offset];
        if (m > v) goto skip_it;
        if (m < v) goto subtract_it;

subtract_it:
        if (d != NULL)
            d->Value[(a_bits - module_bits) / BASETYPE_BITS] |= 1 << ((a_bits - module_bits) % BASETYPE_BITS);

        // subtract
        m = (module->Value[0] << shift) & BASETYPE_MASK;
        x = (BASETYPE1)a->Value[offset] - (BASETYPE1)m;
        a->Value[offset] = SHAVE1(x);
        carry = CARRY1(x) & 1;

        for (i = 1; i < l; i++)
        {
            m = ((module->Value[i] << shift) & BASETYPE_MASK) | (module->Value[i-1] >> (BASETYPE_BITS-shift));
            x = (BASETYPE1)a->Value[offset+i] - (BASETYPE1)m - (BASETYPE1)carry;
            a->Value[offset+i] = SHAVE1(x);
            carry = CARRY1(x) & 1;
        }

        m = (module->Value[l-1] >> (BASETYPE_BITS-shift));
        x = (BASETYPE1)a->Value[offset+l] - (BASETYPE1)m - (BASETYPE1)carry;
        a->Value[offset+l] = SHAVE1(x);
        carry = CARRY1(x) & 1;

        _ASSERTE(carry == 0);

skip_it: ;
    }

    a->nLength = l;

    Normalize(a);
    if (d != NULL)
        Normalize(d);
}

void BigNum::Mul(BigNum *result, BigNum *a, BigNum *b)
{
    INT i, j, l1, l2;
    BASETYPE carry = 0;

    l1 = a->nLength;
    l2 = b->nLength;

    if (l1 == 0 || l2 == 0)
    {
        result->nLength = 0;
        return;
    }

    memset(result->Value, 0, sizeof(BASETYPE) * l2);

    for (i = 0; i < l1; i++)
    {
        BASETYPE a_value = a->Value[i];
        carry = 0;

        for (j = 0; j < l2; j++)
        {
            BASETYPE2 x = (BASETYPE2)result->Value[i+j] + (BASETYPE2)a_value * (BASETYPE2)b->Value[j] + (BASETYPE2)carry;
            result->Value[i+j] = SHAVE2(x);
            carry = CARRY2(x);
        }

        result->Value[i+l2] = carry;
    }

    result->nLength = l1 + l2 - ((carry == 0) ? 1 : 0);
}

#define swap(type, x, y) do { type _t = x; x = y; y = _t; } while(0)

void BigNum::PowMod(BigNum *result, BigNum *a, BigNum *b, BigNum *module)
{
    BigNum *r, *s, *temp;
    UINT i, bits = GetBitSize(b);
    UINT safe_size = offsetof(BigNum, Value) + 2 * (module->nLength + 1) * sizeof(BASETYPE);

    r = (BigNum*)alloca(safe_size);
    s = (BigNum*)alloca(safe_size);    
    temp = (BigNum*)alloca(safe_size);

    Set(s, a);

    if (!IsBit(b, 0))
        SetSimple(r, 1);
    else
        Set(r, a);

    for (i = 1; i < bits; i++)
    {
        Mul(temp, s, s);
        Modulize(temp, module);
        swap(BigNum*, temp, s);

        if (IsBit(b, i))
        {
            Mul(temp, r, s);
            Modulize(temp, module);
            swap(BigNum*, temp, r);
        }
    }

    Set(result, r);
}

void BigNum::Shl(BigNum *result, BigNum *a, UINT bits)
{
    UINT offset = bits / BASETYPE_BITS;
    UINT shift = bits % BASETYPE_BITS;
    BASETYPE t;
    INT i, l;

    l = a->nLength;

    t = a->Value[l-1] >> (BASETYPE_BITS - shift);
    if (t != 0)
    {
        result->Value[l + offset] = t;
        result->nLength = l + offset + 1;
    }
    else
    {
        result->nLength = l + offset;
    }

    for (i = l - 1; i > 0; i--)
    {
        result->Value[i + offset] = 
            (a->Value[i] << shift) | (a->Value[i-1] >> (BASETYPE_BITS - shift));
    }

    result->Value[offset] = a->Value[0] << shift;

    for(i = offset - 1; i >= 0; i--)
        result->Value[i] = 0;
}

void BigNum::Shr(BigNum *result, BigNum *a, UINT bits)
{
    UINT offset = bits / BASETYPE_BITS;
    UINT shift = bits % BASETYPE_BITS;
    BASETYPE t;
    INT i, l;

    l = a->nLength;

    if ((INT)offset >= l)
    {
        SetZero(result);
        return;
    }
    l -= (offset + 1);

    for (i = 0; i < l; i++)
    {
        result->Value[i] = 
            (a->Value[i+offset] >> shift) | (a->Value[i+offset+1] << (BASETYPE_BITS - shift));
    }

    t = a->Value[l+offset] >> shift;
    if (t != 0)
    {
        result->Value[l] = t;
        result->nLength = l + 1;
    }
    else
    {
        result->nLength = l;
    }
}

UINT BigNum::GetPowersOfTwo(BigNum* a)
{
    UINT ret = 0;

    if (!IsZero(a))
    {
        while (!IsBit(a, ret)) {
            ret++;
        }
    }

    return ret;
}

BOOL BigNum::InvMod(BigNum *result, BigNum* a, BigNum *module)
{
    DWORD dwBufferSize = max(GetBufferSize(a), GetBufferSize(module));

    BigNum* r1 = (BigNum*)alloca(dwBufferSize);
    BigNum* r2 = (BigNum*)alloca(dwBufferSize);
    BigNum* u1 = (BigNum*)alloca(dwBufferSize);
    BigNum* u2 = (BigNum*)alloca(dwBufferSize);
    BigNum* t1 = (BigNum*)alloca(dwBufferSize);
    BigNum* t2 = (BigNum*)alloca(dwBufferSize);
    INT s1; // sign of u1
    INT s2; // sign of u2
    BOOL b;

    // Extended Euclid Algorithm
    // Invariant: r = u * a + v * module

    // r1 = a; u1 = 1; v1 = 0;
    Set(r1, a);
    SetOne(u1);

    // r2 = module; u2 = 0; v2 = 1;
    Set(r2, module);
    SetZero(u2);    

    if (Cmp(r1, r2) > 0)
    {
        swap(BigNum*, r1, r2);
        swap(BigNum*, u1, u2);
    }

    s1 = 1;
    s2 = 1;

    while(!IsZero(r1))
    {
        DivMod(r2, r1, t1);

        // u = u2 - (r2/r1) * u1; v = v2 - (r2/r1)*v1
        Mul(t2, t1, u1);

        if (s1 != s2)
        {
            Add(u2, u2, t2);
        }
        else
        {
            if (Cmp(u2, t2) > 0)
            {
                b = Sub(u2, u2, t2);
                _ASSERTE(!b);
            }
            else
            {
                b = Sub(u2, t2, u2);
                _ASSERTE(!b);
                s2 = -s2;
            }
        }

        swap(BigNum*, r1, r2);
        swap(BigNum*, u1, u2);
        swap(INT, s1, s2);
    }

    if (s2 > 0)
    {
        Set(result, u2);
    }
    else
    {
        b = Sub(result, module, u2);
        _ASSERTE(!b);
    }

    return IsOne(r2);
}

BOOL BigNum::GenRandom(BigNum* a, UINT bits, BOOL bStrong)
{
    _ASSERTE(bits > 0);

    DWORD size = (bits+7)/8;
    BYTE* buf = (BYTE*)alloca(size);

    if (!PAL_Random(bStrong, buf, size))
        return FALSE;

    // clear the excessive bits
    if ((bits&7) != 0)
        buf[size-1] &= (1 << (bits&7)) - 1;

    SetBytes(a, buf, size);
    return TRUE;
}

// primes < 256
static const BYTE c_Primes[] = {
    3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 
    97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 
    191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251 };

// check the primality by trial division
BOOL BigNum::TrialDivisionTest(BigNum* a, INT start, INT end)
{
    BigNum* t = (BigNum*)alloca(GetBufferSize(a));
    BigNum* m = (BigNum*)alloca(GetBufferSize(sizeof(c_Primes[0])));

    for (INT i = start; i < end; i++)
    {
        // if (a%primes[i]==0) <composite>;
        SetSimple(m, c_Primes[i]);
        Set(t,a);
        Modulize(t,m);
        if (IsZero(t)) {
            if (Cmp(m, a) >= 0)
                return TRUE;
            return FALSE;
        }
    }

    return TRUE;
}

// check the primality by Fermat test
BOOL BigNum::FermatTest(BigNum* a, INT count)
{
    DWORD dwBufferSize = GetBufferSize(a);
    BigNum* t = (BigNum*)alloca(dwBufferSize);
    BigNum* a1 = (BigNum*)alloca(dwBufferSize);

    Sub(a1, a, BigNum::One());

    for (INT i = 0; i < count; i++)
    {
        // if ((primes[i]^(a-1))%a!=1) <composite>;
        SetSimple(t, c_Primes[i]);
        PowMod(t,t,a1,a);
        if (!IsOne(t))
            return FALSE;
    }

    return TRUE;
}

// check the primality by Rabin-Miller test (strong pseudoprimes)
BOOL BigNum::RabinMillerTest(BigNum* a, INT count)
{
    DWORD dwBufferSize = GetBufferSize(a);
    UINT nBits = GetBitSize(a);
    BigNum* b = (BigNum*)alloca(dwBufferSize);
    BigNum* m = (BigNum*)alloca(dwBufferSize);
    BigNum* t = (BigNum*)alloca(dwBufferSize);
    UINT powers_of_two;
    bool bComposite;

    for (INT i = 0; i < count; i++)
    {
again:
        bComposite = true;

        if (!GenRandom(b, nBits, FALSE))
            return FALSE;
        Modulize(b, a);      
        if (IsZero(b))
            goto again;

        Sub(m, a, One());
        powers_of_two = GetPowersOfTwo(m);
        if (powers_of_two > 0)
            Shr(m, m, powers_of_two);

        // if (b ^ m mod a == 1) <inconclusive>;
        PowMod(t, b, m, a);
        if (IsOne(t))
        {
            bComposite = false;
        }
        else
        {
            while (powers_of_two > 0)
            {
                // if (b ^ (m2^i) mod a == (a-1)) <inconclusive>;
                PowMod(t, b, m, a);
                Sub(t, a, t);
                if (IsOne(t))
                {
                    bComposite = false;
                    break;
                }

                Shl(m,m,1);
                powers_of_two--;
            }
        }

        if (bComposite)
            return FALSE;
    }
    
    return TRUE;
}

BOOL BigNum::GenPrime(BigNum* a, UINT bits, BOOL bStrong)
{
again:
    if (!GenRandom(a, bits, TRUE))
        return FALSE;

    // set the 0-th bit to make the number odd
    SetBit(a, 0);

    // set the top bit to use the full range
    SetBit(a, (bits-1));

    for (;;)
    {
        // pass all the prime tests
        if (TrialDivisionTest(a, 0, sizeof(c_Primes)/sizeof(c_Primes[0])))
            if (FermatTest(a, sizeof(c_Primes)/sizeof(c_Primes[0])))
                if (RabinMillerTest(a, 10))
                    break;

        Add(a, a, Two());

        // handle a rare overflow
        if ((a->nLength > (INT)bits / BASETYPE_BITS) && IsBit(a, bits))
            goto again;
    }

    return TRUE;
}
