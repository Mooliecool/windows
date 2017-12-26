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
// File: array.h
//
// ===========================================================================

#ifndef __array_h__
#define __array_h__

////////////////////////////////////////////////////////////////////////////////
// CSimpleArray
//
// Simple array that holds pointers to T's.  Grows in blocks of 8 pointers
// (32 bytes).  Removal causes re-arrangement (the last element is moved to take
// the removed one's place).

template <class T>
class CSimpleArray
{
private:

    // no allocation is made until >1 elements are added...

    T       **m_pElements;      // Elements of the array
    DWORD   m_iCount;           // Number of elements in array

public:
    CSimpleArray() : m_pElements(NULL), m_iCount(0) {}
    ~CSimpleArray() { if (m_pElements != NULL) VSFree (m_pElements); }

    HRESULT     Add (T *p)
    {
        if ((m_iCount & 7) == 0)
        {
            if (m_iCount >= (UINT_MAX / sizeof (T *)) - 8)
                // Prevent overflow/wrap-around
                return E_OUTOFMEMORY;

            // Time to grow
            DWORD   iSize = (m_iCount + 8) * sizeof (T *);
            void    *pNew = (m_pElements == NULL) ? VSAlloc (iSize) : VSRealloc (m_pElements, iSize);

            if (pNew != NULL)
                m_pElements = (T **)pNew;
            else
                return E_OUTOFMEMORY;
        }

        m_pElements[m_iCount++] = p;
        return S_OK;
    }

    HRESULT     Remove (T *p)
    {
        for (DWORD i=0; i<m_iCount; i++)
        {
            if (m_pElements[i] == p)
            {
                if (i < m_iCount - 1)
                    m_pElements[i] = m_pElements[m_iCount - 1];
                m_iCount--;
                return S_OK;
            }
        }

        return E_HANDLE;
    }

    DWORD       Count() { return m_iCount; }
    T           *GetAt (DWORD i) { ASSERT (i < m_iCount); return m_pElements[i]; }
    T           * operator[](DWORD i) const { ASSERT (i < m_iCount); return m_pElements[i]; }
    T           **Base () { return m_pElements; }
};

////////////////////////////////////////////////////////////////////////////////
// CStructArray
//
// Simple array that holds T's.  Grows in blocks of 8 T's at a time.  Like
// CSimpleArray, removal causes re-arrangement (the last element is moved to take
// the removed one's place).
//
// NOTE:  T must not require ctor/dtor calls.  They will be zero-inited on
// initial creation.  They must have copy capability, however.  (This is intended
// for simple structs, such as RECT, TextSpan, etc.)

template <class T>
class CStructArray
{
private:
    T       *m_pElements;       // Elements of the array
    DWORD    m_iCount;          // Number of elements in array

public:
    CStructArray() : m_pElements(NULL), m_iCount(0) {}
    ~CStructArray() { if (m_pElements != NULL) VSFree (m_pElements); }

    HRESULT     Add (DWORD *piIndex, T **pp)        // (returns the index, and a pointer to the new T -- both optional)
    {
        if ((m_iCount & 7) == 0)
        {
            if (m_iCount >= (UINT_MAX / sizeof (T)) - 8)
                // Prevent overflow/wrap-around
                return E_OUTOFMEMORY;

            // Time to grow
            DWORD   iSize = (m_iCount + 8) * sizeof (T);
            void    *pNew = (m_pElements == NULL) ? VSAlloc (iSize) : VSRealloc (m_pElements, iSize);

            if (pNew != NULL)
                m_pElements = (T *)pNew;
            else
                return E_OUTOFMEMORY;
        }

        ZeroMemory (m_pElements + m_iCount, sizeof (T));

        if (pp != NULL)
            *pp = m_pElements + m_iCount;

        if (piIndex != NULL)
            *piIndex = m_iCount;

        m_iCount++;
        return S_OK;
    }

    HRESULT     Remove (DWORD i)
    {
        if (i < m_iCount)
        {
            if (i < m_iCount - 1)
                m_pElements[i] = m_pElements[m_iCount - 1];
            m_iCount--;
            return S_OK;
        }

        return E_HANDLE;
    }

    DWORD       Count() { return m_iCount; }
    T           *GetAt (DWORD i) { ASSERT (i < m_iCount); return m_pElements + i; }
    T           *Base () { return m_pElements; }
    void        ClearAll () { m_iCount = 0; }
};
#endif // __array_h__

