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
// File: ARRAY.CPP
//
// File which contains a bunch of of array related things.
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
#include "array.h"
#include "ecall.h"
#include "typestring.h"

#define MAX_SIZE_FOR_VALUECLASS_IN_ARRAY 0xffff
#define MAX_PTRS_FOR_VALUECLASSS_IN_ARRAY 0xffff


/*****************************************************************************************/
LPCUTF8 ArrayMethodDesc::GetMethodName()
{
    LEAF_CONTRACT;

    switch (GetArrayFuncIndex())
    {
    case ARRAY_FUNC_GET:
        return "Get";
    case ARRAY_FUNC_SET:
        return "Set";
    case ARRAY_FUNC_ADDRESS:
        return "Address";
    default:
        return COR_CTOR_METHOD_NAME;    // ".ctor"
    }
}

/*****************************************************************************************/
DWORD ArrayMethodDesc::GetAttrs()
{
    LEAF_CONTRACT;
    return (GetArrayFuncIndex() >= ARRAY_FUNC_CTOR) ? (mdPublic | mdRTSpecialName) : mdPublic;
}

/*****************************************************************************************/
CorInfoIntrinsics ArrayMethodDesc::GetIntrinsicID()
{
    LEAF_CONTRACT;

    switch (GetArrayFuncIndex())
    {
    case ARRAY_FUNC_GET:
        return CORINFO_INTRINSIC_Array_Get;
    case ARRAY_FUNC_SET:
        return CORINFO_INTRINSIC_Array_Set;
    case ARRAY_FUNC_ADDRESS:
        return CORINFO_INTRINSIC_Array_Address;
    default:
        return CORINFO_INTRINSIC_Illegal;
    }
}

#ifndef DACCESS_COMPILE
////////////////////////////////////////////////////////////////////////////
//
// System/Array class methods
//
////////////////////////////////////////////////////////////////////////////
FCIMPL1(INT32, Array_Rank, ArrayBase* array)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(array);

    if (array == NULL)
        FCThrow(kNullReferenceException);

    return array->GetRank();
}
FCIMPLEND

FCIMPL1(INT32, Array_GetDataPtrOffsetInternal, ArrayBase* array)
{
    VALIDATEOBJECTREF(array);

    if (array == NULL)
        FCThrow(kNullReferenceException);

    return ArrayBase::GetDataPtrOffset(array->GetMethodTable());
}
FCIMPLEND


// array is GC protected by caller
void ArrayInitializeWorker(ARRAYBASEREF * arrayRef,
                           MethodTable* pArrayMT,
                           MethodDesc* ctorMD)
{
    WRAPPER_CONTRACT;
    //can not use contract here because of SEH
    _ASSERTE(IsProtectedByGCFrame (arrayRef));
    
    unsigned offset = ArrayBase::GetDataPtrOffset(pArrayMT);
    unsigned size = pArrayMT->GetComponentSize();
    unsigned cElements = (*arrayRef)->GetNumComponents();

    //
    // This is quite a bit slower, but it is portable.
    //

    MethodDescCallSite ctorMethod(ctorMD);

    for (unsigned i =0; i < cElements; i++)
    {
        ARG_SLOT args = PtrToArgSlot((BYTE*)OBJECTREFToObject (*arrayRef) + offset);
        ctorMethod.Call(&args);
        offset += size;
    }
}


FCIMPL1(void, Array_Initialize, ArrayBase* array)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (array == NULL)
    {
        FCThrowVoid(kNullReferenceException);
    }


    MethodTable* pArrayMT = array->GetMethodTable();
    ArrayClass* pArrayCls = (ArrayClass*) pArrayMT->GetClass();
    _ASSERTE(pArrayCls->IsArrayClass());

    // value class array, check to see if it has a constructor
    MethodDesc* ctorMD = pArrayCls->GetArrayElementCtor();
    if (ctorMD == 0)
        return;     // Nothing to do
        
    ARRAYBASEREF arrayRef (array);
    HELPER_METHOD_FRAME_BEGIN_1(arrayRef);

    ArrayInitializeWorker(&arrayRef, pArrayMT, ctorMD);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND



// Get inclusive upper bound
FCIMPL2(INT32, Array_UpperBound, ArrayBase* array, unsigned int dimension)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(array);

    if (array == NULL)
        FCThrow(kNullReferenceException);

    // What is this an array of?
    MethodTable *pArrayMT = array->GetMethodTable();
    DWORD Rank = pArrayMT->GetRank();

    if (dimension >= Rank)
        FCThrowRes(kIndexOutOfRangeException, L"IndexOutOfRange_ArrayRankIndex");

    return array->GetBoundsPtr()[dimension] + array->GetLowerBoundsPtr()[dimension] - 1;
}
FCIMPLEND


FCIMPL2(INT32, Array_LowerBound, ArrayBase* array, unsigned int dimension)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(array);

    if (array == NULL)
        FCThrow(kNullReferenceException);

    // What is this an array of?
    MethodTable *pArrayMT = array->GetMethodTable();
    DWORD Rank = pArrayMT->GetRank();

    if (dimension >= Rank)
        FCThrowRes(kIndexOutOfRangeException, L"IndexOutOfRange_ArrayRankIndex");

    return array->GetLowerBoundsPtr()[dimension];
}
FCIMPLEND


FCIMPL2(INT32, Array_GetLength, ArrayBase* array, unsigned int dimension)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(array);

    if (array==NULL)
        FCThrow(kNullReferenceException);
    unsigned int rank = array->GetRank();
    if (dimension >= rank)
        FCThrow(kIndexOutOfRangeException);
    return array->GetBoundsPtr()[dimension];
}
FCIMPLEND


