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
// MarshalNative.CPP
//
// ECall's for the PInvoke classlibs
//


#include "common.h"
#include "clsload.hpp"
#include "method.hpp"
#include "class.h"
#include "object.h"
#include "field.h"
#include "util.hpp"
#include "excep.h"
#include "siginfo.hpp"
#include "threads.h"
#include "stublink.h"
#include "ecall.h"
#include "dllimport.h"
#include "gcdesc.h"
#include "jitinterface.h"
#include "eeconfig.h"
#include "log.h"
#include "fieldmarshaler.h"
#include "cgensys.h"
#include "gc.h"
#include "security.h"
#include "comstringbuffer.h"
#include "dbginterface.h"
#include "objecthandle.h"
#include "marshalnative.h"
#include "fcall.h"
#include "dllimportcallback.h"
#include "ml.h"
#include "comstring.h"
#include "remoting.h"
#include "comdelegate.h"
#include "handletablepriv.h"
#include "mdaassistantsptr.h"
#include "typestring.h"
#include "appdomain.inl"


#define IDISPATCH_NUM_METHS 7
#define IUNKNOWN_NUM_METHS 3

MethodTable *pWeakReferenceMT = NULL;
const size_t pWeakRefHandleFO = offsetof(WeakReferenceObject, m_Handle);

void EEAllocateInstanceWorker(LPUNKNOWN pOuter, MethodTable* pMT, BOOL fHasLicensing, REFIID riid, BOOL fDesignTime, BSTR bstrKey, void** ppv);


FCIMPL3(VOID, MarshalNative::StructureToPtr, Object* pObjUNSAFE, LPVOID ptr, CLR_BOOL fDeleteOld)
{    
    OBJECTREF pObj = (OBJECTREF) pObjUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(pObj);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(ptr, NULL_OK));
    }
    CONTRACTL_END;

    if (ptr == NULL)
        COMPlusThrowArgumentNull(L"ptr");
    if (pObj == NULL) 
        COMPlusThrowArgumentNull(L"structure");

    // Code path will accept both regular layout objects and boxed value classes
    // with layout.

    MethodTable *pMT = pObj->GetMethodTable();

    if (pMT->HasInstantiation())
        COMPlusThrowArgumentException(L"structure", L"Argument_NeedNonGenericObject");
    
    if (pMT->IsBlittable())
    {
        memcpyNoGCRefs(ptr, pObj->GetData(), pMT->GetNativeSize());
    }
    else if (pMT->HasLayout())
    {
        if (fDeleteOld)
            LayoutDestroyNative(ptr, pMT);

        FmtClassUpdateNative( &(pObj), (LPBYTE)(ptr), NULL );
    }
    else
    {
        COMPlusThrowArgumentException(L"structure", L"Argument_MustHaveLayoutOrBeBlittable");
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL3(VOID, MarshalNative::PtrToStructureHelper, LPVOID ptr, Object* pObjIn, CLR_BOOL allowValueClasses)
{
    OBJECTREF  pObj = ObjectToOBJECTREF(pObjIn);

    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS); // only triggers on pcls->HasLayout() path.  The LayoutUpdateCLR func will handle the assertion.
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(ptr, NULL_OK));
    }
    CONTRACTL_END;

    if (ptr == NULL)
        FCThrowArgumentNullVoid(L"ptr");
    if (pObj == NULL) 
        FCThrowArgumentNullVoid(L"structure");

    // Code path will accept regular layout objects.
    MethodTable *pMT = pObj->GetMethodTable();

    // Validate that the object passed in is not a value class.
    if (!allowValueClasses && pMT->IsValueClass())
    {
        FCThrowArgumentVoid(L"structure", L"Argument_StructMustNotBeValueClass");
    }
    else if (pMT->IsBlittable())
    {
        memcpyNoGCRefs(pObj->GetData(), ptr, pMT->GetNativeSize());
    }
    else if (pMT->HasLayout())
    {
        HELPER_METHOD_FRAME_BEGIN_1(pObj);
            LayoutUpdateCLR((LPVOID*) &(pObj), Object::GetOffsetOfFirstField(), pMT, (LPBYTE)(ptr), FALSE);
        HELPER_METHOD_FRAME_END();
    }
    else
    {
        FCThrowArgumentVoid(L"structure", L"Argument_MustHaveLayoutOrBeBlittable");
    }
}
FCIMPLEND


