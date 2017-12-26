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

#ifndef __HOLDER_H_
#define __HOLDER_H_

#include <wincrypt.h>
#include "cor.h"
#include "genericstackprobe.h"
#include "staticcontract.h"

// Note: you can't use CONTRACT's in this file. You can't use dynamic contracts because the impl of dynamic
// contracts depends on holders. You can't use static contracts because they include the function name as a string,
// and the template names in this file are just too darn long, so you get a compiler error.
//
// All the functions in this file are pretty basic, so the lack of CONTRACT's isn't a big loss.

#ifdef _MSC_VER
// Make sure we can recurse deep enough for FORCEINLINE
#pragma inline_recursion(on)
#pragma inline_depth(16)
#pragma warning(disable:4714)
#endif  // _MSC_VER

//-----------------------------------------------------------------------------
// Holder is the base class of all holder objects.  Any backout object should derive from it.
// (Eventually some additional bookeeping and exception handling code will be placed in this 
// base class.)
//
// There are several ways to use this class:
//  1. Derive from HolderBase, and instantiate a Holder or Wrapper around your base.  This is necessary
//      if you need to add more state to your holder.
//  2. Instantiate the Holder template with your type and functions.
//  3. Instantiate the Wrapper template with your type and functions.  The Wrapper adds some additional
//      operator overloads to provide "smart pointer" like behavior
//  4. Use a prebaked Holder.  This is ALWAYS the preferable strategy.  It is expected that
//      the general design patter is that Holders will be provided as part of a typical data abstraction. 
//      (See Crst for an example of this.)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// HolderBase defines the base holder functionality. You can subtype and plug in 
// a different base if you need to add more members for access during 
// acquire & release
//-----------------------------------------------------------------------------
template <typename TYPE>
class HolderBase
{
  protected:
    TYPE    m_value;
  
    HolderBase(TYPE value)
      : m_value(value)
    {
        EnsureSOIntolerantOK(__FUNCTION__, __FILE__, __LINE__);
    }

    void DoAcquire()
    {
        // Insert any global or thread bookeeping here
    }

    void DoRelease()
    {
        // Insert any global or thread bookeeping here
    }
};

template <typename TYPE>
BOOL CompareDefault(TYPE value, TYPE defaultValue)
{
    return value == defaultValue;
}

template <typename TYPE>
BOOL NoNull(TYPE value, TYPE defaultValue)
{
    return FALSE;
}

template <typename TYPE> 
class NoOpBaseHolder
{
  public:
    FORCEINLINE NoOpBaseHolder() 
    {
    }
    FORCEINLINE NoOpBaseHolder(TYPE value, BOOL take = TRUE) 
    {
    }
    FORCEINLINE ~NoOpBaseHolder() 
    {
    }
    FORCEINLINE void Assign(TYPE value)
    {
    }
    FORCEINLINE void Acquire()
    {
    }
    FORCEINLINE void Release()
    {
    }
    FORCEINLINE void Clear()
    {
    }
    FORCEINLINE void SuppressRelease()
    {
    }
    FORCEINLINE TYPE Extract()
    {
    }
    FORCEINLINE TYPE GetValue()
    {
    }
    FORCEINLINE BOOL IsDefault() const
    {
    }
    FORCEINLINE BOOL IsNull() const
    {
    }

  private:
    FORCEINLINE NoOpBaseHolder<TYPE>& operator=(const NoOpBaseHolder<TYPE> &holder)
    {
    }

    FORCEINLINE NoOpBaseHolder(const NoOpBaseHolder<TYPE> &holder)
    {
    }
};


#if defined(_MSC_VER) && (_MSC_VER <= 1300) 
template <typename TYPE, typename BASE, UINT_PTR DEFAULTVALUE = 0, BOOL IS_NULL(TYPE, TYPE) = CompareDefault, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq> 
#else
template <typename TYPE, typename BASE, UINT_PTR DEFAULTVALUE = 0, BOOL IS_NULL(TYPE, TYPE) = CompareDefault<TYPE>, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq> 
#endif
class BaseHolder : protected BASE
{
  protected:
    BOOL    m_acquired;      // Have we acquired the resource?

