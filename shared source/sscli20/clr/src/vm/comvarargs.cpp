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
////////////////////////////////////////////////////////////////////////////////
// This module contains the implementation of the native methods for the
//  varargs class(es)..
//
////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "object.h"
#include "excep.h"
#include "frames.h"
#include "vars.hpp"
#include "comvariant.h"
#include "comvarargs.h"

static void InitCommon(VARARGS *data, VASigCookie** cookie);
static void AdvanceArgPtr(VARARGS *data);

////////////////////////////////////////////////////////////////////////////////
// ArgIterator constructor that initializes the state to support iteration
// of the args starting at the first optional argument.
////////////////////////////////////////////////////////////////////////////////
FCIMPL2(void, COMVarArgs::Init, VARARGS* _this, LPVOID cookie)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    HELPER_METHOD_FRAME_BEGIN_0();

    _ASSERTE(_this != NULL);
    VARARGS* data = _this;
    if (cookie == 0)
        COMPlusThrow(kArgumentException, L"InvalidOperation_HandleIsNotInitialized");

    VASigCookie* pCookie = *(VASigCookie**)(cookie); 

    if (pCookie->mdVASig == NULL)
    {
        data->SigPtr = NULL;
        data->ArgCookie = NULL;
        data->ArgPtr = (BYTE*)((VASigCookieEx*)pCookie)->m_pArgs;
    }
    else
    {
        // Use common code to pick the cookie apart and advance to the ...
        InitCommon(data, (VASigCookie**)cookie);
        AdvanceArgPtr(data);
    }

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////////
// ArgIterator constructor that initializes the state to support iteration
// of the args starting at the argument following the supplied argument pointer.
// Specifying NULL as the firstArg parameter causes it to start at the first
// argument to the call.
////////////////////////////////////////////////////////////////////////////////
FCIMPL3(void, COMVarArgs::Init2, VARARGS* _this, LPVOID cookie, LPVOID firstArg)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    }
    CONTRACTL_END;
    
    HELPER_METHOD_FRAME_BEGIN_0();

    _ASSERTE(_this != NULL);
    VARARGS* data = _this;
    if (cookie == 0)
        COMPlusThrow(kArgumentException, L"InvalidOperation_HandleIsNotInitialized");

    // Init most of the structure.
    InitCommon(data, (VASigCookie**)cookie);

    // If it is NULL, start at the first arg.
    if (firstArg != NULL)
    {
        //
        // The expectation made by COMVarArgs is that:
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
        // data->ArgPtr points to the end of the next argument.
        //    <varargs_cookie>         
        //    <argN>                   <-- data->ArgPtr after InitCommon
        //                            
        //    <argM 1st optional arg>  
        // *@ <arg2>                   <-- firstArg, data->ArgPtr leaving Init2()
        //    <arg1>
        //    <this>            ;; if an instance method
        //                     ;; higher memory
        //
#else // STACK_GROWS_UP_ON_ARGS_WALK
        // data->ArgPtr points to the beginning of the next argument
        //    <this>            ;; if an instance method
        //    <varargs_cookie>  
        //    <arg1>                     <-- data->ArgPtr after InitCommon
        // *  <arg2>                     <-- firstArg
        //  @ <argM - 1st optional arg>  <-- data->ArgPtr leaving Init2()
        //                          
        //    <argN>            
        //                     ;; higher memory
#endif // STACK_GROWS_**_ON_ARGS_WALK
        // where * indicates the place on the stack that firstArg points upon
        // entry to Init2. We need to correct in the STACK_GROWS_UP... case since
        // we actually want to point at the argument after firstArg. This confusion
        // comes from the difference in expectation of whether ArgPtr is pointing
        // at the beginning or end of the argument on the stack.
        //
        // @ indicates where we want data->ArgPtr to point to after we're done with Init2
        //

#ifdef STACK_GROWS_UP_ON_ARGS_WALK
        // special case when we're already setup from InitCommon case; this can only 
        // happen if the stack grows up on args walk and firstArg is the first argument
        // to the method.
        if (data->ArgPtr == firstArg)
        {
            if (data->SigPtr.AtSentinel())
            {
                COMPlusThrow(kArgumentException);
            }
            
            SigTypeContext typeContext;
            data->ArgPtr += StackElemSize(data->SigPtr.SizeOf(data->ArgCookie->pModule,&typeContext));
            data->SigPtr.SkipExactlyOne();
            data->RemainingArgs--;
        }
        else
#endif
        {
            // Advance to the specified arg.
            while (data->RemainingArgs > 0)
            {
                if (data->SigPtr.AtSentinel())
                {
                    COMPlusThrow(kArgumentException);
                }

                SigTypeContext      typeContext; // This is an empty type context.  This is OK because the vararg methods may not be generic
                SIZE_T cbArg = StackElemSize(data->SigPtr.SizeOf(data->ArgCookie->pModule,&typeContext));

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                if (cbArg > ENREGISTERED_PARAMTYPE_MAXSIZE)
                {
                    cbArg = sizeof(void*);
                }
#endif

                // Adjust the frame pointer and the signature info.
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                data->ArgPtr -= cbArg;
#else  // STACK_GROWS_UP_ON_ARGS_WALK
                data->ArgPtr += cbArg;
#endif // STACK_GROWS_**_ON_ARGS_WALK

                data->SigPtr.SkipExactlyOne();
                data->RemainingArgs--;

                // Stop when we get to where the user wants to be.
                if (data->ArgPtr == firstArg)
                {
#ifdef STACK_GROWS_UP_ON_ARGS_WALK
                    // Skip to the end of firstArg so that we're pointing at the 
                    // beginning of the next argument.
                    data->ArgPtr += StackElemSize(data->SigPtr.SizeOf(data->ArgCookie->pModule,&typeContext));
                    data->SigPtr.SkipExactlyOne();
                    data->RemainingArgs--;
#else  // STACK_GROWS_DOWN_ON_ARGS_WALK
                    // !!DO NOTHING!!, See comment above.
#endif // STACK_GROWS_**_ON_ARGS_WALK

                    break;
                }
            }
        }
    }
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////////
// Initialize the basic info for processing a varargs parameter list.
////////////////////////////////////////////////////////////////////////////////
static void InitCommon(VARARGS *data, VASigCookie** cookie)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(data));
        PRECONDITION(CheckPointer(cookie));
     } CONTRACTL_END;

    // Save the cookie and a copy of the signature.
    data->ArgCookie = *cookie;
    data->SigPtr.SetSig(data->ArgCookie->mdVASig);

    // Skip the calling convention, get the # of args and skip the return type.
    ULONG callConv;
    IfFailThrow(data->SigPtr.GetCallingConvInfo(&callConv));

    ULONG sigData;
    IfFailThrow(data->SigPtr.GetData(&sigData));
    data->RemainingArgs = sigData;

    IfFailThrow(data->SigPtr.SkipExactlyOne());

    // Get a pointer to the cookie arg.
    data->ArgPtr = (BYTE *) cookie;

