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
// ****************************************************************************
// File: funceval.cpp
//
// funceval.cpp - Debugger func-eval routines.
//
// ****************************************************************************
// Putting code & #includes, #defines, etc, before the stdafx.h will
// cause the code,etc, to be silently ignored


#include "stdafx.h"
#include "comsystem.h"
#include "debugdebugger.h"
#include "ipcmanagerinterface.h"
#include "../inc/common.h"
#include "comstring.h"
#include "perflog.h"
#include "eeconfig.h" // This is here even for retail & free builds...
#include "../../dlls/mscorrc/resource.h"
#include "remoting.h"
#include "context.h"
#include "vars.hpp"
#include "threads.h"
#include "appdomain.inl"
#include <limits.h>
#include "ilformatter.h"
#include "memoryreport.h"

#ifndef DACCESS_COMPILE

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

//
//
//
//
inline static bool IsElementTypeSpecial(CorElementType type)
{
    LEAF_CONTRACT;

    return ((type == ELEMENT_TYPE_CLASS)   ||
            (type == ELEMENT_TYPE_OBJECT)  ||
            (type == ELEMENT_TYPE_ARRAY)   ||
            (type == ELEMENT_TYPE_SZARRAY) ||
            (type == ELEMENT_TYPE_STRING));
}

//
// GetAndSetLiteralValue()
//
// This helper function extracts the value out of the source pointer while taking into account alignment and size.
// Then it stores the value into the destination pointer, again taking into account alignment and size.
//
// parameters:   pDst    - destination pointer
//               dstType - the CorElementType of the destination value
//               pSrc    - source pointer
//               srcType - the CorElementType of the source value
//
// return value: none
//
inline static void GetAndSetLiteralValue(LPVOID pDst, CorElementType dstType, LPVOID pSrc, CorElementType srcType)
{
    LEAF_CONTRACT;

    UINT64 srcValue;

    // Retrieve the value using the source CorElementType.
    switch (g_pEEInterface->GetSizeForCorElementType(srcType))
    {
    case 1:
        srcValue = (UINT64)*((BYTE*)pSrc);
        break;
    case 2:
        srcValue = (UINT64)*((USHORT*)pSrc);
        break;
    case 4:
        srcValue = (UINT64)*((UINT32*)pSrc);
        break;
    case 8:
        srcValue = (UINT64)*((UINT64*)pSrc);
        break;

    default:
        UNREACHABLE();
    }

    // Cast to the appropriate type using the destination CorElementType.
    switch (dstType)
    {
    case ELEMENT_TYPE_BOOLEAN:
        *(BYTE*)pDst = (BYTE)!!srcValue;
        break;
    case ELEMENT_TYPE_I1:
        *(INT8*)pDst = (INT8)srcValue;
        break;
    case ELEMENT_TYPE_U1:
        *(UINT8*)pDst = (UINT8)srcValue;
        break;
    case ELEMENT_TYPE_I2:
        *(INT16*)pDst = (INT16)srcValue;
        break;
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        *(UINT16*)pDst = (UINT16)srcValue;
        break;
    case ELEMENT_TYPE_I4:
        *(int*)pDst = (int)srcValue;
        break;
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_R4:
        *(unsigned*)pDst = (unsigned)srcValue;
        break;
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_R8:
        *(INT64*)pDst = (INT64)srcValue;
    case ELEMENT_TYPE_U8:
        *(UINT64*)pDst = (UINT64)srcValue;
        break;
    case ELEMENT_TYPE_PTR:
        *(void **)pDst = (void *)(SIZE_T)srcValue;
        break;

    default:
        UNREACHABLE();
    }

}


//
// Throw on not supported func evals
//
static void ValidateFuncEvalReturnType(DebuggerIPCE_FuncEvalType evalType, MethodTable * pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;
    
    if (pMT == g_pStringClass) 
    {
        if (evalType == DB_IPCE_FET_NEW_OBJECT || evalType == DB_IPCE_FET_NEW_OBJECT_NC) 
        {
            // Cannot call New object on String constructor.
            COMPlusThrow(kArgumentException,L"Argument_CannotCreateString");
        }
    }
    else if (g_pEEInterface->IsTypedReference(pMT)) 
    {
        // Cannot create typed references through funceval.
        if (evalType == DB_IPCE_FET_NEW_OBJECT || evalType == DB_IPCE_FET_NEW_OBJECT_NC || evalType == DB_IPCE_FET_NORMAL)
        {
            COMPlusThrow(kArgumentException, L"Argument_CannotCreateTypedReference");
        }
    }
}

//
// Given a register, return the value.
//
static SIZE_T GetRegisterValue(DebuggerEval *pDE, CorDebugRegister reg, void *regAddr)
{
    LEAF_CONTRACT;

    SIZE_T ret = 0;

    // A non-NULL register address indicates the value of the register was pushed because we're not on the leaf frame,
    // so we use the address of the register given to us instead of the register value in the context.
    if (regAddr != NULL)
    {
        ret = *((SIZE_T*)regAddr);
    }
    else
    {
        switch (reg)
        {
        case REGISTER_STACK_POINTER:
            ret = (SIZE_T)GetSP(&pDE->m_context);
            break;

        case REGISTER_FRAME_POINTER:
            ret = (SIZE_T)GetFP(&pDE->m_context);
            break;

#if defined(_X86_)
        case REGISTER_X86_EAX:
            ret = pDE->m_context.Eax;
            break;

        case REGISTER_X86_ECX:
            ret = pDE->m_context.Ecx;
            break;

        case REGISTER_X86_EDX:
            ret = pDE->m_context.Edx;
            break;

        case REGISTER_X86_EBX:
            ret = pDE->m_context.Ebx;
            break;

        case REGISTER_X86_ESI:
            ret = pDE->m_context.Esi;
            break;

        case REGISTER_X86_EDI:
            ret = pDE->m_context.Edi;
            break;

#endif // !_X86_ && !_AMD64_
        default:
            _ASSERT(!"Invalid register number!");

        }

    }

    return ret;
}

//
// Given a register, set its value.
//
static void SetRegisterValue(DebuggerEval *pDE, CorDebugRegister reg, void *regAddr, SIZE_T newValue)
{
    LEAF_CONTRACT;

    // A non-NULL register address indicates the value of the register was pushed because we're not on the leaf frame,
    // so we use the address of the register given to us instead of the register value in the context.
    if (regAddr != NULL)
    {
        *((SIZE_T*)regAddr) = newValue;
    }
    else
    {
        switch (reg)
        {
        case REGISTER_STACK_POINTER:
            SetSP(&pDE->m_context, (LPVOID)(size_t)newValue);
            break;

        case REGISTER_FRAME_POINTER:
            SetFP(&pDE->m_context, (LPVOID)(size_t)newValue);
            break;

#ifdef _X86_
        case REGISTER_X86_EAX:
            pDE->m_context.Eax = newValue;
            break;

        case REGISTER_X86_ECX:
            pDE->m_context.Ecx = newValue;
            break;

        case REGISTER_X86_EDX:
            pDE->m_context.Edx = newValue;
            break;

        case REGISTER_X86_EBX:
            pDE->m_context.Ebx = newValue;
            break;

        case REGISTER_X86_ESI:
            pDE->m_context.Esi = newValue;
            break;

        case REGISTER_X86_EDI:
            pDE->m_context.Edi = newValue;
            break;

#endif // !_X86_ && !_AMD64_
        default:
            _ASSERT(!"Invalid register number!");

        }

    }
}


/*
 * GetRegsiterValueAndReturnAddress
 *
 * This routine sucks out a value from a register, or set of registers, into one of 
 * the given buffers (depending on size), and returns a pointer to the filled in
 * buffer, or NULL on error.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    pFEAD - Information about this particular argument.
 *    pInt64Buf - pointer to a buffer of type INT64
 *    pSizeTBuf - pointer to a buffer of native size type.
 *
 * Returns:
 *    pointer to the filled in buffer, else NULL on error.
 *
 */
static PVOID GetRegisterValueAndReturnAddress(DebuggerEval *pDE,
                                              DebuggerIPCE_FuncEvalArgData *pFEAD,
                                              INT64 *pInt64Buf,
                                              SIZE_T *pSizeTBuf
                                              )
{
    LEAF_CONTRACT;

    PVOID pAddr;

    pAddr = pInt64Buf;
    DWORD *pLow = (DWORD*)(pInt64Buf);
    DWORD *pHigh  = pLow + 1;

    switch (pFEAD->argHome.kind)
    {
    case RAK_REGREG:
        *pLow = GetRegisterValue(pDE, pFEAD->argHome.u.reg2, pFEAD->argHome.u.reg2Addr);
        *pHigh = GetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr);
        break;

    case RAK_MEMREG:
        *pLow = GetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr);
        *pHigh = *((DWORD*)CORDB_ADDRESS_TO_PTR(pFEAD->argHome.addr));
        break;

    case RAK_REGMEM:
        *pLow = *((DWORD*)CORDB_ADDRESS_TO_PTR(pFEAD->argHome.addr));
        *pHigh = GetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr);
        break;

    case RAK_REG:
        // Simply grab the value out of the proper register.
        *pSizeTBuf = GetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr);
        pAddr = pSizeTBuf;
        break;

    default:
        pAddr = NULL;
        break;
    }

    return pAddr;
}


#ifdef _DEBUG

//
// Create a parallel array that tracks that we have initialized information in
// each array.
//
#define MAX_DATA_LOCATIONS_TRACKED 100

typedef DWORD DataLocation;

#define DL_NonExistent           0x00
#define DL_ObjectRefArray        0x01
#define DL_MaybeInteriorPtrArray 0x02
#define DL_BufferForArgsArray    0x04
#define DL_All                   0xFF

#endif // _DEBUG


/*
 * GetFuncEvalArgValue
 *
 * This routine is used to fill the pArgument array with the appropriate value.  This function
 * uses the three parallel array entries given, and places the correct value, or reference to
 * the value in pArgument.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    pFEAD - Information about this particular argument.
 *    isByRef - Is the argument being passed ByRef.
 *    fNeedBoxOrUnbox - Did the argument need boxing or unboxing.
 *    argTH - The type handle for the argument.
 *    byrefArgSigType - The signature type of a parameter that isByRef == true.
 *    pArgument - Location to place the reference or value.
 *    pMaybeInteriorPtrArg - A pointer that contains a value that may be pointers to
 *         the interior of a managed object.
 *    pObjectRefArg - A pointer that contains an object ref.  It was built previously.
 *    pBufferArg - A pointer for holding stuff that did not need to be protected.
 *
 * Returns:
 *    None.
 *
 */
