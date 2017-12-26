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
#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

#include "metadata.h"
#include "interoputil.h"

//-------------------------------------------------------------------------
// Macro for declaring a generic holder class.  Methods on the holder have
// the same names as the methods on the underlying objects.
// 
// It declares 3 classes:
//      Class##Taker
//      Class##HolderNoDtor
//      Class##Holder
//-------------------------------------------------------------------------
#define DECLARE_SIMPLE_WRAPPER_CLASSES(Class, fEnter, fExit)                            \
class Class##Taker {                                                                    \
    Class *m_pObject;                                                                   \
public:                                                                                 \
    Class##Taker(Class* pObj) : m_pObject(pObj) { WRAPPER_CONTRACT; m_pObject->fEnter(); } \
    ~Class##Taker() { WRAPPER_CONTRACT; m_pObject->fExit(); }                           \
    FORCEINLINE void Acquire() {WRAPPER_CONTRACT; m_pObject->fEnter();}												\
    FORCEINLINE void Release() {WRAPPER_CONTRACT; m_pObject->fExit();}												\
};                                                                                      \
                                                                                        \
class Class##HolderNoDtor {                                                             \
    Class *m_pObject;                                                                   \
    DWORD m_held;                                                                       \
public:                                                                                 \
    Class##HolderNoDtor(Class* pObj) : m_pObject(pObj), m_held(FALSE) { LEAF_CONTRACT; }\
                                                                                        \
    void fEnter() {                                                                     \
        WRAPPER_CONTRACT;                                                               \
        _ASSERTE(!m_held);                                                              \
        m_pObject->fEnter();                                                            \
        m_held = TRUE;                                                                  \
    }                                                                                   \
                                                                                        \
    void fExit() {                                                                      \
        WRAPPER_CONTRACT;                                                               \
        _ASSERTE(m_held);                                                               \
        m_pObject->fExit();                                                             \
        m_held = FALSE;                                                                 \
    }                                                                                   \
                                                                                        \
    BOOL IsHeld() {                                                                     \
        LEAF_CONTRACT;                                                                  \
        return m_held;                                                                  \
    }                                                                                   \
                                                                                        \
    void Destroy() {                                                                    \
        WRAPPER_CONTRACT;                                                               \
        if (m_held)                                                                     \
            m_pObject->fExit();                                                         \
        m_held = FALSE;                                                                 \
    }                                                                                   \
	FORCEINLINE void Acquire() {WRAPPER_CONTRACT; fEnter();}												\
    FORCEINLINE void Release() {WRAPPER_CONTRACT; fExit();}												\
};                                                                                      \
                                                                                        \
class Class##Holder : public Class##HolderNoDtor {                                      \
public:                                                                                 \
    Class##Holder(Class* pObj) : Class##HolderNoDtor(pObj) { LEAF_CONTRACT; }           \
    ~Class##Holder() { WRAPPER_CONTRACT; Destroy(); }                                   \
};


//-------------------------------------------------------------------------------------
// When a single class has multiple resources that you want to create wrappers for,
// you need an extra argument to create unique wrapper class names.  This macro creates
// those unique names by taking an extra argument that is appended to the class
// name.
// 
// It declares classes called:
//      Class##Style##Taker
//      Class##Style##HolderNoDtor
//      Class##Style##Holder
//-------------------------------------------------------------------------------------

#define DECLARE_WRAPPER_CLASSES(Class, Style, fEnter, fExit)                            \
class Class##Style##Taker {                                                             \
    Class *m_pObject;                                                                   \
public:                                                                                 \
    Class##Style##Taker(Class* pObj) : m_pObject(pObj) { WRAPPER_CONTRACT; m_pObject->fEnter(); }         \
    ~Class##Style##Taker() { WRAPPER_CONTRACT; m_pObject->fExit(); }                    \
};                                                                                      \
                                                                                        \
class Class##Style##HolderNoDtor {                                                      \
    Class *m_pObject;                                                                   \
    DWORD m_held;                                                                       \
public:                                                                                 \
    Class##Style##HolderNoDtor(Class* pObj) : m_pObject(pObj), m_held(FALSE) { LEAF_CONTRACT; }        \
                                                                                        \
    void fEnter() {                                                                     \
        WRAPPER_CONTRACT;                                                               \
        _ASSERTE(!m_held);                                                              \
        m_pObject->fEnter();                                                            \
        m_held = TRUE;                                                                  \
    }                                                                                   \
                                                                                        \
    void fExit() {                                                                      \
        WRAPPER_CONTRACT;                                                               \
        _ASSERTE(m_held);                                                               \
        m_pObject->fExit();                                                             \
        m_held = FALSE;                                                                 \
    }                                                                                   \
                                                                                        \
    BOOL IsHeld() {                                                                     \
        LEAF_CONTRACT;                                                                  \
        return m_held;                                                                  \
    }                                                                                   \
                                                                                        \
    void Destroy() {                                                                    \
        WRAPPER_CONTRACT;                                                               \
        if (m_held)                                                                     \
            m_pObject->fExit();                                                         \
        m_held = FALSE;                                                                 \
    }                                                                                   \
};                                                                                      \
                                                                                        \