#if defined(_X86_)
    //  STACK_GROWS_DOWN_ON_ARGS_WALK

    //   <return address>  ;; lower memory                  
    //   <varargs_cookie>         \
    //   <argN>                    \
    //                              += sizeOfArgs     
    //                             /
    //   <arg1>                   /
    // * <this>            ;; if an instance method (note: <this> is usally passed in 
    //                     ;; a register and wouldn't appear on the stack frame)
    //                     ;; higher memory
    //
    // When the stack grows down, ArgPtr always points to the end of the next
    // argument passed. So we initialize it to the address that is the
    // end of the first fixed arg (arg1) (marked with a '*').

    data->ArgPtr += data->ArgCookie->sizeOfArgs;
#else
    //  STACK_GROWS_UP_ON_ARGS_WALK

    //   <this>	           ;; lower memory
    //   <varargs_cookie>  ;; if an instance method
    // * <arg1>
    //                          
    //   <argN>            ;; higher memory
    //
    // When the stack grows up, ArgPtr always points to the start of the next
    // argument passed. So we initialize it to the address marked with a '*',
    // which is the start of the first fixed arg (arg1).

    // Always skip over the varargs_cookie.
    data->ArgPtr += StackElemSize(sizeof(LPVOID));
#endif

    COMVariant::EnsureVariantInitialized();
}

