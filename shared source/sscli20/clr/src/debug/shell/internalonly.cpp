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
//*****************************************************************************
// File: InternalOnly.cpp
//
//*****************************************************************************
#include "stdafx.h"

// The Shell project is for internal testing purposes only at this point.
// We are therefore not going to scrub the code.  It should be moved out
// of the dev project longer term.


#include "cordbpriv.h"
#include "internalonly.h"

class StepDebuggerCommand;

// Format for a CORDB_ADDRESS (which is always 64-bits long, even on 32-bit).
#define PTR_FORMAT L"0x%08I64x"

//-----------------------------------------------------------------------------
// Call IsTransitionStub at an address.
//-----------------------------------------------------------------------------
IsTransitionStubCommand::IsTransitionStubCommand(const WCHAR *name, int minMatchLength)
    : DebuggerCommand(name, minMatchLength)
{
}

void IsTransitionStubCommand::Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args)
{
    unsigned __int64 addr;
    bool fValid = shell->GetInt64Arg(args, addr);

    if (!fValid)
    {
        shell->Write(L"Invalid argument. Expected address.\n");
        return;
    }

    if (shell->m_currentProcess == NULL)
    {
        shell->Error(L"Process not running.\n");
        return;
    }

    CORDB_ADDRESS a = (CORDB_ADDRESS) addr;
    BOOL fStub;
    HRESULT hr = shell->m_currentProcess->IsTransitionStub(a, &fStub);

    if (FAILED(hr))
    {
        shell->Write(L"Call to IsTransitionStub at address %p failed with hr=%08x.\n", CORDB_ADDRESS_TO_PTR(a), hr);
    }
    else
    {
        shell->Write(L"Address %p '%s' a stub\n", CORDB_ADDRESS_TO_PTR(a), (fStub ? L"IS" : L"IS NOT"));
    }
}

void IsTransitionStubCommand::Help(Shell *shell)
{
    ShellCommand::Help(shell);
    shell->Write(L"<address>\n");
    shell->Write(L"Determines if the given address is a transition stub.\n");
}
const WCHAR *IsTransitionStubCommand::ShortHelp(Shell *shell)
{
    return L"determine if address is a stub";
}

//-----------------------------------------------------------------------------
PrintGCHandleCommand::PrintGCHandleCommand(const WCHAR *name, int minMatchLength)
    : DebuggerCommand(name, minMatchLength)
    {}