  public:
    FORCEINLINE BaseHolder() 
      : BASE((TYPE) DEFAULTVALUE), 
        m_acquired(FALSE)
    {
    }
    FORCEINLINE BaseHolder(TYPE value, BOOL take = TRUE) 
      : BASE(value), 
        m_acquired(FALSE)
    {
        if (take)
            Acquire();
    }
    FORCEINLINE ~BaseHolder() 
    {
        Release();
    }
    FORCEINLINE void Assign(TYPE value)
    {
        Release();
        m_value = value;
        Acquire();
    }
    FORCEINLINE void Acquire()
    {
        STATIC_CONTRACT_WRAPPER;
        _ASSERTE(!m_acquired);

        if (!IsNull())
        {
            DoAcquire();
            m_acquired = TRUE;
        }
    }
    FORCEINLINE void Release()
    {
        STATIC_CONTRACT_WRAPPER;
        if (m_acquired)
        {
            _ASSERTE(!IsNull());
        
            if (VALIDATION_TYPE != HSV_NoValidation)
            {
                VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(VALIDATION_TYPE);
                DoRelease();
            }
            else
            {
                DoRelease();
            }

            m_acquired = FALSE;
        }
    }
    FORCEINLINE void Clear()
    {
        STATIC_CONTRACT_WRAPPER;
        Release();
        m_value = (TYPE) DEFAULTVALUE;
    }
    FORCEINLINE void SuppressRelease()
    {
        STATIC_CONTRACT_LEAF;
        m_acquired = FALSE;
    }
    FORCEINLINE TYPE Extract()
    {
        STATIC_CONTRACT_WRAPPER;
        m_acquired = FALSE;
        return GetValue();
    }
    FORCEINLINE TYPE GetValue()
    {
        STATIC_CONTRACT_LEAF;
        return m_value;
    }
    FORCEINLINE BOOL IsDefault() const
    {
        STATIC_CONTRACT_LEAF;
        return m_value == (TYPE) DEFAULTVALUE;
    }
    FORCEINLINE BOOL IsNull() const
    {
        STATIC_CONTRACT_WRAPPER;
        return IS_NULL(m_value, (TYPE) DEFAULTVALUE);
    }

   private:
    FORCEINLINE BaseHolder<TYPE, BASE, DEFAULTVALUE, IS_NULL>& operator=(const BaseHolder<TYPE, BASE, DEFAULTVALUE, IS_NULL> &holder)
    {
        _ASSERTE(!"No assignment allowed");
        return *this;
    }

    FORCEINLINE BaseHolder(const BaseHolder<TYPE, BASE, DEFAULTVALUE, IS_NULL> &holder)
    {
        _ASSERTE(!"No copy construction allowed");
    }
};

#if __GNUC__
// GCC 2.95.4 doesn't properly handle template function parameters that
// take no arguments, so we have to give these functions an argument.
typedef int StateHolderParam;
#define StateHolderParamValue   0
#else   // __GNUC__
typedef void StateHolderParam;
#define StateHolderParamValue
#endif  // __GNUC__

template <void (*ACQUIRE)(StateHolderParam), void (*RELEASEF)(StateHolderParam), HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq>
class StateHolder
{
  private:
    BOOL    m_acquired;      // Have we acquired the state?

  public:
    FORCEINLINE StateHolder(BOOL take = TRUE) 
      : m_acquired(FALSE)
    {
        STATIC_CONTRACT_WRAPPER;
        if (take)
            Acquire();
    }
    FORCEINLINE ~StateHolder() 
    {
        STATIC_CONTRACT_WRAPPER;
        Release();
    }
    FORCEINLINE void Acquire()
    {
        STATIC_CONTRACT_WRAPPER;
        // Insert any global or thread bookeeping here

        _ASSERTE(!m_acquired);

        ACQUIRE(StateHolderParamValue);
        m_acquired = TRUE;
    }
    FORCEINLINE void Release()
    {
        STATIC_CONTRACT_WRAPPER;
        // Insert any global or thread bookeeping here

        if (m_acquired)
        {
            if (VALIDATION_TYPE != HSV_NoValidation)
            {
                VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(VALIDATION_TYPE);
                RELEASEF(StateHolderParamValue);
            }
            else
            {
                RELEASEF(StateHolderParamValue);
            }
            m_acquired = FALSE;
        }
    }
    FORCEINLINE void Clear()
    {
        STATIC_CONTRACT_WRAPPER;
        if (m_acquired)
        {
            RELEASEF(StateHolderParamValue);
        }

        m_acquired = FALSE;
    }
    FORCEINLINE void SuppressRelease()
    {
        STATIC_CONTRACT_LEAF;
        m_acquired = FALSE;
    }

  private:
    FORCEINLINE StateHolder<ACQUIRE, RELEASEF>& operator=(const StateHolder<ACQUIRE, RELEASEF> &holder)
    {
        _ASSERTE(!"No assignment allowed");
    }

    FORCEINLINE StateHolder(const StateHolder<ACQUIRE, RELEASEF> &holder)
    {
        _ASSERTE(!"No copy construction allowed");
    }
};