////////////////////////////////////////////////////////////////////////////////
// After initialization advance the next argument pointer to the first optional
// argument.
////////////////////////////////////////////////////////////////////////////////
void AdvanceArgPtr(VARARGS *data)
{ 
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(data));
     } CONTRACTL_END;

    // Advance to the first optional arg.
    while (data->RemainingArgs > 0)
    {
        if (data->SigPtr.AtSentinel())
            break;

        SigTypeContext      typeContext; // This is an empty type context.  This is OK because the vararg methods may not be generic
        SIZE_T cbArg = StackElemSize(data->SigPtr.SizeOf(data->ArgCookie->pModule, &typeContext)); 

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
        if (cbArg > ENREGISTERED_PARAMTYPE_MAXSIZE)
            cbArg = sizeof(void*);
#endif

        // Adjust the frame pointer and the signature info.
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
        data->ArgPtr -= cbArg;
#else  // STACK_GROWS_UP_ON_ARGS_WALK
        data->ArgPtr += cbArg;
#endif // STACK_GROWS_**_ON_ARGS_WALK

        data->SigPtr.SkipExactlyOne();
        --data->RemainingArgs;
    }
}




////////////////////////////////////////////////////////////////////////////////
// Return the number of unprocessed args in the argument iterator.
////////////////////////////////////////////////////////////////////////////////
FCIMPL1(int, COMVarArgs::GetRemainingCount, VARARGS* _this)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    _ASSERTE(_this != NULL);
    if (!(_this->ArgCookie))
    {
        // this argiterator was created by marshaling from an unmanaged va_list -
        // can't do this operation
        COMPlusThrow(kNotSupportedException); 
    }
    HELPER_METHOD_FRAME_END();
    return (_this->RemainingArgs);
}
FCIMPLEND


////////////////////////////////////////////////////////////////////////////////
// Retrieve the type of the next argument without consuming it.
////////////////////////////////////////////////////////////////////////////////
FCIMPL1(void*, COMVarArgs::GetNextArgType, VARARGS* _this)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    TypedByRef  value;

    HELPER_METHOD_FRAME_BEGIN_RET_0();

    _ASSERTE(_this != NULL);
    VARARGS     data = *_this;

    if (!(_this->ArgCookie))
    {
        // this argiterator was created by marshaling from an unmanaged va_list -
        // can't do this operation
        COMPlusThrow(kNotSupportedException);
    }


    // Make sure there are remaining args.
    if (data.RemainingArgs == 0)
        COMPlusThrow(kInvalidOperationException, L"InvalidOperation_EnumEnded");

    GetNextArgHelper(&data, &value, FALSE);
    HELPER_METHOD_FRAME_END();
    return value.type.AsPtr();
}
FCIMPLEND

////////////////////////////////////////////////////////////////////////////////
// Retrieve the next argument and return it in a TypedByRef and advance the
// next argument pointer.
////////////////////////////////////////////////////////////////////////////////
FCIMPL2(void, COMVarArgs::DoGetNextArg, VARARGS* _this, void * value)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypedByRef * result = (TypedByRef *)value;
    HELPER_METHOD_FRAME_BEGIN_0();
    GCPROTECT_BEGININTERIOR (result);

    _ASSERTE(_this != NULL);
    if (!(_this->ArgCookie))
    {
        // this argiterator was created by marshaling from an unmanaged va_list -
        // can't do this operation
        COMPlusThrow(kInvalidOperationException);
    }

    // Make sure there are remaining args.
    if (_this->RemainingArgs == 0)
        COMPlusThrow(kInvalidOperationException, L"InvalidOperation_EnumEnded");

    GetNextArgHelper(_this, result, TRUE);
    GCPROTECT_END ();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND



////////////////////////////////////////////////////////////////////////////////
// Retrieve the next argument and return it in a TypedByRef and advance the
// next argument pointer.
////////////////////////////////////////////////////////////////////////////////
FCIMPL3(void, COMVarArgs::GetNextArg2, VARARGS* _this, void * value, TypeHandle typehandle)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    TypedByRef * result = (TypedByRef *)value;
    HELPER_METHOD_FRAME_BEGIN_0();
    GCPROTECT_BEGININTERIOR (result);

    COMPlusThrow(kNotSupportedException);

    GCPROTECT_END ();
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND



////////////////////////////////////////////////////////////////////////////////
// This is a helper that uses a VARARGS tracking data structure to retrieve
// the next argument out of a varargs function call.  This does not check if
// there are any args remaining (it assumes it has been checked).
////////////////////////////////////////////////////////////////////////////////
void  COMVarArgs::GetNextArgHelper(VARARGS *data, TypedByRef *value, BOOL fData)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(data));
        PRECONDITION(CheckPointer(value));
    } CONTRACTL_END;

    GCPROTECT_BEGININTERIOR (value);
    unsigned __int8 elemType;

    _ASSERTE(data->RemainingArgs != 0);

    SigTypeContext typeContext; // This is an empty type context.  This is OK because the vararg methods may not be generic
    SIZE_T cbRaw = data->SigPtr.SizeOf(data->ArgCookie->pModule,&typeContext);
    SIZE_T cbArg = StackElemSize(cbRaw);

    // Get a pointer to the beginning of the argument. 
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
    data->ArgPtr -= cbArg;
#endif

    // Assume the ref pointer points directly at the arg on the stack.
    void* origArgPtr = data->ArgPtr;
    value->data = origArgPtr;

#ifndef STACK_GROWS_DOWN_ON_ARGS_WALK
    data->ArgPtr += cbArg;
#endif // STACK_GROWS_**_ON_ARGS_WALK


TryAgain:
    switch (elemType = data->SigPtr.PeekElemTypeClosed(&typeContext))
    {
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
#if BIGENDIAN
        if ( origArgPtr == value->data ) {
            value->data = (BYTE*)origArgPtr + (sizeof(void*)-1);
        }
#endif
        value->type = GetTypeHandleForCVType(elemType);
        break;

        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
#if BIGENDIAN
        if ( origArgPtr == value->data ) {
            value->data = (BYTE*)origArgPtr + (sizeof(void*)-2);
        }
#endif
        value->type = GetTypeHandleForCVType(elemType);
        break;

        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_R4:
        case ELEMENT_TYPE_STRING:
        value->type = GetTypeHandleForCVType(elemType);
        break;

        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R8:
        value->type = GetTypeHandleForCVType(elemType);
#if !defined(_WIN64) && (DATA_ALIGNMENT > 4)
        if ( fData && origArgPtr == value->data ) {
            // allocate an aligned copy of the value
            value->data = value->type.GetMethodTable()->Box(origArgPtr, FALSE)->UnBox();
        }
#endif
        break;


        case ELEMENT_TYPE_I:
        value->type = ElementTypeToTypeHandle(ELEMENT_TYPE_I);
        break;

        case ELEMENT_TYPE_U:
        value->type = ElementTypeToTypeHandle(ELEMENT_TYPE_U);
        break;

        case ELEMENT_TYPE_PTR:
            value->type = data->SigPtr.GetTypeHandleThrowing(data->ArgCookie->pModule, &typeContext);
            break;

        case ELEMENT_TYPE_BYREF:
            // Check if we have already processed a by-ref.
            if (value->data != origArgPtr)
            {
                _ASSERTE(!"Can't have a ByRef of a ByRef");
                COMPlusThrow(kNotSupportedException, L"NotSupported_Type");
            }

            // Dereference the argument to remove the indirection of the ByRef.
            value->data = *((void **) value->data);

            // Consume and discard the element type.
            IfFailThrow(data->SigPtr.GetElemType(NULL));
            goto TryAgain;

        case ELEMENT_TYPE_VALUETYPE:

#if defined(VALUETYPES_BY_REFERENCE) || defined(ENREGISTERED_PARAMTYPE_MAXSIZE)
            if (   origArgPtr == value->data
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                && cbArg > ENREGISTERED_PARAMTYPE_MAXSIZE
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE
                )
            {
                CorElementType realType = data->SigPtr.PeekElemTypeNormalized(data->ArgCookie->pModule, &typeContext);

                if (realType == ELEMENT_TYPE_VALUETYPE) {
                    // Adjust the arg pointer so only one word has been skipped
                    data->ArgPtr = (BYTE *)origArgPtr + sizeof(void*);
                    // Dereference the argument because the valuetypes are always passed by reference
                    value->data = *((void **) origArgPtr);
                }
                else {
                    // Adjust the arg pointer by appropariate amount
                    data->ArgPtr = (BYTE *)origArgPtr + cbArg;
                    // We don't need to dereference enums
                    value->data = StackElemEndianessFixup(origArgPtr, cbRaw);
                }
            }
#endif
        // fall through
        case ELEMENT_TYPE_CLASS: {
            value->type = data->SigPtr.GetTypeHandleThrowing(data->ArgCookie->pModule, &typeContext);

#if BIGENDIAN && !defined(VALUETYPES_BY_REFERENCE)
            // Adjust the pointer for small valuetypes
            if (elemType == ELEMENT_TYPE_VALUETYPE && origArgPtr == value->data) {
                value->data = StackElemEndianessFixup(origArgPtr, value->type.GetSize());
            }
#endif
            if (elemType == ELEMENT_TYPE_CLASS && value->type.GetClass()->IsValueClass())
                value->type = g_pObjectClass;
            } break;

        case ELEMENT_TYPE_TYPEDBYREF:
            if (value->data != origArgPtr)
            {
                _ASSERTE(!"Can't have a ByRef of a TypedByRef");
                COMPlusThrow(kNotSupportedException, L"NotSupported_Type");
            }
#if defined(VALUETYPES_BY_REFERENCE) || defined(ENREGISTERED_PARAMTYPE_MAXSIZE)
#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            if (sizeof(TypedByRef) > ENREGISTERED_PARAMTYPE_MAXSIZE)
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE
            {
                // Adjust the arg pointer so only one word has been skipped
                data->ArgPtr = (BYTE *)origArgPtr + sizeof(void *);
                // Dereference the argument because the valuetypes are always passed by reference
                value->data = *((void **)origArgPtr);
            }
#endif
            // Load the TypedByRef
            value->type = ((TypedByRef*)value->data)->type;
            value->data = ((TypedByRef*)value->data)->data;
            break;

        case ELEMENT_TYPE_SZARRAY:
        case ELEMENT_TYPE_ARRAY:
#ifdef VALUE_ARRAYS
        case ELEMENT_TYPE_VALUEARRAY:
#endif
            {
                value->type = data->SigPtr.GetTypeHandleThrowing(data->ArgCookie->pModule,&typeContext);

                break;
            }

        case ELEMENT_TYPE_FNPTR:
        case ELEMENT_TYPE_OBJECT:
            _ASSERTE(!"Not implemented");
            COMPlusThrow(kNotSupportedException);
            break;

        case ELEMENT_TYPE_SENTINEL:
        default:
            _ASSERTE(!"Should be unreachable");
            COMPlusThrow(kNotSupportedException, L"NotSupported_Type");
            break;
    }

    // Update the tracking stuff to move past the argument.
    --data->RemainingArgs;
    data->SigPtr.SkipExactlyOne();
    GCPROTECT_END ();
}


