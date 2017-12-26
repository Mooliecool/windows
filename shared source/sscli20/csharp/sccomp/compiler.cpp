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
// ===========================================================================
// File: compiler.cpp
//
// Defined the main compiler class.
// ===========================================================================

#include "stdafx.h"


#ifdef PLATFORM_UNIX
#define ENVIRONMENT_SEPARATOR L':'
#else   // PLATFORM_UNIX
#define ENVIRONMENT_SEPARATOR L';'
#endif  // PLATFORM_UNIX

#define MSCORPE_NAME        MAKEDLLNAME_W(L"mscorpe")
#define ALINK_NAME          MAKEDLLNAME_W(L"alink")

static const DWORD rgPEKind [] = {
    peILonly,                   // platformAgnostic
    peILonly | pe32BitRequired, // platformX86
    peILonly | pe32Plus,        // platformIA64
    peILonly | pe32Plus,        // platformAMD64
};
static const DWORD rgMachine [] = {
    IMAGE_FILE_MACHINE_I386,    // platformAgnostic
    IMAGE_FILE_MACHINE_I386,    // platformX86
    IMAGE_FILE_MACHINE_IA64,    // platformIA64
    IMAGE_FILE_MACHINE_AMD64,   // platformAMD64
};

C_ASSERT(lengthof(rgPEKind) == platformLast);
C_ASSERT(lengthof(rgMachine) == platformLast);

// Routine to load a helper DLL from a particular search path.
// We search the following path:
//   1. First, the directory where the compiler DLL is.
//   2. Second, the shim DLL.

HINSTANCE FindAndLoadHelperLibrary(PCWSTR filename)
{
    WCHAR path[MAX_PATH];
    WCHAR * pEnd;
    HINSTANCE hInstance;

    // 1. The directory where the compiler DLL is.
    if (PAL_GetPALDirectoryW(path, lengthof(path)))
    {
        pEnd = wcsrchr(path, L'\\');
#ifdef PLATFORM_UNIX
        WCHAR *pEndSlash = wcschr(path, L'/');
        if (pEndSlash > pEnd) {
            pEnd = pEndSlash;
        }
#endif  // PLATFORM_UNIX
        if (pEnd && wcslen(filename) + pEnd - path + 1 < (int)lengthof(path)) {
            ++pEnd;  // point just beyond.

            // Append new file
            if (SUCCEEDED(StringCchCopyW(pEnd, lengthof(path) - (pEnd - path), filename))) {

                // Try to load it.
                if ((hInstance = LoadLibraryW(path)))
                    return hInstance;
            }
        }
    }

    // 2. The shim DLL.
    HRESULT hr = LoadLibraryShim(filename, NULL, NULL, &hInstance);
    if (SUCCEEDED(hr) && hInstance)
        return hInstance;
        
    return 0;
}


/* Construct a compiler. All the real work
 * is done in the Init() routine. This primary initializes
 * all the sub-components.
 */
#if defined(_MSC_VER)
#pragma warning(disable:4355)  // allow "this" in member initializer list
#endif  // defined(_MSC_VER)

COMPILER::COMPILER(CController *pCtrl, PAGEHEAP &ph, NAMEMGR *pNameMgr) :
    pController(pCtrl),
    pageheap(ph),
    localSymAlloc(this),
    globalHeap(this, true),
    privGlobalSymAlloc(this),
    privlsymmgr(this, &localSymAlloc),
    privsymmgr(&getGlobalSymAlloc()),
    privExternAliasContainer(NULL),
    ccsymbols(this),
    assemblyAttributes(0),
    unknownGlobalAttributes(0),
    cInputFiles(0),
    cOutputFiles(0),
    location(NULL),
    stackOverflowLocation(NULL),
#ifdef DEBUG
#endif //DEBUG
    checkCLS(true),
    m_fAssemble(false),
    m_fFriendsDeclared(false),
    m_fEmitRelaxations(true),
    m_fEmitRuntimeCompatibility(true),
    m_fWrapNonExceptionThrows(true),
    corSystemDirectory(0),
    pfnCreateCeeFileGen(0),
    pfnDestroyCeeFileGen(0),
    hmodALink(NULL),
    hmodCorPE(NULL)
{

    isInited = false;   // not yet initialized.
    isCanceled = false;
    cRef = 0;
    //cError = cWarn = 0;
    errBufferNext = errBuffer = errBufferStart = 0;
    errBufferNextPage = 0;
    dispenser = NULL;
    linker = NULL;
    m_arlRefsToOutput = NULL;
    m_cnttnbUsage = 0;
    pCtrl->CloneOptions(options);
    host = pCtrl->GetHost();
    host->QueryInterface(IID_ICSCommandLineCompilerHost, (void**)&cmdHost);
    namemgr = pNameMgr;

    sidLast = 0;
    m_pszLibPath = NULL;

    compPhase = CompilerPhase::None;
    aggStateMax = AggState::None;
}

#if defined(_MSC_VER)
#pragma warning(default:4355)
#endif  // defined(_MSC_VER)

/* Destruct the compiler. Make sure we've
 * been deinitialized.
 */
COMPILER::~COMPILER()
{
    if (isInited) {
        //ASSERT(0);      // We should have been terminated by now.
        Term(false);
    }
}

HRESULT COMPILER::InitErrorBuffer()
{
    // Start by Reserving a large chunk of memory for error buffers
    // but we only commit 1 page now, and then commit new pages as needed
    if (NULL == (errBuffer = (WCHAR*)VirtualAlloc(NULL, ERROR_BUFFER_MAX_BYTES, MEM_RESERVE, PAGE_NOACCESS))) {
        Error(NULL, FTL_NoMemory);
        return HRESULT_FROM_WIN32(GetLastError());
    }
    if (NULL == (errBufferNext = (WCHAR*)VirtualAlloc( errBuffer, pageheap.pageSize, MEM_COMMIT, PAGE_READWRITE))) {
        Error(NULL, FTL_NoMemory);
        return HRESULT_FROM_WIN32(GetLastError());
    }
    errBufferNextPage = (BYTE*)errBufferNext + pageheap.pageSize;
    errBufferStart = errBuffer = errBufferNext;

    return S_OK;
}

/* Initialize the compiler. This does the heavy lifting of
 * setting up the memory management, and so forth.
 */
HRESULT COMPILER::Init ()
{
    if (isInited)
    {
        // This should never happen
        VSFAIL ("Compiler initialization called twice?");
        return E_FAIL;
    }

    HRESULT hr = S_OK;

    PAL_TRY {
        PAL_TRY {
            hr = InitWorker();
        } EXCEPT_COMPILER_EXCEPTION {
            CLEANUP_STACK // handle stack overflow
            if (GetExceptionCode() == STATUS_STACK_OVERFLOW)
                ReportStackOverflow();
            else
                pController->GetHost()->OnCatastrophicError(TRUE, GetExceptionCode(), pController->GetExceptionAddress());
            hr = E_UNEXPECTED;
        } PAL_ENDTRY
    } EXCEPT_EXCEPTION {
        CLEANUP_STACK // handle stack overflow
        hr = (GetExceptionCode() == FATAL_EXCEPTION_CODE) ? E_FAIL : E_UNEXPECTED;
    } PAL_ENDTRY

    return hr;
}

HRESULT COMPILER::InitWorker ()
{
    HRESULT hr = S_OK;

    // isInited controls if we tear down in COMPILER::Term()
    // Make sure things can handle calling Term without Init
    isInited = true;

    ASSERT (globalHeap.GetMaxSize() == 0);

    // Initialize the error buffer
    hr = InitErrorBuffer();
    if (FAILED(hr)) return hr;

    // Load ALINK.DLL and initialize it. Do this in a late bound way so
    // we load the correct copy via the shim.
    ASSERT(linker == NULL);
    hmodALink = FindAndLoadHelperLibrary(ALINK_NAME);
    if (hmodALink != NULL) {
        HRESULT (WINAPI * pfnCreateALink)(REFIID, IUnknown**);
        pfnCreateALink = (HRESULT (WINAPI *)(REFIID, IUnknown**))GetProcAddress(hmodALink, "CreateALink");
        if (pfnCreateALink) {
            if (FAILED(hr = (*pfnCreateALink)(IID_IALink2, (IUnknown**)&linker))) 
                goto LOAD_FAILED;
        }
    }
    if (linker == NULL) {
        hr = HRESULT_FROM_WIN32(GetLastError());
LOAD_FAILED:
        Error(NULL, FTL_RequiredFileNotFound, ALINK_NAME);
        return hr;
    }

    W_IsUnicodeSystem();    // Use this to Init Unilib stuff

    // Initialize global symbols that we always use.
    getBSymmgr().Init();
    privExternAliasContainer = getBSymmgr().CreateGlobalSym( SK_SCOPESYM, getNamemgr()->GetPredefName(PN_EXTERNALIASCONTAINER), NULL)->asPARENTSYM();
    importer.Init();

    // Force us to check the search path for warnings
    GetSearchPath();

    // Add each of the imports (must be done after initialization of symmgr...)
    if (options.m_sbstrIMPORTS != NULL) {
        size_t      bufferLen = options.m_sbstrIMPORTS.Length()+1;
        PWSTR       buffer = STACK_ALLOC(WCHAR, bufferLen);
        HRESULT hr;
        hr = StringCchCopyW (buffer, bufferLen, options.m_sbstrIMPORTS);
        ASSERT(SUCCEEDED(hr));
        WCHAR *pchContext = NULL;
        PWSTR pszTok = wcstok_s(buffer, L"|", &pchContext);  // imports are pipe seperated

        while (pszTok != NULL) {
            PWSTR pszFile = pszTok;
            PWSTR pszAlias = wcschr(pszTok, L'\x01');
            if (pszAlias != NULL) {
                pszFile = pszAlias + 1;
                *pszAlias = L'\0';
                pszAlias = pszTok;
            }

            if (!*pszFile) {
                Error(NULL, ERR_AliasMissingFile, pszAlias);
            }
            else {
                INFILESYM * infileRef = FindAndAddMetadataFile(pszFile, kaidNil /* allocate the aid */);

                if (infileRef != NULL) {
                    if (pszAlias != NULL)
                        AddInfileToExternAliasWithErrors(infileRef, pszAlias);
                    else {
                        infileRef->AddToAlias(kaidGlobal, &getGlobalSymAlloc());
                        getBSymmgr().bsetGlobalAssemblies.SetBit(infileRef->GetAssemblyID(), &getGlobalSymAlloc());
                    }
                }
            }
            pszTok = wcstok_s(NULL, L"|", &pchContext);
        }
    }

    // Add each of the addmodules
    if (options.m_sbstrMODULES != NULL) {
        size_t      bufferLen = options.m_sbstrMODULES.Length()+1;
        PWSTR       buffer = STACK_ALLOC(WCHAR, bufferLen);
        HRESULT hr2;
        hr2 = StringCchCopyW (buffer, bufferLen, options.m_sbstrMODULES);
        ASSERT(SUCCEEDED(hr2));
        WCHAR *pchContext = NULL;
        PWSTR pszTok = wcstok_s(buffer, L"|", &pchContext);

        while (pszTok != NULL) {
            FindAndAddMetadataFile(pszTok, kaidThisAssembly);
            pszTok = wcstok_s(NULL, L"|", &pchContext);
        }
    }

    ASSERT(isInited);

    return hr;
}

