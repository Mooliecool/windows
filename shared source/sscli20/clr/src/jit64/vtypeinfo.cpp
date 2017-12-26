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
/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XX                                                                           XX
XX                          typeInfo                                         XX
XX                                                                           XX
XX                                                                           XX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*/


    #include "config.h"
    #include "jit.h"



#include "corinfo.h"
#include "vtypeinfo.h"
#include "reader.h"

#ifndef NODEBUG
#define VASSERT(p) do { if (!(p)) {ReaderBase::GenIR_DebugError(__FILE__,__LINE__,#p);}} while (0)
#else
#define VASSERT(p) 
#endif

/*****************************************************************************
 * Verify child is compatible with the template parent.  Basically, that 
 * child is a "subclass" of parent -it can be substituted for parent 
 * anywhere.  Note that if parent contains fancy flags, such as "uninitialised"
 * , "is this ptr", or  "has byref local/field" info, then child must also 
 * contain those flags, otherwise FALSE will be returned !
 *
 * Rules for determining compatibility:
 *
 * If parent is a primitive type or value class, then child must be the 
 * same primitive type or value class.  The exception is that the built in 
 * value classes System/Boolean etc. are treated as synonyms for 
 * TI_BYTE etc.
 *
 * If parent is a byref of a primitive type or value class, then child
 * must be a byref of the same (rules same as above case).
 *
 * Byrefs are compatible only with byrefs.
 *
 * If parent is an object, child must be a subclass of it, implement it 
 * (if it is an interface), or be null.
 *
 * If parent is an array, child must be the same or subclassed array.
 *
 * If parent is a null objref, only null is compatible with it.
 *
 * If the "uninitialised", "by ref local/field", "this pointer" or other flags 
 * are different, the items are incompatible.
 *
 * parent CANNOT be an undefined (dead) item.
 *
 */

BOOL typeInfo::tiCompatibleWith(ICorJitInfo *jitInfo, const vertype& child, const vertype& parent)
{
    return ::tiCompatibleWith(jitInfo, child, parent);
}

BOOL tiCompatibleWith(ICorJitInfo *jitInfo, const vertype& child, const vertype& parent)
{
    if (child == parent)
    {
        return(TRUE);
    }

    // Normally we just let the runtime sort it out but we wish to be more strict
    // than the runtime wants to be.  For backwards compatibility, the runtime considers 
    // int32[] and nativeInt[] to be the same on 32-bit machines.  It also is OK with
    // int64[] and nativeInt[] on a 64-bit machine.  

    if (child.IsType(TI_REF) && parent.IsType(TI_REF)
        && jitInfo->isSDArray(child.GetClassHandleForObjRef())
        && jitInfo->isSDArray(parent.GetClassHandleForObjRef()))
    {
        BOOL runtime_OK;

        // never be more lenient than the runtime
        runtime_OK = jitInfo->canCast(child.m_cls, parent.m_cls);
        if (!runtime_OK)
            return false;

        CORINFO_CLASS_HANDLE handle;
        CorInfoType pType = jitInfo->getChildType(child.GetClassHandleForObjRef(),  &handle);
        CorInfoType cType = jitInfo->getChildType(parent.GetClassHandleForObjRef(), &handle);

        // don't care whether it is signed
        if (cType == CORINFO_TYPE_NATIVEUINT)
            cType = CORINFO_TYPE_NATIVEINT;
        if (pType == CORINFO_TYPE_NATIVEUINT)
            pType = CORINFO_TYPE_NATIVEINT;
        
        if (cType == CORINFO_TYPE_NATIVEINT)
            return pType == CORINFO_TYPE_NATIVEINT;
            
        if (pType == CORINFO_TYPE_NATIVEINT)
            return cType == CORINFO_TYPE_NATIVEINT;

        return runtime_OK;
    }
   
    if(child.IsByRef() && parent.IsReadonlyByRef())
    {
        return (DereferenceByRef(child) == DereferenceByRef(parent));
    }

    if (parent.IsUnboxedGenericTypeVar() || child.IsUnboxedGenericTypeVar())
    {
        return (FALSE);  // need to have had child == parent
    }
    else if (parent.IsType(TI_REF))
    {
        // An uninitialized objRef is not compatible to initialized.
        if (child.IsUninitialisedObjRef() && !parent.IsUninitialisedObjRef())
            return FALSE;

        if (child.IsNullObjRef())                   // NULL can be any reference type
            return TRUE;
        if (!child.IsType(TI_REF))
            return FALSE;

        return jitInfo->canCast(child.m_cls, parent.m_cls);

    }
    else if (parent.IsType(TI_METHOD))
    {
        if (!child.IsType(TI_METHOD))
            return FALSE;

        // Right now we don't bother merging method handles
        return FALSE;
    }
    return FALSE;
}


