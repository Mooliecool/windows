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
// File: bitset.cpp
//
// BitSet implementation
// ===========================================================================

#include "stdafx.h"

/***************************************************************************************************
    Initialize the bitset to all zeros. Make sure there is room for cbit bits. Uses the given
    heap to allocate a BitSetImp (if needed).
***************************************************************************************************/
void BitSet::Init(int cbit, NRHEAP * heap)
{
    // cbit should be non-negative and shouldn't overflow.
    ASSERT(0 <= cbit && 0 <= (cbit + kcbitBlob - 1));

    if (cbit <= kcbitSmall)
        m_bits = 1;
    else {
        // Allocate at least kcbitBlob more bits than kcbitSmall.
        // Then we'll always grow by at least kcbitBlob.
        if (cbit < kcbitSmall + kcbitBlob)
            cbit = kcbitSmall + kcbitBlob;

        int cblob = (int)((uint)(cbit + kcbitBlob - 1) / kcbitBlob);
        int cbTot = cblob * sizeof(Blob) + sizeof(BitSetImp);
        BitSetImp * pbsi = (BitSetImp *)heap->AllocZero(cbTot);
        pbsi->cblob = cblob;
        m_bits = (Blob)pbsi;
        ASSERT(!FSmall());
    }
}


/***************************************************************************************************
    Grow the bitset to accomodate at least cbit bits. This preserves any existing bits.
***************************************************************************************************/
void BitSet::Grow(int cbit, NRHEAP * heap)
{
    if (!m_bits)
        Init(cbit, heap);
    else if (Cbit() >= cbit)
        VSFAIL("Why is Grow being called?");
   else if (FSmall()) {
        Blob blob = m_bits >> 1;
        Init(cbit, heap);
        ASSERT(cbit <= Cbit());

        // Preserve the previous bits.
        Pbsi()->rgblob[0] = blob;
    }
    else {
        BitSetImp * pbsi = Pbsi();
        Init(cbit, heap);
        ASSERT(cbit <= Cbit());

        // Preserve the previous bits.
        memcpy(Pbsi()->rgblob, pbsi->rgblob, pbsi->cblob * sizeof(Blob));
    }
}


/***************************************************************************************************
    Return true iff the given bit is set. If ibit >= Cbit(), returns false.
***************************************************************************************************/
bool BitSet::TestBit(int ibit)
{
    ASSERT(0 <= ibit);

    if (!m_bits)
        return false;
    if (FSmall())
        return ibit < kcbitSmall && ((m_bits >> (ibit + 1)) & 1);

    BitSetImp * pbsi = Pbsi();
    int iblob = (uint)ibit / kcbitBlob;
    return iblob < pbsi->cblob && ((pbsi->rgblob[iblob] >> ((uint)ibit % kcbitBlob)) & 1);
}


/***************************************************************************************************
    Return true iff all the bits in the range [ibitMin, ibitLim) are set. If ibitLim > Cbit(),
    returns false. If ibitMin == ibitLim, returns true (vacuous).
***************************************************************************************************/
bool BitSet::TestAllRange(int ibitMin, int ibitLim)
{
    ASSERT(0 <= ibitMin && ibitMin <= ibitLim);
    if (ibitLim - ibitMin <= 1) {
        if (ibitLim - ibitMin == 1)
            return TestBit(ibitMin);
        return true; // Vacuous
    }

    if (!m_bits)
        return false;
    if (FSmall()) {
        if (ibitLim > kcbitSmall)
            return false;
        Blob blobMask = MaskSmall(ibitMin, ibitLim);
        return (m_bits & blobMask) == blobMask;
    }

    MaskData md(ibitMin, ibitLim);
    BitSetImp * pbsi = Pbsi();
    if (md.iblobLast >= pbsi->cblob)
        return false;

    if ((pbsi->rgblob[md.iblobMin] & md.blobMaskMin) != md.blobMaskMin)
        return false;
    if (md.iblobMin >= md.iblobLast)
        return true;
    return (pbsi->rgblob[md.iblobLast] & md.blobMaskLast) == md.blobMaskLast &&
        AreBitsOne(pbsi->rgblob + md.iblobMin + 1, md.iblobLast - md.iblobMin - 1);
}