/*
 * Terminate the compiler. Terminates all subsystems and
 * frees all allocated memory.
 */
void COMPILER::Term(bool normalTerm)
{
   if (!isInited)
        return;     // nothing to do.

    isInited = false; // set this to false to prevent recalling Term()

    // Terminate everything. Check leaks on a normal termination.

    // Shutdown the major components first
    emitter.Term();
    assemFile.Term();

    importer.Term();

    getBSymmgr().Term();

    // Free the search path
    if (m_pszLibPath != NULL) {
        globalHeap.Free(m_pszLibPath);
        m_pszLibPath = NULL;
    }

    // Free the link to alink.dll
    if (linker) {
        linker->Release();
        linker = NULL;
    }
    if (hmodALink) {
        FreeLibrary(hmodALink);
        hmodALink = 0;
    }

    // Free the metadata dispenser
    if (dispenser) {
        dispenser->Release();
        dispenser = NULL;
    }

    // free link to mscorpe.dll
    pfnCreateCeeFileGen = NULL;
    pfnDestroyCeeFileGen = NULL;
    if (hmodCorPE) {
        FreeLibrary(hmodCorPE);
        hmodCorPE = 0;
    }

    // Free the various heaps
    ccsymbols.ClearAll(TRUE);
    localSymAlloc.FreeHeap();
    getGlobalSymAlloc().FreeHeap();

    
    globalHeap.FreeHeap(normalTerm);

    // Done with everything, release the host.
    if (host) {
        //host->Release();
        host = NULL;
    }

    // Last but not least, release the error buffer
    if (errBuffer != NULL) {
        BOOL bSuccess; bSuccess = VirtualFree(errBuffer, 0, MEM_RELEASE);
        ASSERT(bSuccess);
    }
}

/*
 * checks each element of the path list (separated by '|')
 * to see if it exists, is accessible, and is a directory
 *
 * assumes that all ENVIRONMENT_SEPARATOR characters have
 * been replaced by '|', and quotes have been stripped already
 * idsSource is the source of the directory string
 * (currently on "/LIB" or "LIB environment variable")
 */
void COMPILER::CheckSearchPath(__inout PWSTR wzPathList, int idsSource)
{
    if (wzPathList == NULL)
        return;

    WCHAR *pchContext = NULL;
    PWSTR wzDirName = wcstok_s(wzPathList, L"|", &pchContext);
    while (wzDirName) {
        // Only check non-empty path parts
        if (*wzDirName) {
            DWORD dwAttrib = W_GetFileAttributes(wzDirName);
            if (dwAttrib == INVALID_FILE_ATTRIBUTES) {
                // report the OS error
                DWORD dwErr = GetLastError();
                if (dwErr == ERROR_FILE_NOT_FOUND) // but change file-not-found to path-not-found
                    dwErr = ERROR_PATH_NOT_FOUND;
                Error(NULL, WRN_InvalidSearchPathDir, wzDirName, ErrArgIds(idsSource), ErrHR(HRESULT_FROM_WIN32(dwErr)));
            } else if (0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
                // we found a file not a directory
                Error(NULL, WRN_InvalidSearchPathDir, wzDirName, ErrArgIds(idsSource), ErrHR(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)));
            }
        }
        wzDirName = wcstok_s(NULL, L"|", &pchContext);

        if (wzDirName) {
            // Stick back in the separator, since wcstok_s set it to nul
            wzDirName[-1] = L'|';
        }
    }
}

/*
 * Get the path to search for imports
 * = Current Directory, CORSystemDir, /LIB otpion, %LIB%
 */
PCWSTR COMPILER::GetSearchPath()
{
    if (m_pszLibPath == NULL)
    {
        // Setup the Lib Search path
        WCHAR * pszPathBuffer;
        int len = 0, cchPathBuffer = 0;
        StringBldrMemHeap strPath(&globalHeap);

        // Use the ANSI version, because we just want the length and ANSI always works
        // and the unicode length will never be longer than the ANSI length
        cchPathBuffer = GetCurrentDirectoryA( 0 , NULL);
        cchPathBuffer += GetEnvironmentVariableA( "LIB", NULL, 0);
        pszPathBuffer = STACK_ALLOC(WCHAR, cchPathBuffer);

        // add current directory
        len = W_GetCurrentDirectory( cchPathBuffer, pszPathBuffer);
        if (len > 0 && len < cchPathBuffer) {
            strPath.Add( pszPathBuffer);
            strPath.Add( L'|');
        }

        // add CorSystemDir
        strPath.Add( GetCorSystemDirectory());
        strPath.Add( L'|');

        // add /lib
        if (options.m_sbstrLIBPATH.Length()) {
            CheckSearchPath(options.m_sbstrLIBPATH, IDS_LIB_OPTION);
            strPath.Add( options.m_sbstrLIBPATH, options.m_sbstrLIBPATH.Length());
            strPath.Add( L'|');
        }

        // add LIB= environment variable
        len = GetEnvironmentVariableW( L"LIB", pszPathBuffer, cchPathBuffer);

        if (len > 0 && len < cchPathBuffer) {
            RemoveQuotesAndReplacePathDelim(WCBuffer(pszPathBuffer, cchPathBuffer));
            CheckSearchPath(pszPathBuffer, IDS_LIB_ENV);
            strPath.Add(pszPathBuffer);
        }

        ASSERT(!strPath.Error());
        m_pszLibPath = strPath.Detach();
    }

    return m_pszLibPath;
}

/*
 * Discards all state accumulated in the local heap
 * and local symbols
 */
void COMPILER::DiscardLocalState()
{
    localSymAlloc.FreeHeap();
    getLSymmgr().DestroyLocalSymbols();
    funcBRec.DiscardLocalState();
}

/*
 * get the cor system directory
 */
PCWSTR COMPILER::GetCorSystemDirectory()
{
    if (!corSystemDirectory)
    {
        // cache the CorSystemDirectory
        // Setup the Lib Search path
        HRESULT hr;
        WCHAR buffer[MAX_PATH+5];
        DWORD dwLen = lengthof(buffer) - 1;

        if (FAILED(hr = GetMetadataDispenser()->GetCORSystemDirectory( buffer, dwLen, &dwLen))) {
            VSASSERT(hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER),
                "MAX_PATH should be big enough for the CORSystemDir!");
            // Don't report an error, we'll get it later when we try
            // "AddStandardMetaData()"
            Error(NULL, ERR_CantGetCORSystemDir, ErrHR(hr, false));
        }

        corSystemDirectory = this->namemgr->AddString(buffer);
    }

    return corSystemDirectory->text;
}

/*
 * Get the metadata dispenser.
 */
IMetaDataDispenserEx * COMPILER::GetMetadataDispenser()
{
    if (! dispenser) {
        // Obtain the dispenser.
        HRESULT hr = E_FAIL;
        if (FAILED(hr = PAL_CoCreateInstance(CLSID_CorMetaDataDispenser,
                                         IID_IMetaDataDispenserEx,
                                         (LPVOID *) & dispenser)))
        {
            Error(NULL, FTL_ComPlusInit, ErrHR(hr));
        }

        SetDispenserOptions();

        if (FAILED(hr = linker->Init(dispenser, &alError)))
        {
            Error(NULL, FTL_ComPlusInit, ErrHR(hr));
        }
    }

    return dispenser;
}

ITypeNameFactory * COMPILER::GetTypeNameFactory()
{
    if (! m_qtypenamefactory) {
        // Obtain the ITypeNameFactory.
        HRESULT hr = E_FAIL;
        if (FAILED(hr = PAL_CoCreateInstance(CLSID_TypeNameFactory,
                                         IID_ITypeNameFactory,
                                         (LPVOID *) & m_qtypenamefactory)))
        {
            Error(NULL, FTL_ComPlusInit, ErrHR(hr));
        }
    }

    return m_qtypenamefactory;
}

ITypeNameBuilder * COMPILER::GetTypeNameBuilder()
{
    if (m_cnttnbUsage != 0) {
        // This guy is still being used, so get a new one
        m_qtypenamebuilder = NULL;
        m_cnttnbUsage = 0;
    }

    if (! m_qtypenamebuilder) {
        // Obtain the ITypeNameFactory.
        HRESULT hr = E_FAIL;
        ITypeNameFactory * ptnf = GetTypeNameFactory();
        if (FAILED(hr = ptnf->GetTypeNameBuilder(&m_qtypenamebuilder)))
        {
            Error(NULL, FTL_ComPlusInit, ErrHR(hr));
        }
    }

    m_cnttnbUsage++;
    ASSERT(m_cnttnbUsage == 1);
    return m_qtypenamebuilder;
}