FCIMPL1(INT32, Array_GetLengthNoRank, ArrayBase* array)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    VALIDATEOBJECTREF(array);

    if (array==NULL)
        FCThrow(kNullReferenceException);
    return array->GetNumComponents();
}
FCIMPLEND


/*****************************************************************************************/

//
// Generate a short sig (descr) for an array accessors
//

VOID Module::GenerateArrayAccessorCallSig(
    DWORD   dwRank,
    DWORD   dwFuncType,    // Load, store, or <init>
    PCCOR_SIGNATURE *ppSig,// Generated signature
    DWORD * pcSig,         // Generated signature size
    AllocMemTracker *pamTracker
)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(dwRank >= 1 && dwRank < 0x3ffff);
    } CONTRACTL_END;

    PCOR_SIGNATURE pSig;
    PCOR_SIGNATURE pSigMemory;
    DWORD   dwCallSigSize = dwRank;
    DWORD   dwArgCount = (dwFuncType == ArrayMethodDesc::ARRAY_FUNC_SET) ? dwRank+1 : dwRank;
    DWORD   i;

    switch (dwFuncType)
    {
        // <callconv> <argcount> VAR 0 I4 , ... , I4
        case ArrayMethodDesc::ARRAY_FUNC_GET:
            dwCallSigSize += 4;
            break;

        // <callconv> <argcount> VOID I4 , ... , I4
        case ArrayMethodDesc::ARRAY_FUNC_CTOR:
            dwCallSigSize += 3;
            break;

        // <callconv> <argcount> VOID I4 , ... , I4 VAR 0
        case ArrayMethodDesc::ARRAY_FUNC_SET:
            dwCallSigSize += 5;
            break;

        // <callconv> <argcount> BYREF VAR 0 I4 , ... , I4
        case ArrayMethodDesc::ARRAY_FUNC_ADDRESS:
            dwCallSigSize += 5;
            break;
    }

    // If the argument count is larger than 127 then it will require 2 bytes for the encoding
    if (dwArgCount > 0x7f)

        dwCallSigSize++;

    pSigMemory = (PCOR_SIGNATURE)pamTracker->Track(GetDomain()->GetHighFrequencyHeap()->AllocMem(dwCallSigSize));

#ifdef _DEBUG 
    m_dwDebugArrayClassSize += dwCallSigSize;
#endif

    pSig = pSigMemory;
    BYTE callConv = IMAGE_CEE_CS_CALLCONV_DEFAULT + IMAGE_CEE_CS_CALLCONV_HASTHIS;

    if (dwFuncType == ArrayMethodDesc::ARRAY_FUNC_ADDRESS)
    {
        callConv |= CORINFO_CALLCONV_PARAMTYPE;     // Address routine needs special hidden arg
    }

    *pSig++ = callConv;
    pSig += CorSigCompressData(dwArgCount, pSig);   // Argument count
    switch (dwFuncType)
    {
        case ArrayMethodDesc::ARRAY_FUNC_GET:
            *pSig++ = ELEMENT_TYPE_VAR;
            *pSig++ = 0;        // variable 0
            break;
        case ArrayMethodDesc::ARRAY_FUNC_CTOR:
            *pSig++ = (BYTE) ELEMENT_TYPE_VOID;             // Return type
            break;
        case ArrayMethodDesc::ARRAY_FUNC_SET:
            *pSig++ = (BYTE) ELEMENT_TYPE_VOID;             // Return type
            break;
        case ArrayMethodDesc::ARRAY_FUNC_ADDRESS:
            *pSig++ = (BYTE) ELEMENT_TYPE_BYREF;            // Return type
            *pSig++ = ELEMENT_TYPE_VAR;
            *pSig++ = 0;        // variable 0
            break;
    }
    for (i = 0; i < dwRank; i++)
        *pSig++ = ELEMENT_TYPE_I4;

    if (dwFuncType == ArrayMethodDesc::ARRAY_FUNC_SET)
    {
        *pSig++ = ELEMENT_TYPE_VAR;
        *pSig++ = 0;        // variable 0
    }

    // Make sure the sig came out exactly as large as we expected
    _ASSERTE(pSig == pSigMemory + dwCallSigSize);

    *ppSig = pSigMemory;
    *pcSig = (DWORD)(pSig-pSigMemory);
}

//
// Allocate a new MethodDesc for a fake array method.
//
// Based on code in class.cpp.
//
void ArrayClass::InitArrayMethodDesc(
    ArrayMethodDesc *pNewMD,
    PCCOR_SIGNATURE pShortSig,
    DWORD   cShortSig,
    DWORD   dwVtableSlot,
    BaseDomain *pDomain,
    AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;    // On IA64
    } CONTRACTL_END;

    // Note: The method desc memory is zero initialized

    pNewMD->SetMemberDef(0);

    pNewMD->SetSlot((WORD) dwVtableSlot);
    pNewMD->SetStoredMethodSig(pShortSig, cShortSig);

    _ASSERTE(!pNewMD->ComputeMayHaveNativeCode());
    pNewMD->SetTemporaryEntryPoint(pDomain, pamTracker);

#ifdef _DEBUG 
    _ASSERTE(pNewMD->GetMethodName() && GetDebugClassName());
    pNewMD->m_pszDebugMethodName = pNewMD->GetMethodName();
    pNewMD->m_pszDebugClassName  = GetDebugClassName();
    pNewMD->m_pDebugMethodTable  = GetMethodTable();

    pNewMD->InitPrestubCallChecking();
#endif // _DEBUG
}

