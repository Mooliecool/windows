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
// File: BitSet.h
//
// BitSet implementation.
// ===========================================================================

#ifndef __bitset_h__
#define __bitset_h__

/******************************************************************************
    A BitSet can be in one of 3 states:

    1) Uninited: m_bits == 0. The bitset is also considered to be empty. Any
                 bit test operations will behave as if the set is empty.
    2) Small:    (m_bits & 1) != 0. The bitset is (m_bits >> 1). Note that if
                 m_bits == 1 then the bitset is empty.
    3) Large:    m_bits is a pointer to a BitSetImp.

    Bit indices are zero based.
******************************************************************************/
class BitSet {
private:
    typedef ULONG_PTR Blob;

    enum {
        kcbitByte = 8,
        kcbitBlob = sizeof(Blob) * kcbitByte,

        // The number of bits that a small bitset can hold. The -1 is for the sentinel bit (the low bit).
        kcbitSmall = kcbitBlob - 1,
    };

    // If m_bits is zero, the bitset is uninited. Otherwise, if the low bit is set,
    // the remaining kcbitSmall bits are the actual data. Otherwise, m_bits is a
    // pointer to a BitSetImp.
    Blob m_bits;

    struct BitSetImp {
        int cblob;      // Size in blobs.
        Blob rgblob[0]; // The bits. Actual size is cblob.
    };

    BitSetImp * Pbsi() {
        ASSERT(m_bits && !(m_bits & 1));
        return (BitSetImp *)m_bits;
    }

    bool FSmall() {
        return m_bits & 1;
    }

    // Used to get mask and blob index information for a bit range.
    struct MaskData {
        int iblobMin;
        int iblobLast;
        Blob blobMaskMin;
        Blob blobMaskLast;

        MaskData(int ibitMin, int ibitLim) {
            ASSERT(0 <= ibitMin && ibitMin < ibitLim);

            // Get iblobMin and adjust ibitMin to be relative.
            iblobMin = (int)(uint)ibitMin / kcbitBlob;
            ibitMin -= (uint)iblobMin * kcbitBlob;
            ASSERT(0 <= ibitMin && ibitMin < kcbitBlob);
            blobMaskMin = ~(Blob)0 << ibitMin;

            // Get iblobLast and adjust ibitLim to be relative.
            iblobLast = (int)(uint)(ibitLim - 1) / kcbitBlob;
            ibitLim -= (uint)iblobLast * kcbitBlob;
            ASSERT(0 < ibitLim && ibitLim <= kcbitBlob);
            blobMaskLast = ~(Blob)0 >> (kcbitBlob - ibitLim);
            if (iblobMin == iblobLast) {
                blobMaskMin &= blobMaskLast;
                blobMaskLast = blobMaskMin;
            }
        }
    };

    // Returns a mask for a single bit in a small bitset.
    static Blob MaskSmall(int ibit) {
        ASSERT(0 <= ibit && ibit < kcbitSmall);
        return (Blob)1 << (ibit + 1);
    }

    // Returns a mask for a range of bits in a small bitset.
    static Blob MaskSmall(int ibitMin, int ibitLim) {
        ASSERT(0 <= ibitMin && ibitMin <= ibitLim && ibitLim <= kcbitSmall);
        return (((Blob)1 << (ibitLim - ibitMin)) - 1) << (ibitMin + 1);
    }

    // If any bits are set in blob1 but not in blob2, clear them and return true.
    // Otherwise return false.
    static bool AndBlobChanged(Blob & blob1, Blob blob2) {
        if (blob1 & ~blob2) {
            blob1 &= blob2;
            return true;
        }
        return false;
    }

    // Return true iff the blobs are all zero.
    static bool AreBitsZero(Blob * prgblob, int cblob) {
        while (--cblob >= 0) {
            if (prgblob[cblob])
                return false;
        }
        return true;
    }

    // Return true iff the blobs have all bits set.
    static bool AreBitsOne(Blob * prgblob, int cblob) {
        while (--cblob >= 0) {
            if (~prgblob[cblob])
                return false;
        }
        return true;
    }

    static void SetBlobs(Blob * prgblob, int cblob, Blob blob) {
        while (--cblob >= 0)
            prgblob[cblob] = blob;
    }

    // Foil the default copy constructor.
    BitSet(BitSet & bset) {
        VSFAIL("No copy ctor!");
    }

public:
    BitSet() {
        // Set it to uninited.
        m_bits = 0;
    }
    BitSet(int cbit, NRHEAP * heap) {
        Init(cbit, heap);
    }
    BitSet(BitSet & bset, NRHEAP * heap) {
        m_bits = 0;
        Set(bset, heap);
    }

    int Cbit() {
        if (!m_bits)
            return 0;
        if (m_bits & 1)
            return kcbitSmall;
        return Pbsi()->cblob * kcbitBlob;
    }

    bool FInited() { return m_bits != 0; }
    void Init(int cbit, NRHEAP * heap);
    void Reset() { m_bits = 0; }

    void Grow(int cbit, NRHEAP * heap);

    // Returns false if ibit >= Cbit().
    bool TestBit(int ibit);
    bool TestAllRange(int ibitMin, int ibitLim);
    bool TestAnyRange(int ibitMin, int ibitLim);

    // Returns true if any bits are set.
    bool TestAnyBits();
    bool TestAnyBits(BitSet & bsetCheck);

    void SetBit(int ibit, NRHEAP * heap);
    void ClearBit(int ibit);
    void SetBitRange(int ibitMin, int ibitLim, NRHEAP * heap);
    void ClearBitRange(int ibitMin, int ibitLim);
    void ClearAll();

    void Set(BitSet & bset, NRHEAP * heap);
    bool Equals(BitSet & bset);
    void Union(BitSet & bset, NRHEAP * heap);
    bool FIntersectChanged(BitSet & bset);
    void Intersect(BitSet & bset);

    // Used in debug to scatter junk in the bitset. Sets every other bit. Whether
    // the high bit is set is determined by a static long that is incremented on
    // each call.
    void Trash() {
#if DEBUG
        if (!m_bits)
            return;

        static LONG s_cvTrash;
        bool fSet = !(InterlockedIncrement(&s_cvTrash) & 1);

        if (FSmall()) {
            memset(&m_bits, fSet ? 0xAA : 0x55, sizeof(Blob));
            m_bits |= 1;
            return;
        }

        BitSetImp * pbsi = Pbsi();
        memset(pbsi->rgblob, fSet ? 0xAA : 0x55, SizeMul(pbsi->cblob, sizeof(Blob)));
#endif
    }
};

#endif // !__bitset_h__