/*
 * sets the options on the current metadata dispenser
 */
void COMPILER::SetDispenserOptions()
{
    VARIANT v;

    if (dispenser) {
        // Set the emit options for maximum speed: no token remapping, no ref to def optimization,
        // no duplicate checking. We do all these optimizations
        // ourselves.

        // Only check typerefs, member refs, modulerefs and assembly refs -- we need to do this
        // because DefineImportMember or ALink may create these refs for us.

        // We do not set the duplicate checking flags here, ALink sets them when we call Init().
        // MetaDataCheckDuplicatesFor is set to:  MDDupTypeRef | MDDupMemberRef | MDDupModuleRef | MDDupAssemblyRef | MDDupExportedType;

        // Never change refs to defs
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDRefToDefNone;
        dispenser->SetOption(MetaDataRefToDefCheck, &v);

        // Don't give error if emitting out of order because we'll just be reordering it
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDErrorOutOfOrderNone;
        dispenser->SetOption(MetaDataErrorIfEmitOutOfOrder, &v);

        // Notify of all token remaps
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDNotifyAll;
        dispenser->SetOption(MetaDataNotificationForTokenMovement, &v);

        // Turn on full update build for the schema
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDUpdateFull;

        dispenser->SetOption(MetaDataSetUpdate, &v);
    }
}


/* Do the compilation. */
HRESULT COMPILER::Compile(ICSCompileProgress * progressSink, bool * pfNeedsCleanUp)
{
    HRESULT hr = NOERROR;

    if (pfNeedsCleanUp)
        *pfNeedsCleanUp = false;


    PAL_TRY {
        // Do compilation here.

        {
            PAL_TRY {
                // This is so hr is an error code if we throw a fatal exception inside CompileAll.
                // Then CleanUp will do the right thing.
                hr = E_FAIL;

                PAL_TRY {
                    CompileAll(progressSink);
                    hr = NOERROR;
                } EXCEPT_COMPILER_EXCEPTION {
                    CLEANUP_STACK // handle stack overflow
                    if (GetExceptionCode() == STATUS_STACK_OVERFLOW)
                        ReportStackOverflow();
                    hr = E_UNEXPECTED;
                } PAL_ENDTRY

                if (pfNeedsCleanUp)
                    *pfNeedsCleanUp = true;
            } PAL_FINALLY {
                if (!pfNeedsCleanUp || !*pfNeedsCleanUp)
                    hr = CleanUp(hr);
            } PAL_ENDTRY
        }
    } EXCEPT_EXCEPTION {
        CLEANUP_STACK // handle stack overflow
        hr = (GetExceptionCode() == FATAL_EXCEPTION_CODE) ? E_FAIL : E_UNEXPECTED;
    } PAL_ENDTRY

    return hr;
}

HRESULT COMPILER::CleanUp(HRESULT hr)
{
    SourceFileIterator files;
    for (files.Reset(this); files.Current(); files.Next()) {
        if (files.Current()->pData) {
            files.Current()->pData->Release();
        }
    }

    if (SUCCEEDED(hr)) {
        if (GetFirstOutFile() == getBSymmgr().GetMDFileRoot() && !GetFirstOutFile()->nextOutfile())
            hr = E_FAIL;    // didn't compile any source files
        else if (FAbortEarly(0))
            hr = E_FAIL;
    }

    Term(true);         // Normal termination; check memory leaks.
    return hr;
}

#undef IfFailRet
#define IfFailRet(expr) if (FAILED((hr = (expr)))) return hr;
#undef IfFailGo
#define IfFailGo(expr) if (FAILED((hr = (expr)))) goto Error;

// These indicate phases in the compile. These are confusingly similar to the "compile stages",
// yet different. They are only used for progress reporting.
enum COMPILE_PHASE {
    PHASE_DECLARETYPES,
    PHASE_IMPORTTYPES,
    PHASE_DEFINE,
    PHASE_PREPARE,
    PHASE_CHECKIFACECHANGE,
    PHASE_COMPILE,
    PHASE_WRITEOUTPUT,
    PHASE_WRITEINCFILE,

    PHASE_MAX,
};

// The integers in this array indicate rough proportional amount of time
// each phase takes.
static const int relativeTimeInPhase[PHASE_MAX] = 
{
    12,         // PHASE_DECLARETYPES
    2,          // PHASE_IMPORTTYPES
    1,          // PHASE_DEFINE
    5,          // PHASE_PREPARE
    1,          // PHASE_CHECKIFACECHANGE
    40,         // PHASE_COMPILE
    15,         // PHASE_WRITEOUTPUT
    1,          // PHASE_WRITEINCFILE
};


/*
 * declare all the input files
 *
 * adds symbol table entries for all namespaces and
 * user defined types (classes, enums, structs, interfaces, delegates)
 * sets access modifiers on all user defined types
 */
void COMPILER::DeclareTypes(ICSCompileProgress * progressSink)
{
    //
    // parse & declare the input files
    //

    int filesDone = 0;
    SourceFileIterator infiles;
    for (infiles.Reset(this); infiles.Current(); infiles.Next())
    {
        DeclareOneFile(infiles.Current());
        if (ReportProgress(progressSink, PHASE_DECLARETYPES, ++filesDone, cInputFiles))
            break;
    }
}

void COMPILER::EmitTokens(OUTFILESYM *outfile)
{
    InFileIterator infiles;
    INFILESYM *pInfile;
    long ErrCnt = ErrorCount();
    for (pInfile = infiles.Reset(outfile); pInfile != NULL; pInfile = infiles.Next())
    {

        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.emitTypedefsNamespace(pInfile->rootDeclaration);
    }

    CErrorSuppression es;

    // This means we might not have tokens for all typedefs
    // so bail early
    if (FAbortEarly(ErrCnt, &es))
        return;

    for (pInfile = infiles.Reset(outfile); pInfile != NULL; pInfile = infiles.Next())
    {

        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.emitBasesNamespace(pInfile->rootDeclaration);
    }

    for (pInfile = infiles.Reset(outfile); pInfile != NULL; pInfile = infiles.Next())
    {

        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.emitMemberdefsNamespace(pInfile->rootDeclaration);
    }
}

/*
 * Report compiler progress and give the user a chance to cleanly cancel the compile.
 * The compiler is now in the middle of phase "phase", and has completed "itemsComplete"
 * of the "itemsTotal" in this phase.
 *
 * returns TRUE if the compilation should be canceled (an error has already been reported.
 */

bool COMPILER::ReportProgress(ICSCompileProgress * progressSink, int phase, int itemsComplete, int itemsTotal)
{
    bool cancel = false;
    long totItemsComplete, totItems;
    const int SCALE = 1024;

    if (progressSink == NULL)
        return false;

    ASSERT(phase < PHASE_MAX);

    // convert phase , itemsComplex, itemsTotal into totItemsComplex and totItems, using the
    // relative time in phase array.
    totItems = totItemsComplete = 0;

    int iMaxPhase = PHASE_MAX;

    for (int i = 0; i < iMaxPhase; ++i) {
        totItems += SCALE * relativeTimeInPhase[i];
        if (i < phase)
            totItemsComplete += SCALE * relativeTimeInPhase[i];
    }
    if (itemsTotal > 0)
        totItemsComplete += (SCALE * itemsComplete / itemsTotal) * FetchAtIndex(relativeTimeInPhase, phase);

    cancel = !!(progressSink->ReportProgress (L"",  totItems - totItemsComplete, totItems));

    if (cancel && !isCanceled) {
        Error (NULL, ERR_CompileCancelled);
        isCanceled = true;
    }

    return isCanceled;
}

bool COMPILER::ReportCompileProgress(ICSCompileProgress *progressSink, int iFile, int iFileCount)
{
    return ReportProgress(progressSink, PHASE_COMPILE, iFile, iFileCount);
}


void COMPILER::ParseOneFile(INFILESYM *infile)
{
    ICSSourceModule     *pModule = NULL;
    ICSSourceData       *pData = NULL;
    HRESULT              hr;

    SETLOCATIONSTAGE(PARSE);
    SETLOCATIONFILE(infile);

    hr = host->GetSourceModule (infile->name->text, options.m_fEMITDEBUGINFO, &pModule);

    if (SUCCEEDED(hr)) {
        hr = pModule->GetSourceData (TRUE, &pData);
    }
    if (FAILED (hr))
    {
        Error (NULL, ERR_NoSourceFile, infile->name->text, ErrHR(hr));
    }
    else
    {
        BASENODE                    *pNode;
        CComPtr<ICSErrorContainer>  spErrors;

        // Build the top-level parse tree.  Note that this may already be done...
        pData->ParseTopLevel (&pNode);

        // Get any tokenization errors that may have occurred and send them to the host.
        if (SUCCEEDED (pData->GetErrors (EC_TOKENIZATION, &spErrors)))
            pController->ReportErrorsToHost (spErrors);
        spErrors.Release();

        // Same for top-level parse errors
        if (SUCCEEDED (pData->GetErrors (EC_TOPLEVELPARSE, &spErrors)))
            pController->ReportErrorsToHost (spErrors);
        spErrors.Release();

        infile->nspace = pNode->asNAMESPACE();

        infile->pData = static_cast<CSourceData*>(pData);     // NOTE:  Ref ownership transferred here...

    }

    if (pModule)
        pModule->Release();     // (if pData is valid, it has a ref on this)
}

void COMPILER::DeclareOneFile(INFILESYM *infile)
{
    // do parsing on demand
    ParseOneFile(infile);

    if (infile->nspace)
    {
        SETLOCATIONFILE(infile);
        SETLOCATIONSTAGE(DECLARE);
        clsDeclRec.declareInputfile(infile->nspace, infile);
    }
}