FCIMPL2(VOID, MarshalNative::DestroyStructure, LPVOID ptr, ReflectClassBaseObject* refClassUNSAFE)
{
    REFLECTCLASSBASEREF refClass = (REFLECTCLASSBASEREF) refClassUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(refClass);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(ptr, NULL_OK));
    }
    CONTRACTL_END;

    if (ptr == NULL)
        COMPlusThrowArgumentNull(L"ptr");
    if (refClass == NULL)
        COMPlusThrowArgumentNull(L"structureType");
    if (refClass->GetMethodTable() != g_Mscorlib.FetchClass(CLASS__CLASS))
        COMPlusThrowArgumentException(L"structureType", L"Argument_MustBeRuntimeType");

    TypeHandle th = refClass->GetType();

    if (th.HasInstantiation())
        COMPlusThrowArgumentException(L"structureType", L"Argument_NeedNonGenericType");

    if (th.IsBlittable())
    {
        // ok to call with blittable structure, but no work to do in this case.
    }
    else if (th.HasLayout())
    {
        LayoutDestroyNative(ptr, th.GetMethodTable());
    }
    else
    {
        COMPlusThrowArgumentException(L"structureType", L"Argument_MustHaveLayoutOrBeBlittable");
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


/************************************************************************
 * PInvoke.SizeOf(Class)
 */
FCIMPL1(UINT32, MarshalNative::SizeOfClass, ReflectClassBaseObject* refClassUNSAFE)
{
    UINT32 rv = 0;
    REFLECTCLASSBASEREF refClass = (REFLECTCLASSBASEREF)refClassUNSAFE;
    
    HELPER_METHOD_FRAME_BEGIN_RET_1(refClass);
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (refClass == NULL)
        COMPlusThrowArgumentNull(L"t");
    if (refClass->GetMethodTable() != g_Mscorlib.FetchClass(CLASS__CLASS))
        COMPlusThrowArgumentException(L"t", L"Argument_MustBeRuntimeType");

    TypeHandle th = refClass->GetType();

    if (th.HasInstantiation())
        COMPlusThrowArgumentException(L"t", L"Argument_NeedNonGenericType");

    // Determine if the type is marshalable
    if (!th.IsUnsharedMT() || !th.AsMethodTable()->IsStructMarshalable())
    {
        if (!IsStructMarshalable(th))
        {
            // It isn't marshalable so throw an ArgumentException.
            StackSString strTypeName;
            TypeString::AppendType(strTypeName, th);                
            COMPlusThrow(kArgumentException, IDS_CANNOT_MARSHAL, strTypeName.GetUnicode(), NULL, NULL);
        }
    }

    // The type is marshalable so return it's size.
    rv = th.GetMethodTable()->GetNativeSize();
    HELPER_METHOD_FRAME_END();
    return rv;   
}
FCIMPLEND


/************************************************************************
 * PInvoke.UnsafeAddrOfPinnedArrayElement(Array arr, int index)
 */

FCIMPL2(LPVOID, MarshalNative::FCUnsafeAddrOfPinnedArrayElement, ArrayBase *arr, INT32 index) 
{   
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (!arr)
        FCThrowArgumentNull(L"arr");

    return (arr->GetDataPtr() + (index*arr->GetComponentSize())); 
}
FCIMPLEND


/************************************************************************
 * PInvoke.SizeOf(Object)
 */

FCIMPL1(UINT32, MarshalNative::FCSizeOfObject, Object* pObjUNSAFE)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS); // GC_TRIGGERS isn't supported in FCall's that lazily erect a frame.
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pObjUNSAFE, NULL_OK));
    }
    CONTRACTL_END;

    if (!pObjUNSAFE)
        FCThrowArgumentNull(L"structure");

    TypeHandle th = pObjUNSAFE->GetTypeHandle();

    // Determine if the type is marshalable.
    if (!th.IsUnsharedMT() || !th.AsMethodTable()->IsStructMarshalable())
    {
        if (!IsStructMarshalable(th))
        {
            // It isn't marshalable so throw an ArgumentException. Note that we don't need to protect 
            // pObjUNSAFE since we already extracted the information we needed from it.
            HELPER_METHOD_FRAME_BEGIN_RET_0();       
            StackSString strTypeName;
            TypeString::AppendType(strTypeName, th);                
            COMPlusThrow(kArgumentException, IDS_CANNOT_MARSHAL, strTypeName.GetUnicode(), NULL, NULL);       
            HELPER_METHOD_FRAME_END();
        }
    }
    
    // The type is marshalable so return it's size.
    return th.GetMethodTable()->GetNativeSize();
}
FCIMPLEND