static void GetFuncEvalArgValue(DebuggerEval *pDE,
                                DebuggerIPCE_FuncEvalArgData *pFEAD,
                                bool isByRef,
                                bool fNeedBoxOrUnbox,
                                TypeHandle argTH,
                                CorElementType byrefArgSigType,
                                TypeHandle byrefArgTH,
                                ARG_SLOT *pArgument,
                                void *pMaybeInteriorPtrArg,
                                OBJECTREF *pObjectRefArg,
                                INT64 *pBufferArg,
                                CorElementType argSigType
                                DEBUG_ARG(DataLocation dataLocation)
                               )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE((dataLocation != DL_NonExistent) ||
             (pFEAD->argElementType == ELEMENT_TYPE_VALUETYPE));

    switch (pFEAD->argElementType)
    {
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:
        {
            INT64 *pSource;

            _ASSERTE(dataLocation & DL_BufferForArgsArray);

            pSource = pBufferArg;

            if (!isByRef)
            {
                *((INT64*)pArgument) = *pSource;
            }
            else
            {
                *pArgument = PtrToArgSlot(pSource);
            }
        }
        break;

    case ELEMENT_TYPE_VALUETYPE:
        {
            SIZE_T v;
            LPVOID pAddr = NULL;
            INT64 bigVal = 0;

            if (pFEAD->argAddr != NULL)
            {
                pAddr = *((void **)pMaybeInteriorPtrArg);
            }
            else
            {
                pAddr = GetRegisterValueAndReturnAddress(pDE, pFEAD, &bigVal, &v);

                if (pAddr == NULL)
                {
                    COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");
                }
            }


            _ASSERTE(pAddr);

            if (!fNeedBoxOrUnbox && !isByRef)
            {
                _ASSERTE(argTH.GetMethodTable());

                unsigned size = argTH.GetMethodTable()->GetNumInstanceFieldBytes();
                if (size <= sizeof(ARG_SLOT)
                   )
                {
                    CopyValueClassUnchecked(ArgSlotEndianessFixup(pArgument, sizeof(LPVOID)), pAddr, argTH.GetMethodTable());
                }
                else
                {
                    // No need to treat ENREGISTERED_PARAMTYPE_MAXSIZE differently.  MethodDesc::CallDescr() will take
                    // care of it.
                    _ASSERTE(pFEAD->argAddr != NULL);
                    *pArgument = PtrToArgSlot(pAddr);
                }
            }
            else
            {
                if (fNeedBoxOrUnbox)
                {
                    *pArgument = ObjToArgSlot(*pObjectRefArg);
                }
                else
                {
                    if (pFEAD->argAddr)
                    {
                        *pArgument = PtrToArgSlot(pAddr);
                    }
                    else
                    {
                        // The argument is the address of where we're holding the primitive in the PrimitiveArg array. We
                        // stick the real value from the register into the PrimitiveArg array.
                        *pArgument = PtrToArgSlot(pBufferArg);
                        *pBufferArg = (INT64)v;
                    }
                }
            }
        }
        break;

    default:
        // literal values smaller than 8 bytes and "special types" (e.g. object, string, etc.)

        {
            INT64 *pSource;

            INDEBUG(DataLocation expectedLocation);

#ifdef _X86_
            if ((pFEAD->argElementType == ELEMENT_TYPE_I4) ||
                (pFEAD->argElementType == ELEMENT_TYPE_U4) ||
                (pFEAD->argElementType == ELEMENT_TYPE_R4))
            {
                INDEBUG(expectedLocation = DL_MaybeInteriorPtrArray);

                pSource = (INT64 *)pMaybeInteriorPtrArg;
            }
            else
#endif
            if (IsElementTypeSpecial(pFEAD->argElementType))
            {
                INDEBUG(expectedLocation = DL_ObjectRefArray);

                pSource = (INT64 *)pObjectRefArg;
            }
            else
            {
                INDEBUG(expectedLocation = DL_BufferForArgsArray);

                pSource = pBufferArg;
            }

            if (pFEAD->argAddr != NULL)
            {
                if (!isByRef)
                {
                    if (pFEAD->argIsHandleValue)
                    {
                        _ASSERTE(dataLocation & DL_BufferForArgsArray);

                        OBJECTHANDLE oh = *((OBJECTHANDLE*)(pBufferArg));  // Always comes from buffer
                        *pArgument = PtrToArgSlot(g_pEEInterface->GetObjectFromHandle(oh));
                    }
                    else
                    {
                        _ASSERTE(dataLocation & expectedLocation);

                        if (pSource != NULL)
                        {
                            *pArgument = *pSource; // may come from either array.
                        }
                        else
                        {
                            *pArgument = NULL;
                        }
                    }
                }
                else
                {
                    if (pFEAD->argIsHandleValue)
                    {
                        _ASSERTE(dataLocation & DL_BufferForArgsArray);

                        *pArgument = *pBufferArg; // Buffer contains the object handle, in this case, so
                                                  // just copy that across.
                    }
                    else
                    {
                        _ASSERTE(dataLocation & expectedLocation);

                        *pArgument = PtrToArgSlot(pSource); // Load the argument with the address of our buffer.
                    }
                }
            }
            else if (pFEAD->argIsLiteral)
            {
                _ASSERTE(dataLocation & expectedLocation);

                if (!isByRef)
                {
                    if (pSource != NULL)
                    {
                        *pArgument = *pSource; // may come from either array.
                    }
                    else
                    {
                        *pArgument = NULL;
                    }
                }
                else
                {
                    *pArgument = PtrToArgSlot(pSource); // Load the argument with the address of our buffer.
                }
            }
            else
            {
                if (!isByRef)
                {
                    if (pSource != NULL)
                    {
                        *pArgument = *pSource; // may come from either array.
                    }
                    else
                    {
                        *pArgument = NULL;
                    }
                }
                else
                {
                    *pArgument = PtrToArgSlot(pSource); // Load the argument with the address of our buffer.
                }
            }

            // If we need to unbox, then unbox the arg now.
            if (fNeedBoxOrUnbox)
            {
                if (!isByRef)
                {
                    // function expects valuetype, argument received is class or object

                    // Take the ObjectRef off the stack.
                    ARG_SLOT oi1 = *pArgument;
                    OBJECTREF o1 = ArgSlotToObj(oi1);

                    // For Nullable types, we need a 'true' nullable to pass to the function, and we do this
                    // by passing a boxed nullable that we unbox.  We allocated this space earlier however we
                    // did not know the data location until just now.  Fill it in with the data and use that
                    // to pass to the function.

                    if (Nullable::IsNullableType(argTH)) 
                    {
                         _ASSERTE(*pObjectRefArg != 0);
						 _ASSERTE((*pObjectRefArg)->GetMethodTable() == argTH.GetMethodTable());
                        if (o1 != *pObjectRefArg) 
                        {
                            BOOL typesMatch = Nullable::UnBox((*pObjectRefArg)->GetData(), o1, (*pObjectRefArg)->GetMethodTable());
                            _ASSERTE(typesMatch);
                            o1 = *pObjectRefArg;
                        }
                    }

                    if (o1 == NULL)
                    {
                        COMPlusThrow(kArgumentException, L"ArgumentNull_Obj");
                    }


                    if (!o1->GetMethodTable()->IsValueClass())
                    {
                        COMPlusThrow(kArgumentException, L"Argument_BadObjRef");
                    }


                    // Unbox the little fella to get a pointer to the raw data.
                    void *pData = o1->GetData();

                    // Get its size to make sure it fits in an ARG_SLOT
                    unsigned size = o1->GetMethodTable()->GetNumInstanceFieldBytes();

                    if (size <= sizeof(ARG_SLOT))
                    {
                        // Its not ByRef, so we need to copy the value class onto the ARG_SLOT.
                        CopyValueClassUnchecked(ArgSlotEndianessFixup(pArgument, sizeof(LPVOID)), pData, o1->GetMethodTable());
                    }
                    else
                    {
                        // Store pointer to the space in the ARG_SLOT
                        *pArgument = PtrToArgSlot(pData);
                    }
                }
                else
                {
                    // Function expects byref valuetype, argument received is byref class.

                    // Grab the ObjectRef off the stack via the pointer on the stack. Note: the stack has a pointer to the
                    // ObjectRef since the arg was specified as byref.
                    OBJECTREF* op1 = (OBJECTREF*)ArgSlotToPtr(*pArgument);
                    if (op1 == NULL)
                    {
                        COMPlusThrow(kArgumentException, L"ArgumentNull_Obj");
                    }
                    OBJECTREF o1 = *op1;

                    // For Nullable types, we need a 'true' nullable to pass to the function, and we do this
                    // by passing a boxed nullable that we unbox.  We allocated this space earlier however we
                    // did not know the data location until just now.  Fill it in with the data and use that
                    // to pass to the function.

                    if (Nullable::IsNullableType(byrefArgTH)) 
                    {
                         _ASSERTE(*pObjectRefArg != 0 && (*pObjectRefArg)->GetMethodTable() == byrefArgTH.GetMethodTable());
                        if (o1 != *pObjectRefArg) 
                        {
                            BOOL typesMatch = Nullable::UnBox((*pObjectRefArg)->GetData(), o1, (*pObjectRefArg)->GetMethodTable());
                            _ASSERTE(typesMatch);
                            o1 = *pObjectRefArg;
                        }
                    }

                    if (o1 == NULL)
                    {
                        COMPlusThrow(kArgumentException, L"ArgumentNull_Obj");
                    }

                    _ASSERTE(o1->GetMethodTable()->IsValueClass());

                    // Unbox the little fella to get a pointer to the raw data.
                    void *pData = o1->GetData();

                    // If it is ByRef, then we just replace the ObjectRef with a pointer to the data.
                    *pArgument = PtrToArgSlot(pData);
                }
            }

            // Validate any objectrefs that are supposed to be on the stack.
            if (!fNeedBoxOrUnbox)
            {
                Object *objPtr;
                if (!isByRef)
                {
                    if (IsElementTypeSpecial(argSigType))
                    {
                        // validate the integrity of the object
                        objPtr = (Object*)ArgSlotToPtr(*pArgument);
                        if (FAILED(ValidateObject(objPtr)))
                        {
                            COMPlusThrow(kArgumentException, L"Argument_BadObjRef");
                        }
                    }
                }
                else
                {
                    _ASSERTE(argSigType == ELEMENT_TYPE_BYREF);
                    if (IsElementTypeSpecial(byrefArgSigType))
                    {
                        objPtr = *(Object**)(ArgSlotToPtr(*pArgument));
                        if (FAILED(ValidateObject(objPtr)))
                        {
                            COMPlusThrow(kArgumentException, L"Argument_BadObjRef");
                        }
                    }
                }
            }
        }
    }
}

static CorDebugRegister GetArgAddrFromReg( DebuggerIPCE_FuncEvalArgData *pFEAD)
{
    CorDebugRegister retval = REGISTER_INSTRUCTION_POINTER; // good as default as any
    retval = pFEAD->argHome.reg1;
    return retval;
}

// 
// Given info about a byref argument, retrieve the current value from the pBufferForArgsArray, 
// the pMaybeInteriorPtrArray, the pByRefMaybeInteriorPtrArray, or the pObjectRefArray.  Then 
// place it back into the proper register or address.
//
// Note that we should never use the argAddr of the DebuggerIPCE_FuncEvalArgData in this function
// since the address may be an interior GC pointer and may have been moved by the GC.  Instead,
// use the pByRefMaybeInteriorPtrArray.
//
static void SetFuncEvalByRefArgValue(DebuggerEval *pDE,
                                     DebuggerIPCE_FuncEvalArgData *pFEAD,
                                     CorElementType byrefArgSigType,
                                     INT64 bufferByRefArg,
                                     void *maybeInteriorPtrArg,
                                     void *byRefMaybeInteriorPtrArg,
                                     OBJECTREF objectRefByRefArg)
{
    WRAPPER_CONTRACT;

    switch (pFEAD->argElementType)
    {
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:
        // 64bit values
        {
            INT64 source;

            source = bufferByRefArg;

            if (pFEAD->argIsLiteral)
            {
                // If this was a literal arg, then copy the updated primitive back into the literal.
                memcpy(pFEAD->argLiteralData, &source, sizeof(pFEAD->argLiteralData));
            }
            else if (pFEAD->argAddr != NULL)
            {
                *((INT64 *)byRefMaybeInteriorPtrArg) = source;
                return;
            }
            else
            {
                // RAK_REG is the only 4 byte type, all others are 8 byte types.
                _ASSERTE(pFEAD->argHome.kind != RAK_REG);

                SIZE_T *pLow = (SIZE_T*)(&source);
                SIZE_T *pHigh  = pLow + 1;

                switch (pFEAD->argHome.kind)
                {
                case RAK_REGREG:
                    SetRegisterValue(pDE, pFEAD->argHome.u.reg2, pFEAD->argHome.u.reg2Addr, *pLow);
                    SetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr, *pHigh);
                    break;

                case RAK_MEMREG:
                    SetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr, *pLow);
                    *((SIZE_T*)CORDB_ADDRESS_TO_PTR(pFEAD->argHome.addr)) = *pHigh;
                    break;

                case RAK_REGMEM:
                    *((SIZE_T*)CORDB_ADDRESS_TO_PTR(pFEAD->argHome.addr)) = *pLow;
                    SetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr, *pHigh);
                    break;

                default:
                    break;
                }
            }
        }
        break;

    default:
        // literal values smaller than 8 bytes and "special types" (e.g. object, array, string, etc.)
        {
            SIZE_T source;

#ifdef _X86_
            if ((pFEAD->argElementType == ELEMENT_TYPE_I4) ||
                (pFEAD->argElementType == ELEMENT_TYPE_U4) ||
                (pFEAD->argElementType == ELEMENT_TYPE_R4))
            {
                source = (SIZE_T)maybeInteriorPtrArg;
            }
            else
            {
#endif
                source = (SIZE_T)bufferByRefArg;
#ifdef _X86_
            }
#endif

            if (pFEAD->argIsLiteral)
            {
                // If this was a literal arg, then copy the updated primitive back into the literal.
                // The literall buffer is a fixed size (8 bytes), but our source may be 4 or 8 bytes
                // depending on the platform.  To prevent reading past the end of the source, we
                // zero the destination buffer and copy only as many bytes as available.
                memset( pFEAD->argLiteralData, 0, sizeof(pFEAD->argLiteralData) );
                if (IsElementTypeSpecial(pFEAD->argElementType))
                {
                    _ASSERTE( sizeof(pFEAD->argLiteralData) >= sizeof(objectRefByRefArg) );
                    memcpy(pFEAD->argLiteralData, &objectRefByRefArg, sizeof(objectRefByRefArg));
                }
                else
                {
                    _ASSERTE( sizeof(pFEAD->argLiteralData) >= sizeof(source) );
                    memcpy(pFEAD->argLiteralData, &source, sizeof(source));
                }
            }
            else if (pFEAD->argAddr == NULL)
            {
                // If the 32bit value is enregistered, copy it back to the proper regs.

                // RAK_REG is the only valid 4 byte type on WIN32.  On WIN64, both RAK_REG and RAK_FLOAT can be
                // 4 bytes or 8 bytes.
                _ASSERTE((pFEAD->argHome.kind == RAK_REG)
                         WIN64_ONLY(|| (pFEAD->argHome.kind == RAK_FLOAT)));

                CorDebugRegister regNum = GetArgAddrFromReg(pFEAD);

                // Shove the result back into the proper register.
                if (IsElementTypeSpecial(pFEAD->argElementType))
                {
                    SetRegisterValue(pDE, regNum, pFEAD->argHome.reg1Addr, (SIZE_T)ObjToArgSlot(objectRefByRefArg));
                }
                else
                {
                    SetRegisterValue(pDE, regNum, pFEAD->argHome.reg1Addr, (SIZE_T)source);
                }
            }
            else
            {
                // If the result was an object by ref, then copy back the new location of the object (in GC case).
                if (pFEAD->argIsHandleValue) 
                {
                    // do nothing.  The Handle was passed in the pArgument array directly
                }
                else if (IsElementTypeSpecial(pFEAD->argElementType))
                {
                    *((SIZE_T*)byRefMaybeInteriorPtrArg) = (SIZE_T)ObjToArgSlot(objectRefByRefArg);
                }
                else if (pFEAD->argElementType == ELEMENT_TYPE_VALUETYPE)
                {
                    // Do nothing, we passed in the pointer to the valuetype in the pArgument array directly.
                }
                else
                {
                    GetAndSetLiteralValue(byRefMaybeInteriorPtrArg, pFEAD->argElementType, &source, ELEMENT_TYPE_PTR);
                }
            }
        } // end default
    } // end switch
}


/*
 * GCProtectAllPassedArgs
 *
 * This routine is the first step in doing a func-eval.  For a complete overview, see
 * the comments at the top of this file.
 *
 * This routine over-aggressively protects all arguments that may be references to
 * managed objects.  This function cannot crawl the function signature, since doing
 * so may trigger a GC, and thus, we must assume everything is ByRef.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    pObjectRefArray - An array that contains any object refs.  It was built previously.
 *    pMaybeInteriorPtrArray - An array that contains values that may be pointers to
 *         the interior of a managed object.
 *    pBufferForArgsArray - An array for holding stuff that does not need to be protected.
 *         Any handle for the 'this' pointer is put in here for pulling it out later.
 *
 * Returns:
 *    None.
 *
 */
static void GCProtectAllPassedArgs(DebuggerEval *pDE,
                                   OBJECTREF *pObjectRefArray,
                                   void **pMaybeInteriorPtrArray,
                                   void **pByRefMaybeInteriorPtrArray,
                                   INT64 *pBufferForArgsArray
                                   DEBUG_ARG(DataLocation pDataLocationArray[])
                                  )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    DebuggerIPCE_FuncEvalArgData *argData = pDE->GetArgData();

    unsigned currArgIndex = 0;

    //
    // Gather all the information for the parameters.
    //
    for ( ; currArgIndex < pDE->m_argCount; currArgIndex++)
    {
        DebuggerIPCE_FuncEvalArgData *pFEAD = &argData[currArgIndex];

        // In case any of the arguments is a by ref argument and points into the GC heap, 
        // we need to GC protect their addresses as well.
        if (pFEAD->argAddr != NULL)
        {
            pByRefMaybeInteriorPtrArray[currArgIndex] = pFEAD->argAddr;
        }

        switch (pFEAD->argElementType)
        {
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R8:
            // 64bit values

            break;

        case ELEMENT_TYPE_VALUETYPE:
            //
            // If the value type address could be an interior pointer.
            //
            if (pFEAD->argAddr != NULL)
            {
                pMaybeInteriorPtrArray[currArgIndex] = ((void **)(pFEAD->argAddr));
            }

            INDEBUG(pDataLocationArray[currArgIndex] |= DL_MaybeInteriorPtrArray);
            break;

        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:

            if (pFEAD->argAddr != NULL)
            {
                if (pFEAD->argIsHandleValue)
                {
                    OBJECTHANDLE oh = (OBJECTHANDLE)(pFEAD->argAddr);
                    pBufferForArgsArray[currArgIndex] = (INT64)(size_t)oh;

                    INDEBUG(pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray);
                }
                else
                {
                    pObjectRefArray[currArgIndex] = *((OBJECTREF *)(pFEAD->argAddr));

                    INDEBUG(pDataLocationArray[currArgIndex] |= DL_ObjectRefArray);
                }
            }
            else if (pFEAD->argIsLiteral)
            {
                _ASSERTE(sizeof(pFEAD->argLiteralData) >= sizeof(OBJECTREF));
                OBJECTREF v = NULL;
                memcpy(&v, pFEAD->argLiteralData, sizeof(v));
                pObjectRefArray[currArgIndex] = v;
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_ObjectRefArray;
                }
#endif
            }
            else
            {
                // RAK_REG is the only valid pointer-sized type.
                _ASSERTE(pFEAD->argHome.kind == RAK_REG);

                // Simply grab the value out of the proper register.
                SIZE_T v = GetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr);

                // The argument is the address.
                pObjectRefArray[currArgIndex] = (OBJECTREF)v;
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_ObjectRefArray;
                }