/*static*/ void COMVarArgs::MarshalToManagedVaList(va_list va, VARARGS *dataout)
{
    WRAPPER_CONTRACT

    _ASSERTE(dataout != NULL);
    dataout->SigPtr = NULL;
    dataout->ArgCookie = NULL;
    dataout->ArgPtr = (BYTE*)va;
}

////////////////////////////////////////////////////////////////////////////////
// Creates an unmanaged va_list equivalent. (WARNING: Allocated from the
// LIFO memory manager so this va_list is only good while that memory is in "scope".) 
////////////////////////////////////////////////////////////////////////////////
/*static*/ va_list COMVarArgs::MarshalToUnmanagedVaList(const VARARGS *data)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(data));
     } CONTRACTL_END;


    // Must make temporary copy so we don't alter the original
    SigPointer sp = data->SigPtr;

    SigTypeContext typeContext; // This is an empty type context.  This is OK because the vararg methods may not be generic

    // Calculate how much space we need for the marshaled stack. This actually overestimates
    // the value since it counts the fixed args as well as the varargs. But that's harmless.
    // Generics: unmanaged varargs have NULL class and method instantiations as
    // they can't occur inside generic code i.e. can't involve generic type parameters).
    // On IA64 and AMD64, the managed and unmanaged calling conventions are the same, so we
    // can simply use the size stored in the VASigCookie.
    DWORD      cbAlloc;
    cbAlloc = MetaSig::SizeOfActualFixedArgStack(data->ArgCookie->pModule , 
                                                 data->ArgCookie->mdVASig, 
                                                 SigParser::LengthOfSig(data->ArgCookie->mdVASig),
                                                 FALSE, 
                                                 &typeContext);

    BYTE*      pdstbuffer = (BYTE*)(GetThread()->m_MarshalAlloc.Alloc(cbAlloc));

    int        remainingArgs = data->RemainingArgs;
    BYTE*      psrc = (BYTE*)(data->ArgPtr);
    BYTE*      pdst = pdstbuffer;
    while (remainingArgs--) 
    {
        CorElementType elemType = sp.PeekElemTypeClosed(&typeContext); 
        switch (elemType)
        {
            case ELEMENT_TYPE_I1:
            case ELEMENT_TYPE_U1:
            case ELEMENT_TYPE_I2:
            case ELEMENT_TYPE_U2:
            case ELEMENT_TYPE_I4:
            case ELEMENT_TYPE_U4:
            case ELEMENT_TYPE_I8:
            case ELEMENT_TYPE_U8:
            case ELEMENT_TYPE_R4:
            case ELEMENT_TYPE_R8:
            case ELEMENT_TYPE_I:
            case ELEMENT_TYPE_U:
            case ELEMENT_TYPE_PTR:
                {
                    DWORD cbSize = StackElemSize(sp.SizeOf(data->ArgCookie->pModule,&typeContext));

                    #ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                    if (cbSize > ENREGISTERED_PARAMTYPE_MAXSIZE)
                        cbSize = sizeof(void*);
                    #endif
                    
                    #ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                    psrc -= cbSize;
                    #endif // STACK_GROWS_DOWN_ON_ARGS_WALK

                    CONSISTENCY_CHECK(pdst + cbSize <= pdstbuffer + cbAlloc);
                    CopyMemory(pdst, psrc, cbSize);

                    #ifdef STACK_GROWS_UP_ON_ARGS_WALK
                    psrc += cbSize;
                    #endif // STACK_GROWS_UP_ON_ARGS_WALK

                    pdst += cbSize;
                    sp.SkipExactlyOne();
                }
                break;

            default:
                // non-IJW data type - we don't support marshaling these inside a va_list.
                COMPlusThrow(kNotSupportedException);


        }
    }

    return (va_list)pdstbuffer;
}