/************************************************************************
 * PInvoke.OffsetOfHelper(Class, Field)
 */
FCIMPL1(UINT32, MarshalNative::OffsetOfHelper, FieldDesc *pField)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS); // GC_TRIGGERS isn't supported in FCall's that lazily erect a frame.
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pField));
    }
    CONTRACTL_END;

    TypeHandle th = TypeHandle(pField->GetApproxEnclosingMethodTable());

    // Determine if the type is marshalable.
    if (!IsStructMarshalable(th))
    {
        // It isn't marshalable so throw an ArgumentException.
        HELPER_METHOD_FRAME_BEGIN_RET_0();       
        StackSString strTypeName;
        TypeString::AppendType(strTypeName, th);                
        COMPlusThrow(kArgumentException, IDS_CANNOT_MARSHAL, strTypeName.GetUnicode(), NULL, NULL);       
        HELPER_METHOD_FRAME_END();
    }

    FieldMarshaler *pFM = th.GetMethodTable()->GetLayoutInfo()->GetFieldMarshalers();
    UINT  numReferenceFields = th.GetMethodTable()->GetLayoutInfo()->GetNumCTMFields();

    while (numReferenceFields--) 
    {
        if (pFM->GetFieldDesc() == pField) 
        {
            return pFM->GetExternalOffset();
        }
        ((BYTE*&)pFM) += MAXFIELDMARSHALERSIZE;
    }

    UNREACHABLE_MSG("We should never hit this point since we already verified that the requested field was present from managed code");   
}
FCIMPLEND

FCIMPL2(Object*, MarshalNative::GetDelegateForFunctionPointerInternal, LPVOID FPtr, ReflectClassBaseObject* refTypeUNSAFE)
{
    OBJECTREF refDelegate = NULL;
    
    REFLECTCLASSBASEREF refType = (REFLECTCLASSBASEREF) refTypeUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, refType, refDelegate);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(refType != NULL);
    }
    CONTRACTL_END;

    // Retrieve the method table from the RuntimeType. We already verified in managed
    // code that the type was a RuntimeType that represented a delegate. Because type handles
    // for delegates must have a method table, we are safe in telling prefix to assume it below.
    MethodTable* pMT = refType->GetType().GetMethodTable();   
    PREFIX_ASSUME(pMT != NULL);
    refDelegate = COMDelegate::ConvertToDelegate(FPtr, pMT);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(refDelegate);
}
FCIMPLEND

FCIMPL1(LPVOID, MarshalNative::GetFunctionPointerForDelegateInternal, Object* refDelegateUNSAFE)
{
    LPVOID pFPtr = NULL;
    
    OBJECTREF refDelegate = (OBJECTREF) refDelegateUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(refDelegate);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    pFPtr = COMDelegate::ConvertToCallback(refDelegate);
    
    HELPER_METHOD_FRAME_END();

    return pFPtr;
}
FCIMPLEND



FCIMPL0(UINT32, MarshalNative::GetSystemMaxDBCSCharSize)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    
    return GetMaxDBCSCharByteSize();
}
FCIMPLEND


/************************************************************************
 * PInvoke.PtrToStringAnsi()
 */

FCIMPL2(Object*, MarshalNative::PtrToStringAnsi, LPVOID ptr, INT32 len)
{
    STRINGREF pString = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pString);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(ptr, NULL_OK));
    }
    CONTRACTL_END;

    if (ptr == NULL)
        COMPlusThrowArgumentNull(L"ptr");
    if (len < 0)
        COMPlusThrowNonLocalized(kArgumentException, L"len");

    int nwc = 0;
    if (len != 0)
    {
        nwc = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)(ptr), len, NULL, 0);
        if (nwc == 0)
            COMPlusThrow(kArgumentException, IDS_UNI2ANSI_FAILURE);
    }
    
    GCPROTECT_BEGININTERIOR(ptr);
    pString = COMString::NewString(nwc);
    GCPROTECT_END();
    
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)(ptr), len, pString->GetBuffer(), nwc);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(pString);
}
FCIMPLEND