void COMPILER::ResolveInheritanceHierarchy()
{
    //
    // do object first to avoid special cases later
    //
    AGGSYM *object = compiler()->GetReqPredefAgg(PT_OBJECT, false);
    if (!object->ifaces) {
        object->ifaces = BSYMMGR::EmptyTypeArray();
        object->ifacesAll = BSYMMGR::EmptyTypeArray();
    }
    ASSERT(object->AggState() == AggState::Declared);
    object->SetAggState(AggState::Inheritance);

    SourceFileIterator infiles;
    for (INFILESYM *pInfile = infiles.Reset(this); pInfile != NULL; pInfile = infiles.Next())
    {
        SETLOCATIONSTAGE(DEFINE);
        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.ResolveInheritance(pInfile->rootDeclaration);
    }
}

//
// resolve the bounds for all types defined in source
//
void COMPILER::DefineBounds()
{
    SourceFileIterator infiles;
    for (INFILESYM *pInfile = infiles.Reset(this); pInfile != NULL; pInfile = infiles.Next())
    {
        SETLOCATIONSTAGE(DEFINE);
        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.defineBounds(pInfile->rootDeclaration);
    }
}


// This is called after all types are prepared to check for bogus, deprecated and constraints.
void COMPILER::CheckForTypeErrors()
{
    SourceFileIterator infiles;
    for (INFILESYM *pInfile = infiles.Reset(this); pInfile != NULL; pInfile = infiles.Next())
    {
        SETLOCATIONSTAGE(DEFINE);
        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.CheckForTypeErrors(pInfile->rootDeclaration);
    }
}


void COMPILER::DefineOneFile(INFILESYM *pInfile)
{
    if (!pInfile->isDefined) {
        SETLOCATIONSTAGE(DEFINE);
        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.defineNamespace(pInfile->rootDeclaration);
        pInfile->isDefined = true;
    }
}


//
// define all the input files
//
// adds symbols for all fields & methods including types
// does name conflict checking within a class
// checks field & method modifiers within a class
// does access checking for all types
//
// 1 - define members of changed files only
// 2 - check interface changes for changed files
// 3 a - on iface changed       - do a full rebuild
// 3 b - small iface changed    - define from source dependant files
// 4 - define unchanged (and undependant) files from metadata
//
void COMPILER::DefineMembers()
{
    ASSERT(aggStateMax == AggState::Bounds);

    // bring object up to declared/defined state first
    // so that we don't have to special case it elsewhere
    clsDeclRec.defineObject();

    SourceFileIterator infiles;
    for (infiles.Reset(this); infiles.Current(); infiles.Next())
    {
        INFILESYM * infile = infiles.Current();

        if (infile->nspace) {
            ASSERT(!infile->isDefined);
            SETLOCATIONFILE(infile);
            DefineOneFile(infile);
            ASSERT(infile->isDefined && !infile->isConstsEvaled);
        }
    }

    ASSERT(aggStateMax == AggState::Bounds);
    aggStateMax = AggState::DefinedMembers;

    AGGSYM * aggTmp;
    if ((aggTmp = GetOptPredefAgg(PT_ATTRIBUTEUSAGE, false)) != NULL)
        EnsureState(aggTmp, AggState::DefinedMembers);
    if ((aggTmp = GetOptPredefAgg(PT_OBSOLETE, false)) != NULL)
        EnsureState(aggTmp, AggState::DefinedMembers);
    if ((aggTmp = GetOptPredefAgg(PT_CONDITIONAL, false)) != NULL)
        EnsureState(aggTmp, AggState::DefinedMembers);
    if ((aggTmp = GetOptPredefAgg(PT_CLSCOMPLIANT, false)) != NULL)
        EnsureState(aggTmp, AggState::DefinedMembers);
}


// Evaluates all the constants and compiles all the attributes.
void COMPILER::EvaluateConstants()
{
    SourceFileIterator infiles;
    for (infiles.Reset(this); infiles.Current(); infiles.Next())
    {
        INFILESYM * infile = infiles.Current();

        if (infile->nspace && !infile->isConstsEvaled) {
            SETLOCATIONFILE(infile);
            AggIterator aggs;
            for (AGGSYM * agg = aggs.Reset(infile); agg; agg = aggs.Next()) {
                clsDeclRec.evaluateConstants(agg);
            }
            infile->isConstsEvaled = true;
        }
    }

    CheckCLS();
}



// returns if CLS complianceChecking is enabled in any of these attributes.
// pbVal is set to indicate the CLS value (true means compliant, false means non-compliant)
bool COMPILER::ScanAttributesForCLS(GLOBALATTRSYM * attributes, GLOBALATTRSYM ** ppAttrLoc, bool * pbVal)
{
    // Compile the CLS attributes (this will loop through all of them, so only call it once!)
    // Becuase this will also give regular compiler errors for all badly formed global attributes
    if (attributes) {
        funcBRec.setUnsafe(false);
        EarlyGlobalAttrBind::Compile(this, attributes);
        funcBRec.resetUnsafe();
    }

    while (attributes) {
        if (attributes->hasCLSattribute) {
            *pbVal = attributes->isCLS;
            *ppAttrLoc = attributes;
            return true;
        }
        attributes = attributes->nextAttr;
    }

    *pbVal = false;
    *ppAttrLoc = NULL;
    
    return false;
}


// Set the global CLS attribute on the infiles
void COMPILER::CheckCLS()
{
    checkCLS = false;
    if (!AllowCLSErrors())
        return;

    SYM * pAssemblyAttr = NULL;  // The location of the CLS attribute -- either a GLOBALATTRSYM or an INFILESYM.
    bool assemblyCLSValue = false;
    bool assemblyCLSCheck;
    
    {
        GLOBALATTRSYM * tempAssemblyAttr = NULL;
        assemblyCLSCheck = ScanAttributesForCLS(this->assemblyAttributes, &tempAssemblyAttr, &assemblyCLSValue);
        pAssemblyAttr = tempAssemblyAttr;
    }

    if (assemblyCLSCheck)
        checkCLS = true;

    SourceOutFileIterator files;
    PINFILESYM pInfile;
    InFileIterator infiles;

    // differentiate between in-source and the global settings
    bool hasCLS = assemblyCLSCheck;
    bool CLSValue = assemblyCLSValue;

    if (!hasCLS)
    {
        // check imports for attributes
        for (pInfile = infiles.Reset(getBSymmgr().GetMDFileRoot()); pInfile != NULL; pInfile = infiles.Next())
        {
            if (pInfile->isAddedModule && pInfile->hasCLSattribute) {
                if (pAssemblyAttr == NULL || (!CLSValue && pInfile->isCLS))
                    // point to the place where it's true!
                    pAssemblyAttr = pInfile;
                hasCLS = true;
                CLSValue = pInfile->isCLS;
                break;
            }
        }
    }

    if (hasCLS)
        checkCLS = true;

    // Now we know that somewhere there was a CLS Compliant attribute
    // CLSValue is the logical OR of that/those value(s)

    // Set the bit on each INFILESYM and OUTFILESYM so the symbols don't have to check back with the compiler
    // and check for consistency
    for (OUTFILESYM *pOutfile = files.Reset(this); pOutfile != NULL; pOutfile = files.Next())
    {
        bool moduleCLSCheck = false;        // checking on for this module
        bool moduleCLSValue = false;    // always the same as the assembly or an error
        GLOBALATTRSYM * pModuleAttr = NULL;

        moduleCLSCheck = ScanAttributesForCLS(pOutfile->attributes, &pModuleAttr, &moduleCLSValue);
        if (hasCLS && moduleCLSCheck && moduleCLSValue != CLSValue) {
            // If the module doesn't match the assembly, it's an warning
            CError  *pError = MakeError(pModuleAttr->parseTree, WRN_CLS_NotOnModules2);
            if (pAssemblyAttr->isINFILESYM())
                AddLocationToError(pError, ERRLOC(pAssemblyAttr->asINFILESYM()));
            else
                AddLocationToError(pError, ERRLOC(& getBSymmgr(), pAssemblyAttr->asGLOBALATTRSYM()->parseTree));
            SubmitError (pError);
        } else if (!hasCLS && moduleCLSCheck) {
            // If the module doesn't match the assembly, it's an warning
            Error(pModuleAttr->parseTree, WRN_CLS_NotOnModules);
        }

        if (hasCLS) {
            pOutfile->hasCLSattribute = true;
            pOutfile->isCLS = CLSValue;
            for (pInfile = infiles.Reset(pOutfile); pInfile != NULL; pInfile = infiles.Next())
            {
                pInfile->hasCLSattribute = true;
                pInfile->isCLS = CLSValue;
            }
        }
    }

    // check imports (and set them too)
    for (pInfile = infiles.Reset(getBSymmgr().GetMDFileRoot()); pInfile != NULL; pInfile = infiles.Next())
    {
        if (pInfile->isAddedModule) {
            if (checkCLS) {
                if (pInfile->hasCLSattribute || pInfile->hasModuleCLSattribute) {
                    if (CLSValue != pInfile->isCLS) {
                        // Error, module differs from assembly
                        CError  *pError = MakeError(ERRLOC(pInfile), WRN_CLS_NotOnModules2);
                        if (pAssemblyAttr->isINFILESYM())
                            AddLocationToError (pError, ERRLOC(pAssemblyAttr->asINFILESYM()));
                        else
                            AddLocationToError (pError, ERRLOC(& getBSymmgr(), pAssemblyAttr->asGLOBALATTRSYM()->parseTree));
                        SubmitError (pError);
                    }
                } else {
                    // Error, assembly is marked, but module has no attribute
                    // Added modules must be CLS compliant (or at least have the bit set),
                    // so we know that all defined classes are properly marked and checked
                    Error(ERRLOC(pInfile), WRN_CLS_ModuleMissingCLS);
                }
            } else if (pInfile->hasModuleCLSattribute) {
                // Warn, ignored attribute on module
                Error(ERRLOC(pInfile), WRN_CLS_NotOnModules);
            }
            pInfile->hasCLSattribute = hasCLS;
            pInfile->isCLS = CLSValue;
        }
    }
}