// Holder for the case where the acquire function can fail.
template <typename VALUE, BOOL (*ACQUIRE)(VALUE value), void (*RELEASEF)(VALUE value), HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq>
class ConditionalStateHolder
{
  private:
    VALUE   m_value;
    BOOL    m_acquired;      // Have we acquired the state?

  public:
    FORCEINLINE ConditionalStateHolder(VALUE value, BOOL take = TRUE) 
      : m_value(value), m_acquired(FALSE)
    {
        STATIC_CONTRACT_WRAPPER;
        if (take)
            Acquire();
    }
    FORCEINLINE ~ConditionalStateHolder() 
    {
        STATIC_CONTRACT_WRAPPER;
        Release();
    }
    FORCEINLINE BOOL Acquire()
    {
        STATIC_CONTRACT_WRAPPER;
        // Insert any global or thread bookeeping here

        _ASSERTE(!m_acquired);

        m_acquired = ACQUIRE(m_value);
        
        return m_acquired;
    }
    FORCEINLINE void Release()
    {
        STATIC_CONTRACT_WRAPPER;
        // Insert any global or thread bookeeping here

        if (m_acquired)
        {
            if (VALIDATION_TYPE != HSV_NoValidation)
            {
                VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(VALIDATION_TYPE);
                RELEASEF(m_value);
            }
            else
            {
                RELEASEF(m_value);
            }
            m_acquired = FALSE;
        }
    }
    FORCEINLINE void Clear()
    {
        STATIC_CONTRACT_WRAPPER;
        if (m_acquired)
        {
            RELEASEF(m_value);
        }

        m_acquired = FALSE;
    }
    FORCEINLINE void SuppressRelease()
    {
        STATIC_CONTRACT_LEAF;
        m_acquired = FALSE;
    }
    FORCEINLINE BOOL Acquired()
    {
        STATIC_CONTRACT_LEAF;

        return m_acquired;
    }
  private:
    FORCEINLINE ConditionalStateHolder<VALUE, ACQUIRE, RELEASEF>& operator=(const ConditionalStateHolder<VALUE, ACQUIRE, RELEASEF> &holder)
    {
        _ASSERTE(!"No assignment allowed");
    }

    FORCEINLINE ConditionalStateHolder(const ConditionalStateHolder<VALUE, ACQUIRE, RELEASEF> &holder)
    {
        _ASSERTE(!"No copy construction allowed");
    }
};


// Making the copy constructor private produces a warning about "can't generate copy
// constructor" on all holders (duh, that's the point.)
#ifdef _MSC_VER
#pragma warning(disable:4511)
#endif  // _MSC_VER

//-----------------------------------------------------------------------------
// BaseWrapper is just Baselike a Holder, but it "transparently" proxies the type it contains,
// using operator overloads.  Use this when you want a holder to expose the functionality of 
// the value it contains.
//-----------------------------------------------------------------------------
#if defined(_MSC_VER) && (_MSC_VER <= 1300)
template <typename TYPE, typename BASE,
          UINT_PTR DEFAULTVALUE = 0, BOOL IS_NULL(TYPE, TYPE) = CompareDefault, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq> 
#else
template <typename TYPE, typename BASE,
          UINT_PTR DEFAULTVALUE = 0, BOOL IS_NULL(TYPE, TYPE) = CompareDefault<TYPE>, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq> 
#endif
class BaseWrapper : public BaseHolder<TYPE, BASE, DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>
{
    // This temporary object takes care of the case where we are initializing 
    // a holder's contents by passing it as an out parameter.  The object is 
    // guaranteed to live longer than the call it is passed to, hence we should
    // properly acquire the object on return
    friend class AddressInitHolder;
    class AddressInitHolder
    {
      protected:
        BaseWrapper<TYPE,BASE,DEFAULTVALUE,IS_NULL> &m_holder;

      public:
        FORCEINLINE AddressInitHolder(BaseWrapper<TYPE,BASE,DEFAULTVALUE,IS_NULL> &holder) 
          : m_holder(holder) 
        {
            m_holder.Release();
        }
        FORCEINLINE ~AddressInitHolder()
        {
            m_holder.Acquire();
        }


        FORCEINLINE operator IUnknown **()
        {
            IUnknown *unknown;
            // Typesafe check.  This will fail at compile time if
            // m_holder.m_value can't be converted to an IUnknown *.
            unknown = static_cast<IUnknown*>(m_holder.m_value);
            // do the cast with an unsafe cast
            return (IUnknown **)(&m_holder.m_value);
        }
        FORCEINLINE operator void **()
        {
            return (void **)(&m_holder.m_value);
        }
        FORCEINLINE operator void *()
        {
            return (void *)(&m_holder.m_value);
        }
    };