FCIMPL2(Object*, MarshalNative::PtrToStringUni, LPVOID ptr, INT32 len)
{
    STRINGREF pString = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pString);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(ptr, NULL_OK));
    }
    CONTRACTL_END;

    if (ptr == NULL)
        COMPlusThrowArgumentNull(L"ptr");
    if (len < 0)
        COMPlusThrowNonLocalized(kArgumentException, L"len");

    GCPROTECT_BEGININTERIOR(ptr);
    pString = COMString::NewString(len);
    GCPROTECT_END();
    
    memcpyNoGCRefs(pString->GetBuffer(), (LPVOID)(ptr), len*sizeof(WCHAR));

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(pString);
}
FCIMPLEND

/************************************************************************
 * Handles all PInvoke.Copy(array source, ....) methods.
 */
FCIMPL4(void, MarshalNative::CopyToNative, Object* psrcUNSAFE, INT32 startindex, LPVOID pdst, INT32 length)
{
    // The BCL code guarantees that Array will be passed in
    _ASSERTE(!psrcUNSAFE || psrcUNSAFE->GetMethodTable()->IsArray());

    BASEARRAYREF psrc = (BASEARRAYREF)(OBJECTREF)psrcUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(psrc);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(pdst, NULL_OK));
    }
    CONTRACTL_END;

    if (pdst == NULL)
        COMPlusThrowArgumentNull(L"destination");
    if (psrc == NULL)
        COMPlusThrowArgumentNull(L"source");

    INT32 numelem = (INT32)psrc->GetNumComponents();

    if (startindex < 0 || 
        length < 0 ||
        startindex > numelem  ||
        length > numelem      ||
        startindex > (numelem - length)) {
        COMPlusThrow(kArgumentOutOfRangeException, IDS_EE_COPY_OUTOFRANGE);
    }

    UINT32 componentsize = psrc->GetMethodTable()->GetComponentSize();

    memcpyNoGCRefs(pdst,
               componentsize*startindex + (BYTE*)(psrc->GetDataPtr()),
               componentsize*length);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL4(void, MarshalNative::CopyToManaged, LPVOID psrc, Object* pdstUNSAFE, INT32 startindex, INT32 length)
{
    // The BCL code guarantees that Array will be passed in
    _ASSERTE(!pdstUNSAFE || pdstUNSAFE->GetMethodTable()->IsArray());

    BASEARRAYREF pdst = (BASEARRAYREF)(OBJECTREF)pdstUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_1(pdst);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(psrc, NULL_OK));
    }
    CONTRACTL_END;

    if (pdst == NULL)
        COMPlusThrowArgumentNull(L"destination");
    if (psrc == NULL)
        COMPlusThrowArgumentNull(L"source");
    if (startindex < 0)
        COMPlusThrowArgumentOutOfRange(L"startIndex", L"ArgumentOutOfRange_Count");
    if (length < 0)
        COMPlusThrowArgumentOutOfRange(L"length", L"ArgumentOutOfRange_NeedNonNegNum");

    DWORD numelem = pdst->GetNumComponents();

    if (startindex < 0 ||
        length < 0 ||
        (DWORD)startindex > numelem  ||
        (DWORD)length > numelem      ||
        (DWORD)startindex > (numelem - (DWORD)length)) {
        COMPlusThrow(kArgumentOutOfRangeException, IDS_EE_COPY_OUTOFRANGE);
    }

    UINT32 componentsize = pdst->GetMethodTable()->GetComponentSize();

    _ASSERTE(CorTypeInfo::IsPrimitiveType(pdst->GetArrayElementTypeHandle().GetInternalCorElementType()));
    memcpyNoGCRefs(componentsize*startindex + (BYTE*)(pdst->GetDataPtr()),
               psrc,
               componentsize*length);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


/************************************************************************
 * PInvoke.GetLastWin32Error
 */
FCIMPL0(int, MarshalNative::GetLastWin32Error)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return (UINT32)(GetThread()->m_dwLastError);
}
FCIMPLEND


/************************************************************************
 * PInvoke.SetLastWin32Error
 */
FCIMPL1(void, MarshalNative::SetLastWin32Error, int error)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    GetThread()->m_dwLastError = (DWORD)error;
}
FCIMPLEND


/************************************************************************
 * Support for the GCHandle class.
 */

