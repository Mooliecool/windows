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
/*============================================================
**
** Header: COMObject.cpp
**
**                                             
**
** Purpose: Native methods on System.Object
**
** Date:  March 27, 1998
** 
===========================================================*/

#include "common.h"

#include <object.h>
#include "excep.h"
#include "vars.hpp"
#include "field.h"
#include "comobject.h"
#include "comsynchronizable.h"
#include "gcscan.h"
#include "remoting.h"
#include "eeconfig.h"
#include "comdatetime.h"
#include "comdecimal.h"
#include "objectclone.h"
#include "mdaassistants.h"


/********************************************************************/
/* gets an object's 'value'.  For normal classes, with reference
   based semantics, this means the object's pointer.  For boxed
   primitive types, it also means just returning the pointer (because
   they are immutable), for other value class, it means returning
   a boxed copy.  */

FCIMPL1(Object*, ObjectNative::GetObjectValue, Object* obj) 
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCallCheck calls ForbidenGC now
        INJECT_FAULT(FCThrow(kOutOfMemoryException););
        SO_TOLERANT;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(obj);

    if (obj == 0)
        return(obj);

    MethodTable* pMT = obj->GetMethodTable();
    // optimize for primitive types since GetVerifierCorElementType is slow.
    if (pMT->IsTruePrimitive() || TypeHandle(pMT).GetVerifierCorElementType() != ELEMENT_TYPE_VALUETYPE) {
        return(obj);
    }

    Object* retVal = NULL;
    OBJECTREF objRef(obj);
    HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);
    
    // 
    retVal = OBJECTREFToObject(FastAllocateObject(pMT));
    CopyValueClass(retVal->GetData(), objRef->GetData(), pMT, retVal->GetAppDomain());
    HELPER_METHOD_FRAME_END();

    return(retVal);
}
FCIMPLEND

// Note that we obtain a sync block index without actually building a sync block.
// That's because a lot of objects are hashed, without requiring support for
FCIMPL1(INT32, ObjectNative::GetHashCode, Object* obj) {
    
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);
        INJECT_FAULT(FCThrow(kOutOfMemoryException););
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(obj);
    
    DWORD idx = 0;
    
    if (obj == 0)
        return 0;
    
    OBJECTREF objRef(obj);

    HELPER_METHOD_FRAME_BEGIN_RET_1(objRef);	    // Set up a frame

        
    idx = GetHashCodeEx(OBJECTREFToObject(objRef));

    
    HELPER_METHOD_FRAME_END();

    return idx;
}
FCIMPLEND

INT32 ObjectNative::GetHashCodeEx(Object *objRef)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END

    VALIDATEOBJECTREF(objRef);
    // This loop exists because we're inspecting the header dword of the object
    // and it may change under us because of races with other threads.
    // On top of that, it may have the spin lock bit set, in which case we're
    // not supposed to change it.
    // In all of these case, we need to retry the operation.
    DWORD iter = 0;
    while (true)
    {
        DWORD bits = objRef->GetHeader()->GetBits();

        if (bits & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
        {
            if (bits & BIT_SBLK_IS_HASHCODE)
            {
                // Common case: the object already has a hash code
                return  bits & MASK_HASHCODE;
            }
            else
            {
                // We have a sync block index. This means if we already have a hash code,
                // it is in the sync block, otherwise we generate a new one and store it there
                SyncBlock *psb = objRef->GetSyncBlock();
                DWORD hashCode = psb->GetHashCode();
                if (hashCode != 0)
                    return  hashCode;

                hashCode = Object::ComputeHashCode();

                return psb->SetHashCode(hashCode);
            }
        }
        else
        {
            // If a thread is holding the thin lock or an appdomain index is set, we need a syncblock
            if ((bits & (SBLK_MASK_LOCK_THREADID | (SBLK_MASK_APPDOMAININDEX << SBLK_APPDOMAIN_SHIFT))) != 0)
            {
                objRef->GetSyncBlock();
                // No need to replicate the above code dealing with sync blocks
                // here - in the next iteration of the loop, we'll realize
                // we have a syncblock, and we'll do the right thing.
            }
            else
            {
                // We want to change the header in this case, so we have to check the BIT_SBLK_SPIN_LOCK bit first
                if (bits & BIT_SBLK_SPIN_LOCK)
                {
                    iter++;
                    if ((iter % 1024) != 0 && g_SystemInfo.dwNumberOfProcessors > 1)
                    {
                        YieldProcessor();           // indicate to the processor that we are spining
                    }
                    else
                    {
                        __SwitchToThread(0);
                    }
                    continue;
                }

                DWORD hashCode = Object::ComputeHashCode();

                DWORD newBits = bits | BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX | BIT_SBLK_IS_HASHCODE | hashCode;

                if (objRef->GetHeader()->SetBits(newBits, bits) == bits)
                    return hashCode;
                // Header changed under us - let's restart this whole thing.
            }
        }
    }
}