    // Separate out method with TYPE * cast operator, since it may clash with IUnknown ** or 
    // void ** cast operator.
    friend class TypedAddressInitHolder;
    class TypedAddressInitHolder : public AddressInitHolder
    {
      public:
        FORCEINLINE TypedAddressInitHolder(BaseWrapper<TYPE,BASE,DEFAULTVALUE,IS_NULL> &holder) 
          : AddressInitHolder(holder)
        {
        }

        FORCEINLINE operator TYPE *()
        {
            return static_cast<TYPE *>(&m_holder.m_value);
        }
    };

  public:
    FORCEINLINE BaseWrapper(TYPE value = (TYPE) DEFAULTVALUE, BOOL take = TRUE)
        : BaseHolder<TYPE, BASE, DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>(value, take)
    {
    }
    FORCEINLINE TYPE& operator=(TYPE value)
    {
        Assign(value);
        return m_value;
    }
    FORCEINLINE operator TYPE()
    {
        return m_value;
    }
    TypedAddressInitHolder operator&() 
    {
        return TypedAddressInitHolder(*this);
    }
   
    FORCEINLINE int operator==(TYPE value)
    {
        return m_value == value;
    }
    FORCEINLINE int operator!=(TYPE value)
    {
        return m_value != value;
    }
    FORCEINLINE const TYPE &operator->()
    {
        return m_value;
    }
    FORCEINLINE int operator!() const
    {
        return IsNull();
    }
};

//-----------------------------------------------------------------------------
// Generic templates to use to wrap up acquire/release functionality for Holder
//----------------------------------------------------------------------------- 

template <typename TYPE>
FORCEINLINE void DoNothing(TYPE value)
{
}

FORCEINLINE void DoNothing(StateHolderParam)
{
}

// Prefast stuff.We should have DoNothing<type*> in the holder declaration, but currently
// prefast doesnt support, it, so im stuffing all these here so if we need to change the template you can change
// everything here. When prefast works, remove the following functions
struct ConnectionCookie;
FORCEINLINE void ConnectionCookieDoNothing(ConnectionCookie* p)
{
}

class ComCallWrapper;
FORCEINLINE void CCWHolderDoNothing(ComCallWrapper* p)
{
}


FORCEINLINE void DispParamHolderDoNothing(VARIANT* p)
{
}

FORCEINLINE void VariantPtrDoNothing(VARIANT* p)
{
}

FORCEINLINE void VariantDoNothing(VARIANT)
{
}

FORCEINLINE void ZeroDoNothing(VOID* p)
{
}

struct IUnkEntry;
FORCEINLINE void IUnkEntryDoNothing(IUnkEntry* p)
{
}

class CleanupWorkList;
FORCEINLINE void CleanupWLExceptionDoNothing(CleanupWorkList* p)
{
}


class CtxEntry;
FORCEINLINE void CtxEntryDoNothing(CtxEntry* p)
{
}

struct RCW;
FORCEINLINE void NewRCWHolderDoNothing(RCW*)
{
}

// Prefast stuff.We should have DoNothing<SafeArray*> in the holder declaration
FORCEINLINE void SafeArrayDoNothing(SAFEARRAY* p)
{
}




//-----------------------------------------------------------------------------
// Holder/Wrapper are the simplest way to define holders - they synthesizes a base class out of 
// function pointers
//-----------------------------------------------------------------------------

template <typename TYPE, void (*ACQUIREF)(TYPE), void (*RELEASEF)(TYPE), HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq>  
class FunctionBase : protected HolderBase<TYPE>
{
  protected:

    FunctionBase(TYPE value)
      : HolderBase<TYPE>(value)
    {
    }

    void DoAcquire()
    {
        ACQUIREF(m_value);
    }

    void DoRelease()
    {
        if (VALIDATION_TYPE != HSV_NoValidation)
        {
            VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(VALIDATION_TYPE);
            RELEASEF(m_value);
        }
        else
        {
            RELEASEF(m_value);
        }
    }
};

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
template <typename TYPE, void (*ACQUIREF)(TYPE), void (*RELEASEF)(TYPE), UINT_PTR DEFAULTVALUE = 0, 
    BOOL IS_NULL(TYPE, TYPE) = CompareDefault, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq>
#else
template <typename TYPE, void (*ACQUIREF)(TYPE), void (*RELEASEF)(TYPE), UINT_PTR DEFAULTVALUE = 0, 
    BOOL IS_NULL(TYPE, TYPE) = CompareDefault<TYPE>, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq>
#endif

