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
// File: objptr.h
//
// ===========================================================================
#ifndef _OBJPTR_H
#define _OBJPTR_H

////////////////////////////////////////////////////////////////////////////////
// CSmartPtr
//
// This is a smart-pointer object for any object that has an AddRef/Release pair.
// If you need to avoid ambiguity, declare RT as the un-ambiguous class to which
// the object will be cast to before calling AddRef().

template <class T>
class CDefaultSmartPointerTraits
{
public:
    static void AddRef(T* t) {
        t->AddRef();
    }

    static void Release(T* t) {
        t->Release();
    }
}; 

template <class T, class RT = T, class RTTraits = CDefaultSmartPointerTraits<RT> >
class CSmartPtr
{
private:
    T *Assign (T **pp, T *pObj)
    {
        if (pObj != NULL) {
            RTTraits::AddRef((RT*)pObj);
        }

        if (*pp != NULL) {
            RTTraits::Release((RT*)*pp);
        }

        *pp = pObj;
        return pObj;
    }

public:
    T     *p;

    CSmartPtr () {
        p = NULL;
    }

    CSmartPtr (T *pObj) {
        if ((p = pObj) != NULL) {
            RTTraits::AddRef((RT*)p);
        }
    }

    CSmartPtr (const CSmartPtr<T,RT> &sp) {
        if ((p = sp.p) != NULL) {
            RTTraits::AddRef((RT*)p);
        }
    }

    ~CSmartPtr () {
        if (p != NULL) {
            RTTraits::Release((RT*)p);
        }
    }

    void Release () {
        if (p != NULL) {
            RTTraits::Release((RT*)p);
            p = NULL;
        }
    }

    operator T * () { return p; }
    operator const T * () const { return p; }
    //operator bool () { return (p != NULL); }
    T ** operator & () { ASSERT (p == NULL); return &p; }
    T * operator -> () { ASSERT (p != NULL); return p; }
    const T * operator -> () const { ASSERT (p != NULL); return p; }
    T * operator = (T *pObj) { return Assign (&p, pObj); }
    T * operator = (const CSmartPtr<T,RT> &sp) { return Assign (&p, sp.p); }
    bool operator ! () { return p == NULL; }
    bool operator != (const T * _p) const { return _p != p; }
    bool operator == (const T * _p) const { return _p == p; }
    bool operator != (const CSmartPtr<T,RT> &sp) const { return sp.p != p; }
    bool operator == (const CSmartPtr<T,RT> &sp) const { return sp.p == p; }
    T       *Detach () { T *pTmp = p; p = NULL; return pTmp; }
};


////////////////////////////////////////////////////////////////////////////////
// CAutoRef

class CAutoRef
{
private:
    LONG    m_iRef;                 // Ref count
public:
    CAutoRef() : m_iRef(0) {};
    virtual ~CAutoRef() {}
    DWORD   AddRef () { return InterlockedIncrement (&m_iRef); }
    DWORD   Release ()
    {
        long l = InterlockedDecrement (&m_iRef);
        if (l == 0) 
            delete this; 
        return l; 
    }

    long GetReferenceCount() { return m_iRef; }
};

#endif  // _OBJPTR_H_