/*****************************************************************************************/
MethodTable* Module::CreateArrayMethodTable(TypeHandle elemTypeHnd, CorElementType arrayKind, unsigned Rank, AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(Rank > 0);
    } CONTRACTL_END;

    // Arrays of reference types all share the same MethodTable.
    //
    // We can't share nested SZARRAYs because they have different
    // numbers of constructors.
    CorElementType elemType = elemTypeHnd.GetSignatureCorElementType();

    // This set of checks matches precisely those in ParamTypeDesc::Fixup
    if (CorTypeInfo::IsObjRef(elemType) && elemType != ELEMENT_TYPE_SZARRAY &&
        elemTypeHnd.GetMethodTable() != g_pObjectClass) {
        return(ClassLoader::LoadArrayTypeThrowing(TypeHandle(g_pObjectClass), arrayKind, Rank).GetMethodTable());
    }

    // Strictly speaking no method table should be needed for
    // arrays of the faked up TypeDescs for variable types that are
    // used when verfifying generic code.
    // However verification is tied in with some codegen in the JITs, so give these
    // the shared MT just in case.

    if (CorTypeInfo::IsGenericVariable(elemType)) {
        return(ClassLoader::LoadArrayTypeThrowing(TypeHandle(g_pObjectClass), arrayKind, Rank).GetMethodTable());
    }


    BOOL            containsPointers = CorTypeInfo::IsObjRef(elemType);
    if (elemType == ELEMENT_TYPE_VALUETYPE && elemTypeHnd.AsMethodTable()->ContainsPointers())
        containsPointers = TRUE;

    // this is the base for every array type
    _ASSERTE(g_pArrayClass);        // Must have already loaded the System.Array class
    MethodTable *pParentClass = g_pArrayClass;


    pParentClass->CheckRestore();

    DWORD numCtors = 2;         // ELEMENT_TYPE_ARRAY has two ctor functions, one with and one without lower bounds
    if (arrayKind == ELEMENT_TYPE_SZARRAY)
    {
        numCtors = 1;
        TypeHandle ptr = elemTypeHnd;
        while (ptr.IsTypeDesc() && ptr.AsTypeDesc()->GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY) {
            numCtors++;
            ptr = ptr.AsTypeDesc()->GetTypeParam();
        }
    }

    /****************************************************************************************/

    // Parent class is the top level array
    // The vtable will have all of top level class's methods, plus any methods we have for array classes
    WORD wTotalSlots = (WORD) (pParentClass->GetNumVirtuals() + numCtors +
                                   3    // 3 for the proper rank Get, Set, Address
                                  );

    size_t cbMT = sizeof(MethodTable);
    cbMT += (wTotalSlots * sizeof(SLOT));

    // GC info
    size_t cbCGCDescData = 0;
    if (containsPointers)
    {
        cbCGCDescData += CGCDesc::ComputeSize(1);
        if (elemType == ELEMENT_TYPE_VALUETYPE)
        {
            size_t nSeries = CGCDesc::GetCGCDescFromMT(elemTypeHnd.AsMethodTable())->GetNumSeries();
            cbCGCDescData += (nSeries - 1)*sizeof (val_serie_item);
            _ASSERTE(cbCGCDescData == CGCDesc::ComputeSizeRepeating(nSeries));
        }
    }

    // Allocate space for optional members
    cbMT += pParentClass->GetNumInterfaces() ? sizeof(UINT_PTR) : 0; // no per inst info


    // This is the offset of the beginning of the interface map
    size_t imapOffset = cbMT;

    // This is added after we determine the offset of the interface maps
    // because the memory appears before the pointer to the method table
    cbMT += cbCGCDescData;

    // Inherit top level class's interface map
    cbMT += pParentClass->GetNumInterfaces() * sizeof(InterfaceInfo_t);

    // Allocate ArrayClass, MethodTable, and class name in one alloc

    // ArrayClass already includes one void*
    BYTE* pMemory = (BYTE *)pamTracker->Track(GetDomain()->GetHighFrequencyHeap()->AllocMem(sizeof(ArrayClass) + cbMT));

    // Zero the ArrayClass and the MethodTable
    memset(pMemory, 0, sizeof(ArrayClass) + cbMT);

    ArrayClass* pClass =::new (pMemory) ArrayClass(elemTypeHnd.GetModule());

    // Head of MethodTable memory (starts after ArrayClass), this points at the GCDesc stuff in front
    // of a method table (if needed)
    BYTE* pMTHead = pMemory + sizeof(ArrayClass) + cbCGCDescData;

    MethodTable* pMT = (MethodTable *) pMTHead;

    // Allocate the private data block ("private" during runtime in the ngen'ed case).
    BYTE* pMTWriteableData = (BYTE *) pamTracker->Track(GetDomain()->GetHighFrequencyHeap()->AllocMem(sizeof(MethodTableWriteableData)));
    pMT->SetWriteableData((PTR_MethodTableWriteableData)pMTWriteableData);

    if (elemTypeHnd.ContainsGenericVariables())
        pMT->SetContainsGenericVariables();

    // Fill in pClass
    pClass->SetAttrClass (tdPublic | tdSerializable | tdSealed);  // This class is public, serializable, sealed
    pClass->SetRank (Rank);
    pClass->SetArrayElementTypeHandle (elemTypeHnd);
    pClass->SetArrayElementType (elemType);
    pClass->Setcl (mdTypeDefNil);
    DWORD flags = 0;
    if (elemTypeHnd.GetClass()->ContainsStackPtr())
        pClass->SetContainsStackPtr();
    pClass->SetVMFlags (flags);
    pClass->SetMethodTable (pMT);
    pClass->SetClassConstructorSlot(MethodTable::NO_SLOT);
#ifdef _DEBUG 
    pClass->SetAuxFlags(EEClass::AUXFLAG_ARRAY_CLASS);
#endif

#if CHECK_APP_DOMAIN_LEAKS 
    // Non-covariant arrays of agile types are agile
    if (elemType != ELEMENT_TYPE_CLASS && elemTypeHnd.IsAppDomainAgile())
        pClass->SetAppDomainAgile();
    pClass->SetAppDomainAgilityDone();