class Holder : public BaseHolder<TYPE, FunctionBase<TYPE, ACQUIREF, RELEASEF, VALIDATION_TYPE>,
                                 DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>
{
  public:
    FORCEINLINE Holder(TYPE p = (TYPE) DEFAULTVALUE, BOOL Take = TRUE) 
      : BaseHolder<TYPE, FunctionBase<TYPE, ACQUIREF, RELEASEF, VALIDATION_TYPE>, DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>(p, Take)
    {
        STATIC_CONTRACT_WRAPPER;
    }
    FORCEINLINE TYPE& operator=(TYPE p)
    {
        STATIC_CONTRACT_WRAPPER;
        return BaseHolder<TYPE, FunctionBase<TYPE, ACQUIREF, RELEASEF, VALIDATION_TYPE>, DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>::operator=(p);
    }
};

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
template <typename TYPE, void (*ACQUIREF)(TYPE), void (*RELEASEF)(TYPE), UINT_PTR DEFAULTVALUE = 0, 
    BOOL IS_NULL(TYPE, TYPE) = CompareDefault, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq> 
#else
template <typename TYPE, void (*ACQUIREF)(TYPE), void (*RELEASEF)(TYPE), UINT_PTR DEFAULTVALUE = 0,
    BOOL IS_NULL(TYPE, TYPE) = CompareDefault<TYPE>, HolderStackValidation VALIDATION_TYPE = HSV_ValidateNormalStackReq>
#endif
class Wrapper : public BaseWrapper<TYPE, FunctionBase<TYPE, ACQUIREF, RELEASEF, VALIDATION_TYPE>,
                                   DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>
{
  public:
    FORCEINLINE Wrapper(TYPE p = (TYPE) DEFAULTVALUE, BOOL Take = TRUE) 
      : BaseWrapper<TYPE, FunctionBase<TYPE, ACQUIREF, RELEASEF, VALIDATION_TYPE>, DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>(p, Take)
    {
        STATIC_CONTRACT_WRAPPER;
    }
    FORCEINLINE TYPE& operator=(TYPE p)
    {
        STATIC_CONTRACT_WRAPPER;
        return BaseWrapper<TYPE, FunctionBase<TYPE, ACQUIREF, RELEASEF, VALIDATION_TYPE>, DEFAULTVALUE, IS_NULL, VALIDATION_TYPE>::operator=(p);
    }
};

//-----------------------------------------------------------------------------
// New template holder type macros.   These save some effort when specializing
// existing holder templates.  (We would rather use a construct like:
//
// template <P1>
// typedef Holder<...> NewHolder;
//
// But this construct doesn't exist in C++.  These macros ease some of the cruft necessary
// to get similar functionality out of class templates.
//----------------------------------------------------------------------------- 

#define NEW_HOLDER_TEMPLATE1(_NAME, _P1, _HOLDER, _TYPE, _ACQUIREF, _RELEASE, _NULL)        \
                                                                                            \
template <_P1,                                                                              \
          /* NOTE: ignore HOLDER parameter - it is a bug workaround for VC7 */              \
          typename HOLDER = _HOLDER<_TYPE, _ACQUIREF, _RELEASE, _NULL > >                   \
class _NAME : public HOLDER                                                                 \
{                                                                                           \
public:                                                                                     \
    FORCEINLINE _NAME(_TYPE p = _NULL) : HOLDER(p)                                          \
    {                                                                                       \
        STATIC_CONTRACT_WRAPPER;                                                            \
    }                                                                                       \
    FORCEINLINE _TYPE& operator=(_TYPE p)                                                    \
    {                                                                                       \
        STATIC_CONTRACT_WRAPPER;                                                            \
        return HOLDER::operator=(p);                                                        \
    }                                                                                       \
    /* Since operator& may be overloaded (definitely for the Wrapper base class */          \
    /* we need a way to get a type safe this pointer. */                                    \
    FORCEINLINE _NAME* GetAddr()                                                            \
    {                                                                                       \
        STATIC_CONTRACT_LEAF;                                                               \
        return this;                                                                        \
    }                                                                                       \
};

//-----------------------------------------------------------------------------
// NOTE: THIS IS UNSAFE TO USE IN THE VM for interop COM objects!!
//  WE DO NOT CORRECTLY CHANGE TO PREEMPTIVE MODE BEFORE CALLING RELEASE!!
//  USE SafeComHolder
//
// ReleaseHolder : COM Interface holder for use outside the VM (or on well known instances
//                  which do not need preemptive Relesae)
//
// Usage example:
//
//  {
//      ReleaseHolder<IFoo> foo;
//      hr = FunctionToGetRefOfFoo(&foo); 
//      // Note ComHolder doesn't call AddRef - it assumes you already have a ref (if non-0).
//  } // foo->Release() on out of scope (WITHOUT RESPECT FOR GC MODE!!)
//
//-----------------------------------------------------------------------------