void COMPILER::RecordAssemblyRefToOutput( NAME * nameRef, MODULESYM * modSrc, bool fIsFriendAssemblyRef)
{
    if (!modSrc)
        return;

    AssemblyRefList ** parlCurrent = &m_arlRefsToOutput;

    // Find the end of the list, or the element that has the same name
    while (*parlCurrent && nameRef != (*parlCurrent)->nameRef) {
        parlCurrent = &(*parlCurrent)->next;
    }

    // If this name doesn't already exist, add it
    if (*parlCurrent == NULL) {
        *parlCurrent = (AssemblyRefList*)getGlobalSymAlloc().AllocZero(sizeof(AssemblyRefList));
        (*parlCurrent)->nameRef = nameRef;
        (*parlCurrent)->fIsFriendAssemblyRefOnly = fIsFriendAssemblyRef;
    } else {
        (*parlCurrent)->fIsFriendAssemblyRefOnly &= fIsFriendAssemblyRef;
    }
        

    // Shouldn't add the same source twice!
    if ((*parlCurrent)->listModSrc->contains(modSrc))
        return;

    // Now add the module
    SYMLIST * listNew = (SYMLIST*)getGlobalSymAlloc().Alloc(sizeof(SYMLIST));
    listNew->sym = modSrc;
    listNew->next = (*parlCurrent)->listModSrc;
    (*parlCurrent)->listModSrc = listNew;
}

// Mark infile as having it's friend reference to the assembly being built as used.  
// If this turns out to be a bad reference, we will not error if it is never marked as being used.
void COMPILER::MarkUsedFriendAssemblyRef(INFILESYM *infile)
{
    ASSERT(!infile->isSource && infile->InternalsVisibleTo(kaidThisAssembly));
    infile->fFriendAccessUsed = true;
}


void COMPILER::PostCompileChecks()
{
    //
    // check if any non-external fields are never assigned to
    // or private fields and events are never referenced.
    //
    INFILESYM *pInfile;
    SourceFileIterator infiles;
    bool fConsiderInternal = FriendsDeclared() || !BuildAssembly();
    for (pInfile = infiles.Reset(this); pInfile != NULL; pInfile = infiles.Next())
    {
        AggIterator aggs;
        for (AGGSYM *cls = aggs.Reset(pInfile); cls != NULL; cls = aggs.Next())
        {
            // Don't Check Certain structs
            if (cls->hasExternReference)
                continue;

            bool hasExternalVisibility = fConsiderInternal ? cls->hasExternalOrFriendAccess() : cls->hasExternalAccess();
            FOREACHCHILD(cls, member)
                if (member->isMEMBVARSYM())
                {
                    MEMBVARSYM *field = member->asMEMBVARSYM();
                    // Only check non-Const internall-only fields and events
                    if (field->isConst || (hasExternalVisibility && fConsiderInternal ? field->hasExternalOrFriendAccess() : field->hasExternalAccess()))
                        continue;

                    if (!field->isReferenced && field->GetAccess() == ACC_PRIVATE)
                    {
                        if (!field->isEvent) {
                            ErrorRef(NULL, field->isAssigned ? WRN_UnreferencedFieldAssg : WRN_UnreferencedField, field);
                        }
                        else {
                            ErrorRef(NULL, WRN_UnreferencedEvent, field);
                        }
                    }
                    else if (!field->isAssigned && !field->isEvent)
                    {
                        PCWSTR zeroString;
                        if (field->type->isNumericType() || field->type->isEnumType())
                            zeroString = L"0";
                        else if (field->type->isPredefType(PT_BOOL))
                            zeroString = L"false";
                        else if (field->type->fundType() == FT_REF)
                            zeroString = L"null";
                        else
                            zeroString = L"";
                        ErrorRef(NULL, WRN_UnassignedInternalField, field, zeroString);
                    }
                }
#if USAGEHACK
                else if (member->isMETHSYM()) {
                    METHSYM * meth = member->asMETHSYM();
                    if (!meth->isPropertyAccessor() && !meth->isUsed && (meth->access == ACC_INTERNAL || meth->access == ACC_PRIVATE) && !meth->isOverride && meth->getInputFile()->isSource) {
                        printf("%ls : %ls\n", meth->getInputFile()->name->text, ErrSym(meth));
                    }
                }
                else if (member->isPROPSYM()) {
                    PROPSYM * prop = member->asPROPSYM();
                    if ((prop->access == ACC_INTERNAL || prop->access == ACC_PRIVATE)&& !prop->isOverride && prop->getInputFile()->isSource) {
                        if (prop->methGet && !prop->methGet->isUsed) {
                            printf("%ls : %ls\n", prop->getInputFile()->name->text, ErrSym(prop->methGet));
                        }
                        if (prop->methSet && !prop->methSet->isUsed) {
                            printf("%ls : %ls\n", prop->getInputFile()->name->text, ErrSym(prop->methSet));
                        }
                    }
                }
#endif
            ENDFOREACHCHILD
        }
    }

    //
    // Now that we have an actual AssemblyDef record, compare it against all of the assumed matched references
    // If we are not building an assembly, then we will not have an AssemblyDef record we assume the module matches the reference because the shortname matched before.
    //
    if (BuildAssembly() && m_arlRefsToOutput != NULL) {
        CComPtr<IAssemblyName> panOutput;
        NAME * nameOutput;

        panOutput = importer.GetOutputAssemblyName( &assemFile, &nameOutput);
        for (AssemblyRefList * arlCurrent = m_arlRefsToOutput; arlCurrent != NULL; arlCurrent = arlCurrent->next) {
            importer.ConfirmMatchesThisAssembly(panOutput, nameOutput, arlCurrent->nameRef, arlCurrent->listModSrc, arlCurrent->fIsFriendAssemblyRefOnly);
        }
    }
}



/*
 * Do the full compile. The main process of compilation after all options
 * have been accumulated.
 */