#endif // CHECK_APP_DOMAIN_LEAKS

#ifdef _DEBUG 
    m_dwDebugArrayClassSize += (DWORD) (sizeof(ArrayClass) + cbMT);
#endif


    // Fill In the method table
    pMT->SetNumVirtuals(g_pArrayClass->GetNumVirtuals());
    pMT->SetNumMethods(wTotalSlots);
    pMT->SetParentMethodTable(g_pArrayClass);

    DWORD dwComponentSize = elemTypeHnd.GetSize();

    pMT->InitializeFlags(MethodTable::enum_flag_Array);
    pMT->SetComponentSize(dwComponentSize);
    pMT->SetClass(pClass);
    pClass->SetDefaultConstructorSlot(MethodTable::NO_SLOT);
    pMT->SetInternalCorElementType(arrayKind);
    pMT->SetClassPreInited();
    pMT->SetLoaderModule(this);

    if (CorTypeInfo::IsObjRef(elemType))
        pMT->SetSharedByReferenceArrayTypes();

    // Set BaseSize to be size of non-data portion of the array
    DWORD baseSize = ObjSizeOf(ArrayBase);
    if (pMT->IsSharedByReferenceArrayTypes())
        baseSize += sizeof(TypeHandle);  // Add in the type handle that is also stored in this case
    if (arrayKind == ELEMENT_TYPE_ARRAY)
        baseSize += Rank*sizeof(DWORD)*2;

#if !defined(_WIN64) && (DATA_ALIGNMENT > 4) 
    if (dwComponentSize >= DATA_ALIGNMENT)
        baseSize = (DWORD)ALIGN_UP(baseSize, DATA_ALIGNMENT);
#endif // !defined(_WIN64) && (DATA_ALIGNMENT > 4)
    pMT->SetBaseSize(baseSize);
    // Interface map can be the same as my parent
    // Because of array method table persisting, we need to copy the map
    memcpy(pMTHead + imapOffset, pParentClass->GetInterfaceMap(),
           pParentClass->GetNumInterfaces() * sizeof(InterfaceInfo_t));
    pMT->SetInterfaceMap(pParentClass->GetNumInterfaces(), (InterfaceInfo_t *)(pMTHead + imapOffset));

    if (pParentClass->HasCriticalFinalizer())
        pMT->SetHasCriticalFinalizer();

    // Copy top level class's vtable - note, vtable is contained within the MethodTable
    for (UINT32 i = 0; i < pParentClass->GetNumVirtuals(); i++)
        pMT->SetSlot(i, pParentClass->GetSlot(i));

#ifdef _DEBUG 
    StackSString debugName;
    TypeString::AppendType(debugName, TypeHandle(pMT));
    StackScratchBuffer buff;
    const char* pDebugNameUTF8 = debugName.GetUTF8(buff);
    size_t len = strlen(pDebugNameUTF8)+1;
    char * name = (char*) pamTracker->Track(GetDomain()->
                                GetHighFrequencyHeap()->
                                AllocMem(len));
    strcpy_s(name, len, pDebugNameUTF8);

    pClass->SetDebugClassName(name);
    pMT->SetDebugClassName(name);
#endif // _DEBUG

    // Count the number of method descs we need so we can allocate chunks.
    DWORD dwMethodDescs = numCtors
                        + 3;        // for rank specific Get, Set, Address

    // allocate as many chunks as needed to hold the methods
    DWORD cChunks = MethodDescChunk::GetChunkCount(dwMethodDescs, mcArray);
    DWORD AllocSize;
    if (!ClrSafeInt<DWORD>::multiply(cChunks, sizeof(MethodDescChunk*), AllocSize))
        COMPlusThrowOM();
    MethodDescChunk **pChunks = (MethodDescChunk**)_alloca(AllocSize);
    DWORD cMethodsLeft = dwMethodDescs;
    for (DWORD i = 0; i < cChunks; i++) {
        DWORD cMethods = min(cMethodsLeft, MethodDescChunk::GetMaxMethodDescs(mcArray));
        pChunks[i] = MethodDescChunk::CreateChunk(GetDomain()->GetHighFrequencyHeap(),
                            cMethods, mcArray, 0, NULL, pamTracker);
        _ASSERTE(pChunks[i] != NULL); // throws on oom
        cMethodsLeft -= cMethods;
        pChunks[i]->SetMethodTable(pMT);
#ifdef _DEBUG 
        m_dwDebugArrayClassSize += pChunks[i]->Sizeof();
#endif
    }
    _ASSERTE(cMethodsLeft == 0);


    DWORD dwMethodDescIndex = 0;
    DWORD dwCurrentChunk = 0;

    for (DWORD dwMethodIndex = 0; dwMethodIndex < dwMethodDescs; dwMethodIndex++)
    {
        DWORD dwFuncRank;
        DWORD dwFuncType;

        if (dwMethodIndex < ArrayMethodDesc::ARRAY_FUNC_CTOR)
        {
            // Generate a new stand-alone, Rank Specific Get, Set and Address method.
            dwFuncRank = Rank;
            dwFuncType = dwMethodIndex;
        }
        else
        {
            if (arrayKind == ELEMENT_TYPE_SZARRAY)
            {
                // For SZARRAY arrays, set up multiple constructors.
                dwFuncRank = 1 + (dwMethodIndex - ArrayMethodDesc::ARRAY_FUNC_CTOR);
            }
            else
            {
                // ELEMENT_TYPE_ARRAY has two constructors, one without lower bounds and one with lower bounds
                _ASSERTE((dwMethodIndex == ArrayMethodDesc::ARRAY_FUNC_CTOR) || (dwMethodIndex == ArrayMethodDesc::ARRAY_FUNC_CTOR+1));
                dwFuncRank = (dwMethodIndex == ArrayMethodDesc::ARRAY_FUNC_CTOR) ? Rank : 2 * Rank;
            }
            dwFuncType = ArrayMethodDesc::ARRAY_FUNC_CTOR;
        }

        PCCOR_SIGNATURE pSig;
        DWORD           cSig;

        GenerateArrayAccessorCallSig(dwFuncRank, dwFuncType, &pSig, &cSig, pamTracker);

        ArrayMethodDesc* pNewMD = (ArrayMethodDesc *) pChunks[dwCurrentChunk]->GetMethodDescAt(dwMethodDescIndex);
        _ASSERTE(pNewMD->GetClassification() == mcArray);

        pClass->InitArrayMethodDesc(pNewMD, pSig, cSig, pParentClass->GetNumVirtuals() + dwMethodIndex, GetDomain(), pamTracker);

        dwMethodDescIndex++;
        if (dwMethodDescIndex == MethodDescChunk::GetMaxMethodDescs(mcArray)) {
            dwMethodDescIndex = 0;
            dwCurrentChunk++;
        }
    }       