BOOL typeInfo::tiMergeCompatibleWith     (ICorJitInfo *jitInfo,
                                          const typeInfo& child,
                                          const typeInfo& parent) 
{
    if (!child.IsPermanentHomeByRef() && parent.IsPermanentHomeByRef())
    {
        return FALSE;
    }

    return typeInfo::tiCompatibleWith(jitInfo, child, parent);
}


/*****************************************************************************
 * Merge pDest and pSrc to find some commonality (e.g. a common parent).
 * Copy the result to pDest, marking it dead if no commonality can be found.
 *
 * null ^ null                  -> null
 * Object ^ null                -> Object
 * [I4 ^ null                   -> [I4
 * InputStream ^ OutputStream   -> Stream
 * InputStream ^ NULL           -> InputStream
 * [I4 ^ Object                 -> Object
 * [I4 ^ [Object                -> Array
 * [I4 ^ [R8                    -> Array
 * [Foo ^ I4                    -> DEAD
 * [Foo ^ [I1                   -> Array
 * [InputStream ^ [OutputStream -> Array
 * DEAD ^ X                     -> DEAD
 * [Intfc ^ [OutputStream       -> Array
 * Intf ^ [OutputStream         -> Object
 * [[InStream ^ [[OutStream     -> Array
 * [[InStream ^ [OutStream      -> Array
 * [[Foo ^ [Object              -> Array
 *
 * Importantly:
 * [I1 ^ [U1                    -> either [I1 or [U1
 * etc.
 *
 * Also, System/Int32 and I4 merge -> I4, etc.
 *
 * Returns FALSE if the merge was completely incompatible (i.e. the item became 
 * dead).
 *
 */

BOOL tiMergeToCommonParent (ICorJitInfo *jitInfo, 
                            vertype *pDest, 
                            const vertype *pSrc)
{
    VASSERT(pSrc->IsDead() || NormaliseForStack(*pSrc) == *pSrc);
    VASSERT(pDest->IsDead() || NormaliseForStack(*pDest) == *pDest);

    // Merge the axuillary information like This poitner tracking etc...

    // This bit is only set if both pDest and pSrc have it set
    pDest->m_flags &= (pSrc->m_flags | ~TI_FLAG_THIS_PTR);

    // This bit is set if either pDest or pSrc have it set
    pDest->m_flags |= (pSrc->m_flags & TI_FLAG_UNINIT_OBJREF);

    // This bit is set if either pDest or pSrc have it set
    pDest->m_flags |= (pSrc->m_flags & TI_FLAG_BYREF_READONLY);

    // If the byref wasn't permanent home in both sides, then merge wont have the bit set
    pDest->m_flags &= (pSrc->m_flags | ~TI_FLAG_BYREF_PERMANENT_HOME);

    // OK the main event.  Merge the main types
    if (*pDest == *pSrc)
        return(TRUE);

    if (pDest->IsUnboxedGenericTypeVar() || pSrc->IsUnboxedGenericTypeVar())
    {
        // Should have had *pDest == *pSrc
        goto FAIL;
    }

    if (pDest->IsType(TI_REF))
    {
        if (pSrc->IsType(TI_NULL))                  // NULL can be any reference type
            return TRUE;
        if (!pSrc->IsType(TI_REF))
            goto FAIL;

            // Ask the EE to find the common parent,  This always succeeds since System.Object always works
        pDest->m_cls = jitInfo->mergeClasses(pDest->GetClassHandle(), pSrc->GetClassHandle());
        return TRUE;
    }
    else if (pDest->IsType(TI_NULL))
    {
        if (pSrc->IsType(TI_REF))                   // NULL can be any reference type
        {
            *pDest = *pSrc;
            return TRUE;
        }
        goto FAIL;
    }


FAIL:
    *pDest = vertype();
    return FALSE;
}









#ifndef MAKE_WIDEPTR_FROMANSI
#define MAKE_WIDEPTR_FROMANSI(ptr, ansi) \
    WCHAR* ptr = (WCHAR*)_alloca(sizeof(WCHAR) * (strlen(ansi) + 1)); \
    {\
        int i;\
        for(i = (int)strlen(ansi); i >= 0; i--)\
            ptr[i] = (WCHAR)ansi[i];\
    }
#endif