//
// Compare by ref for normal classes, by value for value types.
//  
//

FCIMPL2(FC_BOOL_RET, ObjectNative::Equals, Object *pThisRef, Object *pCompareRef)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);
        INJECT_FAULT(FCThrow(kOutOfMemoryException););
        MODE_COOPERATIVE;
        SO_TOLERANT;          
    }
    CONTRACTL_END;
    
    if (pThisRef == pCompareRef)    
        FC_RETURN_BOOL(TRUE);

    // Since we are in FCALL, we must handle NULL specially.
    if (pThisRef == NULL || pCompareRef == NULL)
        FC_RETURN_BOOL(FALSE);

    MethodTable *pThisMT = pThisRef->GetMethodTable();

    // If it's not a value class, don't compare by value
    if (!pThisMT->IsValueClass())
        FC_RETURN_BOOL(FALSE);

    // Make sure they are the same type.
    if (pThisMT != pCompareRef->GetMethodTable())
        FC_RETURN_BOOL(FALSE);

    // Compare the contents (size - vtable - sink block index).
    BOOL ret = memcmp(
        (void *) (pThisRef+1), 
        (void *) (pCompareRef+1), 
        pThisRef->GetMethodTable()->GetBaseSize() - sizeof(Object) - sizeof(int)) == 0;

    FC_GC_POLL_RET();

    FC_RETURN_BOOL(ret);
}
FCIMPLEND

FCIMPL1(Object*, ObjectNative::GetClass, Object* pThis)
{
    CONTRACTL
    {
        THROWS;
        SO_TOLERANT;
        DISABLED(GC_TRIGGERS); // FCallCheck calls ForbidenGC now
        INJECT_FAULT(FCThrow(kOutOfMemoryException););
        SO_TOLERANT;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;
    
    OBJECTREF   objRef   = ObjectToOBJECTREF(pThis);
    OBJECTREF   refType  = NULL;
    TypeHandle  typeHandle = TypeHandle();
    
    if (objRef == NULL) 
        FCThrow(kNullReferenceException);

    typeHandle = objRef->GetTypeHandle();
    if (typeHandle.IsUnsharedMT())
        refType = typeHandle.AsMethodTable()->GetManagedClassObjectIfExists();
    else
        refType = typeHandle.GetManagedClassObjectIfExists();
    
    if (refType != NULL)
        return OBJECTREFToObject(refType);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, objRef, refType);
    
    if (!objRef->IsThunking())
        refType = typeHandle.GetManagedClassObject();
    else
        refType = CRemotingServices::GetClass(objRef);
    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refType);
}
FCIMPLEND

FCIMPL1(Object*, ObjectNative::Clone, Object* pThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF refClone = NULL;
    OBJECTREF refThis  = ObjectToOBJECTREF(pThisUNSAFE);

    if (refThis == NULL)
        FCThrow(kNullReferenceException);

    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, refClone, refThis);

    // ObjectNative::Clone() ensures that the source and destination are always in
    // the same context.

    MethodTable* pMT;
    DWORD cb;

    pMT = refThis->GetMethodTable();

    // assert that String has overloaded the Clone() method
    _ASSERTE(pMT != g_pStringClass);

    cb = pMT->GetBaseSize() - sizeof(ObjHeader);


    if (pMT->IsArray()) {

        BASEARRAYREF base = (BASEARRAYREF)refThis;
        cb += base->GetNumComponents() * pMT->GetComponentSize();

        refClone = DupArrayForCloning(base);
    } else {
        // We don't need to call the <cinit> because we know
        //  that it has been called....(It was called before this was created)
        refClone = AllocateObject(pMT);
    }

    // copy contents of "this" to the clone
    memcpyGCRefs(OBJECTREFToObject(refClone), OBJECTREFToObject(refThis), cb);

    HELPER_METHOD_FRAME_END();
        
    return OBJECTREFToObject(refClone);
}
FCIMPLEND