/***************************************************************************************************
    Returns true iff any bits in the range [ibitMin, ibitLim) are set.
***************************************************************************************************/
bool BitSet::TestAnyRange(int ibitMin, int ibitLim)
{
    ASSERT(0 <= ibitMin && ibitMin <= ibitLim);
    if (ibitLim - ibitMin <= 1) {
        if (ibitLim - ibitMin == 1)
            return TestBit(ibitMin);
        return false;
    }

    if (!m_bits)
        return false;
    if (FSmall()) {
        if (ibitMin >= kcbitSmall)
            return false;
        if (ibitLim > kcbitSmall)
            ibitLim = kcbitSmall;
        Blob blobMask = MaskSmall(ibitMin, ibitLim);
        return (m_bits & blobMask) != 0;
    }

    MaskData md(ibitMin, ibitLim);
    BitSetImp * pbsi = Pbsi();
    if (md.iblobMin >= pbsi->cblob)
        return false;
    if ((pbsi->rgblob[md.iblobMin] & md.blobMaskMin) != 0)
        return true;
    if (md.iblobLast >= pbsi->cblob)
        md.iblobLast = pbsi->cblob;
    else if ((pbsi->rgblob[md.iblobLast] & md.blobMaskLast) != 0)
        return true;
    return !AreBitsZero(pbsi->rgblob + md.iblobMin + 1, md.iblobLast - md.iblobMin - 1);
}


/***************************************************************************************************
    Returns true iff any bits in the range [0, infinity) are set.
***************************************************************************************************/
bool BitSet::TestAnyBits()
{
    if (!m_bits)
        return false;
    if (FSmall())
        return m_bits != 1;

    BitSetImp * pbsi = Pbsi();
    return !AreBitsZero(pbsi->rgblob, pbsi->cblob);
}


/***************************************************************************************************
    Returns true iff there are any bits that are set in both this bitset and bsetCheck.
***************************************************************************************************/
bool BitSet::TestAnyBits(BitSet & bsetCheck)
{
    if (!m_bits || !bsetCheck.m_bits)
        return false;
    if (FSmall()) {
        if (bsetCheck.FSmall())
            return (m_bits & bsetCheck.m_bits) != 1;
        ASSERT(bsetCheck.Pbsi()->cblob > 0);
        return (bsetCheck.Pbsi()->rgblob[0] & (m_bits >> 1)) != 0;
    }

    BitSetImp * pbsi1 = Pbsi();
    ASSERT(pbsi1->cblob > 0);

    if (bsetCheck.FSmall())
        return (pbsi1->rgblob[0] & (bsetCheck.m_bits >> 1)) != 0;

    BitSetImp * pbsi2 = bsetCheck.Pbsi();
    ASSERT(pbsi2->cblob > 0);

    for (int iblob = min(pbsi1->cblob, pbsi2->cblob); --iblob >= 0; ) {
        if (pbsi1->rgblob[iblob] & pbsi2->rgblob[iblob])
            return true;
    }
    return false;
}


/***************************************************************************************************
    Sets the bit at position ibit, ensuring that the bitset is large enough.
***************************************************************************************************/
void BitSet::SetBit(int ibit, NRHEAP * heap)
{
    // ibit should be non-negative and shouldn't overflow.
    ASSERT(0 <= ibit && 0 <= (ibit + kcbitBlob - 1));

    if (ibit >= Cbit())
        Grow(ibit + 1, heap);

    if (FSmall())
        m_bits |= MaskSmall(ibit);
    else {
        BitSetImp * pbsi = Pbsi();
        pbsi->rgblob[(uint)ibit / kcbitBlob] |= ((Blob)1 << ((uint)ibit % kcbitBlob));
    }
}


/***************************************************************************************************
    Clear all the bits in the bitset, but don't discard any memory allocated.
***************************************************************************************************/
void BitSet::ClearAll()
{
    if (FSmall())
        m_bits = 1;
    else if (m_bits) {
        BitSetImp * pbsi = Pbsi();
        SetBlobs(pbsi->rgblob, pbsi->cblob, 0);
    }
}


/***************************************************************************************************
    Clear the given bit. If the bit is out of the range of the bitset, it is already considered
    cleared, so nothing is changed.
***************************************************************************************************/
void BitSet::ClearBit(int ibit)
{
    ASSERT(0 <= ibit);

    if (FSmall()) {
        if (ibit < kcbitSmall)
            m_bits &= ~MaskSmall(ibit);
    }
    else if (m_bits) {
        BitSetImp * pbsi = Pbsi();
        int iblob = (uint)ibit / kcbitBlob;
        if (iblob < pbsi->cblob)
            pbsi->rgblob[iblob] &= ~((Blob)1 << ((uint)ibit % kcbitBlob));
    }
}


