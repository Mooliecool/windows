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

#ifndef _ARRAY_H_
#define _ARRAY_H_

////////////////////////////////////////////////////////////////////////////////
// CSmallArray
//
// Simple array that holds pointers to T's.

template <class T>
class CSmallArray
{
private:
    CAtlArray<T *>  m_Elements;

public:
    CSmallArray()
    {
    }

    ~CSmallArray()
    {
        FreeAll();
    }

    HRESULT     Add (T *p)
    {
        m_Elements.Add(p);
        return S_OK;
    }

    HRESULT     Remove (T *p)
    {
        for (size_t i = 0; i < m_Elements.GetCount(); i++)
        {
            if (m_Elements.GetAt(i) == p)
            {
                m_Elements.RemoveAt(i);
                return S_OK;
            }
        }

        return E_INVALIDARG;
    }

    long        Count() const 
    {
        size_t cnt = m_Elements.GetCount();
        RETAILVERIFY(cnt < LONG_MAX);
        return (long)cnt;
    }

    T*          GetAt (long i) const 
    {
        return m_Elements[i];
    }

    T*          operator[](int i) const 
    {
        return m_Elements[i];
    }

    T**         Base ()
    {
        return m_Elements.GetData(); 
    }

    void        ClearAll ()
    {
        m_Elements.RemoveAll();
    }

    void        FreeAll ()
    {
        m_Elements.RemoveAll();
    }
};

////////////////////////////////////////////////////////////////////////////////
// CStructArray
//
// Simple array that holds T's.
//
// NOTE:  T must not require ctor/dtor calls.  They will be zero-inited on
// initial creation.  They must have copy capability, however.  (This is intended
// for simple structs, such as RECT, TextSpan, etc.)

template <class T>
class CStructArray
{
private:
    CAtlArray<T>    m_Elements;

public:
    CStructArray()
    {
    }

    ~CStructArray()
    {
    }

    HRESULT Add(long *piIndex, T **pp)        // (returns the index, and a pointer to the new T -- both optional)
    {
        long iNewIndex = Count();

        m_Elements.Add(T());

        T *pNewT = &m_Elements[iNewIndex];
        ZeroMemory(pNewT, sizeof(T));

        if (pp != NULL)
            *pp = pNewT;

        if (piIndex != NULL)
            *piIndex = iNewIndex;

        return S_OK;
    }

    HRESULT Add(const T &t)
    {
        T       *pNew;
        HRESULT hr = Add(NULL, &pNew);
        if (SUCCEEDED (hr))
            *pNew = t;
        return hr;
    }

    HRESULT Remove (long i)
    {
        if (i >= 0 && i < Count())
        {
            m_Elements.RemoveAt(i);
            return S_OK;
        }

        return E_INVALIDARG;
    }

    long Count() const
    {
        size_t cnt = m_Elements.GetCount();
        RETAILVERIFY(cnt < LONG_MAX);
        return (long)cnt;
    }

    T * GetAt (long i)
    {
        return &m_Elements[i];
    }

    T * Base ()
    {
        return m_Elements.GetData();
    }

    T & operator[](long i)
    {
        return m_Elements[i];
    }

    void        ClearAll ()
    {
        m_Elements.RemoveAll();
    }
};

////////////////////////////////////////////////////////////////////////////////
// CRefArray

template <class T>
class CRefArray : public CStructArray<T>
{
private:
    long    m_iRef;

public:
    CRefArray() : m_iRef(0) {}

    long    AddRef () { return InterlockedIncrement (&m_iRef); }
    long    Release () { long l = InterlockedDecrement (&m_iRef); if (l == 0) delete this; return l; }
};

////////////////////////////////////////////////////////////////////////////////
// CAutoDelSimpleArray 
// 
// Self deleting simple array
//

template <class T>
class CAutoDelSimpleArray : public CSmallArray<T>
{
public:
    ~CAutoDelSimpleArray() 
    { 
        int count = this->Count();
        for (int i = 0 ; i < count ; i++)
        {
            delete this->GetAt(i);
        }
    }   
};

#endif  // _ARRAY_H_