class Class##Style##Holder : public Class##Style##HolderNoDtor {                        \
public:                                                                                 \
    Class##Style##Holder(Class* pObj) : Class##Style##HolderNoDtor(pObj) { LEAF_CONTRACT; }            \
    ~Class##Style##Holder() { WRAPPER_CONTRACT; Destroy(); }                            \
};


//-------------------------------------------
// This variant supports Enter/TryEnter/Exit.  It also has a "style" parameter that
// is used to create the name.  It declares classes called:
//      Class##Style##Taker
//      Class##Style##HolderNoDtor
//      Class##Style##Holder
//-------------------------------------------

#define DECLARE_WRAPPER_CLASSES_WITH_TRY(Class, Style, fEnter, fTryEnter, fExit)        \
class Class##Style##Taker {                                                             \
    Class *m_pObject;                                                                   \
public:                                                                                 \
    Class##Style##Taker(Class* pObj) : m_pObject(pObj) { WRAPPER_CONTRACT; m_pObject->fEnter(); }         \
    ~Class##Style##Taker() { WRAPPER_CONTRACT; m_pObject->fExit(); }                    \
};                                                                                      \
                                                                                        \
class Class##Style##HolderNoDtor {                                                      \
    Class *m_pObject;                                                                   \
    DWORD m_held;                                                                       \
public:                                                                                 \
    Class##Style##HolderNoDtor(Class* pObj) : m_pObject(pObj), m_held(FALSE) { LEAF_CONTRACT; }        \
                                                                                        \
    void fEnter() {                                                                     \
        WRAPPER_CONTRACT;                                                               \
        _ASSERTE(!m_held);                                                              \
        m_pObject->fEnter();                                                            \
        m_held = TRUE;                                                                  \
    }                                                                                   \
                                                                                        \
    BOOL fTryEnter() {                                                                  \
        WRAPPER_CONTRACT;                                                               \
        _ASSERTE(!m_held);                                                              \
        BOOL ret = m_pObject->fTryEnter();                                              \
        if (ret)                                                                        \
            m_held = TRUE;                                                              \
        return ret;                                                                     \
    }                                                                                   \
                                                                                        \
    void fExit() {                                                                      \
        WRAPPER_CONTRACT;                                                               \
        _ASSERTE(m_held);                                                               \
        m_pObject->fExit();                                                             \
        m_held = FALSE;                                                                 \
    }                                                                                   \
                                                                                        \
    BOOL IsHeld() {                                                                     \
        LEAF_CONTRACT;                                                                  \
        return m_held;                                                                  \
    }                                                                                   \
                                                                                        \
    void Destroy() {                                                                    \
        WRAPPER_CONTRACT;                                                               \
        if (m_held)                                                                     \
            m_pObject->fExit();                                                         \
        m_held = FALSE;                                                                 \
    }                                                                                   \
};                                                                                      \
                                                                                        \
class Class##Style##Holder : public Class##Style##HolderNoDtor {                        \
public:                                                                                 \
    Class##Style##Holder(Class* pObj) : Class##Style##HolderNoDtor(pObj) { LEAF_CONTRACT; }     \
    ~Class##Style##Holder() { WRAPPER_CONTRACT; Destroy(); }                            \
};


class HCorAsmEnumHolder
{
public:
    inline HCorAsmEnumHolder(IMetaDataAssemblyImport* IMDII)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(IMDII));
        }
        CONTRACTL_END;

        m_IMDII = IMDII;
        m_HEnum = 0;
    }

    inline ~HCorAsmEnumHolder()
    {
        WRAPPER_CONTRACT;
        m_IMDII->CloseEnum(m_HEnum);
    }

    inline operator HCORENUM()
    {
        LEAF_CONTRACT;
        return m_HEnum;   
    }

    inline HCORENUM* operator&()
    {
        LEAF_CONTRACT;
        return static_cast<HCORENUM*>(&m_HEnum);
    }

private:
    HCorAsmEnumHolder() {LEAF_CONTRACT;} // Must use parameterized constructor

    HCORENUM m_HEnum;
    IMetaDataAssemblyImport* m_IMDII;
};


class HCorEnumHolder
{
public:
    inline HCorEnumHolder(IMetaDataImport2* IMDII)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(IMDII));
        }
        CONTRACTL_END;

        m_IMDII = IMDII;
        m_HEnum = 0;
    }

    inline ~HCorEnumHolder()
    {
        WRAPPER_CONTRACT;
        m_IMDII->CloseEnum(m_HEnum);
    }

    inline operator HCORENUM()
    {
        LEAF_CONTRACT;
        return m_HEnum;   
    }

    inline HCORENUM* operator&()
    {
        LEAF_CONTRACT;
        return static_cast<HCORENUM*>(&m_HEnum);
    }

private:
    HCorEnumHolder() {LEAF_CONTRACT;} // Must use parameterized constructor

    HCORENUM m_HEnum;
    IMetaDataImport2* m_IMDII;
};