void wszAdd(__inout __deref_out_ecount (*pnBufLen) WCHAR** ppBuf, __inout int* pnBufLen, __in __in_z WCHAR* szString)
{
    if(*pnBufLen <= 0)
        return;
    int nLen = (int)wcslen(szString);
    wcsncpy_s(*ppBuf, *pnBufLen, szString, nLen);
    (*ppBuf) += nLen;
    (*pnBufLen) -= nLen;
}

void typeInfo::ToString(__out_ecount (nBufLen) WCHAR* pBuf, int nBufLen, ICorJitInfo* pJI, CORINFO_METHOD_HANDLE ctxt) const
{
    pBuf[0] = 0;

    ti_types type = GetRawType();


    if (type == TI_METHOD)
    {
        const char* szClassName;
        const char* szMethodName = pJI->getMethodName(m_method, &szClassName);
        if(!szClassName)
            szClassName = "<Global>";
        MAKE_WIDEPTR_FROMANSI(wszMethodName, szMethodName);
        MAKE_WIDEPTR_FROMANSI(wszClassName, szClassName);
        wszAdd(&pBuf, &nBufLen, wszClassName);
        wszAdd(&pBuf, &nBufLen, L"::");
        wszAdd(&pBuf, &nBufLen, wszMethodName);
        
        return;
    }

    if (IsReadonlyByRef()) 
        wszAdd(&pBuf, &nBufLen, L"readonly ");

    if (IsByRef())
        wszAdd(&pBuf, &nBufLen, L"address of ");

    if (IsUninitialisedObjRef())
        wszAdd(&pBuf, &nBufLen, L"<uninitialized> ");

    switch (type)
    {
    default:
        wszAdd(&pBuf, &nBufLen, L"<error>");
        break;

    case TI_NULL:
        wszAdd(&pBuf, &nBufLen, L"Null");
        break;
    
    case TI_BYTE:
        wszAdd(&pBuf, &nBufLen, L"Byte");
        break;

    case TI_SHORT:
        wszAdd(&pBuf, &nBufLen, L"Short");
        break;

    case TI_INT:
        wszAdd(&pBuf, &nBufLen, L"Int32");
        break;

    case TI_I:
        wszAdd(&pBuf, &nBufLen, L"Native Int");
        break;

    case TI_LONG:
        wszAdd(&pBuf, &nBufLen, L"Long");
        break;

    case TI_FLOAT:
        wszAdd(&pBuf, &nBufLen, L"Single");
        break;

    case TI_DOUBLE:
        wszAdd(&pBuf, &nBufLen, L"Double");
        break;

    case TI_REF:
        if(!IsUnboxedGenericTypeVar())
            wszAdd(&pBuf, &nBufLen, L"ref ");
        break;

    case TI_STRUCT:
        wszAdd(&pBuf, &nBufLen, L"value ");
        wszAdd(&pBuf, &nBufLen, L"'");
        pJI->appendClassName(&pBuf, &nBufLen, m_cls, TRUE, FALSE, FALSE);
        wszAdd(&pBuf, &nBufLen, L"'");
        break;

    case TI_PTR:
        wszAdd(&pBuf, &nBufLen, L"unmanaged pointer");
        break;

    case TI_ERROR:
        wszAdd(&pBuf, &nBufLen, L"error");
        break;
    }

    //todo: more precision -- requires changes to JIT interface
    if(IsUnboxedGenericTypeVar())
    {
        wszAdd(&pBuf, &nBufLen, L"(unboxed) ");
        wszAdd(&pBuf, &nBufLen, L"'");
        pJI->appendClassName(&pBuf, &nBufLen, m_cls, TRUE, FALSE, FALSE);
        wszAdd(&pBuf, &nBufLen, L"'");
    }

    if (IsType(TI_REF))
    {
        if (IsThisPtr())
            wszAdd(&pBuf, &nBufLen, L"('this' ptr) ");

        //todo: more precision -- requires changes to JIT interface
        if (pJI->getClassAttribs(m_cls,ctxt) & CORINFO_FLG_GENERIC_TYPE_VARIABLE)
            wszAdd(&pBuf, &nBufLen, L"(boxed) "); 

        if (pJI->isSDArray(m_cls))
            wszAdd(&pBuf, &nBufLen, L"array ");

        if (pJI->getArrayRank(m_cls) > 0)
            wszAdd(&pBuf, &nBufLen, L"mdarray ");

        wszAdd(&pBuf, &nBufLen, L"'");
        pJI->appendClassName(&pBuf, &nBufLen, m_cls, TRUE, FALSE, FALSE);
        wszAdd(&pBuf, &nBufLen, L"'");
    }
    else if (IsNullObjRef())
    {
        wszAdd(&pBuf, &nBufLen, L"objref 'NullReference'");
    }
}


#define NAME_BUFFER_SIZE 512