/***************************************************************************************************
    Set the range of bits [ibitMin, ibitLim), growing the bitset if needed.
***************************************************************************************************/
void BitSet::SetBitRange(int ibitMin, int ibitLim, NRHEAP * heap)
{
    ASSERT(0 <= ibitMin && ibitMin <= ibitLim);
    if (ibitLim - ibitMin <= 1) {
        if (1 == ibitLim - ibitMin)
            SetBit(ibitMin, heap);
        return;
    }

    if (ibitLim > Cbit())
        Grow(ibitLim, heap);
    ASSERT(ibitLim <= Cbit());

    if (FSmall()) {
        m_bits |= MaskSmall(ibitMin, ibitLim);
        return;
    }

    BitSetImp * pbsi = Pbsi();
    MaskData md(ibitMin, ibitLim);

    pbsi->rgblob[md.iblobMin] |= md.blobMaskMin;
    if (md.iblobMin >= md.iblobLast)
        return;
    SetBlobs(pbsi->rgblob + md.iblobMin + 1, md.iblobLast - md.iblobMin - 1, ~(Blob)0);
    pbsi->rgblob[md.iblobLast] |= md.blobMaskLast;
}


/***************************************************************************************************
    Clear the range of bits [ibitMin, ibitLim).
***************************************************************************************************/
void BitSet::ClearBitRange(int ibitMin, int ibitLim)
{
    ASSERT(0 <= ibitMin && ibitMin <= ibitLim);

    int cbit = Cbit();
    if (ibitLim > cbit)
        ibitLim = cbit;
    if (ibitMin >= ibitLim)
        return;
    ASSERT(0 <= ibitMin && ibitMin < ibitLim && ibitLim <= Cbit());

    if (1 == ibitLim - ibitMin) {
        ClearBit(ibitMin);
        return;
    }

    if (FSmall()) {
        m_bits &= ~MaskSmall(ibitMin, ibitLim);
        return;
    }

    BitSetImp * pbsi = Pbsi();
    MaskData md(ibitMin, ibitLim);

    pbsi->rgblob[md.iblobMin] &= ~md.blobMaskMin;
    if (md.iblobMin >= md.iblobLast)
        return;
    SetBlobs(pbsi->rgblob + md.iblobMin + 1, md.iblobLast - md.iblobMin - 1, 0);
    pbsi->rgblob[md.iblobLast] &= ~md.blobMaskLast;
}


/***************************************************************************************************
    Set the bitset to a copy of bset.
***************************************************************************************************/
void BitSet::Set(BitSet & bset, NRHEAP * heap)
{
    ASSERT(this != &bset);

    if (!bset.m_bits) {
        ClearAll();
        return;
    }

    int cbit = bset.Cbit();
    if (Cbit() < cbit)
        Init(cbit, heap);

    if (FSmall()) {
        ASSERT(bset.FSmall());
        m_bits = bset.m_bits;
        return;
    }

    BitSetImp * pbsiDst = Pbsi();

    if (bset.FSmall()) {
        pbsiDst->rgblob[0] = bset.m_bits >> 1;
        SetBlobs(pbsiDst->rgblob + 1, pbsiDst->cblob - 1, 0);
        return;
    }

    BitSetImp * pbsiSrc = bset.Pbsi();
    ASSERT(pbsiDst->cblob >= pbsiSrc->cblob);
    memcpy(pbsiDst->rgblob, pbsiSrc->rgblob, SizeMul(pbsiSrc->cblob, sizeof(Blob)));
    if (pbsiDst->cblob > pbsiSrc->cblob)
        SetBlobs(pbsiDst->rgblob + pbsiSrc->cblob, pbsiDst->cblob - pbsiSrc->cblob, 0);
}


/***************************************************************************************************
    Return true iff this bitset contains the same set as bset.
***************************************************************************************************/
bool BitSet::Equals(BitSet & bset)
{
    if (m_bits == bset.m_bits)
        return true;
    if (!m_bits)
        return !bset.TestAnyBits();
    if (!bset.m_bits)
        return !TestAnyBits();

    BitSetImp * pbsi;

    if (FSmall()) {
        if (bset.FSmall())
            return false;

        pbsi = bset.Pbsi();
        return (pbsi->rgblob[0] == (m_bits >> 1)) &&
            AreBitsZero(pbsi->rgblob + 1, pbsi->cblob - 1);
    }

    if (bset.FSmall()) {
        pbsi = Pbsi();
        return (pbsi->rgblob[0] == (bset.m_bits >> 1)) &&
            AreBitsZero(pbsi->rgblob + 1, pbsi->cblob - 1);
    }

    pbsi = Pbsi();
    BitSetImp * pbsi2 = bset.Pbsi();
    int cblob = min(pbsi->cblob, pbsi2->cblob);

    if (memcmp(pbsi->rgblob, pbsi2->rgblob, SizeMul(cblob, sizeof(Blob))))
        return false;
    if (pbsi->cblob == pbsi2->cblob)
        return true;
    if (cblob < pbsi->cblob)
        return AreBitsZero(pbsi->rgblob + cblob, pbsi->cblob - cblob);
    ASSERT(cblob < pbsi2->cblob);
    return AreBitsZero(pbsi2->rgblob + cblob, pbsi2->cblob - cblob);
}