#if defined(_DEBUG) && !defined(STUB_DISPATCH_ALL) 
    MethodTableBuilder::MarkInheritedVirtualMethods(pMT, pParentClass);
#endif

    // Set up GC information
    if (elemType == ELEMENT_TYPE_VALUETYPE || elemType == ELEMENT_TYPE_VOID)
    {
        // The only way for dwComponentSize to be large is to be part of a value class. If this changes
        // then the check will need to be moved outside valueclass check.
        if(dwComponentSize > MAX_SIZE_FOR_VALUECLASS_IN_ARRAY) {
            StackSString ssElemName;
            elemTypeHnd.GetName(ssElemName);

            StackScratchBuffer scratch;
            elemTypeHnd.GetAssembly()->ThrowTypeLoadException(ssElemName.GetUTF8(scratch), IDS_CLASSLOAD_VALUECLASSTOOLARGE);
        }

        // If it's an array of value classes, there is a different format for the GCDesc if it contains pointers
        if (elemTypeHnd.AsMethodTable()->ContainsPointers())
        {
            CGCDescSeries  *pSeries;

            // There must be only one series for value classes
            CGCDescSeries  *pByValueSeries = CGCDesc::GetCGCDescFromMT(elemTypeHnd.AsMethodTable())->GetHighestSeries();

            pMT->SetContainsPointers();

            // negative series has a special meaning, indicating a different form of GCDesc
            SSIZE_T nSeries = (SSIZE_T) CGCDesc::GetCGCDescFromMT(elemTypeHnd.AsMethodTable())->GetNumSeries();
            CGCDesc::GetCGCDescFromMT(pMT)->InitValueClassSeries(pMT, nSeries);

            pSeries = CGCDesc::GetCGCDescFromMT(pMT)->GetHighestSeries();

            // sort by offset
            SSIZE_T AllocSizeSeries;
            if (!ClrSafeInt<SSIZE_T>::multiply(sizeof(CGCDescSeries*), nSeries, AllocSizeSeries))
                COMPlusThrowOM();
            CGCDescSeries** sortedSeries = (CGCDescSeries**) _alloca(AllocSizeSeries);
            int index;
            for (index = 0; index < nSeries; index++)
                sortedSeries[index] = &pByValueSeries[-index];

            // section sort
            for (int i = 0; i < nSeries; i++) {
                for (int j = i+1; j < nSeries; j++)
                    if (sortedSeries[j]->GetSeriesOffset() < sortedSeries[i]->GetSeriesOffset())
                    {
                        CGCDescSeries* temp = sortedSeries[i];
                        sortedSeries[i] = sortedSeries[j];
                        sortedSeries[j] = temp;
                    }
            }

            // Offset of the first pointer in the array
            // This equals the offset of the first pointer if this were an array of entirely pointers, plus the offset of the
            // first pointer in the value class
            pSeries->SetSeriesOffset(ArrayBase::GetDataPtrOffset(pMT)
                + (sortedSeries[0]->GetSeriesOffset()) - sizeof (Object) );
            for (index = 0; index < nSeries; index ++)
            {
                size_t numPtrsInBytes = sortedSeries[index]->GetSeriesSize()
                    + elemTypeHnd.AsMethodTable()->GetBaseSize();
                size_t currentOffset;
                size_t skip;
                currentOffset = sortedSeries[index]->GetSeriesOffset()+numPtrsInBytes;
                if (index != nSeries-1)
                {
                    skip = sortedSeries[index+1]->GetSeriesOffset()-currentOffset;
                }
                else if (index == 0)
                {
                    skip = elemTypeHnd.GetMethodTable()->GetAlignedNumInstanceFieldBytes() - numPtrsInBytes;
                }
                else
                {
                    skip = sortedSeries[0]->GetSeriesOffset() + elemTypeHnd.AsMethodTable()->GetBaseSize()
                         - ObjSizeOf(Object) - currentOffset;
                }

                _ASSERTE(!"Module::CreateArrayMethodTable() - unaligned GC info" || IS_ALIGNED(skip, sizeof(size_t)));

                unsigned short NumPtrs = (unsigned short) (numPtrsInBytes / sizeof(void*));
                if(skip > MAX_SIZE_FOR_VALUECLASS_IN_ARRAY || numPtrsInBytes > MAX_PTRS_FOR_VALUECLASSS_IN_ARRAY) {
                    StackSString ssElemName;
                    elemTypeHnd.GetName(ssElemName);

                    StackScratchBuffer scratch;
                    elemTypeHnd.GetAssembly()->ThrowTypeLoadException(ssElemName.GetUTF8(scratch),
                                                                      IDS_CLASSLOAD_VALUECLASSTOOLARGE);
                }

                val_serie_item *val_item = &(pSeries->val_serie[-index]);

                val_item->set_val_serie_item (NumPtrs, (unsigned short)skip);
            }
        }

        MethodTable *pMT1 = elemTypeHnd.AsMethodTable();
        if (pMT1->HasDefaultConstructor())
            pClass->SetElementCtor (pMT1->GetDefaultConstructor());
        else
            pClass->SetElementCtor (NULL);
    }
    else if (CorTypeInfo::IsObjRef(elemType))
    {
        CGCDescSeries  *pSeries;

        pMT->SetContainsPointers();

        // This array is all GC Pointers
        CGCDesc::GetCGCDescFromMT(pMT)->Init( pMT, 1 );

        pSeries = CGCDesc::GetCGCDescFromMT(pMT)->GetHighestSeries();

        pSeries->SetSeriesOffset(ArrayBase::GetDataPtrOffset(pMT));
        // For arrays, the size is the negative of the BaseSize (the GC always adds the total
        // size of the object, so what you end up with is the size of the data portion of the array)
        pSeries->SetSeriesSize(-(SSIZE_T)(pMT->GetBaseSize()));
    }

    // If we get here we are assuming that there was no truncation. If this is not the case then
    // an array whose base type is not a value class was created and was larger then 0xffff (a word)
    _ASSERTE(dwComponentSize == pMT->GetComponentSize());


    return(pMT);
}