#endif
            }
            break;

        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_R4:
            // 32bit values

#ifdef _X86_
            _ASSERTE(sizeof(void *) == sizeof(INT32));

            if (pFEAD->argAddr != NULL)
            {
                if (pFEAD->argIsHandleValue)
                {
                    //
                    // Ignorable - no need to protect
                    //
                }
                else
                {
                    pMaybeInteriorPtrArray[currArgIndex] = *((void **)(pFEAD->argAddr));
#ifdef _DEBUG
                    if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                    {
                        pDataLocationArray[currArgIndex] |= DL_MaybeInteriorPtrArray;
                    }
#endif
                }
            }
            else if (pFEAD->argIsLiteral)
            {
                _ASSERTE(sizeof(pFEAD->argLiteralData) >= sizeof(INT32));

                //
                // If this is a byref literal arg, then it maybe an interior ptr.
                //
                void *v = NULL;
                memcpy(&v, pFEAD->argLiteralData, sizeof(v));
                pMaybeInteriorPtrArray[currArgIndex] = v;
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_MaybeInteriorPtrArray;
                }
#endif
            }
            else
            {
                // RAK_REG is the only valid 4 byte type on WIN32.
                _ASSERTE(pFEAD->argHome.kind == RAK_REG);

                // Simply grab the value out of the proper register.
                SIZE_T v = GetRegisterValue(pDE, pFEAD->argHome.reg1, pFEAD->argHome.reg1Addr);

                // The argument is the address.
                pMaybeInteriorPtrArray[currArgIndex] = (void *)v;
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_MaybeInteriorPtrArray;
                }
#endif
            }
#endif // _X86_

        default:
            //
            // Ignorable - no need to protect
            //
            break;
        }
    }
}

/*
 * ResolveFuncEvalGenericArgInfo
 *
 * This function pulls out any generic args and makes sure the method is loaded for it.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *
 * Returns:
 *    None.
 *
 */
void ResolveFuncEvalGenericArgInfo(DebuggerEval *pDE)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    DebuggerIPCE_TypeArgData *firstdata = pDE->GetTypeArgData();
    DWORD nGenericArgs = pDE->m_genericArgsCount;
    TypeHandle *pGenericArgs = (nGenericArgs == 0) ? NULL : (TypeHandle *) _alloca(nGenericArgs * sizeof (TypeHandle *));

    //
    // Snag the type arguments from the input and get the
    // method desc that corresponds to the instantiated desc.
    //
    Debugger::TypeDataWalk walk(firstdata, pDE->m_genericArgsNodeCount);
    walk.ReadTypeHandles(nGenericArgs, pGenericArgs);

    if (!walk.Finished())
    {
        COMPlusThrow(kArgumentException, L"Argument_InvalidGenericArg");
    }

    // Find the proper MethodDesc that we need to call.
    // Since we're already in the target domain, it can't be unloaded so it's safe to 
    // use domain specific structures like the Module*.
    _ASSERTE( GetAppDomain() == pDE->m_debuggerModule->GetAppDomain() );
    pDE->m_md = g_pEEInterface->LoadMethodDef(pDE->m_debuggerModule->GetRuntimeModule(),
                                              pDE->m_methodToken,
                                              nGenericArgs,
                                              pGenericArgs,
                                              &(pDE->m_ownerTypeHandle));


    // We better have a MethodDesc at this point.
    _ASSERTE(pDE->m_md != NULL);

    IMDInternalImport *pInternalImport = pDE->m_md->GetMDImport();
    DWORD dwAttr = pInternalImport->GetMethodDefProps(pDE->m_methodToken);

    if (dwAttr & mdRequireSecObject)
    {
        // command window cannot evaluate a function with mdRequireSecObject is turned on because
        // this is expecting to put a security object into caller's frame which we don't have.
        //
        COMPlusThrow(kArgumentException,L"Argument_CantCallSecObjFunc");
    }

    ValidateFuncEvalReturnType(pDE->m_evalType , pDE->m_md->GetMethodTable());

    // If this is a new object operation, then we should have a .ctor.
    if ((pDE->m_evalType == DB_IPCE_FET_NEW_OBJECT) && !pDE->m_md->IsCtor())
    {
        COMPlusThrow(kArgumentException, L"Argument_MissingDefaultConstructor");
    }

    // Run the Class Init for this class, if necessary.
    pDE->m_md->EnsureActive();
    pDE->m_md->GetMethodTable()->CheckRunClassInitThrowing();

    if (pDE->m_evalType == DB_IPCE_FET_NEW_OBJECT)
    {
        // Work out the exact type of the allocated object
        pDE->m_resultType = (nGenericArgs == 0)
                           ? TypeHandle(pDE->m_md->GetMethodTable())
                           : g_pEEInterface->LoadInstantiation(pDE->m_md->GetModule(), pDE->m_md->GetMethodTable()->GetCl(), nGenericArgs, pGenericArgs);
    }
}


/*
 * BoxFuncEvalThisParameter
 *
 * This function is a helper for DoNormalFuncEval.  It boxes the 'this' parameter if necessary.
 * For example, when  a method Object.ToString is called on a value class like System.DateTime 
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    argData - Array of information about the arguments.
 *    pMaybeInteriorPtrArray - An array that contains values that may be pointers to
 *         the interior of a managed object.
 *    pObjectRef - A GC protected place to put a boxed value, if necessary.
 *
 * Returns:
 *    None
 *
 */
void BoxFuncEvalThisParameter(DebuggerEval *pDE,
                           DebuggerIPCE_FuncEvalArgData *argData,
                           void **pMaybeInteriorPtrArray,
                           OBJECTREF *pObjectRefArg          // out
                           DEBUG_ARG(DataLocation pDataLocationArray[])
                          )
{
    WRAPPER_CONTRACT;

    //
    // See if we have a value type that is going to be passed as a 'this' pointer.
    //
    if ((pDE->m_evalType != DB_IPCE_FET_NEW_OBJECT) &&
        !pDE->m_md->IsStatic() &&
        (pDE->m_argCount > 0))
    {
        // Allocate the space for box nullables.  Nullable parameters need a unboxed 
        // nullable value to point at, where our current representation does not have
        // an unboxed value inside them. Thus we need another buffer to hold it (and
        // gcprotects it.  We used boxed values for this by converting them to 'true'
        // nullable form, calling the function, and in the case of byrefs, converting
        // them back afterward. 

        MethodTable* pMT = pDE->m_md->GetMethodTable();
        if (Nullable::IsNullableType(pMT)) 
        {
            OBJECTREF obj = AllocateObject(pMT);
            if (*pObjectRefArg != NULL) 
            {
                BOOL typesMatch = Nullable::UnBox(obj->GetData(), *pObjectRefArg, pMT);
                _ASSERTE(typesMatch);
            }
            *pObjectRefArg = obj;
        }

        if (argData[0].argElementType == ELEMENT_TYPE_VALUETYPE)
        {
            //
            // See if we need to box up the 'this' parameter.
            //
            if (!pDE->m_md->GetMethodTable()->IsValueClass())
            {
                DebuggerIPCE_FuncEvalArgData *pFEAD = &argData[0];
                SIZE_T v;
                LPVOID pAddr = NULL;
                INT64 bigVal;

                {
                    GCX_FORBID();    //pAddr is unprotected from the time we initialize it
                    
                    if (pFEAD->argAddr != NULL)
                    {
                        _ASSERTE(pDataLocationArray[0] & DL_MaybeInteriorPtrArray);
                        pAddr = pMaybeInteriorPtrArray[0];
                        INDEBUG(pDataLocationArray[0] &= ~DL_MaybeInteriorPtrArray);
                    }
                    else
                    {

                        pAddr = GetRegisterValueAndReturnAddress(pDE, pFEAD, &bigVal, &v);

                        if (pAddr == NULL)
                        {
                            COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");
                        }
                    }

                    _ASSERTE(pAddr != NULL);
                } //GCX_FORBID
                
                GCPROTECT_BEGININTERIOR(pAddr); //ReadTypeHandle may trigger a GC and move the object that has the value type at pAddr as a field

                //
                // Grab the class of this value type.  If the type is a parameterized
                // struct type then it may not have yet been loaded by the EE (generics
                // code sharing may have meant we have never bothered to create the exact
                // type yet).
                //
                // A buffer should have been allocated for the full struct type
                _ASSERTE(argData[0].fullArgType != NULL);
                Debugger::TypeDataWalk walk((DebuggerIPCE_TypeArgData *) argData[0].fullArgType, argData[0].fullArgTypeNodeCount);

                TypeHandle typeHandle = walk.ReadTypeHandle();

                if (typeHandle.IsNull())
                {
                    COMPlusThrow(kArgumentException, L"Argument_BadObjRef");
                }

                //
                // Box up this value type
                //
                *pObjectRefArg = typeHandle.GetMethodTable()->Box(pAddr, TRUE);
                GCPROTECT_END();

                INDEBUG(pDataLocationArray[0] |= DL_ObjectRefArray);
            }
        }
    }
}


//
// This is used to store (temporarily) information about the arguments that func-eval
// will pass.  It is used only for the args of the function, not the return buffer nor
// the 'this' pointer, if there is any of either.
//
struct FuncEvalArgInfo
{
    CorElementType argSigType;
    CorElementType byrefArgSigType;
    TypeHandle     byrefArgTypeHandle;
    bool fNeedBoxOrUnbox;
    TypeHandle sigTypeHandle;
};



/*
 * GatherFuncEvalArgInfo
 *
 * This function is a helper for DoNormalFuncEval.  It gathers together all the information
 * necessary to process the arguments.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    mSig - The metadata signature of the fuction to call.
 *    argData - Array of information about the arguments.
 *    pFEArgInfo - An array of structs to hold the argument information.
 *
 * Returns:
 *    None.
 *
 */
void GatherFuncEvalArgInfo(DebuggerEval *pDE,
                           MetaSig mSig,
                           DebuggerIPCE_FuncEvalArgData *argData,
                           FuncEvalArgInfo *pFEArgInfo    // out
                          )
{
    WRAPPER_CONTRACT;

    unsigned currArgIndex = 0;

    if ((pDE->m_evalType == DB_IPCE_FET_NORMAL) && !pDE->m_md->IsStatic())
    {
        //
        // Skip over the 'this' arg, since this function is not supposed to mess with it.
        //
        currArgIndex++;
    }

    //
    // Gather all the information for the parameters.
    //
    for ( ; currArgIndex < pDE->m_argCount; currArgIndex++)
    {
        DebuggerIPCE_FuncEvalArgData *pFEAD = &argData[currArgIndex];

        //
        // Move to the next arg in the signature.
        //
        CorElementType argSigType = mSig.NextArgNormalized();
        _ASSERTE(argSigType != ELEMENT_TYPE_END);

        //
        // If this arg is a byref arg, then we'll need to know what type we're referencing for later...
        //
        TypeHandle byrefTypeHandle = TypeHandle();
        CorElementType byrefArgSigType = ELEMENT_TYPE_END;
        if (argSigType == ELEMENT_TYPE_BYREF)
        {
            byrefArgSigType = mSig.GetByRefType(&byrefTypeHandle);
        }

        //
        // If the sig says class but we've got a value class parameter, then remember that we need to box it.  If
        // the sig says value class, but we've got a boxed value class, then remember that we need to unbox it.
        //
        bool fNeedBoxOrUnbox = ((argSigType == ELEMENT_TYPE_CLASS) && (pFEAD->argElementType == ELEMENT_TYPE_VALUETYPE)) ||
            ((argSigType == ELEMENT_TYPE_VALUETYPE) && ((pFEAD->argElementType == ELEMENT_TYPE_CLASS) || (pFEAD->argElementType == ELEMENT_TYPE_OBJECT)) ||
            // This is when method signature is expecting a BYREF ValueType, yet we recieve the boxed valuetype's handle.
            (pFEAD->argElementType == ELEMENT_TYPE_CLASS && argSigType == ELEMENT_TYPE_BYREF && byrefArgSigType == ELEMENT_TYPE_VALUETYPE));

        pFEArgInfo[currArgIndex].argSigType = argSigType;
        pFEArgInfo[currArgIndex].byrefArgSigType = byrefArgSigType;
        pFEArgInfo[currArgIndex].byrefArgTypeHandle = byrefTypeHandle;
        pFEArgInfo[currArgIndex].fNeedBoxOrUnbox = fNeedBoxOrUnbox; 
        pFEArgInfo[currArgIndex].sigTypeHandle = mSig.GetLastTypeHandleThrowing(); 
    } 
} 


/*
 * BoxFuncEvalArguments
 *
 * This function is a helper for DoNormalFuncEval.  It boxes all the arguments that
 * need to be.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    argData - Array of information about the arguments.
 *    pFEArgInfo - An array of structs to hold the argument information.
 *    pMaybeInteriorPtrArray - An array that contains values that may be pointers to
 *         the interior of a managed object.
 *    pObjectRef - A GC protected place to put a boxed value, if necessary.
 *
 * Returns:
 *    None
 *
 */