class MDEnumHolder
{
public:
    inline MDEnumHolder(IMDInternalImport* IMDII)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(IMDII));
        }
        CONTRACTL_END;

        m_IMDII = IMDII;
        
    }

    inline ~MDEnumHolder()
    {
        WRAPPER_CONTRACT;
        m_IMDII->EnumClose(&m_HEnum);
    }

    inline operator HENUMInternal()
    {
        LEAF_CONTRACT;
        return m_HEnum;   
    }

    inline HENUMInternal* operator&()
    {
        LEAF_CONTRACT;
        return static_cast<HENUMInternal*>(&m_HEnum);
    }

private:
    MDEnumHolder() {LEAF_CONTRACT;} // Must use parameterized constructor

    HENUMInternal       m_HEnum;
    IMDInternalImport*  m_IMDII;
};

class MDEnumTypeDefHolder
{
public:
    inline MDEnumTypeDefHolder(IMDInternalImport* IMDII)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(IMDII));
        }
        CONTRACTL_END;

        m_IMDII = IMDII;
        
    }

    inline ~MDEnumTypeDefHolder()
    {
        WRAPPER_CONTRACT;
        m_IMDII->EnumTypeDefClose(&m_HEnum);
    }

    inline operator HENUMInternal()
    {
        LEAF_CONTRACT;
        return m_HEnum;   
    }

    inline HENUMInternal* operator&()
    {
        LEAF_CONTRACT;
        return static_cast<HENUMInternal*>(&m_HEnum);
    }

private:
    MDEnumTypeDefHolder() {LEAF_CONTRACT;} // Must use parameterized constructor

    HENUMInternal       m_HEnum;
    IMDInternalImport*  m_IMDII;
};


//--------------------------------------------------------------------------------
// safe variant helper
void SafeVariantClear(VARIANT* pVar);

class VariantHolder
{
public:
    inline VariantHolder()
    {        
        LEAF_CONTRACT;
        memset(&m_var, 0, sizeof(VARIANT));
    }

    inline ~VariantHolder()
    {
        WRAPPER_CONTRACT;
        SafeVariantClear(&m_var);
    }

    inline VARIANT* operator&()
    {
        LEAF_CONTRACT;
        return static_cast<VARIANT*>(&m_var);
    }

private:
    VARIANT  m_var;
};


template <typename TYPE> 
inline void SafeComRelease(TYPE *value)
{
    WRAPPER_CONTRACT;

    if (value)
    {
        SafeRelease((IUnknown*)value);
    }
}

NEW_HOLDER_TEMPLATE1(SafeComHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, SafeComRelease<TYPE>, NULL);


//-----------------------------------------------------------------------------
// NewPreempHolder : New'ed memory holder, deletes in preemp mode.
//
//  {
//      NewPreempHolder<Foo> foo = new Foo ();
//  } // delete foo on out of scope in preemp mode.
//-----------------------------------------------------------------------------

template <typename TYPE> 
void DeletePreemp(TYPE *value)
{
    WRAPPER_CONTRACT;

    GCX_PREEMP();
    delete value;
}

NEW_HOLDER_TEMPLATE1(NewPreempHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, DeletePreemp<TYPE>, NULL);


//-----------------------------------------------------------------------------
// VariantPtrHolder : Variant holder, Calls VariantClear on scope exit.
//
//  {
//      VariantHolder foo = pVar
//  } // Call SafeVariantClear on out of scope.
//-----------------------------------------------------------------------------

FORCEINLINE void VariantPtrRelease(VARIANT* value)
{
    WRAPPER_CONTRACT;

    if (value)
    {
        SafeVariantClear(value);
    }
}

class VariantPtrHolder : public Wrapper<VARIANT*, VariantPtrDoNothing, VariantPtrRelease, NULL>
{
public:
    VariantPtrHolder(VARIANT* p = NULL)
        : Wrapper<VARIANT*, VariantPtrDoNothing, VariantPtrRelease, NULL>(p)
    {
        LEAF_CONTRACT;
    }
    
    FORCEINLINE void operator=(VARIANT* p)
    {
        WRAPPER_CONTRACT;

        Wrapper<VARIANT*, VariantPtrDoNothing, VariantPtrRelease, NULL>::operator=(p);
    }
};


//-----------------------------------------------------------------------------
// ZeroHolder : Sets value to zero on context exit.
//
//  {
//      ZeroHolder foo = &data;
//  } // set data to zero on context exit
//-----------------------------------------------------------------------------

FORCEINLINE void ZeroRelease(VOID* value)
{
    LEAF_CONTRACT;
    if (value)
    {
        (*(size_t*)value) = 0;
    }
}

class ZeroHolder : public Wrapper<VOID*, ZeroDoNothing, ZeroRelease, NULL>
{
public:
    ZeroHolder(VOID* p = NULL)
        : Wrapper<VOID*, ZeroDoNothing, ZeroRelease, NULL>(p)
    {
        LEAF_CONTRACT;
    }
    
    FORCEINLINE void operator=(VOID* p)
    {
        WRAPPER_CONTRACT;

        Wrapper<VOID*, ZeroDoNothing, ZeroRelease, NULL>::operator=(p);
    }
};


#endif // _WRAPPERS_H_