template <typename TYPE> 
inline void DoTheRelease(TYPE *value)
{
    if (value)
    {
        VALIDATE_HOLDER_STACK_CONSUMPTION_FOR_TYPE(HSV_ValidateNormalStackReq);
        value->Release();
    }
}

NEW_HOLDER_TEMPLATE1(ReleaseHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, DoTheRelease<TYPE>, NULL);

NEW_HOLDER_TEMPLATE1(NonVMComHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, DoTheRelease<TYPE>, NULL);


//-----------------------------------------------------------------------------
// StubHolder : holder for stubs
//
// Usage example:
//
//  {
//      StubHolder<Stub> foo;
//      foo = new Stub();
//      foo->AddRef();
//      // Note StubHolder doesn't call AddRef for you.
//  } // foo->DecRef() on out of scope
//
//-----------------------------------------------------------------------------
template <typename TYPE>
FORCEINLINE void StubRelease(TYPE* value)
{
    if (value)
        value->DecRef();
}

NEW_HOLDER_TEMPLATE1(StubHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, StubRelease<TYPE>, NULL);

//-----------------------------------------------------------------------------
// CoTaskMemHolder : CoTaskMemAlloc allocated memory holder
//
//  {
//      CoTaskMemHolder<Foo> foo = (Foo*) CoTaskMemAlloc(sizeof(Foo));
//  } // delete foo on out of scope
//-----------------------------------------------------------------------------

template <typename TYPE> 
FORCEINLINE void DeleteCoTaskMem(TYPE *value)
{
    if (value)
        CoTaskMemFree(value);
}

NEW_HOLDER_TEMPLATE1(CoTaskMemHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, DeleteCoTaskMem<TYPE>, NULL);

//-----------------------------------------------------------------------------
// NewHolder : New'ed memory holder
//
//  {
//      NewHolder<Foo> foo = new Foo ();
//  } // delete foo on out of scope
//-----------------------------------------------------------------------------

template <typename TYPE> 
FORCEINLINE void Delete(TYPE *value)
{
    STATIC_CONTRACT_LEAF;
    
    delete value;
}

NEW_HOLDER_TEMPLATE1(NewHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, Delete<TYPE>, NULL);

 //-----------------------------------------------------------------------------
// NewExecutableHolder : New'ed memory holder for executable memory.
//
//  {
//      NewExecutableHolder<Foo> foo = (Foo*) new (executable) Byte[num];
//  } // delete foo on out of scope
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// NewHolder : New'ed memory holder
//
//  Foo *global;
//
//  {
//      NewPtrHolder<Foo> foo(&global);
//
//      global = new Foo ();
//  } // delete foo on out of scope
//-----------------------------------------------------------------------------

template <typename TYPE>
FORCEINLINE void InitPtr(TYPE **value)
{
    STATIC_CONTRACT_LEAF;
    *value = NULL;
}

template <typename TYPE> 
FORCEINLINE void DeletePtr(TYPE **value)
{
    STATIC_CONTRACT_LEAF;
    delete *value;
    *value = NULL;
}

NEW_HOLDER_TEMPLATE1(NewPtrHolder, typename TYPE, Wrapper,
                     TYPE**, InitPtr<TYPE>, DeletePtr<TYPE>, NULL);
 
//-----------------------------------------------------------------------------
// NewArrayHolder : New []'ed pointer holder
//  {
//      NewArrayHolder<Foo> foo = new Foo [30];
//  } // delete [] foo on out of scope
//-----------------------------------------------------------------------------

template <typename TYPE> 
FORCEINLINE void DeleteArray(TYPE *value)
{
    STATIC_CONTRACT_WRAPPER;
    delete [] value;
}

NEW_HOLDER_TEMPLATE1(NewArrayHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, DeleteArray<TYPE>, NULL);

//-----------------------------------------------------------------------------
// ResetPointerHolder : pointer which needs to be set to NULL
//  {
//      ResetPointerHolder<Foo> holder = &pFoo;
//  } // "*pFoo=NULL" on out of scope
//-----------------------------------------------------------------------------

template <typename TYPE> 
FORCEINLINE void ResetPointer(TYPE **pPtr)
{
    STATIC_CONTRACT_WRAPPER;
    *pPtr = NULL;
}

NEW_HOLDER_TEMPLATE1(ResetPointerHolder, typename TYPE, Wrapper,
                     TYPE**, DoNothing<TYPE**>, ResetPointer<TYPE>, NULL);

//-----------------------------------------------------------------------------
// Wrap win32 functions using HANDLE
//-----------------------------------------------------------------------------