void BoxFuncEvalArguments(DebuggerEval *pDE,
                          DebuggerIPCE_FuncEvalArgData *argData,
                          FuncEvalArgInfo *pFEArgInfo,
                          void **pMaybeInteriorPtrArray,
                          OBJECTREF *pObjectRef          // out
                          DEBUG_ARG(DataLocation pDataLocationArray[])
                         )
{
    WRAPPER_CONTRACT;

    unsigned currArgIndex = 0;


    if ((pDE->m_evalType == DB_IPCE_FET_NORMAL) && !pDE->m_md->IsStatic())
    {
        //
        // Skip over the 'this' arg, since this function is not supposed to mess with it.
        //
        currArgIndex++;
    }

    //
    // Gather all the information for the parameters.
    //
    for ( ; currArgIndex < pDE->m_argCount; currArgIndex++)
    {
        DebuggerIPCE_FuncEvalArgData *pFEAD = &argData[currArgIndex];

        // Allocate the space for box nullables.  Nullable parameters need a unboxed 
        // nullable value to point at, where our current representation does not have
        // an unboxed value inside them. Thus we need another buffer to hold it (and
        // gcprotects it.  We used boxed values for this by converting them to 'true'
        // nullable form, calling the function, and in the case of byrefs, converting
        // them back afterward. 

        TypeHandle th = pFEArgInfo[currArgIndex].sigTypeHandle;
        if (pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_BYREF)
            th = pFEArgInfo[currArgIndex].byrefArgTypeHandle;

        if (!th.IsNull() && Nullable::IsNullableType(th)) 
        {

            OBJECTREF obj = AllocateObject(th.AsMethodTable());
            if (pObjectRef[currArgIndex] != NULL) 
            {
                BOOL typesMatch = Nullable::UnBox(obj->GetData(), pObjectRef[currArgIndex], th.AsMethodTable());
                _ASSERTE(typesMatch);
            }
            pObjectRef[currArgIndex] = obj;
        }

        //
        // Check if we should box this value now
        //
        if ((pFEAD->argElementType == ELEMENT_TYPE_VALUETYPE) &&
            (pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_BYREF) &&
            pFEArgInfo[currArgIndex].fNeedBoxOrUnbox)
        {
            SIZE_T v;
            INT64 bigVal;
            LPVOID pAddr = NULL;

            if (pFEAD->argAddr != NULL)
            {
                _ASSERTE(pDataLocationArray[currArgIndex] & DL_MaybeInteriorPtrArray);
                pAddr = pMaybeInteriorPtrArray[currArgIndex];
                INDEBUG(pDataLocationArray[currArgIndex] &= ~DL_MaybeInteriorPtrArray);
            }
            else
            {

                pAddr = GetRegisterValueAndReturnAddress(pDE, pFEAD, &bigVal, &v);

                if (pAddr == NULL)
                {
                    COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");
                }
            }

            _ASSERTE(pAddr != NULL);

            MethodTable * pMT = pFEArgInfo[currArgIndex].sigTypeHandle.GetMethodTable();

            //
            // Stuff the newly boxed item into our GC-protected array.
            //
            pObjectRef[currArgIndex] = pMT->Box(pAddr, TRUE);

#ifdef _DEBUG
            if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
            {
                pDataLocationArray[currArgIndex] |= DL_ObjectRefArray;
            }
#endif
        }
    }
}


/*
 * GatherFuncEvalMethodInfo
 *
 * This function is a helper for DoNormalFuncEval.  It gathers together all the information
 * necessary to process the method
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    mSig - The metadata signature of the fuction to call.
 *    argData - Array of information about the arguments.
 *    ppUnboxedMD - Returns a resolve method desc if the original is an unboxing stub.
 *    pObjectRefArray - GC protected array of objects passed to this func-eval call.
 *         used to resolve down to the method target for generics.
 *    pBufferForArgsArray - Array of values not needing gc-protection.  May hold the
 *         handle for the method targer for generics.
 *    pfHasRetBuffArg - TRUE if the function has a return buffer.
 *    pRetValueType - The TypeHandle of the return value.
 *
 *
 * Returns:
 *    None.
 *
 */
void GatherFuncEvalMethodInfo(DebuggerEval *pDE,
                              MetaSig mSig,
                              DebuggerIPCE_FuncEvalArgData *argData,
                              MethodDesc **ppUnboxedMD,
                              OBJECTREF *pObjectRefArray,
                              INT64 *pBufferForArgsArray,
                              BOOL *pfHasRetBuffArg,          // out
                              BOOL *pfHasNonStdByValReturn,   // out
                              TypeHandle *pRetValueType       // out, only if fHasRetBuffArg == true
                              DEBUG_ARG(DataLocation pDataLocationArray[])
                             )
{
    WRAPPER_CONTRACT;

    //
    // If 'this' is a non-static function that points to an unboxing stub, we need to return the
    // unboxed method desc to really call.
    //
    if ((pDE->m_evalType != DB_IPCE_FET_NEW_OBJECT) && !pDE->m_md->IsStatic() && pDE->m_md->IsUnboxingStub())
    {
        *ppUnboxedMD = pDE->m_md->GetMethodTable()->GetUnboxedEntryPointMD(pDE->m_md);
    }

    //
    // Resolve down to the method on the class of the 'this' parameter.
    //
    if ((pDE->m_evalType != DB_IPCE_FET_NEW_OBJECT) &&
        (pDE->m_md->IsInterface() || pDE->m_md->IsVtableMethod()))
    {
        //
        // Assuming that a constructor can't be an interface method...
        //
        _ASSERTE(pDE->m_evalType == DB_IPCE_FET_NORMAL);

        //
        // We need to go grab the 'this' argument to figure out what class we're headed for...
        //
        if (pDE->m_argCount == 0)
        {
            COMPlusThrow(kArgumentException, L"Argument_BadObjRef");
        }

        //
        // We should have a valid this pointer.
        //
        if ((argData[0].argHome.kind == RAK_NONE) && (argData[0].argAddr == NULL))
        {
            COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");
        }

        //
        // Assume we can only have this for real objects or boxed value types, not value classes...
        //
        _ASSERTE((argData[0].argElementType == ELEMENT_TYPE_OBJECT) ||
                 (argData[0].argElementType == ELEMENT_TYPE_STRING) ||
                 (argData[0].argElementType == ELEMENT_TYPE_CLASS) ||
                 (argData[0].argElementType == ELEMENT_TYPE_ARRAY) ||
                 (argData[0].argElementType == ELEMENT_TYPE_SZARRAY) ||
                 ((argData[0].argElementType == ELEMENT_TYPE_VALUETYPE) &&
                  (pObjectRefArray[0] != NULL)));

        //
        // Now get the object pointer to our first arg.
        //
        OBJECTREF objRef = NULL;
        GCPROTECT_BEGIN(objRef);

        if (argData[0].argElementType == ELEMENT_TYPE_VALUETYPE)
        {
            //
            // In this case, we know where it is.
            //
            objRef = pObjectRefArray[0];
            _ASSERTE(pDataLocationArray[0] & DL_ObjectRefArray);
        }
        else
        {
            TypeHandle  dummyTH;
            ARG_SLOT    objSlot;

            //
            // Suck out the first arg. We're gonna trick GetFuncEvalArgValue by passing in just our
            // object ref as the stack.
            //
            // Note that we are passing ELEMENT_TYPE_END in the last parameter because we want to
            // supress the the valid object ref check.
            //
            GetFuncEvalArgValue(pDE,
                                &(argData[0]),
                                false,
                                false,
                                dummyTH,
                                ELEMENT_TYPE_CLASS,
                                dummyTH,
                                &objSlot,
                                NULL,
                                pObjectRefArray,
                                pBufferForArgsArray,
                                ELEMENT_TYPE_END
                                DEBUG_ARG(pDataLocationArray[0])
                               );

            objRef = ArgSlotToObj(objSlot);
        }

        //
        // Validate the object
        //
        if (FAILED(ValidateObject(OBJECTREFToObject(objRef))))
        {
            COMPlusThrow(kArgumentException, L"Argument_BadObjRef");
        }

        //
        // Null isn't valid in this case!
        //
        if (objRef == NULL)
        {
            COMPlusThrow(kArgumentNullException, L"ArgumentNull_Obj");
        }

        //
        // Make sure that the object supplied is of a type that can call the method supplied.
        //
        if (!g_pEEInterface->ObjIsInstanceOf(OBJECTREFToObject(objRef), pDE->m_ownerTypeHandle))
        {
            COMPlusThrow(kArgumentException, L"Argument_CORDBBadMethod");
        }

        //
        // Now, find the proper MethodDesc for this interface method based on the object we're invoking the
        // method on.
        //
        pDE->m_targetCodeAddr = MethodTable::GetTargetFromMethodDescAndServer(pDE->m_ownerTypeHandle, pDE->m_md, &objRef, FALSE);

        GCPROTECT_END();
    }
    else
    {
        TypeHandle ownerType(pDE->m_md->GetMethodTable());
        pDE->m_targetCodeAddr = MethodTable::GetTargetFromMethodDescAndServer(ownerType, pDE->m_md,NULL,FALSE);
    }

    //
    // Get the resulting type now.  Doing this may trigger a GC or throw.
    //
    if (pDE->m_evalType != DB_IPCE_FET_NEW_OBJECT)
    {
        pDE->m_resultType = mSig.GetRetTypeHandleThrowing();
    }

    //
    // Check if there is an explicit retun argument, or if the return type is really a VALUETYPE but our
    // calling convention is treating it as primitive. We just need to remember the pretValueClass so
    // that we will box it properly on our way out.
    //
    // On WIN64, VALUETYPE return values are never normalized to anything else.  Currently, the following check is
    // equivalent to "#if !defined(ENREGISTERED_RETURNTYPE_MAXSIZE)".
    //
    *pfHasRetBuffArg = mSig.HasRetBuffArg();
    CorElementType retType           = mSig.GetReturnType();
    CorElementType retTypeNormalized = mSig.GetReturnTypeNormalized();

    if ((*pfHasRetBuffArg) ||
        ((retType == ELEMENT_TYPE_VALUETYPE) && (retType != retTypeNormalized))
       )
    {
        *pRetValueType          = mSig.GetRetTypeHandleThrowing();
        *pfHasNonStdByValReturn = mSig.HasNonStandardByvalReturn();
    }
    else
    {
        //
        // Make sure the caller initialized this value
        //
        _ASSERTE((*pRetValueType).IsNull());
    }
}

/*
 * CopyArgsToBuffer
 *
 * This routine copies all the arguments to a local buffer, so that any one that needs to be
 * passed can be.  Note that this local buffer is NOT GC-protected, and so all the values
 * in the buffer may not be relied on.  You *must* use GetFuncEvalArgValue() to load up the
 * Arguments for the call, because it has the logic to decide which of the parallel arrays to pull
 * from.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    argData - Array of information about the arguments.
 *    pFEArgInfo - An array of structs to hold the argument information. Must have be previously filled in.
 *    pBufferArray - An array to store values.
 *
 * Returns:
 *    None.
 *
 */