void COMPILER::CompileAll(ICSCompileProgress * progressSink)
{

    SETLOCATIONSTAGE(BEGIN);
    OUTFILESYM *pAsmFile = NULL;
    OUTFILESYM *pOutfile = NULL;
    curFile = NULL;
    SourceOutFileIterator files;
    SourceFileIterator infiles;
    INFILESYM * pInfile = NULL;
    assemID = AssemblyIsUBM;
    HRESULT hr = S_OK;
#ifdef DEBUG
    haveDefinedAnyType = false;
#endif

    CErrorSuppression es;

    if (!AllowCLSErrors())
        checkCLS = false;

    ASSERT(compPhase == CompilerPhase::None);
    compPhase = CompilerPhase::Init;
    ASSERT(aggStateMax == AggState::None);

    // Handle initial incremental building step.
    if (options.m_fINCBUILD) {
        Error(NULL, WRN_IncrSwitchObsolete);
    }

    int oldErrors = ErrorCount();
    isCanceled = false;
    int iFile;

    // We have to check this here because the moduleassemblyname flag is set per compilation but the
    // output files are set per inputset.
    if (options.m_sbstrMODULEASSEMBLY && BuildAssembly())
    {
        Error(NULL, ERR_AssemblyNameOnNonModule);
        goto ENDCOMPILE;
    }

    // Clear any existing error info object, so we don't report stale errors.
    SetErrorInfo(0, NULL);

    // Add the standard metadata to the list of files to process.
    AddStandardMetadata();

    //
    // declare all the input files
    //
    // adds symbol table entries for all namespaces and
    // user defined types (classes, enums, structs, interfaces, delegates)
    // sets access modifiers on all user defined types
    //
    ASSERT(compPhase == CompilerPhase::Init);
    compPhase = CompilerPhase::DeclareTypes;

    DeclareTypes(progressSink);
    ReportProgress(progressSink, PHASE_DECLARETYPES, 1, 1);

    if (FAbortEarly(oldErrors, &es))
        goto ENDCOMPILE;

    // Initialize the alinker by getting the common dispenser
    GetMetadataDispenser();

    // Import meta-data.
    {
        SETLOCATIONSTAGE(PARSE);

        ASSERT(compPhase == CompilerPhase::DeclareTypes);
        compPhase = CompilerPhase::ImportTypes;

        importer.ImportAllTypes();
        ReportProgress(progressSink, PHASE_IMPORTTYPES, 1, 1);
        if (FAbortEarly(oldErrors, &es))
            goto ENDCOMPILE;

        // Initialize predefined types. This is done after a declaration
        // for every predefined type has already been seen.
        ASSERT(compPhase == CompilerPhase::ImportTypes);
        compPhase = CompilerPhase::InitPredefTypes;
        if (!getBSymmgr().InitPredefinedTypes() || FAbortEarly(oldErrors, &es))
            goto ENDCOMPILE;
    }

    //
    // resolves all using clauses
    // resolves all base classes and implemented interfaces
    //
    ASSERT(compPhase == CompilerPhase::InitPredefTypes);
    compPhase = CompilerPhase::ResolveInheritance;
    ASSERT(aggStateMax == AggState::None);
    aggStateMax = AggState::Inheritance;

    ResolveInheritanceHierarchy();

    //
    // define bounds on all type parameters on types
    //
    ASSERT(compPhase == CompilerPhase::ResolveInheritance);
    compPhase = CompilerPhase::DefineBounds;

    ASSERT(aggStateMax == AggState::Inheritance);
    aggStateMax = AggState::Bounds;

    DefineBounds();

    //
    // cannot define any types until the inheritance hierarchy is resolved
    //
    // if this assert fires then set a breakpoint at the 2 locations
    // where this variable is set to true and rerun your build
    //
    ASSERT(!haveDefinedAnyType);

    //
    // define all members of types
    //
    ASSERT(compPhase == CompilerPhase::DefineBounds);
    compPhase = CompilerPhase::DefineMembers;

    ASSERT(aggStateMax == AggState::Bounds);
    DefineMembers();

    // DefineMembers changes aggStateMax
    ASSERT(aggStateMax == AggState::DefinedMembers);

    ReportProgress(progressSink, PHASE_DEFINE, 1, 1);

    // Evaluate constants and compile attributes.
    ASSERT(compPhase == CompilerPhase::DefineMembers);
    compPhase = CompilerPhase::EvalConstants;

    EvaluateConstants();

    if (FAbortEarly(oldErrors, &es))
        goto ENDCOMPILE;

    //
    // prepare all the input files
    //
    // evaluates constants
    // checks field & method modifiers between classes (overriding & shadowing)
    //

    ASSERT(compPhase == CompilerPhase::EvalConstants);
    compPhase = CompilerPhase::Prepare;

    iFile = 0;
    for (pInfile = infiles.Reset(this); pInfile != NULL; pInfile = infiles.Next())
    {
        SETLOCATIONSTAGE(PREPARE);
        SETLOCATIONFILE(pInfile);

        if (pInfile->rootDeclaration)
            clsDeclRec.prepareNamespace(pInfile->rootDeclaration);

        if (ReportProgress(progressSink, PHASE_PREPARE, ++iFile, this->cInputFiles))
            goto ENDCOMPILE;
    }

    ASSERT(compPhase == CompilerPhase::Prepare);
    compPhase = CompilerPhase::PostPrepare;
    ASSERT(aggStateMax == AggState::DefinedMembers);
    aggStateMax = AggState::Last;

    // Right after prepare we check all types used in members for things like bogus, deprecated, constraints.
    CheckForTypeErrors();

    if (FAbortEarly(oldErrors, &es))
        goto ENDCOMPILE;

    if (FEncBuild())
        goto ENDCOMPILE;

    // This loop forces each file to find it's Main()
    // It also cause each output file to have a definite filename
    for (pOutfile = files.Reset(this); pOutfile != NULL; pOutfile = files.Next())
    {
        emitter.FindEntryPoint( pOutfile);
        if (pOutfile->isUnnamed()) {
            getCSymmgr().SetOutFileName( pOutfile->firstInfile());
        }
    }

    if (FAbortEarly(oldErrors, &es))
        goto ENDCOMPILE;

    //
    // initialize the assembly manifest emitter
    // even if we don't emit a manifest, we still
    // emit scoped typerefs
    //
    if (!options.m_fNOCODEGEN) {

        // decide on an Assembly file and create it!

        pAsmFile = GetManifestOutFile();
        if (!pAsmFile) {
            SourceOutFileIterator files;
            pAsmFile = files.Reset(this); // Just need the first one in the module building case.
            ASSERT(pAsmFile);
        }
        ASSERT(!BuildAssembly() == !pAsmFile->isManifest);

        pAsmFile->idFile = mdTokenNil;
        assemFile.BeginOutputFile(this, pAsmFile);

        if (BuildAssembly()) {
            if (FAILED(hr = linker->SetAssemblyFile(pAsmFile->name->text, assemFile.GetEmit(), (AssemblyFlags)(afNoDupTypeCheck | afNoRefHash | afDupeCheckTypeFwds), &assemID)) ||
                FAILED(hr = linker->SetPEKind(assemID, assemID, rgPEKind[options.m_platform], rgMachine[options.m_platform]))) {
                Error( NULL, ERR_ALinkFailed, ErrHR(hr));
            }
            else {
                pAsmFile->idFile = assemID;

                // Now add all the addmodules
                InFileIterator modules;

                for (pInfile = modules.Reset(getBSymmgr().GetMDFileRoot()); pInfile != NULL; pInfile = modules.Next())
                {
                    if (pInfile->isAddedModule) {
                        if (FAILED(hr = linker->AddImport( assemID, pInfile->mdImpFile, 0, &pInfile->mdImpFile))) {
                            Error( NULL, ERR_ALinkFailed, ErrHR(hr));
                        }
                    }
                }

                if (FriendsDeclared() && FAILED(hr = linker->EmitInternalExportedTypes( assemID))) {
                    Error( NULL, ERR_ALinkFailed, ErrHR(hr));
                }
            }
        } else {
            assemID = AssemblyIsUBM;
            if (FAILED(hr = linker->SetNonAssemblyFlags((AssemblyFlags)(afNoDupTypeCheck | afNoRefHash | afDupeCheckTypeFwds))) ||
                FAILED(hr = linker->AddFile(assemID, pAsmFile->name->text, 0, assemFile.GetEmit(), &pAsmFile->idFile)) ||
                FAILED(hr = linker->SetPEKind(assemID, pAsmFile->idFile, rgPEKind[options.m_platform], rgMachine[options.m_platform])))
                Error( NULL, ERR_ALinkFailed, ErrHR(hr));
        }
    }
    if (FAbortEarly(oldErrors, &es))
        goto ENDCOMPILE;

    ASSERT(compPhase == CompilerPhase::PostPrepare);
    compPhase = CompilerPhase::CompileMembers;

    //
    // compile all the input files
    //
    // emits all user defined types including fields, & method signatures
    // constant field values.
    //
    // compiles and emits all methods
    //
    // For each output file, emission of metadata happens in three phases in order to
    // have the metadata emitting work most efficiently. Each phase happens in the same
    // order.
    //   1. Emit typedefs for all types
    //   2. Emit memberdefs for all members
    //   3. Compile members and emit code, additional metadata (param tokens) onto the
    //      type and memberdefs emitted previously.
    //
    // Always does the assembly file first, then the other files.
    files.Reset(this);
    if (!(pOutfile = pAsmFile)) {
        pOutfile = files.Current();
        files.Next();
    }
    iFile = 0;
    do {
        INFILESYM * pInfile;
        InFileIterator iterInfiles;
        PEFile nonManifestOutputFile;

        if (!options.m_fNOCODEGEN) {
            SETLOCATIONSTAGE(EMIT);
            if (pAsmFile == pOutfile) {
                curFile = &assemFile;
            } else {
                curFile = &nonManifestOutputFile;
                curFile->BeginOutputFile(this, pOutfile);
                if (FAILED(hr = linker->AddFile(assemID, pOutfile->name->text, 0, curFile->GetEmit(), &pOutfile->idFile)) ||
                    FAILED(hr = linker->SetPEKind(assemID, pOutfile->idFile, rgPEKind[options.m_platform], rgMachine[options.m_platform]))) {
                    Error(NULL, ERR_ALinkFailed, ErrHR(hr));
                    // We can't go on if we couldn't add the file
                    curFile = NULL;
                    break;
                }
                if (pOutfile->firstInfile() == NULL)
                    Error( NULL, ERR_OutputNeedsInput, pOutfile->name->text);
            }
            emitter.BeginOutputFile();
        }

        oldErrors = ErrorCount();
        EmitTokens(pOutfile);

        if (oldErrors != ErrorCount())
            goto NEXT_FILE;

        if (!options.m_fNOCODEGEN && pAsmFile == pOutfile) {
            //
            // do assembly attributes
            //
            GlobalAttrBind::Compile(this, pAsmFile, assemblyAttributes, mdtAssembly);
            UnknownAttrBind::Compile(this, unknownGlobalAttributes);

            SetSigningOptions(pAsmFile->idFile);
            if (BuildAssembly()) {
                CErrorSuppression esLoc;
                if (!FAbortEarly(0, &esLoc)) {
                    // Write the manifest and save space for the crypto-keys
                    assemFile.WriteCryptoKey();
                }
            }
        }

        {
            SETLOCATIONSTAGE(COMPILE);
            for (pInfile = iterInfiles.Reset(pOutfile); pInfile != NULL; pInfile = iterInfiles.Next()) {
                SETLOCATIONFILE(pInfile);
                if (pInfile->rootDeclaration)
                    clsDeclRec.compileNamespace(pInfile->rootDeclaration);

                if (ReportCompileProgress(progressSink, ++iFile, this->cInputFiles))
                    goto ENDCOMPILE;
            }
        }

NEXT_FILE:
        if (!options.m_fNOCODEGEN) {

            // Write the executable file if no errors occured.
            emitter.EndOutputFile(!FAbortOutFile());
            if (pOutfile != pAsmFile) {
                curFile->EndOutputFile(!FAbortOutFile());
                if (ReportProgress(progressSink, PHASE_WRITEOUTPUT, 1, 1))
                    goto ENDCOMPILE;
            }
        }
        curFile = NULL;

        // Skip the manifest file because we already did it
        if ((pOutfile = files.Current()) == pAsmFile && pOutfile)
            pOutfile = files.Next();
        if (files.Current())
            files.Next();
    } while (pOutfile);

    if (!ErrorCount() && !options.m_fCompileSkeleton)
        PostCompileChecks(); // Only give these warnings if we had no errors.

    if (pAsmFile) {

        ASSERT(!curFile);
        curFile = &assemFile;
        if (!ErrorCount() && !options.m_fCompileSkeleton) {
            // Include the resources
            PRESFILESYM res;
            bool bFail = false;
            for (pOutfile = GetFirstOutFile(); pOutfile != NULL; pOutfile = pOutfile->nextOutfile()) {
                for (res = pOutfile->firstResfile(); res != NULL; res = res->nextResfile()) {
                    bFail |= !assemFile.CalcResource(res);
                }
            }
            if (!bFail && assemFile.AllocResourceBlob()) {
                for (pOutfile = GetFirstOutFile(); pOutfile != NULL; pOutfile = pOutfile->nextOutfile()) {
                    for (res = pOutfile->firstResfile(); res != NULL; res = res->nextResfile()) {
                        assemFile.AddResource(res);
                    }
                }
            }
        }

        if (BuildAssembly() && FAILED(hr = linker->PreCloseAssembly(assemID)))
            Error( NULL, ERR_ALinkFailed, ErrHR(hr));
        assemFile.EndOutputFile(!FAbortOutFile());
        curFile = NULL;
        if (ReportProgress(progressSink, PHASE_WRITEOUTPUT, 1, 1))
            goto ENDCOMPILE;

        {
            CErrorSuppression esLoc;
            if (!FAbortEarly(0, &esLoc) && FAILED(hr = linker->CloseAssembly(assemID)))
                Error(NULL, ERR_ALinkCloseFailed, ErrHR(hr));
        }

    } else if (!options.m_fNOCODEGEN) {
        // Error about including resources in non-Assembly
        PRESFILESYM res;
        for (pOutfile = GetFirstOutFile(); pOutfile != NULL; pOutfile = pOutfile->nextOutfile()) {
            for (res = pOutfile->firstResfile(); res != NULL; res = res->nextResfile()) {
                Error(NULL, ERR_CantRefResource, res->name->text);
            }
        }
    }

    if (ReportProgress(progressSink, PHASE_WRITEINCFILE, 1, 1))
        goto ENDCOMPILE;

ENDCOMPILE: ;
}