FORCEINLINE void VoidCloseHandle(HANDLE h) { if (h != NULL) CloseHandle(h); }
FORCEINLINE void VoidFindClose(HANDLE h) { FindClose(h); }
FORCEINLINE void VoidUnmapViewOfFile(void *ptr) { UnmapViewOfFile(ptr); }

template <typename TYPE> 
FORCEINLINE void TypeUnmapViewOfFile(TYPE *ptr) { UnmapViewOfFile(ptr); }

typedef Wrapper<HANDLE, DoNothing<HANDLE>, VoidCloseHandle, (UINT_PTR) INVALID_HANDLE_VALUE> HandleHolder;
typedef Wrapper<HANDLE, DoNothing<HANDLE>, VoidFindClose, (UINT_PTR) INVALID_HANDLE_VALUE> FindHandleHolder;

typedef Wrapper<void *, DoNothing, VoidUnmapViewOfFile> MapViewHolder;

// Crypto holders
FORCEINLINE void VoidCryptReleaseContext(HCRYPTPROV h) { CryptReleaseContext(h, 0); }
FORCEINLINE void VoidCryptDestroyHash(HCRYPTHASH h) { CryptDestroyHash(h); }
FORCEINLINE void VoidCryptDestroyKey(HCRYPTKEY h) { CryptDestroyKey(h); }

#if defined (_MSC_VER) && (_MSC_VER <= 1300)
template void DoNothing(HCRYPTPROV);
template BOOL CompareDefault(HCRYPTPROV,HCRYPTPROV);
#endif
typedef Wrapper<HCRYPTPROV, DoNothing, VoidCryptReleaseContext, 0> HandleCSPHolder;
typedef Wrapper<HCRYPTHASH, DoNothing, VoidCryptDestroyHash, 0> HandleHashHolder;
typedef Wrapper<HCRYPTKEY, DoNothing, VoidCryptDestroyKey, 0> HandleKeyHolder;


//-----------------------------------------------------------------------------
// Misc holders
//-----------------------------------------------------------------------------

// A holder for HMODULE.
FORCEINLINE void HolderFreeLibrary(HMODULE h) { FreeLibrary(h); }

typedef Wrapper<HMODULE, DoNothing<HMODULE>, HolderFreeLibrary, NULL> HModuleHolder;

FORCEINLINE void VoidLocalFree(HLOCAL l) { (LocalFree)(l); }
typedef Wrapper<HLOCAL, DoNothing, VoidLocalFree> LocalAllocHolder;

inline void BoolSet( bool * val ) { *val = true; }
inline void BoolUnset( bool * val ) { *val = false; }

typedef Wrapper< bool *, BoolSet, BoolUnset > BoolFlagStateHolder;

FORCEINLINE void CounterIncrease(volatile LONG* p) {InterlockedIncrement(p);};
FORCEINLINE void CounterDecrease(volatile LONG* p) {InterlockedDecrement(p);};

#if defined (_MSC_VER) && (_MSC_VER <= 1300)
template BOOL CompareDefault(volatile LONG*, volatile LONG*);
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
typedef Wrapper<volatile LONG*, CounterIncrease, CounterDecrease, (UINT_PTR)0, CompareDefault,                 HSV_NoValidation> CounterHolder;
#else
typedef Wrapper<volatile LONG*, CounterIncrease, CounterDecrease, (UINT_PTR)0, CompareDefault<volatile LONG*>, HSV_NoValidation> CounterHolder;
#endif


class ErrorModeHolder
{
    UINT m_oldMode;
public:
    ErrorModeHolder(UINT newMode){m_oldMode=SetErrorMode(newMode);};
    ~ErrorModeHolder(){SetErrorMode(m_oldMode);};
};

class SaveLastErrorHolder
{
    public:
        enum SaveLastErrorFlag
        {
            SLEF_AlwaysRestore      = 0x00000001,     // always restore the last error
            SLEF_CheckBeforeRestore = 0x00000002,     // only restore if the last error value is indeed an error AND 
                                                      // there is no new error
        };

        SaveLastErrorHolder(SaveLastErrorFlag flag)
        {
            STATIC_CONTRACT_SO_TOLERANT;
            m_dwLastError = GetLastError();
            m_flag        = flag;
        }

        ~SaveLastErrorHolder()
        {
            STATIC_CONTRACT_SO_TOLERANT;
            
            if ( (m_flag == SLEF_AlwaysRestore) ||
                 (m_dwLastError != 0 && GetLastError() == 0) )
            {
                SetLastError(m_dwLastError);
            }
        }