void CopyArgsToBuffer(DebuggerEval *pDE,
                      DebuggerIPCE_FuncEvalArgData *argData,
                      FuncEvalArgInfo *pFEArgInfo,
                      INT64 *pBufferArray
                      DEBUG_ARG(DataLocation pDataLocationArray[])
                     )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    unsigned currArgIndex = 0;


    if ((pDE->m_evalType == DB_IPCE_FET_NORMAL) && !pDE->m_md->IsStatic())
    {
        //
        // Skip over the 'this' arg, since this function is not supposed to mess with it.
        //
        currArgIndex++;
    }

    //
    // Spin thru each argument now
    //
    for ( ; currArgIndex < pDE->m_argCount; currArgIndex++)
    {
        DebuggerIPCE_FuncEvalArgData *pFEAD = &argData[currArgIndex];
        BOOL isByRef = (pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_BYREF);
        BOOL fNeedBoxOrUnbox;
        fNeedBoxOrUnbox = pFEArgInfo[currArgIndex].fNeedBoxOrUnbox;


        LOG((LF_CORDB, LL_EVERYTHING, "CATB: currArgIndex=%d\n",
             currArgIndex));
        LOG((LF_CORDB, LL_EVERYTHING,
            "\t: argSigType=0x%x, byrefArgSigType=0x%0x, inType=0x%0x\n",
             pFEArgInfo[currArgIndex].argSigType,
             pFEArgInfo[currArgIndex].byrefArgSigType,
             pFEAD->argElementType));

        INT64 *pDest = &(pBufferArray[currArgIndex]);

        switch (pFEAD->argElementType)
        {
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R8:

            if (pFEAD->argAddr != NULL)
            {
                *pDest = *(INT64*)(pFEAD->argAddr);
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                }
#endif
            }
            else if (pFEAD->argIsLiteral)
            {
                _ASSERTE(sizeof(pFEAD->argLiteralData) >= sizeof(void *));

                // If this is a literal arg, then we just copy the data.
                memcpy(pDest, pFEAD->argLiteralData, sizeof(INT64));
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                }
#endif
            }
            else
            {

                // RAK_REG is the only 4 byte type, all others are 8 byte types.
                _ASSERTE(pFEAD->argHome.kind != RAK_REG);
                
                INT64 bigVal = 0;
                SIZE_T v;
                INT64 *pAddr;

                pAddr = (INT64*)GetRegisterValueAndReturnAddress(pDE, pFEAD, &bigVal, &v);

                if (pAddr == NULL)
                {
                    COMPlusThrow(kArgumentNullException, L"ArgumentNull_Generic");
                }

                *pDest = *pAddr;




#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                }
#endif
            }
            break;

        case ELEMENT_TYPE_VALUETYPE:

            //
            // For value types, we dont do anything here, instead delay until GetFuncEvalArgInfo
            //
            break;

        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:

            if (pFEAD->argAddr != NULL)
            {
                if (!isByRef)
                {
                    if (pFEAD->argIsHandleValue)
                    {
                        OBJECTHANDLE oh = (OBJECTHANDLE)(pFEAD->argAddr);
                        *pDest = (INT64)(size_t)oh;
                    }
                    else
                    {
                        *pDest = *((SIZE_T*)(pFEAD->argAddr));
                    }
#ifdef _DEBUG
                    if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                    {
                        pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                    }
#endif
                }
                else
                {
                    if (pFEAD->argIsHandleValue)
                    {
                        *pDest = (INT64)(size_t)(pFEAD->argAddr);
                    }
                    else
                    {
                        *pDest = *(SIZE_T*)(pFEAD->argAddr);
                    }
#ifdef _DEBUG
                    if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                    {
                        pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                    }
#endif
                }
            }
            else if (pFEAD->argIsLiteral)
            {
                _ASSERTE(sizeof(pFEAD->argLiteralData) >= sizeof(INT64));

                // The called function may expect a larger/smaller value than the literal value.
                // So we convert the value to the right type.

                CONSISTENCY_CHECK_MSGF(((pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_CLASS)   ||
                                        (pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_SZARRAY) ||
                                        (pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_ARRAY))  ||
                                       (isByRef && ((pFEArgInfo[currArgIndex].byrefArgSigType == ELEMENT_TYPE_CLASS)   ||
                                                    (pFEArgInfo[currArgIndex].byrefArgSigType == ELEMENT_TYPE_SZARRAY) ||
                                                    (pFEArgInfo[currArgIndex].byrefArgSigType == ELEMENT_TYPE_ARRAY))),
                                       ("argSigType=0x%0x, byrefArgSigType=0x%0x, isByRef=%d",
                                        pFEArgInfo[currArgIndex].argSigType,
                                        pFEArgInfo[currArgIndex].byrefArgSigType,
                                        isByRef));

                LOG((LF_CORDB, LL_EVERYTHING,
                     "argSigType=0x%0x, byrefArgSigType=0x%0x, isByRef=%d\n",
                     pFEArgInfo[currArgIndex].argSigType, pFEArgInfo[currArgIndex].byrefArgSigType, isByRef));

                *(SIZE_T*)pDest = *(SIZE_T*)pFEAD->argLiteralData;
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                }
#endif
            }
            else
            {
                // RAK_REG is the only valid 4 byte type on WIN32.  On WIN64, RAK_REG and RAK_FLOAT
                // can both be either 4 bytes or 8 bytes;
                _ASSERTE((pFEAD->argHome.kind == RAK_REG)
                         WIN64_ONLY(|| (pFEAD->argHome.kind == RAK_FLOAT)));

                CorDebugRegister regNum = GetArgAddrFromReg(pFEAD);

                // Simply grab the value out of the proper register.
                SIZE_T v = GetRegisterValue(pDE, regNum, pFEAD->argHome.reg1Addr);
                *pDest = v;
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                }
#endif
            }
            break;

        default:
            // 4-byte, 2-byte, or 1-byte values

            if (pFEAD->argAddr != NULL)
            {
                if (!isByRef)
                {
                    if (pFEAD->argIsHandleValue)
                    {
                        OBJECTHANDLE oh = (OBJECTHANDLE)(pFEAD->argAddr);
                        *pDest = (INT64)(size_t)oh;
                    }
                    else
                    {
                        GetAndSetLiteralValue(pDest, pFEArgInfo[currArgIndex].argSigType,
                                              pFEAD->argAddr, pFEAD->argElementType);
                    }
#ifdef _DEBUG
                    if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                    {
                        pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                    }
#endif
                }
                else
                {
                    if (pFEAD->argIsHandleValue)
                    {
                        *pDest = (INT64)(size_t)(pFEAD->argAddr);
                    }
                    else
                    {
                        // We have to make sure we only grab the correct size of memory from the source.  On IA64, we
                        // have to make sure we don't cause misaligned data exceptions as well.  Then we put the value
                        // into the pBufferArray.  The reason is that we may be passing in some values by ref to a
                        // function that's expecting something of a bigger size.  Thus, if we don't do this, then we'll
                        // be bashing memory right next to the source value as the function being called acts upon some
                        // bigger value.
                        GetAndSetLiteralValue(pDest, pFEArgInfo[currArgIndex].byrefArgSigType,
                                              pFEAD->argAddr, pFEAD->argElementType);
                    }
#ifdef _DEBUG
                    if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                    {
                        pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                    }
#endif
                }
            }
            else if (pFEAD->argIsLiteral)
            {
                _ASSERTE(sizeof(pFEAD->argLiteralData) >= sizeof(INT32));

                // The called function may expect a larger/smaller value than the literal value,
                // so we convert the value to the right type.

                CONSISTENCY_CHECK_MSGF(
                    ((pFEArgInfo[currArgIndex].argSigType>=ELEMENT_TYPE_BOOLEAN) && (pFEArgInfo[currArgIndex].argSigType<=ELEMENT_TYPE_R8)) ||
                    (pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_PTR) ||
                    (isByRef && ((pFEArgInfo[currArgIndex].byrefArgSigType>=ELEMENT_TYPE_BOOLEAN) && (pFEArgInfo[currArgIndex].byrefArgSigType<=ELEMENT_TYPE_R8))),
                    ("argSigType=0x%0x, byrefArgSigType=0x%0x, isByRef=%d", pFEArgInfo[currArgIndex].argSigType, pFEArgInfo[currArgIndex].byrefArgSigType, isByRef));

                LOG((LF_CORDB, LL_EVERYTHING,
                     "argSigType=0x%0x, byrefArgSigType=0x%0x, isByRef=%d\n",
                     pFEArgInfo[currArgIndex].argSigType,
                     pFEArgInfo[currArgIndex].byrefArgSigType,
                     isByRef));

                CorElementType relevantType = (isByRef ? pFEArgInfo[currArgIndex].byrefArgSigType : pFEArgInfo[currArgIndex].argSigType);

                GetAndSetLiteralValue(pDest, relevantType, pFEAD->argLiteralData, pFEAD->argElementType);
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                }
#endif
            }
            else
            {
                // RAK_REG is the only valid 4 byte type on WIN32.  On WIN64, RAK_REG and RAK_FLOAT
                // can both be either 4 bytes or 8 bytes;
                _ASSERTE((pFEAD->argHome.kind == RAK_REG)
                         WIN64_ONLY(|| (pFEAD->argHome.kind == RAK_FLOAT)));

                CorDebugRegister regNum = GetArgAddrFromReg(pFEAD);

                // Simply grab the value out of the proper register.
                SIZE_T v = GetRegisterValue(pDE, regNum, pFEAD->argHome.reg1Addr);
                *pDest = v;
#ifdef _DEBUG
                if (currArgIndex < MAX_DATA_LOCATIONS_TRACKED)
                {
                    pDataLocationArray[currArgIndex] |= DL_BufferForArgsArray;
                }
#endif
            }
        }
    }
}


/*
 * PackArgumentArray
 *
 * This routine fills a given array with the correct values for passing to a managed function.
 * It uses various component arrays that contain information to correctly create the argument array.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    argData - Array of information about the arguments.
 *    pUnboxedMD - MethodDesc of the function to call, after unboxing.
 *    RetValueType - Type Handle of the return value of the managed function we will call.
 *    pFEArgInfo - An array of structs to hold the argument information.  Must have be previously filled in.
 *    pObjectRefArray - An array that contains any object refs.  It was built previously.
 *    pMaybeInteriorPtrArray - An array that contains values that may be pointers to
 *         the interior of a managed object.
 *    pBufferForArgsArray - An array that contains values that need writable memory space
 *         for passing ByRef.
 *    newObj - Pre-allocated object for a 'new' call.
 *    pArguments - This array is packed from the above arrays.
 *    pfHasHiddenParam - returns this value filled in.
 *    pRetValueTypeSize - If *pfHasHiddenParam is returned as TRUE, then the caller must allocate
 *         a buffer of this size before making the managed call.
 *    pRetValueTypeSlotNumber - If *pfHasHiddenParam is returned as TRUE, then the caller
 *         must assigned pArguments[*pRetValueTypeSlotNumber] to be the buffer it allocates.
 *
 * Returns:
 *    None.
 *
 */
void PackArgumentArray(DebuggerEval *pDE,
                       DebuggerIPCE_FuncEvalArgData *argData,
                       FuncEvalArgInfo *pFEArgInfo,
                       MethodDesc *pUnboxedMD,
                       TypeHandle RetValueType,
                       OBJECTREF *pObjectRefArray,
                       void **pMaybeInteriorPtrArray,
                       INT64 *pBufferForArgsArray,
                       OBJECTREF newObj,
                       BOOL fHasRetBuffArg,
                       ARG_SLOT *pArguments,
                       BOOL *pfHasHiddenParam,
                       DWORD *pRetValueTypeSize,
                       DWORD *pRetValueTypeSlotNumber
                       DEBUG_ARG(DataLocation pDataLocationArray[])
                      )
{
    WRAPPER_CONTRACT;

    GCX_FORBID();

    unsigned currArgIndex = 0;
    unsigned currArgSlot = 0;


    //
    // THIS POINTER (if any)
    // For non-static methods, or when returning a new object,
    // the first arg in the array is 'this' or the new object.
    //
    if (pDE->m_evalType == DB_IPCE_FET_NEW_OBJECT)
    {
        //
        // If this is a new object op, then we need to fill in the 0'th
        // arg slot with the 'this' ptr.
        //
        pArguments[0] = ObjToArgSlot(newObj);

        //
        // If we are invoking a function on a value class, but we have a boxed value class for 'this',
        // then go ahead and unbox it and leave a ref to the value class on the stack as 'this'.
        //
        if (pDE->m_md->GetMethodTable()->IsValueClass())
        {
            _ASSERTE(newObj->GetMethodTable()->IsValueClass());

            // This is one of those places we use true boxed nullables
            _ASSERTE(!Nullable::IsNullableType(pDE->m_md->GetMethodTable()) ||
                     newObj->GetMethodTable() == pDE->m_md->GetMethodTable());
            void *pData = newObj->GetData();
            pArguments[0] = PtrToArgSlot(pData);
        }

        //
        // Bump up the arg slot
        //
        currArgSlot++;
    }
    else if (!pDE->m_md->IsStatic())
    {
        //
        // Place 'this' first in the array for non-static methods.
        //
        TypeHandle dummyTH;
        bool isByRef = false;
        bool fNeedBoxOrUnbox = false;

        // We had better have an object for a 'this' argument!
        CorElementType et = argData[0].argElementType;

        if (!(IsElementTypeSpecial(et) ||
              et == ELEMENT_TYPE_VALUETYPE))
        {
            COMPlusThrow(kArgumentOutOfRangeException, L"ArgumentOutOfRange_Enum");
        }

        LOG((LF_CORDB, LL_EVERYTHING, "this: currArgSlot=%d, currArgIndex=%d et=0x%x\n", currArgSlot, currArgIndex, et));

        if (pDE->m_md->GetMethodTable()->IsValueClass())
        {
            // For value classes, the 'this' parameter is always passed by reference.
            // However do not unbox if we are calling an unboxing stub.
            if (pDE->m_md == pUnboxedMD)
            {
                // pDE->m_md is expecting an unboxed this pointer. Then we will unbox it.
                isByRef = true;

                // Remember if we need to unbox this parameter, though.
                if ((et == ELEMENT_TYPE_CLASS) || (et == ELEMENT_TYPE_OBJECT))
                {
                    fNeedBoxOrUnbox = true;
                }
            }
        }
        else if (et == ELEMENT_TYPE_VALUETYPE)
        {
            // When the method that we invoking is defined on non value type and we receive the ValueType as input,
            // we are calling methods on System.Object. In this case, we need to box the input ValueType.
            fNeedBoxOrUnbox = true;
        }

        GetFuncEvalArgValue(pDE,
                            &argData[currArgIndex],
                            isByRef,
                            fNeedBoxOrUnbox,
                            dummyTH,
                            ELEMENT_TYPE_CLASS,
                            pDE->m_md->GetMethodTable(),
                            &(pArguments[currArgSlot]),
                            &(pMaybeInteriorPtrArray[currArgIndex]),
                            &(pObjectRefArray[currArgIndex]),
                            &(pBufferForArgsArray[currArgIndex]),
                            ELEMENT_TYPE_OBJECT
                            DEBUG_ARG((currArgIndex < MAX_DATA_LOCATIONS_TRACKED) ? pDataLocationArray[currArgIndex]
                                                                                  : DL_All)
                            );

        LOG((LF_CORDB, LL_EVERYTHING, "this = 0x%08x\n", ArgSlotToPtr(pArguments[currArgSlot])));

        // We need to check 'this' for a null ref ourselves... NOTE: only do this if we put an object reference on
        // the stack. If we put a byref for a value type, then we don't need to do this!
        if (!isByRef)
        {
            // The this pointer is not a unboxed value type.

            ARG_SLOT oi1 = pArguments[currArgSlot];
            OBJECTREF o1 = ArgSlotToObj(oi1);

            if (FAILED(ValidateObject(OBJECTREFToObject(o1))))
            {
                COMPlusThrow(kArgumentException, L"Argument_BadObjRef");
            }

            if (OBJECTREFToObject(o1) == NULL)
            {
                COMPlusThrow(kNullReferenceException, L"NullReference_This");
            }

            // For interface method, we have already done the check early on.
            if (!pDE->m_md->IsInterface())
            {
                // We also need to make sure that the method that we are invoking is either defined on this object or the direct/indirect
                // base objects.
                Object  *objPtr = OBJECTREFToObject(o1);
                MethodTable *pMT = objPtr->GetMethodTable();
                if (!pMT->IsArray() && !pMT->IsTransparentProxyType() && !pDE->m_md->IsSharedByGenericInstantiations())
                {
                    TypeHandle thFrom = TypeHandle(pMT);
                    TypeHandle thTarget = TypeHandle(pDE->m_md->GetMethodTable());
                    if (thFrom.CanCastToNoGC(thTarget) == TypeHandle::CannotCast)
                    {
                        COMPlusThrow(kArgumentException, L"Argument_CORDBBadMethod");
                    }
                }
            }
        }

        //
        // Increment up both arrays.
        //
        currArgSlot++;
        currArgIndex++;
    }

    *pfHasHiddenParam = false;

    // HIDDEN VALUETYPE RETURN ARGUMENT (if any)
    // Special handling for functions that return value classes.
    if (fHasRetBuffArg)
    {
        LOG((LF_CORDB, LL_EVERYTHING, "retBuff: currArgSlot=%d, currArgIndex=%d\n", currArgSlot, currArgIndex));
        *pfHasHiddenParam = true;
        *pRetValueTypeSize = RetValueType.GetMethodTable()->GetAlignedNumInstanceFieldBytes();
        *pRetValueTypeSlotNumber = currArgSlot;

        //
        // Increment to the next arg slot.
        //
        currArgSlot++;
    }

    // REAL ARGUMENTS (if any)
    // Now do the remaining args
    for ( ; currArgIndex < pDE->m_argCount; currArgSlot++, currArgIndex++)
    {
        DebuggerIPCE_FuncEvalArgData *pFEAD = &argData[currArgIndex];

        LOG((LF_CORDB, LL_EVERYTHING, "currArgSlot=%d, currArgIndex=%d\n",
             currArgSlot,
             currArgIndex));
        LOG((LF_CORDB, LL_EVERYTHING,
            "\t: argSigType=0x%x, byrefArgSigType=0x%0x, inType=0x%0x\n",
             pFEArgInfo[currArgIndex].argSigType,
             pFEArgInfo[currArgIndex].byrefArgSigType,
             pFEAD->argElementType));


        GetFuncEvalArgValue(pDE,
                            pFEAD,
                            pFEArgInfo[currArgIndex].argSigType == ELEMENT_TYPE_BYREF,
                            pFEArgInfo[currArgIndex].fNeedBoxOrUnbox,
                            pFEArgInfo[currArgIndex].sigTypeHandle,
                            pFEArgInfo[currArgIndex].byrefArgSigType,
                            pFEArgInfo[currArgIndex].byrefArgTypeHandle,
                            &(pArguments[currArgSlot]),
                            &(pMaybeInteriorPtrArray[currArgIndex]),
                            &(pObjectRefArray[currArgIndex]),
                            &(pBufferForArgsArray[currArgIndex]),
                            pFEArgInfo[currArgIndex].argSigType
                            DEBUG_ARG((currArgIndex < MAX_DATA_LOCATIONS_TRACKED) ? pDataLocationArray[currArgIndex]
                                                                                  : DL_All)
                           );
    }
}