FCIMPL2(LPVOID, MarshalNative::GCHandleInternalAlloc, Object *obj, int type)
{
    OBJECTREF objRef(obj);
    OBJECTHANDLE hnd = 0;

    HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // If it is a pinned handle, check the object type.
    if (type == HNDTYPE_PINNED)
        GCHandleValidatePinnedObject(objRef);
    
    // Create the handle.
    if((hnd = GetAppDomain()->CreateTypedHandle(objRef, type)) == NULL)
        COMPlusThrowOM();

    // Fill the Weak Reference class 
    if ( (pWeakReferenceMT == NULL) &&  ((type == HNDTYPE_WEAK_LONG) || (type == HNDTYPE_WEAK_SHORT)) )
    {
        pWeakReferenceMT = g_Mscorlib.FetchClass(CLASS__WEAKREFERENCE);
        _ASSERTE(pWeakRefHandleFO == Object::GetOffsetOfFirstField() + g_Mscorlib.GetFieldOffset(FIELD__WEAKREFERENCE__HANDLE));
    }

    HELPER_METHOD_FRAME_END_POLL();
    return (LPVOID) hnd;
}
FCIMPLEND

// Free a GC handle.
FCIMPL1(VOID, MarshalNative::GCHandleInternalFree, OBJECTHANDLE handle)
{
    HELPER_METHOD_FRAME_BEGIN_0();
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;


    DestroyTypedHandle(handle);


    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// Get the object referenced by a GC handle.
FCIMPL1(LPVOID, MarshalNative::GCHandleInternalGet, OBJECTHANDLE handle)
{
    OBJECTREF objRef;

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    objRef = ObjectFromHandle(handle);

    return *((LPVOID*)&objRef);
}
FCIMPLEND

// Update the object referenced by a GC handle.
FCIMPL3(VOID, MarshalNative::GCHandleInternalSet, OBJECTHANDLE handle, Object *obj, CLR_BOOL isPinned)
{
    OBJECTREF objRef(obj);
    HELPER_METHOD_FRAME_BEGIN_1(objRef);
    
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (isPinned)
    {
        GCHandleValidatePinnedObject(objRef);
    }

    // Update the stored object reference.
    StoreObjectInHandle(handle, objRef);
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

// Update the object referenced by a GC handle.
FCIMPL4(Object*, MarshalNative::GCHandleInternalCompareExchange, OBJECTHANDLE handle, Object *obj, Object* oldObj, CLR_BOOL isPinned)
{
    OBJECTREF newObjref(obj);
    OBJECTREF oldObjref(oldObj);
    LPVOID ret = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL();

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (isPinned)
        GCHandleValidatePinnedObject(newObjref);

    // Update the stored object reference.
    ret = InterlockedCompareExchangeObjectInHandle(handle, newObjref, oldObjref);
    HELPER_METHOD_FRAME_END_POLL();
    return (Object*)ret;
}
FCIMPLEND

// Get the address of a pinned object referenced by the supplied pinned
// handle.  This routine assumes the handle is pinned and does not check.
FCIMPL1(LPVOID, MarshalNative::GCHandleInternalAddrOfPinnedObject, OBJECTHANDLE handle)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LPVOID p;
    OBJECTREF objRef = ObjectFromHandle(handle);

    if (objRef == NULL)
    {
        p = NULL;
    }
    else
    {
        // Get the interior pointer for the supported pinned types.
        if (objRef->GetMethodTable() == g_pStringClass)
            p = ((*(StringObject **)&objRef))->GetBuffer();
        else if (objRef->GetMethodTable()->IsArray())
            p = (*((ArrayBase**)&objRef))->GetDataPtr();
        else
            p = objRef->GetData();
    }

    return p;
}
FCIMPLEND

// Make sure the handle is accessible from the current domain.  (Throw if not.)
FCIMPL1(VOID, MarshalNative::GCHandleInternalCheckDomain, OBJECTHANDLE handle)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS); // will trigger on throwing.
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (handle == NULL)
        FCThrowArgumentVoid(L"handle", L"Argument_ArgumentZero");
    
    ADIndex index = HndGetHandleTableADIndex(HndGetHandleTable(handle));

    if (index.m_dwIndex != 1 && index != GetAppDomain()->GetIndex())
        FCThrowArgumentVoid(L"handle", L"Argument_HandleLeak");
}
FCIMPLEND