    private:
        DWORD               m_dwLastError;
        SaveLastErrorFlag   m_flag;
};// SaveLastErroHolderr

#ifdef __STRONG_NAME_H
FORCEINLINE void HolderStrongNameFreeBuffer(BYTE *pbMemory) { StrongNameFreeBuffer(pbMemory); }
typedef Wrapper<BYTE*, DoNothing<BYTE*>, HolderStrongNameFreeBuffer, NULL> StrongNameHolder;
#endif //__STRONG_NAME_H


//-----------------------------------------------------------------------------
// Wrapper to suppress auto-destructor (UNDER CONSTRUCTION)
// Usage:
//
//      BEGIN_MANUAL_HOLDER(NewArrayHolder<Foo>,  foo);
//      ... use foo via ->
//      END_MANUAL_HOLDER(foo);
// 
//-----------------------------------------------------------------------------

template <typename TYPE, SIZE_T SIZE = sizeof(TYPE)>
class NoAuto__DONTUSEDIRECTLY
{
  private:
    BYTE hiddeninstance[SIZE];

  public:
    NoAuto__DONTUSEDIRECTLY()
    {
        new (hiddeninstance) TYPE ();
    }

    operator TYPE& () { return *(TYPE *)hiddeninstance; }
    TYPE& operator->() { return *(TYPE *)hiddeninstance; }
    TYPE& operator*() { return *(TYPE *)hiddeninstance; }

    void Destructor() { (*(TYPE*)hiddeninstance)->TYPE::~TYPE(); }
};

#define BEGIN_MANUAL_HOLDER(_TYPE, _NAME)           \
    {                                               \
        NoAuto__DONTUSEDIRECTLY<_TYPE> _NAME;       \
        __try                                       \
        {

#define END_MANUAL_HOLDER(_NAME)                    \
        }                                           \
        __finally                                   \
        {                                           \
            _NAME.Destructor();                     \
        }                                           \
    }

//----------------------------------------------------------------------------
//
// External data access does not want certain holder implementations
// to be active as locks should not be taken and so on.  Provide
// a no-op in that case.
//
//----------------------------------------------------------------------------

#ifndef DACCESS_COMPILE

#define DacHolder Holder

#else

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
template <typename TYPE, void (*ACQUIRE)(TYPE), void (*RELEASEF)(TYPE), UINT_PTR DEFAULTVALUE = 0, BOOL IS_NULL(TYPE, TYPE) = CompareDefault, BOOL VALIDATE_BACKOUT_STACK = TRUE>
#else
template <typename TYPE, void (*ACQUIRE)(TYPE), void (*RELEASEF)(TYPE), UINT_PTR DEFAULTVALUE = 0, BOOL IS_NULL(TYPE, TYPE) = CompareDefault<TYPE>, BOOL VALIDATE_BACKOUT_STACK = TRUE>
#endif
class DacHolder
{
  protected:
    TYPE    m_value;

  private:
    BOOL    m_acquired;      // Have we acquired the resource?

  public:
    FORCEINLINE DacHolder() 
      : m_value((TYPE) DEFAULTVALUE), 
        m_acquired(FALSE)
    {
    }
    FORCEINLINE DacHolder(TYPE value, BOOL take = TRUE) 
      : m_value(value), 
        m_acquired(FALSE)
    {
    }
    FORCEINLINE ~DacHolder() 
    {
    }
    FORCEINLINE void Assign(TYPE value)
    {
        m_value = value;
    }
    FORCEINLINE void Acquire()
    {
        if (!IsNull())
        {
            m_acquired = TRUE;
        }
    }
    FORCEINLINE void Release()
    {
        // Insert any global or thread bookeeping here

        if (m_acquired)
        {
            m_acquired = FALSE;
        }
    }
    FORCEINLINE void Clear()
    {
        m_value = (TYPE) DEFAULTVALUE;
        m_acquired = FALSE;
    }
    FORCEINLINE void SuppressRelease()
    {
        m_acquired = FALSE;
    }
    FORCEINLINE TYPE GetValue()
    {
        return m_value;
    }
    FORCEINLINE BOOL IsDefault() const
    {
        return m_value == (TYPE) DEFAULTVALUE;
    }
    FORCEINLINE BOOL IsNull() const
    {
        return IS_NULL(m_value, (TYPE) DEFAULTVALUE);
    }

  private:
    FORCEINLINE DacHolder& operator=(const Holder<TYPE, ACQUIRE, RELEASEF> &holder)
    {
    }

    FORCEINLINE DacHolder(const Holder<TYPE, ACQUIRE, RELEASEF> &holder)
    {
    }
};

#endif // #ifndef DACCESS_COMPILE

#endif  // __HOLDER_H_