#ifdef CALLDESCR_REGTYPEMAP
//
// This method is called by SetupVarArgsAndForwardCall to determine which
// integer register slots need to be shadowed in FP registers for a 
// vararg call.
//
HCIMPL1(UINT64, COMVarArgs::GetRegisterTypeMap, VARARGS* _this)
{
    UINT64 map = 0;
    
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_CAPTURE_DEPTH_2 | Frame::FRAME_ATTR_EXACT_DEPTH);

    Module*         pModule = _this->ArgCookie->pModule;
    PCCOR_SIGNATURE pRawSig = _this->ArgCookie->mdVASig;

    SigTypeContext  context;
    MetaSig         sig(pRawSig, SigParser::LengthOfSig(pRawSig), pModule, &context);
    ArgIterator     ai(NULL, &sig, !sig.HasThis());

    UINT32          cbType;
    BYTE            type;
    BYTE*           pMap = (BYTE*)&map;
    int             regArgNum = 0;
    int             fpArgNum = 0;

    ai.GetNextOffsetFaster(&type, &cbType);

    if (sig.HasThis())
    {
        // this ptr
        regArgNum++;
    }

    if (sig.IsVarArg())
    {
        // va sig cookie
        regArgNum++;
    }

#ifndef RETBUF_ARG_SPECIAL_PARAM
    if (sig.HasRetBuffArg())
    {
        regArgNum++;
    }
#endif // RETBUF_ARG_SPECIAL_PARAM

    C_ASSERT(NUM_ARGUMENT_REGISTERS <= sizeof(UINT64));

    while ((regArgNum < NUM_ARGUMENT_REGISTERS) && type != ELEMENT_TYPE_END)
    {
        FillInRegTypeMap(&ai, pMap, &regArgNum, &type, cbType);
        ai.GetNextOffsetFaster(&type, &cbType);
    }

    HELPER_METHOD_FRAME_END();

    return map;
}
HCIMPLEND
#endif // CALLDESCR_REGTYPEMAP