//========================================================================
// Generates the platform-independent arrayop stub.
//========================================================================
void GenerateArrayOpScript(ArrayMethodDesc *pMD, ArrayOpScript *paos)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    ArrayOpIndexSpec *pai = NULL;
    MethodTable *pMT = pMD->GetMethodTable();
    ArrayClass *pcls = (ArrayClass*)(pMT->GetClass());

#ifdef _DEBUG 
    FillMemory(paos, sizeof(ArrayOpScript) + sizeof(ArrayOpIndexSpec) * pMT->GetRank(), 0xcc);
#endif

    // The ArrayOpScript and ArrayOpIndexSpec structs double as hash keys
    // for the MLStubCache.  Thus, it's imperative that there be no
    // unused "pad" fields that contain unstable values.
    // These two asserts confirm that no extra unnamed pad fields exist.
    C_ASSERT(sizeof(ArrayOpScript) == sizeof(paos->m_rank) +
                                    sizeof(paos->m_fHasLowerBounds) +
                                    sizeof(paos->m_flags) +
                                    sizeof(paos->m_signed) +
                                    sizeof(paos->m_op) +
                                    sizeof(paos->m_pad1) +
                                    sizeof(paos->m_fRetBufInReg) +
                                    sizeof(paos->m_fValInReg) +
                                    sizeof(paos->m_fRetBufLoc) +
                                    sizeof(paos->m_fValLoc) +
                                    sizeof(paos->m_cbretpop) +
                                    sizeof(paos->m_pad2) +
                                    sizeof(paos->m_elemsize) +
                                    sizeof(paos->m_ofsoffirst) +
                                    sizeof(paos->m_typeParamReg) +
                                    sizeof(paos->m_typeParamOffs) +
                                    sizeof(paos->m_gcDesc));
    C_ASSERT(sizeof(ArrayOpIndexSpec) == sizeof(pai->m_freg) +
                                       sizeof(pai->m_idxloc) +
                                       sizeof(pai->m_lboundofs) +
                                       sizeof(pai->m_lengthofs));


    paos->m_rank            = (BYTE)(pMT->GetRank());
    paos->m_fHasLowerBounds = (pMT->GetInternalCorElementType() == ELEMENT_TYPE_ARRAY);
    paos->m_flags           = 0;
    paos->m_signed          = FALSE;

    paos->m_gcDesc          = 0;
    paos->m_ofsoffirst      = ArrayBase::GetDataPtrOffset(pMT);
    paos->m_pad1            = 0;

    switch (pMD->GetArrayFuncIndex())
    {
    case ArrayMethodDesc::ARRAY_FUNC_GET:
        paos->m_op = ArrayOpScript::LOAD;
        break;
    case ArrayMethodDesc::ARRAY_FUNC_SET:
        paos->m_op = ArrayOpScript::STORE;
        break;
    case ArrayMethodDesc::ARRAY_FUNC_ADDRESS:
        paos->m_op = ArrayOpScript::LOADADDR;
        break;
    default:
        _ASSERTE(!"Unknown array func");
        ThrowHR(COR_E_EXECUTIONENGINE);
    }

    MetaSig msig(pMD);
    _ASSERTE(!msig.IsVarArg());     // No array signature is varargs, code below does not expect it.

    switch (pcls->GetArrayElementType())
    {
        // These are all different because of sign extension

        case ELEMENT_TYPE_I1:
            paos->m_elemsize = 1;
            paos->m_signed = TRUE;
            break;

        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_U1:
            paos->m_elemsize = 1;
            break;

        case ELEMENT_TYPE_I2:
            paos->m_elemsize = 2;
            paos->m_signed = TRUE;
            break;

        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_U2:
            paos->m_elemsize = 2;
            break;

        case ELEMENT_TYPE_I4:
        IN_WIN32(case ELEMENT_TYPE_I:)
            paos->m_elemsize = 4;
            paos->m_signed = TRUE;
            break;

        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_U:)
        IN_WIN32(case ELEMENT_TYPE_PTR:)
            paos->m_elemsize = 4;
            break;

        case ELEMENT_TYPE_I8:
        IN_WIN64(case ELEMENT_TYPE_I:)
            paos->m_elemsize = 8;
            paos->m_signed = TRUE;
            break;

        case ELEMENT_TYPE_U8:
        IN_WIN64(case ELEMENT_TYPE_U:)
        IN_WIN64(case ELEMENT_TYPE_PTR:)
            paos->m_elemsize = 8;
            break;

        case ELEMENT_TYPE_R4:
            paos->m_elemsize = 4;
            paos->m_flags |= paos->ISFPUTYPE;
            break;

        case ELEMENT_TYPE_R8:
            paos->m_elemsize = 8;
            paos->m_flags |= paos->ISFPUTYPE;
            break;

        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
            paos->m_elemsize = sizeof(LPVOID);
            paos->m_flags |= paos->NEEDSWRITEBARRIER;
            if (paos->m_op != ArrayOpScript::LOAD)
            {
                paos->m_flags |= paos->NEEDSTYPECHECK;
            }

            break;

        case ELEMENT_TYPE_VALUETYPE:
            paos->m_elemsize = pMT->GetComponentSize();
            if (pMT->ContainsPointers()) 
            {
                paos->m_gcDesc = CGCDesc::GetCGCDescFromMT(pMT);
                paos->m_flags |= paos->NEEDSWRITEBARRIER;
            }
            break;

        default:
            _ASSERTE(!"Unsupported Array Type");
            ThrowHR(COR_E_EXECUTIONENGINE);
    }

    paos->m_cbretpop = msig.CbStackPop(FALSE);
    paos->m_pad2     = 0;

    ArgIterator argit(NULL, &msig, FALSE);
    if (msig.HasRetBuffArg())
    {
        paos->m_flags |= ArrayOpScript::HASRETVALBUFFER;
        UINT regstructofs;
        int ofs = argit.GetRetBuffArgOffset(&regstructofs);
        if (regstructofs != (UINT) -1)
        {
            paos->m_fRetBufInReg = TRUE;
            paos->m_fRetBufLoc   = regstructofs;
        }
        else
        {
            paos->m_fRetBufInReg = FALSE;
            paos->m_fRetBufLoc   = ofs;
        }
    }
    else
    {
        // If no retbuf, these values are ignored; but set them to
        // constant values so they don't create unnecessary hash misses.
        paos->m_fRetBufInReg = 0;
        paos->m_fRetBufLoc   = 0;
    }

    for (UINT idx = 0; idx < paos->m_rank; idx++)
    {
        pai = (ArrayOpIndexSpec*)(paos->GetArrayOpIndexSpecs() + idx);
        //        int    GetNextOffset(BYTE *pType, UINT32 *pStructSize, UINT *pRegStructOfs = NULL);

        BYTE ptyp;
        UINT32 structsize;
        UINT regstructofs;
        int ofs = argit.GetNextOffset(&ptyp, &structsize, &regstructofs);
        if (regstructofs != (UINT) -1)
        {
            pai->m_freg   = TRUE;
            pai->m_idxloc = regstructofs;
        }
        else
        {
            pai->m_freg   = FALSE;
            pai->m_idxloc = ofs;
        }
        pai->m_lboundofs = paos->m_fHasLowerBounds ? (UINT32) (ArrayBase::GetLowerBoundsOffset(pMT) + idx*sizeof(DWORD)) : 0;
        pai->m_lengthofs = ArrayBase::GetBoundsOffset(pMT) + idx*sizeof(DWORD);
    }


    if (paos->m_op == ArrayOpScript::LOADADDR) 
    {
        paos->m_typeParamOffs = argit.GetParamTypeArgOffset(&paos->m_typeParamReg);
    }

    if (paos->m_op == paos->STORE)
    {
        BYTE ptyp; UINT32 structsize;
        UINT regstructofs;
        int ofs = argit.GetNextOffset(&ptyp, &structsize, &regstructofs);
        if (regstructofs != (UINT) -1)
        {
            paos->m_fValInReg = TRUE;
            paos->m_fValLoc = regstructofs;
        }
        else
        {
            paos->m_fValInReg = FALSE;
            paos->m_fValLoc = ofs;
        }
    }
}