FCIMPL3(FC_BOOL_RET, ObjectNative::WaitTimeout, CLR_BOOL exitContext, INT32 Timeout, Object* pThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    BOOL retVal = FALSE;
    OBJECTREF pThis = (OBJECTREF) pThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(pThis);
    //-[autocvtpro]-------------------------------------------------------

    if (pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    if ((Timeout < 0) && (Timeout != INFINITE_TIMEOUT))
        COMPlusThrowArgumentOutOfRange(L"millisecondsTimeout", L"ArgumentOutOfRange_NeedNonNegNum");

    retVal = pThis->Wait(Timeout, exitContext);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

FCIMPL1(void, ObjectNative::Pulse, Object* pThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        SO_TOLERANT;
        THROWS;
    }
    CONTRACTL_END;

    OBJECTREF pThis = (OBJECTREF) pThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(pThis);
    //-[autocvtpro]-------------------------------------------------------

    if (pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    pThis->Pulse();

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(void, ObjectNative::PulseAll, Object* pThisUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF pThis = (OBJECTREF) pThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(pThis);
    //-[autocvtpro]-------------------------------------------------------

    if (pThis == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    pThis->PulseAll();

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL2(void, JIT_MonReliableEnter, Object* pThisUNSAFE, CLR_BOOL *tookLock)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF obj = (OBJECTREF) pThisUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(obj);
    //-[autocvtpro]-------------------------------------------------------

    if (obj == NULL)
        COMPlusThrow(kNullReferenceException, L"NullReference_This");

    GCPROTECT_BEGININTERIOR(tookLock);

    class AwareLock *awareLock = NULL;
    SyncBlock* syncBlock = NULL;
    ObjHeader* objHeader = NULL;
    int spincount = 50;
    const int MaxSpinCount = 20000 * g_SystemInfo.dwNumberOfProcessors;
    LONG oldvalue, state;
    DWORD tid;

    Thread *pThread = GetThread();

    tid = pThread->GetThreadId();

    if (tid > SBLK_MASK_LOCK_THREADID)
    {
        goto FramedLockHelper;
    }

    objHeader = obj->GetHeader();

    while (true)
    {
        oldvalue = objHeader->m_SyncBlockValue;

        if ((oldvalue & (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + 
                        BIT_SBLK_SPIN_LOCK + 
                        SBLK_MASK_LOCK_THREADID + 
                        SBLK_MASK_LOCK_RECLEVEL)) == 0)
        {       

            LONG newvalue = oldvalue | tid;
            if (FastInterlockCompareExchangeAcquire((LONG*)&(objHeader->m_SyncBlockValue), newvalue, oldvalue) == oldvalue)
            {
                pThread->IncLockCount();
                goto UpdateLockState;
            }
            continue;
        }

        if (oldvalue & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
        {
            goto HaveHashOrSyncBlockIndex;
        }

        if (oldvalue & BIT_SBLK_SPIN_LOCK)
        {
            if (1 == g_SystemInfo.dwNumberOfProcessors)
            {
                goto FramedLockHelper;
            }
        }
        else if (tid == (DWORD) (oldvalue & SBLK_MASK_LOCK_THREADID))
        {
            LONG newvalue = oldvalue + SBLK_LOCK_RECLEVEL_INC;

            if ((newvalue & SBLK_MASK_LOCK_RECLEVEL) == 0)
            {
                goto FramedLockHelper;
            }

            if (FastInterlockCompareExchangeAcquire((LONG*)&(objHeader->m_SyncBlockValue), newvalue, oldvalue) == oldvalue)
            {
                goto UpdateLockState;
            }
        }

        // exponential backoff
        for (int i = 0; i < spincount; i++)
        {
            YieldProcessor();
        }
        if (spincount > MaxSpinCount)
        {
            goto FramedLockHelper;
        }
        spincount *= 3;
    } /* while(true) */

HaveHashOrSyncBlockIndex:
    if (oldvalue & BIT_SBLK_IS_HASHCODE)
    {
        goto FramedLockHelper;;
    }

    syncBlock = obj->PassiveGetSyncBlock();
    if (NULL == syncBlock)
    {
        goto FramedLockHelper;;
    }

    awareLock = syncBlock->QuickGetMonitor();
    state = awareLock->m_MonitorHeld;
    if (state == 0)
    {
        if (FastInterlockCompareExchangeAcquire((LONG*)&(awareLock->m_MonitorHeld), 1, 0) == 0)
        {
            syncBlock->SetAwareLock(pThread,1);
            pThread->IncLockCount();
            goto UpdateLockState;
        }
        else
        {
            goto FramedLockHelper;;
        }
    }
    else if (awareLock->GetOwningThread() == pThread) /* monitor is held, but it could be a recursive case */
    {
        awareLock->m_Recursion++;
        goto UpdateLockState;
    }
FramedLockHelper:
    obj->EnterObjMonitor();

UpdateLockState:
    tookLock != NULL ? *tookLock = true : false;

    GCPROTECT_END();
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND
