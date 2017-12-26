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
//
// verifier.cpp
//
//
// Registry / Environment settings :
//
//      Create registry entries in CURRENT_USER\Software\Microsoft\.NETFramework
//      or set environment variables COMPlus_* with the names given below. 
//      Environment settings override registry settings.
//
//      For breaking into the debugger / Skipping verification :
//          (available only in the debug build).
//
//      VerBreakOnError  [STRING]    Break into the debugger on error. Set to 1
//      VerSkip          [STRING]    method names (case sensitive)
//      VerBreak         [STRING]    method names (case sensitive)
//      VerOffset        [STRING]    Offset in the method in hex
//      VerPass          [STRING]    1 / 2 ==> First pass, second pass
//      VerMsgMethodInfoOff [STRING]    Print method / module info on error
//
//      NOTE : If there are more than one methods in the list and an offset
//      is specified, this offset is applicable to all methods in the list
//    
//      NOTE : Verifier should be enabled for this to work.
//
//      To Switch the verifier Off (Default is On) :
//          (available on all builds).
//
//      VerifierOff     [STRING]    1 ==> Verifier is Off, 0 ==> Verifier is On 
//
//      [See EEConfig.h / EEConfig.cpp]
//
//
// Meaning of code marked with @XXX
//
//      @VER_ASSERT : Already verified.
//      @VER_IMPL   : Verification rules implemented here.
//      @DEBUG      : To be removed/commented before checkin.
//

#include "common.h"

#include "verifier.hpp"
#include "ceeload.h"
#include "clsload.hpp"
#include "method.hpp"
#include "vars.hpp"
#include "object.h"
#include "field.h"
#include "comdelegate.h"
#include "security.h"
#include "dbginterface.h"
#include "securityattributes.h"
#include "eeconfig.h"
#include "sourceline.h"
#include "typedesc.h"
#include "typestring.h"
#include "../dlls/mscorrc/resource.h"


#define VER_NAME_INFO_SIZE  128
#define VER_SMALL_BUF_LEN 256
#define VER_FAILED_TO_LOAD_RESOURCE_STRING "(Failed to load resource string)"

#define VER_LD_RES(e, fld)                                              \
    {                                                                   \
        if ((sRes.LoadResource(e)))                                     \
        {                                                               \
            sPrint.Printf(sRes.GetUnicode(), err.fld);                  \
            sMessage += sPrint;                                         \
        }                                                               \
        else                                                            \
        {                                                               \
            sMessage += SString(SString::Ascii, VER_FAILED_TO_LOAD_RESOURCE_STRING);    \
        }                                                               \
    }