/***************************************************************************************************
    Ensures that any bits that are set in bset are also set in this bitset.
***************************************************************************************************/
void BitSet::Union(BitSet & bset, NRHEAP * heap)
{
    int cbitSrc = bset.Cbit();
    if (!cbitSrc)
        return;

    if (Cbit() < cbitSrc)
        Grow(cbitSrc, heap);
    ASSERT(Cbit() >= cbitSrc);

    if (FSmall()) {
        ASSERT(bset.FSmall());
        m_bits |= bset.m_bits;
    }
    else if (bset.FSmall())
        Pbsi()->rgblob[0] |= bset.m_bits >> 1;
    else {
        BitSetImp * pbsiDst = Pbsi();
        BitSetImp * pbsiSrc = bset.Pbsi();

        for (int iblob = pbsiSrc->cblob; --iblob >= 0; )
            pbsiDst->rgblob[iblob] |= pbsiSrc->rgblob[iblob];
    }
}


/***************************************************************************************************
    Ensures that any bits that are clear in bset are also clear in this bitset. Returns true iff
    this bitset changes as a result (ie, if this bitset "shrunk").
***************************************************************************************************/
bool BitSet::FIntersectChanged(BitSet & bset)
{
    BitSetImp * pbsiDst;

    if (m_bits <= 1)
        return false;

    if (bset.m_bits <= 1) {
        if (FSmall())
            m_bits = 1;
        else {
            pbsiDst = Pbsi();
            if (AreBitsZero(pbsiDst->rgblob, pbsiDst->cblob))
                return false;
            ClearAll();
        }
        return true;
    }

    if (FSmall()) {
        Blob blob = bset.FSmall() ? bset.m_bits : ((bset.Pbsi()->rgblob[0] << 1) | 1);
        return AndBlobChanged(m_bits, blob);
    }

    bool fChanged;
    int cblob;

    pbsiDst = Pbsi();
    if (bset.FSmall()) {
        fChanged = AndBlobChanged(pbsiDst->rgblob[0], bset.m_bits >> 1);
        cblob = 1;
    }
    else {
        BitSetImp * pbsiSrc = bset.Pbsi();
        cblob = min(pbsiDst->cblob, pbsiSrc->cblob);
        fChanged = false;
        for (int iblob = cblob; --iblob >= 0; )
            fChanged |= AndBlobChanged(pbsiDst->rgblob[iblob], pbsiSrc->rgblob[iblob]);
    }
    if (pbsiDst->cblob == cblob || AreBitsZero(pbsiDst->rgblob + cblob, pbsiDst->cblob - cblob))
        return fChanged;
    SetBlobs(pbsiDst->rgblob + cblob, pbsiDst->cblob - cblob, 0);
    return true;
}


/***************************************************************************************************
    Ensures that any bits that are clear in bset are also clear in this bitset.
***************************************************************************************************/
void BitSet::Intersect(BitSet & bset)
{
    if (m_bits <= 1)
        return;
    if (bset.m_bits <= 1) {
        ClearAll();
        return;
    }

    if (FSmall()) {
        Blob blob = bset.FSmall() ? bset.m_bits : ((bset.Pbsi()->rgblob[0] << 1) | 1);
        m_bits &= blob;
        return;
    }

    BitSetImp * pbsiDst = Pbsi();
    int cblob;

    if (bset.FSmall()) {
        pbsiDst->rgblob[0] &= bset.m_bits >> 1;
        cblob = 1;
    }
    else {
        BitSetImp * pbsiSrc = bset.Pbsi();
        cblob = min(pbsiDst->cblob, pbsiSrc->cblob);
        for (int iblob = cblob; --iblob >= 0; )
            pbsiDst->rgblob[iblob] &= pbsiSrc->rgblob[iblob];
    }
    if (pbsiDst->cblob > cblob)
        SetBlobs(pbsiDst->rgblob + cblob, pbsiDst->cblob - cblob, 0);
}