Stub *GenerateArrayOpStub(CPUSTUBLINKER *psl, ArrayMethodDesc* pMD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    MethodTable *pMT = pMD->GetMethodTable();

        ArrayOpScript *paos = (ArrayOpScript*)_alloca(sizeof(ArrayOpScript) + sizeof(ArrayOpIndexSpec) * pMT->GetRank());

    GenerateArrayOpScript(pMD, paos);

        Stub *pArrayOpStub;
        ArrayStubCache::MLStubCompilationMode mode;
        pArrayOpStub = ECall::m_pArrayStubCache->Canonicalize((const BYTE *)paos, &mode);
        if (mode != MLStubCache::STANDALONE)
            COMPlusThrowOM();

        pArrayOpStub->SetRequiresMethodDescCallingConvention();
        return pArrayOpStub;
}

ArrayStubCache::MLStubCompilationMode ArrayStubCache::CompileMLStub(const BYTE *pRawMLStub,
                                                    StubLinker *psl,
                                                    void *callerContext)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_INTOLERANT;
    }
    CONTRACTL_END;

    MLStubCompilationMode ret = INTERPRETED;
    EX_TRY
    {
        FAULT_NOT_FATAL();
        ((CPUSTUBLINKER*)psl)->EmitArrayOpStub((ArrayOpScript*)pRawMLStub);
        ret = STANDALONE;
    }
    EX_CATCH
    {
        // In case of an error, we'll just leave the mode as "INTERPRETED."
        // and let the caller of Canonicalize() treat that as an error.
    } EX_END_CATCH(SwallowAllExceptions)
    return ret;

}

UINT ArrayStubCache::Length(const BYTE *pRawMLStub)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    return ((ArrayOpScript*)pRawMLStub)->Length();
}

//---------------------------------------------------------------------
// This method returns TRUE if pInterfaceMT could be one of the interfaces
// that are implicitly implemented by SZArrays