// Copies the error message to the input char*
WCHAR* Verifier::GetErrorMsg(
        HRESULT hrError,
        VerError err,
        __inout_ecount(len) WCHAR *wszMsg, 
        int len,
        ValidateWorkerArgs* pArgs)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    SString sMessage;   // to debug, watch "(WCHAR*)sMessage.m_buffer"
    SString sPrint;

    NewHolder<SourceLine> pSL(NULL);

    if (pArgs->pMethodDesc)
    {
        // source lines
        if (pArgs->fShowSourceLines && pArgs->wszFileName)
        {
            pSL = new SourceLine(pArgs->wszFileName);
            if(pSL->IsInitialized())
            {
                DWORD dwFunctionToken = pArgs->pMethodDesc->GetMemberDef();
                WCHAR wcBuffer[VER_SMALL_BUF_LEN];
                wcBuffer[0] = 0;
                DWORD dwLineNumber;
                HRESULT hr;
                hr = pSL->GetSourceLine( dwFunctionToken, err.dwOffset, wcBuffer, VER_SMALL_BUF_LEN, &dwLineNumber );
                sPrint.Printf(L"%s(%d) : ", wcBuffer, dwLineNumber);
                sMessage += sPrint;
            }
            SString sRes;
            sRes.LoadResource(IDS_VER_E_ILERROR);
            sMessage += sRes;
        }

        // module
        sMessage += L"[";
        sMessage += pArgs->pMethodDesc->GetModule()->GetPath();

        // class
        sMessage += L" : ";
        if (pArgs->pMethodDesc->GetMethodTable() != NULL)
        {
            //            DefineFullyQualifiedNameForClass();
            // GetFullyQualifiedNameForClassNestedAware(pClass);
            // sMessage += FilterAscii(_szclsname_, szTemp, VER_NAME_INFO_SIZE);
            SString clsname;
            TypeString::AppendType(clsname,TypeHandle(pArgs->pMethodDesc->GetMethodTable()));
            sMessage += clsname;
        }
        else
        {
            SString sRes;
            sRes.LoadResource(IDS_VER_E_GLOBAL);
            sMessage += sRes;
        }

        // method
        sMessage += L"::";
        sMessage += SString(SString::Utf8, pArgs->pMethodDesc->GetModule()->GetMDImport()->GetNameOfMethodDef(pArgs->pMethodDesc->GetMemberDef()));

        if (pArgs->pMethodDesc->IsGenericMethodDefinition())
        {
            SString inst;
            TypeString::AppendInst(inst,pArgs->pMethodDesc->GetNumGenericMethodArgs(),pArgs->pMethodDesc->GetMethodInstantiation(),TypeString::FormatBasic);
            sMessage += inst;
        }

        sMessage += L"]";

        // MD token
        if(pArgs->fVerbose)
        {
            SString sRes;
            sRes.LoadResource(IDS_VER_E_MDTOKEN);
            DWORD dwMDToken = pArgs->pMethodDesc->GetMemberDef();
            sPrint.Printf(sRes.GetUnicode(), dwMDToken);
            sMessage += sPrint;
        }
    }

    // Fill In the details
    SString sRes;

    // Create the generic error fields

    if (err.dwFlags & VER_ERR_OFFSET)
        VER_LD_RES(VER_E_OFFSET, dwOffset);

    if (err.dwFlags & VER_ERR_OPCODE)
    {
        if (sRes.LoadResource(VER_E_OPCODE))
        {
            sPrint.Printf(sRes, ppOpcodeNameList[err.opcode]);
            sMessage += L" ";
            sMessage += sPrint;
        }
    }

    if (err.dwFlags & VER_ERR_OPERAND)
        VER_LD_RES(VER_E_OPERAND, dwOperand);

    if (err.dwFlags & VER_ERR_TOKEN)
        VER_LD_RES(VER_E_TOKEN, token);

    if (err.dwFlags & VER_ERR_EXCEP_NUM_1)
        VER_LD_RES(VER_E_EXCEPT, dwException1);

    if (err.dwFlags & VER_ERR_EXCEP_NUM_2)
        VER_LD_RES(VER_E_EXCEPT, dwException2);

    if (err.dwFlags & VER_ERR_STACK_SLOT)
        VER_LD_RES(VER_E_STACK_SLOT, dwStackSlot);

    if ((err.dwFlags & VER_ERR_SIG_MASK) == VER_ERR_LOCAL_SIG)
    {
        if (err.dwVarNumber != VER_ERR_NO_LOC)
        {
            if(pArgs->fShowSourceLines && pSL && pSL->IsInitialized() && pArgs->pMethodDesc)
            {
                if ((sRes.LoadResource(VER_E_LOC_BYNAME)))
                {
                    DWORD dwFunctionToken = pArgs->pMethodDesc->GetMemberDef();
                    WCHAR wcBuffer[VER_SMALL_BUF_LEN];
                    wcBuffer[0] = 0;
                    HRESULT hr;
                    hr = pSL->GetLocalName(dwFunctionToken, err.dwVarNumber, wcBuffer, VER_SMALL_BUF_LEN);
                    sPrint.Printf(sRes.GetUnicode(), wcBuffer);
                }
                else
                {
                    sPrint = SString(SString::Ascii, VER_FAILED_TO_LOAD_RESOURCE_STRING);
                }
            }
            else
            {
                if ((sRes.LoadResource(VER_E_LOC)))
                    sPrint.Printf(sRes.GetUnicode(), err.dwVarNumber);
                else
                {
                    sPrint = SString(SString::Ascii, VER_FAILED_TO_LOAD_RESOURCE_STRING);
                }
            }
            sMessage += sPrint;
        }
    }

    if ((err.dwFlags & VER_ERR_SIG_MASK) == VER_ERR_FIELD_SIG)
    {
        if (sRes.LoadResource(VER_E_FIELD_SIG))
        {
            sMessage += L"  ";
            sMessage += sRes;
        }
    }

    if (((err.dwFlags & VER_ERR_SIG_MASK) == VER_ERR_METHOD_SIG) ||
        ((err.dwFlags & VER_ERR_SIG_MASK) == VER_ERR_CALL_SIG))
    {
        if (err.dwArgNumber != VER_ERR_NO_ARG)
        {
            if (err.dwArgNumber != VER_ERR_ARG_RET)
            {
                VER_LD_RES(VER_E_ARG, dwArgNumber);
            }
            else if (sRes.LoadResource(VER_E_RET_SIG))
            {
                sMessage += L"  ";
                sMessage += sRes;
            }
        }
    }

    if (err.dwFlags & VER_ERR_TYPE_1)
        sMessage += err.wszType1;

    if (err.dwFlags & VER_ERR_TYPE_2)
        sMessage += err.wszType2;

    if (err.dwFlags & VER_ERR_ADDL_MSG)
        sMessage += err.wszAdditionalMessage;

    if (err.dwFlags & VER_ERR_TYPE_F)
    {
        if (sRes.LoadResource(VER_E_FOUND))
        {
            sPrint.Printf(sRes, err.wszTypeFound);
            sMessage += sPrint;
        }
    }

    if (err.dwFlags & VER_ERR_TYPE_E)
    {
        if (sRes.LoadResource(VER_E_EXPECTED))
        {
            sPrint.Printf(sRes, err.wszTypeExpected);
            sMessage += sPrint;
        }
    }

    //  Handle the special cases
    switch (hrError)
    {
    case VER_E_UNKNOWN_OPCODE:
        VER_LD_RES(VER_E_UNKNOWN_OPCODE, opcode);
        break;

    case VER_E_SIG_CALLCONV:
        VER_LD_RES(VER_E_SIG_CALLCONV, bCallConv);
        break;

    case VER_E_SIG_ELEMTYPE:
        VER_LD_RES(VER_E_SIG_ELEMTYPE, elem);
        break;

    case COR_E_ASSEMBLYEXPECTED:
        Verifier::GetAssemblyName(hrError,sMessage, sRes, sPrint, pArgs);
        break;

    case SECURITY_E_UNVERIFIABLE:
        Verifier::GetAssemblyName(hrError,sMessage, sRes, sPrint, pArgs);
        break;

    case CORSEC_E_MIN_GRANT_FAIL:
        Verifier::GetAssemblyName(hrError,sMessage, sRes, sPrint, pArgs);
        break;

    case HRESULT_FROM_WIN32(ERROR_BAD_FORMAT):
        // fall through

    default:
        Verifier::GetDefaultMessage(hrError,sMessage, sRes, sPrint);
    }

    wcsncpy_s(wszMsg, len, sMessage.GetUnicode(), _TRUNCATE);
    return wszMsg;
}

/*static*/ VOID Verifier::GetDefaultMessage(HRESULT hrError, SString& sMessage, SString& sRes, SString& sPrint)
{
    if (sMessage.GetCount() > 0)
        sMessage += L" ";

    if (HRESULT_FACILITY(hrError) == FACILITY_URT && sRes.LoadResource(hrError, TRUE))
        sMessage += sRes;
    else
    {
        WCHAR win32Msg[VER_SMALL_BUF_LEN];
        BOOL useWin32Msg = WszFormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | 
                                             FORMAT_MESSAGE_IGNORE_INSERTS,
                                             NULL,
                                             hrError,
                                             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                                             (LPTSTR) win32Msg,
                                             VER_SMALL_BUF_LEN - 1,
                                             NULL );

        if (sRes.LoadResource(VER_E_HRESULT))
        {
            sPrint.Printf(sRes, hrError);

            if (useWin32Msg)
            {
                sPrint += L" - ";
                sPrint += win32Msg;
            }

            sMessage += L" ";
            sMessage += sPrint;
        }
        else
        {
            sMessage += SString(SString::Ascii, VER_FAILED_TO_LOAD_RESOURCE_STRING);
        }
    }
}

/*static*/ HRESULT Verifier::ReportError(IVEHandler *pVeh, HRESULT hrError, VEContext* pVec, ValidateWorkerArgs* pArgs)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    // Filter out error messages that require parameters
    switch(hrError)
    {
        case COR_E_TYPELOAD: hrError = VER_E_TYPELOAD; break;
    }

    // There is no room for expansion in the VEHandler interface, so we're
    // stuffing our extra data into the SafeArray that was originally
    // designed to be used only by the MDValidator.

    // Note: VT_VARIANT is the only supported safe array type on Rotor
    SAFEARRAY* pSafeArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    _ASSERTE(pSafeArray);
    if (pSafeArray)
    {
        VARIANT var;
        V_VT(&var) = VT_UINT; // machine sized int
        V_UINT_PTR(&var) = (ULONG_PTR)(pArgs);
        LONG i = 0;
        HRESULT hrPutElement;
        hrPutElement = SafeArrayPutElement(pSafeArray, &i, &var);
        _ASSERTE(hrPutElement == S_OK);
    }

    // Call the handler
    HRESULT hr = pVeh->VEHandler(hrError, *pVec, pSafeArray);

    // Clean up the SafeArray we allocated
    HRESULT hrDestroy;
    hrDestroy = SafeArrayDestroy(pSafeArray);
    _ASSERTE(hrDestroy == S_OK);

    return hr;
}

/*static*/ VOID Verifier::GetAssemblyName(HRESULT hrError, SString& sMessage, SString& sRes, SString& sPrint, ValidateWorkerArgs* pArgs)
{
    if(sRes.LoadResource(hrError, TRUE))
    {
        // find the '%1'
        SString::Iterator i = sRes.Begin();
        if (sRes.Find(i, L"'%1'"))
        {
            // replace the '%1' with the module name
            if(pArgs->wszFileName)
            {
                sPrint = pArgs->wszFileName;
                sRes.Replace(i + 1, 2, sPrint);
            }
            else
            {
                sPrint = L"";
                sRes.Replace(i, 4, sPrint);
            }
            sMessage += sRes;
        }
    }
    else
    {
        sMessage += SString(SString::Ascii, VER_FAILED_TO_LOAD_RESOURCE_STRING);
    }
}