/*
 * UnpackFuncEvalResult
 *
 * This routine takes the resulting object of a func-eval, and does any copying, boxing, unboxing, necessary.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    newObj - Pre-allocated object for NEW_OBJ func-evals.
 *    retObject - Pre-allocated object to be filled in with the info in pSource.
 *    RetValueType - The return type of the function called.
 *    fHasHiddenParam - TRUE if the function had a hidden parameter.
 *    pSource - The raw bytes returned by the func-eval call when there is a hidden parameter.
 *
 *
 * Returns:
 *    None.
 *
 */
void UnpackFuncEvalResult(DebuggerEval *pDE,
                          OBJECTREF newObj,
                          OBJECTREF retObject,
                          TypeHandle RetValueType,
                          BOOL fHasHiddenParam,
                          BOOL fHasNonStdByValReturn,
                          void *pSource
                          )
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        GC_NOTRIGGER;
    }
    CONTRACTL_END;


    // Ah, but if this was a new object op, then the result is really
    // the object we allocated above...
    if (pDE->m_evalType == DB_IPCE_FET_NEW_OBJECT)
    {
        // We purposely do not morph nullables to be boxed Ts here because debugger EE's otherwise
        // have no way of creating true nullables that they need for their own purposes. 
        pDE->m_result = ObjToArgSlot(newObj);
        pDE->m_retValueBoxing = Debugger::AllBoxed;
    }
    else if (!RetValueType.IsNull())
    {
        LOG((LF_CORDB, LL_EVERYTHING, "FuncEval call is saving a boxed VC return value.\n"));

        //
        // We pre-created it above
        //
        _ASSERTE(retObject != NULL);

        // This is one of those places we use true boxed nullables
        _ASSERTE(!Nullable::IsNullableType(RetValueType)||
                 retObject->GetMethodTable() == RetValueType.GetMethodTable());

        if (fHasHiddenParam)
        {
            // box the object
            CopyValueClass(retObject->GetData(),
                           pSource,
                           RetValueType.GetMethodTable(),
                           retObject->GetAppDomain());
        }
#if defined(CALLDESCR_RETBUF)
        else if (fHasNonStdByValReturn)
        {
            // box the primitive returned, retObject is a true nullable for nullabes, It will be Normalized later
            CopyValueClass(retObject->GetData(),
                           GetThread()->m_SmallVCRetVal,
                           RetValueType.GetMethodTable(),
                           retObject->GetAppDomain());
        }
#endif // CALLDESCR_RETBUF
        else
        {
            // box the primitive returned, retObject is a true nullable for nullabes, It will be Normalized later
            CopyValueClass(retObject->GetData(),
                           &(pDE->m_result),
                           RetValueType.GetMethodTable(),
                           retObject->GetAppDomain());
        }

        pDE->m_result = ObjToArgSlot(retObject);
        pDE->m_retValueBoxing = Debugger::AllBoxed;
    }
    else
    {
        //
        // Other FuncEvals return primitives as unboxed.
        //
        pDE->m_retValueBoxing = Debugger::OnlyPrimitivesUnboxed;
    }

    LOG((LF_CORDB, LL_INFO10000, "FuncEval call has saved the return value.\n"));
    // No exception, so it worked as far as we're concerned.
    pDE->m_successful = true;

    // If the result is an object, then place the object
    // reference into a strong handle and place the handle into the
    // pDE to protect the result from a collection.
    CorElementType retClassET = pDE->m_resultType.GetSignatureCorElementType();

    if ((pDE->m_retValueBoxing == Debugger::AllBoxed) ||
        !RetValueType.IsNull() ||
        IsElementTypeSpecial(retClassET))
    {
        LOG((LF_CORDB, LL_EVERYTHING, "Creating strong handle for boxed DoNormalFuncEval result.\n"));
        OBJECTHANDLE oh = pDE->m_thread->GetDomain()->CreateStrongHandle(ArgSlotToObj(pDE->m_result));
        pDE->m_result = (INT64)(LONG_PTR)oh;
        pDE->m_objectHandle.Set(oh);
    }
}

/*
 * UnpackFuncEvalArguments
 *
 * This routine takes the resulting object of a func-eval, and does any copying, boxing, unboxing, necessary.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    newObj - Pre-allocated object for NEW_OBJ func-evals.
 *    retObject - Pre-allocated object to be filled in with the info in pSource.
 *    RetValueType - The return type of the function called.
 *    fHasHiddenParam - TRUE if the function had a hidden parameter.
 *    pSource - The raw bytes returned by the func-eval call when there is a hidden parameter.
 *
 *
 * Returns:
 *    None.
 *
 */
void UnpackFuncEvalArguments(DebuggerEval *pDE,
                             DebuggerIPCE_FuncEvalArgData *argData,
                             MetaSig mSig,
                             BOOL staticMethod,
                             OBJECTREF *pObjectRefArray,
                             void **pMaybeInteriorPtrArray,
                             void **pByRefMaybeInteriorPtrArray,
                             INT64 *pBufferForArgsArray
                            )
{
    WRAPPER_CONTRACT;

    // Update any enregistered byrefs with their new values from the
    // proper byref temporary array.
    if (pDE->m_argCount > 0)
    {
        mSig.Reset();

        unsigned currArgIndex = 0;

        if ((pDE->m_evalType == DB_IPCE_FET_NORMAL) && !pDE->m_md->IsStatic())
        {
            //
            // Skip over the 'this' arg, since this function is not supposed to mess with it.
            //
            currArgIndex++;
        }

        for (; currArgIndex < pDE->m_argCount; currArgIndex++)
        {
            CorElementType argSigType = mSig.NextArgNormalized();

            LOG((LF_CORDB, LL_EVERYTHING, "currArgIndex=%d argSigType=0x%x\n", currArgIndex, argSigType));

            _ASSERTE(argSigType != ELEMENT_TYPE_END);

            if (argSigType == ELEMENT_TYPE_BYREF)
            {
                TypeHandle byrefClass = TypeHandle();
                CorElementType byrefArgSigType = mSig.GetByRefType(&byrefClass);

                // If these are the true boxed nullables we created in BoxFuncEvalArguments, convert them back
                pObjectRefArray[currArgIndex] = Nullable::NormalizeBox(pObjectRefArray[currArgIndex]);

                LOG((LF_CORDB, LL_EVERYTHING, "DoNormalFuncEval: Updating enregistered byref...\n"));
                SetFuncEvalByRefArgValue(pDE,
                                         &argData[currArgIndex],
                                         byrefArgSigType,
                                         pBufferForArgsArray[currArgIndex],
                                         pMaybeInteriorPtrArray[currArgIndex],
                                         pByRefMaybeInteriorPtrArray[currArgIndex],
                                         pObjectRefArray[currArgIndex]
                                        );
            }
        }
    }
}


/*
 * FuncEvalWrapper
 *
 * Helper function for func-eval. We have to split it out so that we can put a __try / __finally in to
 * notify on a Catch-Handler found.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    pArguments - created stack to pass for the call.
 *    pCatcherStackAddr - stack address to report as the Catch Handler Found location.
 *
 * Returns:
 *    None.
 *
 */
void FuncEvalWrapper(MethodDescCallSite* pMDCS, DebuggerEval *pDE, ARG_SLOT *pArguments, BYTE *pCatcherStackAddr)
{
    PAL_TRY
    {
        pDE->m_result = pMDCS->CallWithValueTypes_RetArgSlot(pArguments);
    }
    PAL_EXCEPT_FILTER(NotifyOfCHFFilterWrapper, pCatcherStackAddr)
    {
        // Should never reach here b/c handler should always continue search.
        _ASSERTE(false);
    }
    PAL_ENDTRY
}

/*
 * RecordFuncEvalException
 *
 * Helper function records the details of an exception that occured during a FuncEval
 * Note that this should be called from within the target domain of the FuncEval.
 *
 * Parameters:
 *   pDE - pointer to the DebuggerEval object being processed
 *   ppException - the Exception object that was thrown
 *
 * Returns:
 *    None.
 */
static void RecordFuncEvalException(DebuggerEval *pDE,
                             OBJECTREF ppException )
{
    CONTRACTL
    {
        THROWS;         // CreateStrongHandle could throw OOM
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // We got an exception. Make the exception into our result.
    pDE->m_successful = false;
    LOG((LF_CORDB, LL_EVERYTHING, "D::FEHW - Exception during funceval.\n"));

    //
    // Special handling for thread abort exceptions. We need to explicitly reset the
    // abort request on the EE thread, then make sure to place this thread on a thunk
    // that will re-raise the exception when we continue the process. Note: we still
    // pass this thread abort exception up as the result of the eval.
    //
    if (IsExceptionOfType(kThreadAbortException, &ppException))
    {
        if (pDE->m_aborting != DebuggerEval::FE_ABORT_NONE)
        {
            //
            // Reset the abort request.
            //
            pDE->m_thread->UserResetAbort(Thread::TAR_FuncEval);

            //
            // This is the abort we sent down.
            //
            pDE->m_result = NULL;
            pDE->m_resultType = TypeHandle();
            pDE->m_aborted = true;
            pDE->m_retValueBoxing = Debugger::NoValueTypeBoxing;

            LOG((LF_CORDB, LL_EVERYTHING, "D::FEHW - funceval abort exception.\n"));

        }
        else
        {
            //
            // This must have come from somewhere else, remember that we need to
            // rethrow this.
            //
            pDE->m_rethrowAbortException = true;

            //
            // The result is the exception object.
            //
            pDE->m_result = ObjToArgSlot(ppException);

            pDE->m_resultType = ppException->GetTypeHandle();
            OBJECTHANDLE oh = pDE->m_thread->GetDomain()->CreateStrongHandle(ArgSlotToObj(pDE->m_result));
            pDE->m_result = (INT64)PTR_TO_CORDB_ADDRESS(oh);
            pDE->m_objectHandle.Set(oh);
            pDE->m_retValueBoxing = Debugger::NoValueTypeBoxing;

            LOG((LF_CORDB, LL_EVERYTHING, "D::FEHW - Non-FE abort thread abort..\n"));
        }
    }
    else
    {

        //
        // The result is the exception object.
        //
        pDE->m_result = ObjToArgSlot(ppException);

        pDE->m_resultType = ppException->GetTypeHandle();
        OBJECTHANDLE oh = pDE->m_thread->GetDomain()->CreateStrongHandle(ArgSlotToObj(pDE->m_result));
        pDE->m_result = (INT64)(LONG_PTR)oh;
        pDE->m_objectHandle.Set(oh);

        pDE->m_retValueBoxing = Debugger::NoValueTypeBoxing;

        LOG((LF_CORDB, LL_EVERYTHING, "D::FEHW - Exception for the user.\n"));
    }
}


/*
 * DoNormalFuncEval
 *
 * Does the main body of work (steps 1c onward) for the normal func-eval algorithm detailed at the
 * top of this file. The args have already been GC protected and we've transitioned into the appropriate
 * domain (steps 1a & 1b).  This has to be a seperate function from GCProtectArgsAndDoNormalFuncEval 
 * because otherwise we can't reliably find the right GCFrames to pop when unwinding the stack due to 
 * an exception on 64-bit platforms (we have some GCFrames outside of the TRY, and some inside, 
 * and they won't necesarily be layed out sequentially on the stack if they are all in the same function).
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    pCatcherStackAddr - stack address to report as the Catch Handler Found location.
 *    pObjectRefArray - An array to hold object ref args. This array is protected from GC's.
 *    pMaybeInteriorPtrArray - An array to hold values that may be pointers into a managed object.  
 *           This array is protected from GCs.
 *    pByRefMaybeInteriorPtrArray - An array to hold values that may be pointers into a managed 
 *           object.  This array is protected from GCs.  This array protects the address of the arguments 
 *           while the pMaybeInteriorPtrArray protects the value of the arguments.  We need to do this 
 *           because of by ref arguments.
 *    pBufferForArgsArray - a buffer of temporary scratch space for things that do not need to be 
 *           protected, or are protected for free (e.g. Handles).
 *    pDataLocationArray - an array of tracking data for debug sanity checks
 *
 * Returns:
 *    None.
 */
static void DoNormalFuncEval( DebuggerEval *pDE,
            BYTE *pCatcherStackAddr,
            OBJECTREF *pObjectRefArray,
            void **pMaybeInteriorPtrArray,
            void **pByRefMaybeInteriorPtrArray,
            INT64 *pBufferForArgsArray
            DEBUG_ARG(DataLocation pDataLocationArray[])
          )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    //
    // Now that all the args are protected, we can go back and deal with generic args and resolving
    // all their information.
    //
    ResolveFuncEvalGenericArgInfo(pDE);

    //
    // Grab the signature of the method we're working on and do some error checking.
    // Note that if this instantiated generic code, then this will
    // correctly give as an instantiated view of the signature that we can iterate without
    // worrying about generic items in the signature.
    //
    MetaSig mSig(pDE->m_md);

    BYTE callingconvention = mSig.GetCallingConvention();
    if (!isCallConv(callingconvention, IMAGE_CEE_CS_CALLCONV_DEFAULT))
    {
        // We don't support calling vararg!
        COMPlusThrow(kArgumentException, L"Argument_CORDBBadVarArgCallConv");
    }

    //
    // We'll need to know if this is a static method or not.
    //
    BOOL staticMethod = pDE->m_md->IsStatic();

    _ASSERTE((pDE->m_evalType == DB_IPCE_FET_NORMAL) || !staticMethod);

    //
    // Do Step 1c - Pre-allocate space for new objects.
    //
    OBJECTREF newObj = NULL;
    GCPROTECT_BEGIN(newObj);

    SIZE_T allocArgCnt = 0;

    if (pDE->m_evalType == DB_IPCE_FET_NEW_OBJECT)
    {
        ValidateFuncEvalReturnType(DB_IPCE_FET_NEW_OBJECT, pDE->m_resultType.GetMethodTable());
        pDE->m_resultType.GetMethodTable()->EnsureInstanceActive();
        newObj = AllocateObject(pDE->m_resultType.GetMethodTable());

        //
        // Note: we account for an extra argument in the count passed
        // in. We use this to increase the space allocated for args,
        // and we use it to control the number of args copied into
        // those arrays below. Note: m_argCount already includes space
        // for this.
        //
        allocArgCnt = pDE->m_argCount + 1;
    }
    else
    {
        allocArgCnt = pDE->m_argCount;
    }

    //
    // Validate the argument count with mSig.
    //
    if (allocArgCnt != (mSig.NumFixedArgs() + (staticMethod ? 0 : 1)))
    {
        COMPlusThrow(kTargetParameterCountException, L"Arg_ParmCnt");
    }

    //
    // Do Step 1d - Gather information about the method that will be called.
    //
    // An array to hold information about the parameters to be passed.  This is
    // all the information we need to gather before entering the GCX_FORBID area.
    //
    DebuggerIPCE_FuncEvalArgData *argData = pDE->GetArgData();

    MethodDesc *pUnboxedMD = pDE->m_md;
    BOOL fHasRetBuffArg;
    BOOL fHasNonStdByValReturn = FALSE;
    TypeHandle RetValueType;

    BoxFuncEvalThisParameter(pDE,
                             argData,
                             pMaybeInteriorPtrArray,
                             pObjectRefArray
                             DEBUG_ARG(pDataLocationArray)
                             );

    GatherFuncEvalMethodInfo(pDE,
                             mSig,
                             argData,
                             &pUnboxedMD,
                             pObjectRefArray,
                             pBufferForArgsArray,
                             &fHasRetBuffArg,
                             &fHasNonStdByValReturn,
                             &RetValueType
                             DEBUG_ARG(pDataLocationArray)
                            );

    //
    // Do Step 1e - Gather info from runtime about args (may trigger a GC).
    //
    FuncEvalArgInfo *pFEArgInfo = (FuncEvalArgInfo *)_alloca(sizeof(FuncEvalArgInfo) * pDE->m_argCount);
    memset(pFEArgInfo, 0, sizeof(FuncEvalArgInfo) * pDE->m_argCount);

    GatherFuncEvalArgInfo(pDE, mSig, argData, pFEArgInfo);

    //
    // Do Step 1f - Box or unbox arguments one at a time, placing newly boxed items into
    // pObjectRefArray immediately after creating them.
    //
    BoxFuncEvalArguments(pDE,
                         argData,
                         pFEArgInfo,
                         pMaybeInteriorPtrArray,
                         pObjectRefArray
                         DEBUG_ARG(pDataLocationArray)
                         );

#ifdef _DEBUG
    if (!RetValueType.IsNull())
    {
        _ASSERTE(RetValueType.IsValueType());
    }
#endif

    //
    // Do Step 1g - Pre-allocate any return value object.
    //
    OBJECTREF retObject = NULL;
    GCPROTECT_BEGIN(retObject);

    if ((pDE->m_evalType != DB_IPCE_FET_NEW_OBJECT) && !RetValueType.IsNull())
    {
        ValidateFuncEvalReturnType(pDE->m_evalType, RetValueType.GetMethodTable());
        RetValueType.GetMethodTable()->EnsureInstanceActive();
        retObject = AllocateObject(RetValueType.GetMethodTable());
    }

    //
    // Do Step 1h - Copy into scratch buffer all enregistered arguments, and
    // ByRef literals.
    //
    CopyArgsToBuffer(pDE,
                     argData,
                     pFEArgInfo,
                     pBufferForArgsArray
                     DEBUG_ARG(pDataLocationArray)
                    );

    //
    // We presume that the function has a return buffer.  This assumption gets squeezed out
    // when we pack the argument array.
    //
    allocArgCnt++;

    LOG((LF_CORDB, LL_EVERYTHING,
         "Func eval for %s::%s: allocArgCnt=%d\n",
         pDE->m_md->m_pszDebugClassName,
         pDE->m_md->m_pszDebugMethodName,
         allocArgCnt));

    MethodDescCallSite funcToEval(pDE->m_md, pDE->m_targetCodeAddr);

    //
    // Do Step 1i - Create and pack argument array for managed function call.
    //
    // Allocate space for argument stack
    //
    ARG_SLOT *pArguments = (ARG_SLOT*)_alloca(sizeof(ARG_SLOT) * allocArgCnt);
    memset(pArguments, 0, sizeof(ARG_SLOT) * allocArgCnt);

    DWORD retValueTypeSize = 0;
    DWORD retValueTypeSlotNumber = 0;
    BOOL fHasHiddenParam;

    PackArgumentArray(pDE,
                      argData,
                      pFEArgInfo,
                      pUnboxedMD,
                      RetValueType,
                      pObjectRefArray,
                      pMaybeInteriorPtrArray,
                      pBufferForArgsArray,
                      newObj,
                      fHasRetBuffArg,
                      pArguments,
                      &fHasHiddenParam,
                      &retValueTypeSize,
                      &retValueTypeSlotNumber
                      DEBUG_ARG(pDataLocationArray)
                     );

    if (fHasHiddenParam)
    {
        BYTE *pBuffer = (BYTE*)_alloca(retValueTypeSize);
        memset(pBuffer, 0, retValueTypeSize);
        pArguments[retValueTypeSlotNumber] = PtrToArgSlot(pBuffer);
    }

    //
    //
    // Do Step 2 - Make the call!
    //
    //
    FuncEvalWrapper(&funcToEval, pDE, pArguments, pCatcherStackAddr);
    {

    // We have now entered the zone where taking a GC is fatal until we get the
    // return value all fixed up.
    //
        GCX_FORBID();


    //
    //
    // Do Step 3 - Unpack results and update ByRef arguments.
    //
    //
    //
        LOG((LF_CORDB, LL_EVERYTHING, "FuncEval call has returned\n"));


    // GC still can't happen until we get our return value out half way through the unpack function

    UnpackFuncEvalResult(pDE,
                         newObj,
                         retObject,
                         RetValueType,
                         fHasHiddenParam,
                         fHasNonStdByValReturn,
                         ArgSlotToPtr(pArguments[retValueTypeSlotNumber])
                        );
    }

    UnpackFuncEvalArguments(pDE,
                            argData,
                            mSig,
                            staticMethod,
                            pObjectRefArray,
                            pMaybeInteriorPtrArray,
                            pByRefMaybeInteriorPtrArray,
                            pBufferForArgsArray
                           );

    GCPROTECT_END();    // retObject
    GCPROTECT_END();    // newObj
}

/*
 * GCProtectArgsAndDoNormalFuncEval
 *
 * This routine is the primary entrypoint for normal func-evals.  It implements the algorithm 
 * described at the top of this file, doing steps 1a and 1b itself, then calling DoNormalFuncEval
 * to do the rest.
 *
 * Parameters:
 *    pDE - pointer to the DebuggerEval object being processed.
 *    pCatcherStackAddr - stack address to report as the Catch Handler Found location.
 *
 * Returns:
 *    None.
 *
 */
static void GCProtectArgsAndDoNormalFuncEval(DebuggerEval *pDE,
                             BYTE *pCatcherStackAddr )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;


    INDEBUG(DataLocation pDataLocationArray[MAX_DATA_LOCATIONS_TRACKED]);

    //
    // An array to hold object ref args. This array is protected from GC's.
    //
    OBJECTREF *pObjectRefArray = (OBJECTREF*)_alloca(sizeof(OBJECTREF) * pDE->m_argCount);
    memset(pObjectRefArray, 0, sizeof(OBJECTREF) * pDE->m_argCount);
    GCPROTECT_ARRAY_BEGIN(*pObjectRefArray, pDE->m_argCount);

    //
    // An array to hold values that may be pointers into a managed object.  This array
    // is protected from GCs.
    //
    void **pMaybeInteriorPtrArray = (void **)_alloca(sizeof(void **) * pDE->m_argCount);
    memset(pMaybeInteriorPtrArray, 0, sizeof(void **) * pDE->m_argCount);
    GCPROTECT_BEGININTERIOR_ARRAY(*pMaybeInteriorPtrArray, pDE->m_argCount * sizeof(void **)/sizeof(OBJECTREF));

    //
    // An array to hold values that may be pointers into a managed object.  This array
    // is protected from GCs.  This array protects the address of the arguments while the
    // pMaybeInteriorPtrArray protects the value of the arguments.  We need to do this because
    // of by ref arguments.
    //
    void **pByRefMaybeInteriorPtrArray = (void **)_alloca(sizeof(void **) * pDE->m_argCount);
    memset(pByRefMaybeInteriorPtrArray, 0, sizeof(void **) * pDE->m_argCount);
    GCPROTECT_BEGININTERIOR_ARRAY(*pByRefMaybeInteriorPtrArray, pDE->m_argCount * sizeof(void **)/sizeof(OBJECTREF));

    //
    // A buffer of temporary scratch space for things that do not need to be protected, or
    // are protected for free (e.g. Handles).
    //
    INT64 *pBufferForArgsArray = (INT64*)_alloca(sizeof(INT64) * pDE->m_argCount);
    memset(pBufferForArgsArray, 0, sizeof(INT64) * pDE->m_argCount);

    //
    // Initialize our tracking array
    //
    INDEBUG(memset(pDataLocationArray, 0, sizeof(DataLocation) * (MAX_DATA_LOCATIONS_TRACKED)));

    {
        GCX_FORBID();

        //
        // Do step 1a
        //
        GCProtectAllPassedArgs(pDE,
                               pObjectRefArray,
                               pMaybeInteriorPtrArray,
                               pByRefMaybeInteriorPtrArray,
                               pBufferForArgsArray
                               DEBUG_ARG(pDataLocationArray)
                               );

    }

    //
    // Do step 1b: we can switch domains since everything is now protected.
    // Note that before this point, it's unsafe to rely on pDE->m_module since it may be
    // invalid due to an AD unload.
    // All normal func evals should have an AppDomain specified.
    //
    _ASSERTE( pDE->m_appDomainId.m_dwId != 0 ); 
    ENTER_DOMAIN_ID( pDE->m_appDomainId );

    // Wrap everything in a EX_TRY so we catch any exceptions that could be thrown.
    // Note that we don't let any thrown exceptions cross the AppDomain boundary because we don't 
    // want them to get marshalled.
    EX_TRY
    {
        DoNormalFuncEval( 
            pDE, 
            pCatcherStackAddr,
            pObjectRefArray,
            pMaybeInteriorPtrArray,
            pByRefMaybeInteriorPtrArray,
            pBufferForArgsArray
            DEBUG_ARG(pDataLocationArray)
            );
    }
    EX_CATCH
    {
        // We got an exception. Make the exception into our result.
        OBJECTREF ppException = GETTHROWABLE();
        GCX_FORBID();
        RecordFuncEvalException( pDE, ppException);
    }
    // Note: we need to swallow all exceptioins here because they all get reported as the result of
    // the funceval.  If a ThreadAbort occured other than for a funcEval abort, we'll re-throw it manually.
    EX_END_CATCH(SwallowAllExceptions);

    // Restore context
    END_DOMAIN_TRANSITION;

    GCPROTECT_END();    // pByRefMaybeInteriorPtrArray
    GCPROTECT_END();    // pMaybeInteriorPtrArray
    GCPROTECT_END();    // pObjectRefArray
    LOG((LF_CORDB, LL_EVERYTHING, "DoNormalFuncEval: returning...\n"));
}