/*
 * Add a conditional symbol to be processed by the lexer.
 */
void COMPILER::AddConditionalSymbol(PNAME name)
{
    ccsymbols.Define (name);
}


/*
 * Replacement routine for SearchPath.  Uses pipe (|) character to separate path
 * elements (so that paths can contain semicolons). Returns TRUE on success.
 * Will return FALSE but set the filename if a matching file is found but the attributes are invalid (i.e. a directory).
 */
BOOL SearchPathDual(LPCWSTR lpPath, LPCWSTR lpFileName, WCBuffer lpBuffer)
{
    DWORD nBufferSize = max(lpBuffer.Count(), MAX_PATH);
    PWSTR wszLocalBuffer = STACK_ALLOC(WCHAR, nBufferSize);
    
    PWSTR wzClose = NULL;
    PCWSTR wzDirName = lpPath;
    while (wzDirName) {
        PCWSTR wzNext = wcschr(wzDirName, L'|');
        size_t len;
        if (wzNext == NULL) {
            len = wcslen(wzDirName);
        } else {
            len = wzNext - wzDirName;
            wzNext++; // skip the '|'
        }

        if (len > 0 && SUCCEEDED(StringCchCopyNW( wszLocalBuffer, nBufferSize, wzDirName, len)) &&
            MakePath(wszLocalBuffer, lpFileName, lpBuffer)) {

            DWORD dwAttrib = W_GetFileAttributes(lpBuffer);
            if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                return TRUE;
            else if (!wzClose)
            {
                // found a match, but the file attributes are invalid, save it for error reporting.
                wzClose = STACK_ALLOC(WCHAR, lpBuffer.Count());
                StringCchCopyNW(wzClose, nBufferSize, lpBuffer, wcslen(lpBuffer));
            }
        }
        wzDirName = wzNext;
    }

    if (wzClose)
        StringCchCopyNW(lpBuffer.GetData(), lpBuffer.Count(), wzClose, wcslen(wzClose));
    return FALSE;
}

bool COMPILER::checkForValidIdentifier(STRCONST *str, bool isPreprocessorString, BASENODE * tree, int id, ErrArg errArg)
{
   
    if (str &&
        namemgr->IsValidIdentifier(str->text, str->length, options.compatMode, (isPreprocessorString ? CheckIdentifierFlags::Simple : CheckIdentifierFlags::StandardSource)))
    {
        return true;
    }
    
    Error(tree, id, errArg);
    return false;
}

// Private helper to create an alias if it doesn't already exist and add this infile sym to it
void COMPILER::AddInfileToExternAliasWithErrors(INFILESYM * infile, PCWSTR pszAlias)
{
    NAME * aliasName = getNamemgr()->AddString(pszAlias);
    if (!namemgr->IsValidIdentifier(pszAlias, (int)wcslen(pszAlias), CompatibilityNone, CheckIdentifierFlags::Simple)) {
        Error(NULL, ERR_BadExternIdentifier, pszAlias);
    }
    else if ( aliasName == getNamemgr()->GetPredefName(PN_GLOBAL)) {
        Error(NULL, ERR_GlobalExternAlias);
    }
    else {
        AddInfileToExternAlias(infile, aliasName);
    }
}

void COMPILER::AddInfileToExternAlias(INFILESYM * infile, NAME * aliasName)
{
    EXTERNALIASSYM * aliasSym = LookupGlobalSym(aliasName, GetExternAliasContainer(), MASK_EXTERNALIASSYM)->asEXTERNALIASSYM();

    if (aliasSym == NULL) {
        aliasSym = getBSymmgr().CreateGlobalSym(SK_EXTERNALIASSYM, aliasName, GetExternAliasContainer())->asEXTERNALIASSYM();
        aliasSym->infileListTail = &aliasSym->infileList;
        aliasSym->aid = getBSymmgr().AidAlloc(aliasSym);
        aliasSym->nsa = getBSymmgr().GetRootNsAid(aliasSym->aid);
    }

    if (!aliasSym->infileList->contains(infile)) {
        getBSymmgr().AddToGlobalSymList(infile, &aliasSym->infileListTail);
        infile->AddToAlias(aliasSym->aid, &getGlobalSymAlloc());
        aliasSym->bsetAssemblies.SetBit(infile->GetAssemblyID(), &getGlobalSymAlloc());
    }
}


/*
 * Private helper function to add one metadata file with a given assembly id.
 */
INFILESYM * COMPILER::AddOneMetadataFile(PCWSTR filename, int aid)
{
    return getCSymmgr().CreateMDFile(filename, aid, getBSymmgr().GetMDFileRoot());
}