void PrintGCHandleCommand::Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args)
{

    if (shell->m_currentProcess == NULL)
    {
        shell->Error(L"Process not running.\n");
        return;
    }

    // Get arg
    // 2 forms:
    // printgc 0x01234567
    //    look at GC handle supplied as literal (0x01234567)
    //
    // printgc var 1
    //    look at GC handle in the nth DWORD in the var. eg, (DWORD*) (&var + 1)
    //
    unsigned __int64 val;
    UINT_PTR val2 = 0;
    bool fValid = shell->GetInt64Arg(args, val);

    HRESULT hr = S_OK;

    if (fValid)
    {
        // First form - directly supplied an address
        val2 = (UINT_PTR) val;
    }
    else
    {
        // 2nd form - var lookup.
        // Get a string and an int.
        const WCHAR *varTemp = NULL;
        int idx = 0;

        if (!shell->GetStringArg(args, varTemp) || (args == varTemp))
        {
            Help(shell);
            return;
        }

        DebuggerString varName;
        if (!varName.CopyNFrom(varTemp, (int) (args-varTemp)))
        {
            shell->Error(L"Out of memory.\n");
            return;
        }

        if (!shell->GetIntArg(args, idx))
        {
            Help(shell);
            return;
        }

        // Now resolve varName so that we can go look at its memory.
        // Get the value for the name provided.
        ICorDebugValue *ivalue;
        ivalue = shell->EvaluateExpression(varName,
                                           shell->m_currentFrame,
                                           true);
        if (ivalue == NULL)
        {
            shell->Error(L"Can't resolve variable.\n");
            return;
        }

        ReleaseHolder<ICorDebugGenericValue> pgv;

        hr = ivalue->QueryInterface(IID_ICorDebugGenericValue, (void**)&pgv);
        ivalue->Release();

        if (pgv == NULL)
        {
            shell->Error(L"Variable is not an opaque blob.\n");
            return;
        }


        ULONG32 objSize = 0;
        hr = pgv->GetSize(&objSize);

        if (idx * sizeof(UINT_PTR) >= objSize)
        {
            shell->Error(L"Index 0x%x is out of range. Object size is only 0x%x bytes.\n", idx, objSize);
            return;
        }
        BYTE *objContents = new BYTE[objSize];
        if (objContents == NULL)
        {
            shell->Error(L"Out of memory.\n");
            return;
        }
        hr = pgv->GetValue(objContents);
        if (FAILED(hr))
        {
            shell->Error(L"Failed to get object contents. hr=0x%08x\n", hr);
            delete [] objContents;
            return;
        }

        val2 = *(((UINT_PTR*) objContents)+idx);
        delete [] objContents;
    }

    // By now, val2 is set to our GC handle.
    shell->Write(L"Inspecting GC Handle 0x%08x.\n", val2);


    ReleaseHolder<ICorDebugReferenceValue> pRef;

    ReleaseHolder<ICorDebugProcess2> pProc2;
    shell->m_currentProcess->QueryInterface(IID_ICorDebugProcess2, reinterpret_cast<void**>(&pProc2));
    if (pProc2 == NULL)
    {
        shell->Error(L"Not supported in pre v2.0");
        return;
    }

    hr = pProc2->GetReferenceValueFromGCHandle(val2, &pRef);

    if (FAILED(hr))
    {
        shell->ReportError(hr);
        return;
    }

    ICorDebugValue * pValue = pRef;
    pValue->AddRef();

    shell->PrintVariable(L"GChandle",
                                  pValue, // passes by reference, may get changed underneath us.
                                  0,
                                  TRUE);

    shell->Write(L"\n");

    if (pValue != NULL)
    {
        pValue->Release();
    }
}
void PrintGCHandleCommand::Help(Shell *shell)
{
    ShellCommand::Help(shell);
    shell->Write(L"[<gchandle> | (<varname> idx)]\n");
    shell->Write(L"Prints the object references by a GC handle.\n");
    shell->Write(L"'gchandle' is the raw bits of a GC handle, such as the contents of a GCRoot<T> object.\n");
    shell->Write(L"printgc 0x1234567\n");
    shell->Write(L"   prints the GC handle at 0x1234567.\n");
    shell->Write(L"printgc foo 4\n");
    shell->Write(L"   prints the GC handle stored in the 4th (0-based) UINT_PTR of the variable 'foo'.\n");
    shell->Write(L"   This form is particularly convenient for inspecting GC handles from MC++ structures.\n");

}
const WCHAR *PrintGCHandleCommand::ShortHelp(Shell *shell)
{
    return L"print GC handle";
}

//-----------------------------------------------------------------------------
// Dump everything in the world we-know about for a given function.
//-----------------------------------------------------------------------------
FuncInfoCommand::FuncInfoCommand(const WCHAR *name, int minMatchLength)
    : DebuggerCommand(name, minMatchLength)
{
}