BOOL IsImplicitInterfaceOfSZArray(MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(pInterfaceMT->IsInterface());
    }
    CONTRACTL_END

    // Is target interface Anything<T> in mscorlib?
    if (!pInterfaceMT->HasInstantiation() || !pInterfaceMT->GetModule()->IsSystem())
        return FALSE;

    mdTypeDef token = pInterfaceMT->GetCl();

    // Is target interface IList<T> or one of its ancestors?
    return (token == g_Mscorlib.GetTypeDef(CLASS__ILISTGENERIC) ||
            token == g_Mscorlib.GetTypeDef(CLASS__ICOLLECTIONGENERIC) ||
            token == g_Mscorlib.GetTypeDef(CLASS__IENUMERABLEGENERIC));
}

//---------------------------------------------------------------------
static BOOL ArraySupportsBizarreInterfaceWorker(OBJECTREF arrayRef, ArrayTypeDesc *pArrayTypeDesc, MethodTable *pInterfaceMT);

//---------------------------------------------------------------------
// Check if arrays supports certain interfaces that don't appear in the base interface
// list. It does not check the base interfaces themselves - you must do that
// separately.
//---------------------------------------------------------------------
BOOL ArraySupportsBizarreInterface(OBJECTREF arrayRef, MethodTable *pInterfaceMT)
{
    WRAPPER_CONTRACT;
    return ArraySupportsBizarreInterfaceWorker(arrayRef, NULL, pInterfaceMT);
}

//---------------------------------------------------------------------
// Check if arrays supports certain interfaces that don't appear in the base interface
// list. It does not check the base interfaces themselves - you must do that
// separately.
//---------------------------------------------------------------------
BOOL ArraySupportsBizarreInterfaceStatic(ArrayTypeDesc *pArrayTypeDesc, MethodTable *pInterfaceMT)
{
    WRAPPER_CONTRACT;
    return ArraySupportsBizarreInterfaceWorker(NULL, pArrayTypeDesc, pInterfaceMT);
}

//---------------------------------------------------------------------
// Common worker for ArraySupportsBizarreInterface and ArraySupportsBizarreInterfaceStatic.
//
// arrayref and pArrayTypeDesc represent two mutually exclusive ways of specifying
// the array to be castchecked (exactly one of them must be NULL.)
//---------------------------------------------------------------------
/*static */ BOOL ArraySupportsBizarreInterfaceWorker(OBJECTREF arrayRef, ArrayTypeDesc *pArrayTypeDesc, MethodTable *pInterfaceMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    BOOL fRet = FALSE;

    _ASSERTE(arrayRef == NULL || pArrayTypeDesc == NULL);
    _ASSERTE(!(arrayRef == NULL && pArrayTypeDesc == NULL));
    _ASSERTE(pArrayTypeDesc == NULL || pArrayTypeDesc->IsArray());

    GCPROTECT_BEGIN(arrayRef);

    MethodTable *pArrayMT = (arrayRef != NULL) ? arrayRef->GetMethodTable() : pArrayTypeDesc->GetMethodTable();

    _ASSERTE(pInterfaceMT->IsInterface());
    _ASSERTE(pArrayMT->IsArray());
    _ASSERTE(pArrayMT->IsRestored());

    pInterfaceMT->CheckRestore();

    // IList<T> only supported for SZ_ARRAYS
    if ((pArrayMT->GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY) &&
        IsImplicitInterfaceOfSZArray(pInterfaceMT))
    {
        // Now check if array is castable to T[].
        TypeHandle *pInstantiation = pInterfaceMT->GetInstantiation();
        TypeHandle exactMatchArrayTH = ClassLoader::LoadArrayTypeThrowing(pInstantiation[0]);
        if (arrayRef != NULL)
        {
            fRet = (ObjIsInstanceOf(OBJECTREFToObject(arrayRef), exactMatchArrayTH) != NULL);
        }
        else
        {
            fRet = pArrayTypeDesc->CanCastTo(exactMatchArrayTH);
        }
    }

    GCPROTECT_END();

    return fRet;
}





//
//
//
MethodDesc* GetActualImplementationForArrayGenericIListMethod(MethodDesc *pItfcMeth, TypeHandle theT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    int slot = pItfcMeth->GetSlot();

    // We need to pick the right starting method depending on the depth of the inheritance chain
    static const BinderMethodID startingMethod[] = {
        METHOD__SZARRAYHELPER__GETENUMERATOR,   // First method of IEnumerable`1
        METHOD__SZARRAYHELPER__GET_COUNT,       // First method of ICollection`1
        METHOD__SZARRAYHELPER__GET_ITEM         // First method of IList`1
    };

    // Subtract one for the non-generic IEnumerable that the generic enumerable inherits from
    unsigned int inheritanceDepth = pItfcMeth->GetMethodTable()->GetNumInterfaces() - 1;
    PREFIX_ASSUME(0 <= inheritanceDepth && inheritanceDepth < NumItems(startingMethod));
   
    MethodDesc *pGenericImplementor = g_Mscorlib.FetchMethod((BinderMethodID)(startingMethod[inheritanceDepth] + slot));

    MethodTable *pArrayHelperMT = g_Mscorlib.FetchClass(CLASS__SZARRAYHELPER);
    // The most common reason for this assert is that the order of the SZArrayHelper methods in
    // mscorlib.h does not match the order they are implemented on the generic interfaces.
    _ASSERTE(pGenericImplementor == pArrayHelperMT->GetClass()->FindMethodByName(pItfcMeth->GetName()));


    MethodDesc *pActualImplementor = MethodDesc::FindOrCreateAssociatedMethodDesc(pGenericImplementor,
                                                                                  pArrayHelperMT,
                                                                                  FALSE,
                                                                                  1,
                                                                                  &theT,
                                                                                  FALSE // allowInstParam
                                                                                  );
    _ASSERTE(pActualImplementor);
    return pActualImplementor;

}
#endif // DACCESS_COMPILE