void FuncEvalHijackRealWorker(DebuggerEval *pDE, Thread* pThread, FuncEvalFrame* pFEFrame)
{
    BYTE * pCatcherStackAddr = (BYTE*) pFEFrame;

    // Handle normal func evals in DoNormalFuncEval
    if ((pDE->m_evalType == DB_IPCE_FET_NEW_OBJECT) || (pDE->m_evalType == DB_IPCE_FET_NORMAL))
    {
        GCProtectArgsAndDoNormalFuncEval(pDE, pCatcherStackAddr);
        LOG((LF_CORDB, LL_EVERYTHING, "DoNormalFuncEval has returned.\n"));
        return;
    }
    
    // The method may be in a different AD than the thread.
    // The RS already verified that all of the arguments are in the same appdomain as the function
    // (because we can't verify it here).
    // Note that this is exception safe, so we are guarenteed to be in the correct AppDomain when
    // we leave this method.
    // Before this, we can't safely use the DebuggerModule* since the domain may have been unloaded.
    ENTER_DOMAIN_ID( pDE->m_appDomainId );

    // Wrap everything in a EX_TRY so we catch any exceptions that could be thrown.
    // Note that we don't let any thrown exceptions cross the AppDomain boundary because we don't 
    // want them to get marshalled.
    EX_TRY
    {
        DebuggerIPCE_TypeArgData *firstdata = pDE->GetTypeArgData();
        DWORD nGenericArgs = pDE->m_genericArgsCount;
        TypeHandle *pGenericArgs = (nGenericArgs == 0) ? NULL : (TypeHandle *) _alloca(nGenericArgs * sizeof (TypeHandle *));
        //
        // Snag the type arguments from the input and get the
        // method desc that corresponds to the instantiated desc.
        //
        Debugger::TypeDataWalk walk(firstdata, pDE->m_genericArgsNodeCount);
        walk.ReadTypeHandles(nGenericArgs, pGenericArgs);

        if (!walk.Finished())
            COMPlusThrow(kArgumentException, L"Argument_InvalidGenericArg");

        switch (pDE->m_evalType)
        {
        case DB_IPCE_FET_NEW_OBJECT_NC:
            {

                // Find the class.
                TypeHandle thClass = g_pEEInterface->LoadClass(pDE->m_debuggerModule->GetRuntimeModule(),
                                                         pDE->m_classToken);

                if (thClass.IsNull())
                    COMPlusThrow(kArgumentNullException, L"ArgumentNull_Type");

                // Apply any type arguments
                TypeHandle th =
                    (nGenericArgs == 0)
                    ? thClass
                    : g_pEEInterface->LoadInstantiation(pDE->m_debuggerModule->GetRuntimeModule(),
                                                         pDE->m_classToken, nGenericArgs, pGenericArgs);

                if (th.IsNull() || th.ContainsGenericVariables())
                    COMPlusThrow(kArgumentException, L"Argument_InvalidGenericArg");

                // Run the Class Init for this type, if necessary.
                th.GetMethodTable()->CheckRunClassInitThrowing();

                // Create a new instance of the class
                OBJECTREF newObj = NULL;
                GCPROTECT_BEGIN(newObj);

                ValidateFuncEvalReturnType(DB_IPCE_FET_NEW_OBJECT_NC, th.GetMethodTable());

                th.GetMethodTable()->EnsureInstanceActive();
                newObj = AllocateObject(th.GetMethodTable());

                // No exception, so it worked.
                pDE->m_successful = true;

                // So is the result type.
                pDE->m_resultType = th;

                //
                // Box up all returned objects
                //
                pDE->m_retValueBoxing = Debugger::AllBoxed;

                // Make a strong handle for the result.
                OBJECTHANDLE oh = pDE->m_thread->GetDomain()->CreateStrongHandle(newObj);
                pDE->m_result = (INT64)(LONG_PTR)oh;
                pDE->m_objectHandle.Set(oh);
                GCPROTECT_END();

                break;
            }

        case DB_IPCE_FET_NEW_STRING:
            {
                // Create the string. m_argData is not necessarily null terminated...
                // The numeration parameter represents the string length, not the buffer size, but
                // we have passed the buffer size across to copy our data properly, so must divide back out.
                // NewString will return NULL if pass null, but want an empty string in that case, so
                // just create an EmptyString explicitly.
                STRINGREF sref = NULL;
                if ((pDE->m_argData == NULL) || (pDE->m_stringSize == 0))
                {
                    sref = COMString::GetEmptyString();
                }
                else
                {
                    sref = COMString::NewString(pDE->GetNewStringArgData(), pDE->m_stringSize/sizeof(WCHAR));
                }
                GCPROTECT_BEGIN(sref);

                // No exception, so it worked.
                pDE->m_successful = true;

                // Result type is, of course, a string.
                pDE->m_resultType = sref->GetTypeHandle();

                // Place the result in a strong handle to protect it from a collection.
                OBJECTHANDLE oh = pDE->m_thread->GetDomain()->CreateStrongHandle((OBJECTREF) sref);
                pDE->m_result = (INT64)(LONG_PTR)oh;
                pDE->m_objectHandle.Set(oh);

                GCPROTECT_END();

                break;
            }

        case DB_IPCE_FET_NEW_ARRAY:
            {
                OBJECTREF arr = NULL;
                GCPROTECT_BEGIN(arr);

                if (pDE->m_arrayRank > 1)
                    COMPlusThrow(kRankException, L"Rank_MultiDimNotSupported");

                // Grab the elementType from the arg/data area.
                _ASSERTE(nGenericArgs == 1);
                TypeHandle th = pGenericArgs[0];

                CorElementType et = th.GetSignatureCorElementType();
                // Gotta be a primitive, class, or System.Object.
                if (((et < ELEMENT_TYPE_BOOLEAN) || (et > ELEMENT_TYPE_R8)) &&
                    !IsElementTypeSpecial(et))
                {
                    COMPlusThrow(kArgumentOutOfRangeException, L"ArgumentOutOfRange_Enum");
                }

                // Grab the dims from the arg/data area.  These come after the type arguments.
                SIZE_T *dims;
                dims = (SIZE_T*) (firstdata + pDE->m_genericArgsNodeCount);

                if (IsElementTypeSpecial(et))
                {
                    arr = AllocateObjectArray(dims[0], th);
                }
                else
                {
                    // Create a simple array. Note: we can only do this type of create here due to the checks above.
                    arr = AllocatePrimitiveArray(et, dims[0]);
                }

                // No exception, so it worked.
                pDE->m_successful = true;

                // Result type is, of course, the type of the array.
                pDE->m_resultType = arr->GetTypeHandle();

                // Place the result in a strong handle to protect it from a collection.
                OBJECTHANDLE oh = pDE->m_thread->GetDomain()->CreateStrongHandle(arr);
                pDE->m_result = (INT64)(LONG_PTR)oh;
                pDE->m_objectHandle.Set(oh);

                GCPROTECT_END();

                break;
            }

        default:
            _ASSERTE(!"Invalid eval type!");
        }
    }
    EX_CATCH
    {
        // We got an exception. Make the exception into our result.
        OBJECTREF ppException = GETTHROWABLE();
        GCX_FORBID();
        RecordFuncEvalException( pDE, ppException);
    }
    // Note: we need to swallow all exceptioins here because they all get reported as the result of
    // the funceval.  If a ThreadAbort occured other than for a funcEval abort, we'll re-throw it manually.
    EX_END_CATCH(SwallowAllExceptions);

    //
    // Restore context
    //
    END_DOMAIN_TRANSITION;

}

//
// FuncEvalHijackWorker is the function that managed threads start executing in order to perform a function
// evaluation. Control is transfered here on the proper thread by hijacking that that's IP to this method in
// Debugger::FuncEvalSetup. This function can also be called directly by a Runtime thread that is stopped sending a
// first or second chance exception to the Right Side.
//
// The DebuggerEval object may get deleted by the helper thread doing a CleanupFuncEval while this thread is blocked
// sending the eval complete.
void *FuncEvalHijackWorker(DebuggerEval *pDE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        GC_TRIGGERS;
        THROWS;
        SO_NOT_MAINLINE;

        PRECONDITION(CheckPointer(pDE));
    }
    CONTRACTL_END;

    Thread *pThread = NULL;
    CONTEXT *filterContext = NULL;

    {
        GCX_FORBID();

        LOG((LF_CORDB, LL_INFO100000, "D:FEHW for pDE:%08x evalType:%d\n", pDE, pDE->m_evalType));

        pThread = GetThread();

#ifndef DACCESS_COMPILE
#ifdef _DEBUG
        //
        // Flush all debug tracking information for this thread on object refs as it 
        // only approximates proper tracking and may have stale data, resulting in false
        // positives.  We dont want that as func-eval runs a lot, so flush them now.
        //
        g_pEEInterface->ObjectRefFlush(pThread);
#endif
#endif
        //
        // From this point forward we use FORBID regions to guard against GCs.
        //
        g_pDebugger->DecThreadsAtUnsafePlaces();

        // Preemptive GC is disabled at the start of this method.
        _ASSERTE(g_pEEInterface->IsPreemptiveGCDisabled());

        DebuggerController::DispatchFuncEvalEnter(pThread);


        // If we've got a filter context still installed, then remove it while we do the work...
        filterContext = g_pEEInterface->GetThreadFilterContext(pDE->m_thread);

        if (filterContext)
        {
            _ASSERTE(pDE->m_evalDuringException);
            g_pEEInterface->SetThreadFilterContext(pDE->m_thread, NULL);
        }

    }

    //
    // Special handling for a re-abort eval. We don't setup a EX_TRY or try to lookup a function to call. All we do
    // is have this thread abort itself.
    //
    if (pDE->m_evalType == DB_IPCE_FET_RE_ABORT)
    {
        //
        // Push our FuncEvalFrame. The return address is equal to the IP in the saved context in the DebuggerEval. The
        // m_Datum becomes the ptr to the DebuggerEval. The frame address also serves as the address of the catch-handler-found.
        //
        FrameWithCookie<FuncEvalFrame> FEFrame(pDE, (void*)GetIP(&pDE->m_context), false);
        FEFrame.Push();

        pDE->m_thread->UserAbort(pDE->m_requester, EEPolicy::TA_Safe, INFINITE, Thread::UAC_Normal);
        _ASSERTE(!"Should not return from UserAbort here!");
        return NULL;
    }

    //
    // We cannot scope the following in a GCX_FORBID(), but we would like to.  But we need the frames on the
    // stack here, so they must never go out of scope.
    //

    //
    // Push our FuncEvalFrame. The return address is equal to the IP in the saved context in the DebuggerEval. The
    // m_Datum becomes the ptr to the DebuggerEval. The frame address also serves as the address of the catch-handler-found.
    //
    FrameWithCookie<FuncEvalFrame> FEFrame(pDE, (void*)GetIP(&pDE->m_context), true);
    FEFrame.Push();

    FuncEvalHijackRealWorker(pDE, pThread, &FEFrame);

    LOG((LF_CORDB, LL_EVERYTHING, "FuncEval has finished its primary work.\n"));

    //
    // The func-eval is now completed, successfully or with failure, aborted or run-to-completion.
    //
    pDE->m_completed = true;

    if (pDE->m_thread->IsAbortRequested())
    {
        //
        // Check if an unmanaged thread tried to also abort this thread while we
        // were doing the func-eval, then that kind we want to rethrow. The check
        // versus m_aborted is for the case where the FE was aborted, we caught that, 
        // then cleared the FEAbort request, but there is still an outstanding abort
        // - then it must be a user abort.
        //
        if ((pDE->m_aborting == DebuggerEval::FE_ABORT_NONE) || pDE->m_aborted)
        {
            pDE->m_rethrowAbortException = true;
        }
        
        //
        // Reset the abort request if a func-eval abort was submitted, but the func-eval completed
        // before the abort could take place, we want to make sure we do not throw an abort exception
        // in this case.
        //
        if (pDE->m_aborting != DebuggerEval::FE_ABORT_NONE)
        {
            pDE->m_thread->UserResetAbort(Thread::TAR_FuncEval);
        }

    }

    // Codepitching can hijack our frame's return address. That means that we'll need to update PC in our saved context
    // so that when its restored, its like we've returned to the codepitching hijack. At this point, the old value of
    // EIP is worthless anyway.
    if (!pDE->m_evalDuringException)
    {
        SetIP(&pDE->m_context, (LPVOID)(SIZE_T)FEFrame.GetReturnAddress());
    }

    //
    // Disable all steppers and breakpoints created during the func-eval
    //
    DebuggerController::DispatchFuncEvalExit(pThread);

    void *dest = NULL;

    if (!pDE->m_evalDuringException)
    {
        // Signal to the helper thread that we're done with our func eval.  Start by creating a DebuggerFuncEvalComplete
        // object. Give it an address at which to create the patch, which is a chunk of memory inside of our
        // DebuggerEval big enough to hold a breakpoint instruction.
        dest = &(pDE->m_breakpointInstruction);

        // Here is kind of a cheat... we make sure that the address that we patch and jump to is actually also the ptr
        // to our DebuggerEval. This works because m_breakpointInstruction is the first field of the DebuggerEval
        // struct.
        _ASSERTE(dest == pDE);

        //
        // The created object below sets up itself as a hijack and will destroy itself when the hijack and work
        // is done.
        //


        DebuggerFuncEvalComplete *comp;
        comp = new (interopsafe) DebuggerFuncEvalComplete(pThread, dest);
        _ASSERTE(comp != NULL); // would have thrown


        // Pop the FuncEvalFrame now that we're pretty much done. Make sure we
        // don't pop the frame too early. Because GC can be triggered in our grabbing of
        // Debugger lock. If we pop the FE frame without setting back thread filter context,
        // the frames left uncrawlable.
        //
        FEFrame.Pop();
    }
    else
    {
        // We don't have to setup any special hijacks to return from here when we've been processing during an
        // exception. We just go ahead and send the FuncEvalComplete event over now. Don't forget to enable/disable PGC
        // around the call...
        _ASSERTE(g_pEEInterface->IsPreemptiveGCDisabled());

        if (filterContext != NULL)
        {
            g_pEEInterface->SetThreadFilterContext(pDE->m_thread, filterContext);
        }

        // Pop the FuncEvalFrame now that we're pretty much done.
        FEFrame.Pop();


        g_pEEInterface->EnablePreemptiveGC();

        //
        // This also grabs the debugger lock, so we can atomically check if a detach has
        // happened.
        //
        Debugger::DebuggerLockHolder dbgLockHolder(g_pDebugger, FALSE);

        g_pDebugger->LockForEventSending(&dbgLockHolder);

        if ((pDE->m_thread->GetDomain() != NULL) && pDE->m_thread->GetDomain()->IsDebuggerAttached())
        {

            if (CORDebuggerAttached()) 
            {
                g_pDebugger->FuncEvalComplete(pDE->m_thread, pDE);

                g_pDebugger->SyncAllThreads(&dbgLockHolder);
            }

        }

        g_pDebugger->UnlockFromEventSending(&dbgLockHolder);

        g_pEEInterface->DisablePreemptiveGC();
    }


    // pDE may now point to deleted memory if the helper thread did a CleanupFuncEval while we
    // were blocked waiting for a continue after the func-eval complete.

    // We return the address that we want to resume executing at.
    return dest;

}


unsigned FuncEvalFrame::GetFrameAttribs(void)
{
    LEAF_CONTRACT;

    if (GetDebuggerEval()->m_evalDuringException)
        return FRAME_ATTR_NONE;
    else
        return FRAME_ATTR_RESUMABLE;    // Treat the next frame as the top frame.
}


TADDR FuncEvalFrame::GetReturnAddressPtr()
{
    LEAF_CONTRACT;

    if (GetDebuggerEval()->m_evalDuringException)
        return NULL;
    else
        return (TADDR)&m_ReturnAddress;
}

//
// This updates the register display for a FuncEvalFrame.
//
void FuncEvalFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    DebuggerEval *pDE = GetDebuggerEval();

    // No context to update if we're doing a func eval from within exception processing.
    if (pDE->m_evalDuringException)
        return;

    // Reset pContext; it's only valid for active (top-most) frame.
    pRD->pContext = NULL;


#ifdef _X86_
    // Update all registers in the reg display from the CONTEXT we stored when the thread was hijacked for this func
    // eval. We have to update all registers, not just the callee saved registers, because we can hijack a thread at any
    // point for a func eval, not just at a call site.
    pRD->pEdi = &(pDE->m_context.Edi);
    pRD->pEsi = &(pDE->m_context.Esi);
    pRD->pEbx = &(pDE->m_context.Ebx);
    pRD->pEdx = &(pDE->m_context.Edx);
    pRD->pEcx = &(pDE->m_context.Ecx);
    pRD->pEax = &(pDE->m_context.Eax);
    pRD->pEbp = &(pDE->m_context.Ebp);
    pRD->Esp  = (DWORD)GetSP(&pDE->m_context);
    pRD->pPC  = (SLOT*)GetReturnAddressPtr();
    pRD->PCTAddr = GetReturnAddressPtr();

#elif defined(_PPC_)
    // update all registers in the reg display
    for (int i=0; i<NUM_CALLEESAVED_REGISTERS; i++)
        pRD->pR[i] = ((DWORD *)&(pDE->m_context.Gpr13)) + i;

    for (int i=0; i<NUM_FLOAT_CALLEESAVED_REGISTERS; i++)
        pRD->pF[i] = &(pDE->m_context.Fpr14) + i;

    pRD->CR = pDE->m_context.Cr;
    pRD->SP  = (DWORD)GetSP(&pDE->m_context);
    pRD->pPC  = (SLOT*)GetReturnAddressPtr();

#else  // !_X86_ && !_PPC_ && !_IA64_ && !_AMD64_
    PORTABILITY_ASSERT("FuncEvalFrame::UpdateRegDisplay is not implemented on this platform.");
#endif // !_X86_ && !_PPC_ && !_IA64_ && !_AMD64_
}


#endif // ifndef DACCESS_COMPILE