//-----------------------------------------------------------------------------
// Print Native-code information
//-----------------------------------------------------------------------------
HRESULT FuncInfoCommand::PrintNativeCode(ICorDebugCode * pCode, DebuggerShell *shell)
{
    HRESULT hr = S_OK;

    COR_DEBUG_IL_TO_NATIVE_MAP * pMap = NULL;

    CORDB_ADDRESS addr;
    ULONG32 size;

    ICorDebugCode2 * pCode2 = NULL;
    pCode->QueryInterface(IID_ICorDebugCode2, (void**) &pCode2);
    if (pCode2 != NULL)
    {
        ULONG32 dwSizeNeeded;
        ULONG32 cTotalSize = 0;
        CodeChunkInfo * pChunks = NULL;

        DWORD flags;
        hr = pCode2->GetCompilerFlags(&flags);

        if (FAILED(hr))
        {
            shell->Write(L"Failed getting compiler flags\n");
            goto DonePrintingChunks;
        }
        else
        {
            int fTrack = ((flags & CORDEBUG_JIT_TRACK_DEBUG_INFO) == CORDEBUG_JIT_TRACK_DEBUG_INFO) ? 1 : 0;
            int fDisableOpt = ((flags & CORDEBUG_JIT_DISABLE_OPTIMIZATION) == CORDEBUG_JIT_DISABLE_OPTIMIZATION) ? 1 : 0;
            shell->Write(L"Compiler flags: trackingEnabled: %d, disableOpt: %d:\n", fTrack, fDisableOpt);
        }

        // Get code chunks
        hr = pCode2->GetCodeChunks(0, &dwSizeNeeded, NULL);
        if (FAILED(hr))
        {
            goto DonePrintingChunks;
        }

        pChunks = (CodeChunkInfo*)alloca(dwSizeNeeded * sizeof(CodeChunkInfo));
        hr = pCode2->GetCodeChunks(dwSizeNeeded, &dwSizeNeeded, pChunks);
        if (FAILED(hr))
        {
            goto DonePrintingChunks;
        }

        shell->Write(L"Native code split into '%d' chunk(s):\n", dwSizeNeeded);
        for(ULONG32 i = 0; i < dwSizeNeeded; i++)
        {
            shell->Write(L"  Address:" PTR_FORMAT L", size=%lu (0x%x) bytes\n",
                pChunks[i].startAddr, pChunks[i].length, pChunks[i].length);
            cTotalSize += pChunks[i].length;
        }

        shell->Write(L"Total Size:=%lu (0x%x) bytes\n", cTotalSize, cTotalSize);

DonePrintingChunks:
        pCode2->Release();
        if (FAILED(hr))
        {
            pCode2 = NULL; // causes us to do the non pCode2 path.
            hr = S_OK;
        }
    }

    if (pCode2 == NULL)
    {
        hr = pCode->GetAddress(&addr);
        if (FAILED(hr))
        {
            shell->Write(L"Failed to get address of native code.\n");
            goto Exit;
        }

        hr = pCode->GetSize(&size);
        if (FAILED(hr))
        {
            shell->Write(L"Failed to get size of native code.\n");
            goto Exit;
        }

        shell->Write(L"Native code: address=" PTR_FORMAT L", size=%lu (0x%x) bytes.\n", addr, size, size);
    }

    // Print the IL->Native map.
    ULONG32 num;
    hr = pCode->GetILToNativeMapping(0, &num, NULL);
    if (FAILED(hr))
    {
        shell->Write(L"    Failed to get il->native map size.\n");
        goto Exit;
    }

    pMap = new COR_DEBUG_IL_TO_NATIVE_MAP[num];
    if (pMap == NULL)
    {
        shell->Write(L"    Out of memory when allocating IL->Native map.\n");
        goto Exit;
    }

    ULONG32 num2;
    hr = pCode->GetILToNativeMapping(num, &num2, pMap);
    if (FAILED(hr))
    {
        shell->Write(L"    Failed to read il->native map contents.\n");
        goto Exit;
    }
    _ASSERTE(num == num2);


    // Now print the map.
    for(ULONG32 i = 0; i < num; i++)
    {
        switch(pMap[i].ilOffset)
        {
            case NO_MAPPING:
                shell->Write(L"    [NO_MAPPING (-1)]");
                break;
            case PROLOG:
                shell->Write(L"    [PROLOG (-2)]");
                break;
            case EPILOG:
                shell->Write(L"    [EPILOG (-3)]");
                break;

            default:
                shell->Write(L"    0x%x il", pMap[i].ilOffset);
                break;
        }
        shell->Write(L" --> [0x%0x, 0x%0x) native.\n",
            pMap[i].nativeStartOffset, pMap[i].nativeEndOffset);
    }


    // Should have gone to 'Exit' label if we failed.
    _ASSERTE(SUCCEEDED(hr));

Exit:
    delete [] pMap;

    if (FAILED(hr))
    {
        shell->ReportError(hr);
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Do real work.
//-----------------------------------------------------------------------------
void FuncInfoCommand::Print(
    ICorDebugFunction *pFunc,
    ICorDebugFunction2 *pFunc2,
    DebuggerShell *shell,
    ICorDebug *cor
)
{
    HRESULT hr = S_OK;


    ReleaseHolder<ICorDebugModule> pModule;
    ReleaseHolder<ICorDebugClass>  pClass;
    ReleaseHolder<IMetaDataImport> pImport;
    ReleaseHolder<ICorDebugCode>   pILCode;
    ReleaseHolder<ICorDebugCode>   pNativeCode;

    // Print token
    mdMethodDef tFunction;
    hr = pFunc->GetToken(&tFunction);
    if (FAILED(hr))
    {
        shell->Write(L"Failed to get function token.\n");
        goto Exit;
    }

    // Query for Module & Class
    hr = pFunc->GetModule(&pModule);
    if (FAILED(hr))
    {
        shell->Write(L"Failed to QI for ICorDebugModule.\n");
        goto Exit;
    }

    hr = pFunc->GetClass(&pClass);
    if (FAILED(hr))
    {
        shell->Write(L"Failed to QI for ICorDebugClass.\n");
        goto Exit;
    }

    // Get Code objects (null if not yet-jitted)
    hr = pFunc->GetNativeCode(&pNativeCode);
    if (FAILED(hr) && hr != CORDBG_E_CODE_NOT_AVAILABLE)
    {
        shell->Write(L"Unexpected failure while getting native code.\n");
        goto Exit;
    }



    // Get more tokens
    mdModule tModule;
    hr = pModule->GetToken(&tModule);
    if (FAILED(hr))
    {
        shell->Write(L"Failed to get module token.\n");
        goto Exit;
    }

    mdTypeDef tClass;
    hr = pClass->GetToken(&tClass);
    if (FAILED(hr))
    {
        shell->Write(L"Failed to get class token.\n");
        goto Exit;
    }

    // Get metadata
    // Get the necessary metadata interfaces now...
    hr = pModule->GetMetaDataInterface(IID_IMetaDataImport, (IUnknown**)&pImport);
    if (FAILED(hr))
    {
        shell->Write(L"Failed to get Metadata importer.\n");
        goto Exit;
    }

    // Print out names.
    shell->Write(L"Func-info: [0x%08x] 0x%08x::0x%08x\n", tModule, tClass, tFunction);


    // Print version (for E&C)
    ULONG32 nVersion;
    hr = pFunc->GetCurrentVersionNumber(&nVersion);
    if (FAILED(hr))
    {
        shell->Write(L"Failed to get function version.\n");
        goto Exit;
    }
    shell->Write(L"Function Version:%d\n", nVersion);


    // Calculate flags:
    shell->Write(L"Flags: ");


    if (pFunc2 != NULL)
    {
        // JMC status
        BOOL bIsJMC;
        HRESULT hr2 = pFunc2->GetJMCStatus(&bIsJMC);
        if (SUCCEEDED(hr2))
        {
            shell->Write(bIsJMC ? L" [just-my-code]" : L" [non-user code]");
        }
    }

    // Jitted status
    shell->Write((pNativeCode == NULL) ? L" [unjitted]" : L" [jitted]");



    shell->Write(L"\n");


    // Print the IL Code info
    hr = pFunc->GetILCode(&pILCode);
    if (FAILED(hr))
    {
        shell->Write(L"Couldn't get IL-code.\n");
        goto Exit;
    }

    {
        CORDB_ADDRESS addr;
        ULONG32 size;

        hr = pILCode->GetAddress(&addr);
        if (FAILED(hr))
        {
            shell->Write(L"Failed to get address of IL code.\n");
            goto Exit;
        }

        hr = pILCode->GetSize(&size);
        if (FAILED(hr))
        {
            shell->Write(L"Failed to get size of IL code.\n");
            goto Exit;
        }

        shell->Write(L"IL code: address=" PTR_FORMAT L", size=%lu (0x%x) bytes.\n", addr, size, size);
    }

    // We have no way of enumerating through the different code-blobs if the generic multiple instantation
    // case.
    if (pNativeCode != NULL)
    {
        PrintNativeCode(pNativeCode, shell);
    }



    // Done, if we failed anything, we should have bailed.
    _ASSERTE(SUCCEEDED(hr));

Exit:
    if (FAILED(hr))
    {
        shell->ReportError(hr);
    }

    return;
}

//-----------------------------------------------------------------------------
// Public entry to parse the command line.
//-----------------------------------------------------------------------------
void FuncInfoCommand::Do(DebuggerShell *shell, ICorDebug *cor, const WCHAR *args)
{
    HRESULT hr;

    ICorDebugFunction *pFunc = NULL;
    ICorDebugFunction2 *pFunc2 = NULL;
    const WCHAR *methodName = NULL;
    WCHAR *tempBuffer;

    // Need a process, but not a current thread.
    if (shell->m_currentProcess == NULL)
    {
        shell->Error(L"Process not running.\n");
        return;
    }


    methodName = args;
    while(*methodName && iswspace(*methodName))
    {
        methodName++;
    }

    ULONG len = (ULONG)wcslen(methodName);

    if (iswspace(methodName[len - 1]))
    {
        //
        // We need to strip of trailing blanks.
        //
        tempBuffer = (WCHAR *)alloca(sizeof(WCHAR) * len);

        wcscpy(tempBuffer, methodName);

        while ((len != 0) && iswspace(tempBuffer[len - 1]))
        {
            len--;
        }

        tempBuffer[len] = L'\0';

        methodName = &(tempBuffer[0]);
    }


    if (len == 0)
    {
        shell->Error(L"Function name is required.\n");
        goto done;
    }

    // Null terminate the method name.
    //if (*args)
    //    *((WCHAR*)args++) = L'\0';




    hr = shell->ResolveFullyQualifiedMethodName(methodName, &pFunc, NULL, NULL);
    if (FAILED(hr))
    {
        shell->Error(L"Could not find function: %s\n", methodName);
        shell->ReportError(hr);
        goto done;
    }

    // Check if we have an IFunction2, though we don't need it.
    pFunc->QueryInterface(IID_ICorDebugFunction2, (void**) &pFunc2);




    // Now do real work
    Print(pFunc, pFunc2, shell, cor);

done:
    if (pFunc != NULL)
        pFunc->Release();

    if (pFunc2 != NULL)
        pFunc2->Release();

    return;
}

//-----------------------------------------------------------------------------
// Print verbose help information
//-----------------------------------------------------------------------------
void FuncInfoCommand::Help(Shell *shell)
{
    ShellCommand::Help(shell);
    shell->Write(L"<function>\n");

    shell->Write(L"Displays verbose function information.\n");
}

//-----------------------------------------------------------------------------
// Print short help information.
//-----------------------------------------------------------------------------
const WCHAR *FuncInfoCommand::ShortHelp(Shell *shell)
{
    return L"Display verbose function information";
}





HRESULT DebuggerCallback::FunctionRemapOpportunity(ICorDebugAppDomain *pAppDomain,
                                                   ICorDebugThread *pThread,
                                                   ICorDebugFunction *pOldFunction,
                                                   ICorDebugFunction *pNewFunction,
                                                   ULONG32 oldOffset)
{
    _ASSERTE(!"FunctionRemapOpportunity not implemented");
    return E_NOTIMPL;
}

HRESULT DebuggerCallback::EditAndContinueRemap2(ICorDebugAppDomain *pAppDomain,
                                                ICorDebugThread *pThread,
                                                ICorDebugFunction *pOldFunction,
                                                ICorDebugFunction *pNewFunction,
                                                ULONG32 oldOffset)
{
    _ASSERTE(!"EditAndContinueRemap2 not implemented");
    return E_NOTIMPL;
}

HRESULT DebuggerCallback::FunctionRemapComplete(ICorDebugAppDomain *pAppDomain,
                                                   ICorDebugThread *pThread,
                                                   ICorDebugFunction *pNewFunction)
{
    _ASSERTE(!"FunctionRemapComplete not implemented");
    return E_NOTIMPL;
}



DisassembleDebuggerCommand::DisassembleDebuggerCommand(const WCHAR *name,
                                                       int minMatchLength)
    : DebuggerCommand(name, minMatchLength)
{
}

void DisassembleDebuggerCommand::Do(DebuggerShell *shell,
                                    ICorDebug *cor,
                                    const WCHAR *args)
{
    // If there is no process, cannot execute this command
    if (shell->m_currentProcess == NULL)
    {
        shell->Write(L"No current process.\n");
        return;
    }

    static int lastCount = 5;
    int count;
    int offset = 0;
    int startAddr = 0;

    while ((*args == L' ') && (*args != L'\0'))
        args++;

    if (*args == L'-')
    {
        args++;

        shell->GetIntArg(args, offset);
        offset *= -1;
    }
    else if (*args == L'+')
    {
        args++;

        shell->GetIntArg(args, offset);
    }
    else if ((*args == L'0') && ((*(args + 1) == L'x') ||
                                 (*(args + 1) == L'X')))
    {
        shell->GetIntArg(args, startAddr);
    }

    // Get the number of lines to print on top and bottom of current IP
    if (!shell->GetIntArg(args, count))
        count = lastCount;
    else
        lastCount = count;

    // Don't do anything if there isn't a current thread.
    if ((shell->m_currentThread == NULL) &&
        (shell->m_currentUnmanagedThread == NULL))
    {
        shell->Write(L"Thread no longer exists.\n");
        return;
    }

    HRESULT hr;
    // Only show the version info if EnC is enabled.
    if ((shell->m_rgfActiveModes & DSM_ENHANCED_DIAGNOSTICS) &&
        (shell->m_rawCurrentFrame != NULL))
    {
        ICorDebugCode *icode;
        hr = shell->m_rawCurrentFrame->GetCode(&icode);

        if (FAILED(hr))
        {
            shell->Write(L"Code information unavailable\n");
        }
        else
        {
            CORDB_ADDRESS codeAddr;
            ULONG32 codeSize;

            hr = icode->GetAddress(&codeAddr);

            if (SUCCEEDED(hr))
            {
                hr = icode->GetSize(&codeSize);

                if (SUCCEEDED(hr))
                {
                    shell->Write(L"Code at 0x%08x", codeAddr);
                    shell->Write(L" size %d\n", codeSize);
                }
                else
                    shell->Write(L"Code address and size not available\n");

                BYTE *buffer = (BYTE*)alloca(codeSize);
                ULONG32 bufSize  = 0;
                hr = icode->GetCode(0, codeSize, codeSize, buffer, &bufSize);

                if (SUCCEEDED(hr))
                {
                    shell->Write(L"Got code", codeAddr);
                    shell->Write(L" size %d\n", codeSize);
                }
                else
                    shell->Write(L"Code address and size not available, hr: %8.8x\n", hr);
            }

            ULONG32 nVer;
            hr = icode->GetVersionNumber(&nVer);
            RELEASE(icode);

            if (SUCCEEDED(hr))
                shell->Write(L"Version %d\n", nVer);
            else
                shell->Write(L"Code version not available\n");
        }
    }

    /*
     *       
     */

    // Print out the disassembly around the current IP.
    shell->PrintCurrentInstruction(count,
                                   offset,
                                   startAddr);

    // Indicate that we are in disassembly display mode
    shell->m_showSource = false;
}

// Provide help specific to this command
void DisassembleDebuggerCommand::Help(Shell *shell)
{
    ShellCommand::Help(shell);
    shell->Write(L"[0x<address>] [{+|-}<delta>] [<line count>]\n");
    shell->Write(L"Displays native or IL disassembled instructions for the current instruction\n");
    shell->Write(L"pointer (ip) or a given address, if specified. The default number of\n");
    shell->Write(L"instructions displayed is five (5). If a line count argument is provided,\n");
    shell->Write(L"the specified number of extra instructions will be shown before and after\n");
    shell->Write(L"the current ip or address. The last line count used becomes the default\n");
    shell->Write(L"for the current session. If a delta is specified then the number specified\n");
    shell->Write(L"will be added to the current ip or given address to begin disassembling.\n");
    shell->Write(L"\n");
    shell->Write(L"Examples:\n");
    shell->Write(L"   dis 20\n");
    shell->Write(L"   dis 0x31102500 +5 20\n");
    shell->Write(L"\n");
}

const WCHAR *DisassembleDebuggerCommand::ShortHelp(Shell *shell)
{
    return L"Display native or IL disassembled instructions";
}


ClearUnmanagedExceptionCommand::ClearUnmanagedExceptionCommand(const WCHAR *name, int minMatchLength)
    : DebuggerCommand(name, minMatchLength)
{
}

void ClearUnmanagedExceptionCommand::Do(DebuggerShell *shell,
                                        ICorDebug *cor,
                                        const WCHAR *args)
{
    if (shell->m_currentProcess == NULL)
    {
        shell->Error(L"Process not running.\n");
        return;
    }

    // We're given the thread id as the only param
    int dwThreadId;
    if (!shell->GetIntArg(args, dwThreadId))
    {
        Help(shell);
        return;
    }

    // Find the unmanaged thread
    DebuggerUnmanagedThread *ut =
        (DebuggerUnmanagedThread*) shell->m_unmanagedThreads.GetBase(dwThreadId);

    if (ut == NULL)
    {
        shell->Write(L"Thread 0x%x (%d) does not exist.\n",
                     dwThreadId, dwThreadId);
        return;
    }

    HRESULT hr =
        shell->m_currentProcess->ClearCurrentException(dwThreadId);

    if (!SUCCEEDED(hr))
        shell->ReportError(hr);
}

    // Provide help specific to this command
void ClearUnmanagedExceptionCommand::Help(Shell *shell)
{
    ShellCommand::Help(shell);
    shell->Write(L"<tid>\n");
    shell->Write(L"Clear the current unmanaged exception for the given tid\n");
    shell->Write(L"\n");
}

const WCHAR *ClearUnmanagedExceptionCommand::ShortHelp(Shell *shell)
{
    return L"Clear the current unmanaged exception (Win32 mode only)";
}

// Unmanaged commands