INFILESYM * COMPILER::FindAndAddMetadataFile(PCWSTR pszFile, int aid)
{
    WCHAR szFull[MAX_PATH];
    WCBuffer szFullBuffer(szFull);

    if (SearchPathDual(GetSearchPath(), pszFile, szFullBuffer))
        return AddOneMetadataFile(szFull, aid);

    DWORD dwAttr = W_GetFileAttributes( szFull);
    if ((dwAttr != INVALID_FILE_ATTRIBUTES) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
        Error(NULL, ERR_CantIncludeDirectory, pszFile);
    else
        Error(NULL, ERR_NoMetadataFile, pszFile);

    return NULL;
}

// Add the standard metadata library, unless disabled. This goes last, so
// explicit libraries can override it.
void COMPILER::AddStandardMetadata()
{
    WCHAR fullFilename[MAX_PATH];
    WCBuffer fullFilenameBuffer(fullFilename);

    if (!options.m_fNOSTDLIB) {
        // Search DLL path for the file.
        if (SearchPathDual(GetCorSystemDirectory(), L"mscorlib.dll", fullFilenameBuffer)) {
            // Found it. Add it as an input file.
            INFILESYM * infile = AddOneMetadataFile(fullFilename, kaidNil /* allocate the aid */);
            if (infile) {
                infile->AddToAlias(kaidGlobal, &getGlobalSymAlloc());
                getBSymmgr().SetAidForMsCorLib(infile->GetAssemblyID());
                getBSymmgr().bsetGlobalAssemblies.SetBit(infile->GetAssemblyID(), &getGlobalSymAlloc());
            }
        }
        else {
            // File not found. Report error.
            Error(NULL, ERR_NoStdLib, L"mscorlib.dll");
        }
    }
}

#if DEBUG

// Return the dword which lives under HKCU\Software\Microsoft\C# Compiler<value>
// If any problems are encountered, return 0
DWORD COMPILER::GetRegDWORD(PCSTR value)
{
    return 0;
}

// Return true if the registry string which lives under HKCU\Software\Microsoft\C# Compiler<value>
// is the same one as the string provided.
bool COMPILER::IsRegString(PCWSTR string, PCWSTR value)
{
    bool rval = false;
    return rval;
}

#endif

// Return the dword which lives under HKCU\Software\Microsoft\C# Compiler<value>
// If any problems are encountered, return 0
DWORD COMPILER::GetRegDWORDRet(PCSTR value)
{
    return 0;
}

// Return true if the registry string which lives under HKCU\Software\Microsoft\C# Compiler<value>
// is the same one as the string provided.
bool COMPILER::IsRegStringRet(PCWSTR string, PCWSTR value)
{
    bool rval = false;
    return rval;
}

BSTR COMPILER::GetRegStringRet(PCSTR value)
{
    BSTR rval = NULL;
    return rval;
}

const PCWSTR g_stages[] = {
#define DEFINE_STAGE(stage) L###stage,
#include "stage.h"
#undef DEFINE_STAGE
};

////////////////////////////////////////////////////////////////////////////////
// CompilerExceptionFilter
//
//  This hits whenever we hit an unhandled ASSERT or GPF in the compiler
//  Here we dump the entire LOCATION stack to the error channel.
//

void COMPILER::ReportICE(EXCEPTION_POINTERS * exceptionInfo)
{
    // We shouldn't get here on a fatal error exception
    ASSERT(exceptionInfo->ExceptionRecord->ExceptionCode != FATAL_EXCEPTION_CODE);
    if (exceptionInfo->ExceptionRecord->ExceptionCode == FATAL_EXCEPTION_CODE) {
        return;
    }

    LOCATION * loc = location;
    if (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_STACK_OVERFLOW) {
        // Don't try for a source location because the SetLine, SetStart, and SetENd will also overflow
        while (loc && !stackOverflowLocation) {
            stackOverflowLocation = loc->getSymbol();
            loc = loc->getPrevious();
        }
        if (stackOverflowLocation == NULL && location)
            stackOverflowLocation = location->getFile();
    } else if (loc) {

        //
        // dump probable culprit
        //
        Error(NULL, ERR_ICE_Culprit, exceptionInfo->ExceptionRecord->ExceptionCode, g_stages[loc->getStage()],
            ErrArgPtr(exceptionInfo->ExceptionRecord->ExceptionAddress));

        //
        // dump location stack
        //
        do {

            PCWSTR stage = g_stages[loc->getStage()];

            //
            // dump one location
            //
            SYM *symbol = loc->getSymbol();
            if (symbol) {
                //
                // we have a symbol report it nicely
                //
                ErrorRef(NULL, ERR_ICE_Symbol, symbol, stage);
            } else {
                INFILESYM *file = loc->getFile();
                if (file) {
                    BASENODE *node = loc->getNode();
                    if (node) {
                        //
                        // we have stage, file and node
                        //
                        compiler()->Error(node, ERR_ICE_Node, stage);
                    } else {
                        //
                        // we have stage and file
                        //
                        Error(ERRLOC(file), ERR_ICE_File, stage);
                    }
                } else {
                    //
                    // only thing we have is the stage
                    //
                    Error(NULL, ERR_ICE_Stage, stage);
                }
            }

            loc = loc->getPrevious();
        } while (loc);
    } else {
        //
        // no location at all!
        //
        Error(NULL, ERR_ICE_Culprit, exceptionInfo->ExceptionRecord->ExceptionCode, g_stages[0],
            ErrArgPtr(exceptionInfo->ExceptionRecord->ExceptionAddress));
    }
}

////////////////////////////////////////////////////////////////////////////////
// COMPILER::GetFirstOutFile

OUTFILESYM * COMPILER::GetFirstOutFile()
{
    return getBSymmgr().GetFileRoot()->firstChild->asOUTFILESYM();
}

OUTFILESYM * COMPILER::GetManifestOutFile()
{
    if (BuildAssembly()) {
        for (OUTFILESYM * outfile = GetFirstOutFile(); outfile; outfile = outfile->nextOutfile()) {
            if (outfile->isManifest)
                return outfile;
        }
        // BuildAssembly shouldn't be true if none of the outfiles is a manifest file.
        ASSERT(false);
    }

    return NULL;
}


/*
 * Loads mscorpe.dll and gets an ICeeFileGen interface from it.
 * The ICeeFileGen interface is used for the entire compile.
 */
ICeeFileGen* COMPILER::CreateCeeFileGen()
{
    // Dynamically bind to ICeeFileGen functions.
    if (!pfnCreateCeeFileGen || !pfnDestroyCeeFileGen) {

        HRESULT hr = LoadLibraryShim(MSCORPE_NAME, NULL, NULL, &hmodCorPE);
        if (SUCCEEDED(hr) && hmodCorPE) {
            // Get the required methods.
            pfnCreateCeeFileGen  = (HRESULT (__stdcall *)(ICeeFileGen **ceeFileGen)) GetProcAddress(hmodCorPE, "CreateICeeFileGen");
            pfnDestroyCeeFileGen = (HRESULT (__stdcall *)(ICeeFileGen **ceeFileGen)) GetProcAddress(hmodCorPE, "DestroyICeeFileGen");
            if (!pfnCreateCeeFileGen || !pfnDestroyCeeFileGen)
                Error(NULL, FTL_ComPlusInit, ErrGetLastError());
        }
        else {
            // MSCorPE.DLL wasn't found.
            Error(NULL, FTL_RequiredFileNotFound, MSCORPE_NAME);
        }
    }

    ICeeFileGen *ceefilegen = NULL;
    HRESULT hr = pfnCreateCeeFileGen(& ceefilegen);
    if (FAILED(hr)) {
        Error(NULL, FTL_ComPlusInit, ErrHR(hr));
    }

    return ceefilegen;
}

void COMPILER::DestroyCeeFileGen(ICeeFileGen *ceefilegen)
{
    pfnDestroyCeeFileGen(&ceefilegen);
}

void COMPILER::SetSigningOptions(mdToken FileToken)
{
    HRESULT hr = S_OK;
    const AssemblyOptions opt[] = { optAssemHalfSign, optAssemKeyFile, optAssemKeyName };
    const int optIndex[] = { OPTID_DELAYSIGN, OPTID_KEYFILE, OPTID_KEYNAME };
    const PCWSTR optCAName [] = { L"System.Reflection.AssemblyDelaySignAttribute",
        L"System.Reflection.AssemblyKeyFileAttribute",
        L"System.Reflection.AssemblyKeyNameAttribute" };

    for (int i = 0; i < (int)lengthof(opt); i++) {
        CComBSTR bstrTemp;
        VARIANT var;
        switch (opt[i]) {
        case optAssemHalfSign:
            // COF_DEFAULTON actually doesn't equal TRUE or FALSE
            // we use that to detect if the option was set on the command-line
            ASSERT(TRUE != COF_DEFAULTON && FALSE != COF_DEFAULTON);
            if (options.m_fDELAYSIGN == COF_DEFAULTON) {
                continue;
            }
            else {
                V_VT (&var) = VT_BOOL;
                V_BOOL (&var) = (options.m_fDELAYSIGN != FALSE);
            }
            break;
        case optAssemKeyFile:
            if (options.m_sbstrKEYFILE == NULL)
                // Don't set it if it's NULL
                continue;
            if (options.m_fCompileSkeleton && PathIsRelativeW(options.m_sbstrKEYFILE)) {
                bstrTemp = L"..\\";
                bstrTemp.Append(options.m_sbstrKEYFILE);
                V_BSTR (&var) = bstrTemp;
            } else {
                V_BSTR (&var) = options.m_sbstrKEYFILE;
            }
            V_VT (&var) = VT_BSTR;
            break;
        case optAssemKeyName:
            if (options.m_sbstrKEYNAME == NULL)
                // Don't set it if it's NULL
                continue;
            V_VT (&var) = VT_BSTR;
            V_BSTR (&var) = options.m_sbstrKEYNAME;
            break;
        default:
            VSFAIL("Need to handle this option");
        }
     
        hr = linker->SetAssemblyProps(assemID, FileToken, opt[i], var);
        if (FAILED(hr)) {
            // Some unknown failure
            Error( NULL, ERR_ALinkFailed, ErrHR(hr));
        } else if (hr == S_FALSE) {
            // This means this is a dup
            Error(NULL, WRN_CmdOptionConflictsSource,
                options.m_rgOptionTable[COptionData::GetOptionIndex(optIndex[i])].pszDescSwitch, optCAName[i]);
        } else if (hr != S_OK) {
            Error( NULL, WRN_ALinkWarn, ErrHR(hr));
        }
    }
}

// Catches ALink warnings and errors (the ones not returned as HRESULTS)
// and reports them
HRESULT ALinkError::OnError(HRESULT hrError, mdToken tkHint)
{
    // We only want ALink errors
    if (HRESULT_FACILITY(hrError) != FACILITY_ITF)
        return S_FALSE;

    // For now, we only want Warnings
    if (FAILED(hrError)) {
        compiler()->Error( NULL, ERR_ALinkFailed, compiler()->ErrHR(hrError));
    } else {
        compiler()->Error( NULL, WRN_ALinkWarn, compiler()->ErrHR(hrError));
    }
    return S_OK;
}

// Function to round a double to float precision. Does as an out-of-line
// function that isn't inlined so that the compiler won't optimize it
// away and keep higher precision.
void RoundToFloat(double d, float * f)
{
    *f = (float) d;
}


unsigned int COMPILER::getPredefIndex(TYPESYM * type)
{
    if (type->isPTRSYM()) {
        return PT_UINTPTR;
    } else if (type->isAGGTYPESYM() && type->isPredefined()) {
        PREDEFTYPE pt = type->getPredefType();
        if (type->isSimpleType() || pt == PT_INTPTR || pt == PT_UINTPTR) {
            return pt;
        }
    }
    return UNDEFINEDINDEX;
}

void COMPILER::ReportStackOverflow()
{
    SYM * sym = stackOverflowLocation;

    if (sym) {
        INFILESYM * infile = sym->GetSomeInputFile();
        if (infile)
            Error(ERRLOC(infile), FTL_StackOverflow, sym);
        else
            Error(NULL, FTL_StackOverflow, sym);
    } else {
        Error(NULL, FTL_StackOverflow, L"");
    }
}

bool COMPILER::IsBuildingMSCORLIB()
{
    AGGSYM *object = GetReqPredefAgg(PT_OBJECT, false);
    return object->isSource;
}

bool COMPILER::EmitRuntimeCompatibility()
{
    return m_fEmitRuntimeCompatibility && GetOptPredefType(PT_RUNTIMECOMPATIBILITY);
}

bool COMPILER::WrapNonExceptionThrows()
{
    return m_fWrapNonExceptionThrows && GetOptPredefType(PT_RUNTIMECOMPATIBILITY);
}






bool GUIDFromString( PCWSTR szGuidStart,  PCWSTR szGuidEnd, GUID * pGUID)
{
    ASSERT(szGuidStart);
    ASSERT(pGUID);

    if (szGuidEnd == NULL)
        szGuidEnd = szGuidStart + wcslen(szGuidStart);

    DWORD Data1;
    DWORD Data2, Data3, Data4, Data5;
    DWORD Data6, Data7, Data8, Data9, Data10, Data11;
    int cchScanned = 0;
    if (szGuidEnd - szGuidStart == 36) {
        if (11 != swscanf_s(szGuidStart, L"%8x-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x%n", &Data1, &Data2,
            &Data3, &Data4, &Data5, &Data6, &Data7, &Data8, &Data9, &Data10, &Data11, &cchScanned) ||
            cchScanned != 36) 
        {
            return false;
        }
    } else if (szGuidEnd - szGuidStart != 38 ||
        11 != swscanf_s(szGuidStart, L"{%8x-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x}%n", &Data1, &Data2,
            &Data3, &Data4, &Data5, &Data6, &Data7, &Data8, &Data9, &Data10, &Data11, &cchScanned) ||
        cchScanned != 38)
    {
        return false;
    }

    pGUID->Data1 = Data1;
    pGUID->Data2 = (WORD)Data2;
    pGUID->Data3 = (WORD)Data3;
    pGUID->Data4[0] = (CHAR)Data4;
    pGUID->Data4[1] = (CHAR)Data5;
    pGUID->Data4[2] = (CHAR)Data6;
    pGUID->Data4[3] = (CHAR)Data7;
    pGUID->Data4[4] = (CHAR)Data8;
    pGUID->Data4[5] = (CHAR)Data9;
    pGUID->Data4[6] = (CHAR)Data10;
    pGUID->Data4[7] = (CHAR)Data11;

    return true;
}