FCIMPL1(INT32, MarshalNative::CalculateCount, ArrayWithOffsetData* pArrayWithOffset)
{
    INT32 uRetVal = 0;
    BASEARRAYREF arrayObj = pArrayWithOffset->m_Array;
    HELPER_METHOD_FRAME_BEGIN_RET_1(arrayObj);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    INT32 cbTotalSize = 0;

    if (arrayObj != NULL)
    {
        if (!(arrayObj->GetMethodTable()->IsArray()))
            COMPlusThrow(kArgumentException, IDS_EE_NOTISOMORPHIC);
        if (arrayObj->GetMethodTable()->IsMultiDimArray())
            COMPlusThrow(kArgumentException, IDS_EE_NOTISOMORPHIC);

        GCHandleValidatePinnedObject(arrayObj);
    }

    if (arrayObj == NULL)
    {
        if (pArrayWithOffset->m_cbOffset != 0)
            COMPlusThrow(kIndexOutOfRangeException, IDS_EE_ARRAYWITHOFFSETOVERFLOW);

        goto lExit;
    }

    cbTotalSize = arrayObj->GetNumComponents() * arrayObj->GetMethodTable()->GetComponentSize();

    if (pArrayWithOffset->m_cbOffset > cbTotalSize)
        COMPlusThrow(kIndexOutOfRangeException, IDS_EE_ARRAYWITHOFFSETOVERFLOW);

    uRetVal = cbTotalSize - pArrayWithOffset->m_cbOffset;
    _ASSERTE(uRetVal > 0);

lExit: ;
    HELPER_METHOD_FRAME_END();
    return uRetVal;
}
FCIMPLEND


//====================================================================
// *** Interop Helpers ***
//====================================================================

FCIMPL2(Object *, MarshalNative::GetExceptionForHR, INT32 errorCode, LPVOID errorInfo)
{
    OBJECTREF RetExceptionObj = NULL;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, RetExceptionObj);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(FAILED(errorCode));
        PRECONDITION(CheckPointer(errorInfo, NULL_OK));
    }
    CONTRACTL_END;

    // Retrieve the IErrorInfo to use.
    IErrorInfo *pErrorInfo = (IErrorInfo*)errorInfo;
    if (pErrorInfo == (IErrorInfo*)(-1))
    {
        pErrorInfo = NULL;
    }
    else if (!pErrorInfo)
    {
        if (SafeGetErrorInfo(&pErrorInfo) != S_OK)
            pErrorInfo = NULL;
    }

    ::GetExceptionForHR(errorCode, pErrorInfo, &RetExceptionObj);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(RetExceptionObj);
}
FCIMPLEND
    
FCIMPL2(void, MarshalNative::ThrowExceptionForHR, INT32 errorCode, LPVOID errorInfo)
{
    HELPER_METHOD_FRAME_BEGIN_0();

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
        PRECONDITION(FAILED(errorCode));
        PRECONDITION(CheckPointer(errorInfo, NULL_OK));
    }
    CONTRACTL_END;

    // Retrieve the IErrorInfo to use.
    IErrorInfo *pErrorInfo = (IErrorInfo*)errorInfo;
    if (pErrorInfo == (IErrorInfo*)(-1))
    {
        pErrorInfo = NULL;
    }
    else if (!pErrorInfo)
    {
        if (SafeGetErrorInfo(&pErrorInfo) != S_OK)
            pErrorInfo = NULL;
    }

    // Throw the exception based on the HR and the IErrorInfo.
    COMPlusThrowHR(errorCode, pErrorInfo);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


FCIMPL1(int, MarshalNative::GetHRForException, Object* eUNSAFE)
{
    int retVal = 0;
    OBJECTREF e = (OBJECTREF) eUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(e);

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    retVal = SetupErrorInfo(e);

    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND



// Check that the supplied object is valid to put in a pinned handle.
// Throw an exception if not.
void GCHandleValidatePinnedObject(OBJECTREF obj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // NULL is fine.
    if (obj == NULL)
        return;

    if (obj->GetMethodTable() == g_pStringClass)
        return;

    if (obj->GetMethodTable()->IsArray())
    {
        BASEARRAYREF asArray = (BASEARRAYREF) obj;
        if (CorTypeInfo::IsPrimitiveType(asArray->GetArrayElementType())) 
            return;

        TypeHandle th = asArray->GetArrayElementTypeHandle();
        if (th.IsUnsharedMT())
        {
            MethodTable *pMT = th.AsMethodTable();
            if (pMT->IsValueClass() && pMT->IsBlittable())
                return;
        }        
    } 
    else if (obj->GetMethodTable()->IsBlittable())
    {
        return;
    }

    COMPlusThrow(kArgumentException, IDS_EE_NOTISOMORPHIC);
}

