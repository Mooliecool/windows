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
// File: error.cpp
//
// Error handling for the compiler
// ===========================================================================

#include "stdafx.h"

/*
 * Information about each error or warning.
 */
struct ERROR_INFO {
    short number;       // error number
    short level;        // warning level; 0 means error
    int   resid;        // resource id.
};

#define ERRORDEF(num, name, strid)       {num,     0, strid},
#define WARNDEF(num, level, name, strid) {num, level, strid},
#define OLDWARN(num, name)               {num,    99,    -1},
#define FATALDEF(num, name, strid)       {num,    -1, strid},

static const ERROR_INFO errorInfo[ERR_COUNT] = {
    {0000, -1, 0},          // ERR_NONE - no error.
    #include "errors.h"
};

#undef ERRORDEF
#undef WARNDEF
#undef OLDWARN
#undef FATALDEF

short ErrorNumberFromID(long iErrorIndex) { return errorInfo[iErrorIndex].number; }

#define LoadStringA(hInstance,uID,lpBuffer,cchBufferMax) \
    PAL_LoadSatelliteStringA((HSATELLITE)hInstance, uID, lpBuffer, cchBufferMax)
#define LoadStringW(hInstance,uID,lpBuffer,cchBufferMax) \
    PAL_LoadSatelliteStringW((HSATELLITE)hInstance, uID, lpBuffer, cchBufferMax)

#ifdef UNICODE
#define LoadString LoadStringW
#else
#define LoadString LoadStringA
#endif // UNICODE


#ifdef DEBUG

//
// Wrapper for LoadString that works on Win9x.
//
int LoadStringWide(HINSTANCE hInstance, UINT uID, WCBuffer lpBuffer)
{
    int cch = LoadStringW(hInstance, uID, lpBuffer.GetData(), lpBuffer.Count());
    if (cch == 0 && GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
        // Use ANSI version:
        CHAR * lpBufferAnsi = (CHAR *) _alloca(lpBuffer.Count() * 2);
        int cchAnsi = LoadStringA(hInstance, uID, lpBufferAnsi, lpBuffer.Count() * 2);
        if (cchAnsi) {
            cch = MultiByteToWideChar(CP_ACP, 0, lpBufferAnsi, cchAnsi, lpBuffer.GetData(), lpBuffer.Count() - 1);
            lpBuffer.SetAt(cch, 0);
        } else {
            ASSERT(!"String Not Found");
            return 0;
        }
    }
    return cch;
}

bool IsWarningID(long iErrorIndex) { return errorInfo[iErrorIndex].level > 0; }

// DEBUG-only function to check the integrity of the error info -- 
// check that all error messages exist, and that no error numbers or
// messages are duplicated. This detects common mistakes when editing
// errors.h
void CheckErrorMessageInfo(HINSTANCE hModuleMessages, MEMHEAP * heap, bool dumpAll)
{
    int * messageIds = (int *) heap->AllocZero(sizeof(int *) * 0x10000);
    int * errorNos = (int *) heap->AllocZero(sizeof(int *) * 0x10000);
    wchar_t dummy[4096];
    WCBuffer dummyBuffer(dummy);

    for (int iErr = 1; iErr < ERR_COUNT; ++iErr) {
        if (errorInfo[iErr].level == 99) {
            ASSERT(errorInfo[iErr].resid == -1);
            // These warnings are no longer used, but kept to prevent
            // spurious errors on the "/nowarn" option
            continue;
        }

        //  a few messages are duplicated intentionally.
        if (iErr != ERR_DeprecatedSymbolStr &&
            iErr != WRN_InvalidNumber &&
            iErr != WRN_FileNameTooLong && 
            iErr != WRN_EndOfPPLineExpected &&
            errorInfo[iErr].resid != IDS_FeatureDeprecated) 
        {
            ASSERT(messageIds[errorInfo[iErr].resid] == 0); // duplicate message ID
            messageIds[errorInfo[iErr].resid] = iErr;
        }

        ASSERT(errorInfo[iErr].number > errorInfo[iErr - 1].number); // They need to stay sorted!

        if (iErr != FTL_NoMessagesDLL) // intentionally no mesage for this one!
            ASSERT(LoadStringWide(hModuleMessages, errorInfo[iErr].resid, dummyBuffer) != 0); // missing message

        ASSERT(errorNos[errorInfo[iErr].number] == 0); // duplicate error number
        errorNos[errorInfo[iErr].number] = iErr;

        if (dumpAll) {
            dummy[0] = L'\0';
            LoadStringWide(hModuleMessages, errorInfo[iErr].resid, dummyBuffer);
            wprintf(L"%d\t%d\t%s\n", errorInfo[iErr].number, errorInfo[iErr].level, dummy);
        }
    }

    ASSERT(errorInfo[ERR_ErrorDirective].number == 1029);
    ASSERT(errorInfo[ERR_NonECMAFeature].number == 1644);
    ASSERT(errorInfo[ERR_EndRegionDirectiveExpected].number == 1038);
    ASSERT(errorInfo[ERR_ExpectedEndTry].number == 1524);
    heap->Free(messageIds);
    heap->Free(errorNos);
}

#endif //DEBUG


////////////////////////////////////////////////////////////////////////////////
// CAutoFree -- helper class to allocate memory from the heap if possible, and
// free on destruction.  Intended to use in possible low-stack conditions; if
// allocation fails, attempt allocation on the stack, as in the following:
//
// CAutoFree<WCHAR>    f;
// PWSTR               p;
//
// p = SAFEALLOC (f, 128);
//

template <class T>
class CAutoFree
{
    T   *m_pMem;
public:
    CAutoFree () : m_pMem(NULL) {}
    ~CAutoFree () { Free(); }
    void    Free() { if (m_pMem) { VSFree (m_pMem); m_pMem = NULL; } }
    bool    AFAlloc(int iSize) { Free(); return !!(m_pMem = (T *)VSAlloc (SizeMul(iSize, sizeof (T)))); }
    T       *Mem() { return m_pMem; }
    T       *Cast(void *p) { return (T*)p; }
    long    ElementSize () { return sizeof (T); }
};

#define SAFEALLOC(mem,size) (mem.AFAlloc(size) ? mem.Mem() : mem.Cast(_alloca(size*mem.ElementSize())))


/*
 * Helper function and load and format a message. Uses Unicode
 * APIs, so it won't work on Win95/98.
 */
static bool LoadAndFormatW(HINSTANCE hModuleMessages, int resid, va_list args, VarArgList args2, __out_ecount(cchMax) PWSTR buffer, int cchMax)
{
    ASSERT(!args || !args2.Args());

    CAutoFree<WCHAR>    mem;
    DWORD               cchLen;
    PWSTR              formatString;
    DWORD flags = args2.Args() ? (FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY) : FORMAT_MESSAGE_FROM_STRING;

    cchLen = cchMax;
    formatString = SAFEALLOC (mem, cchLen);
    if (!formatString) {
        SetLastError(ERROR_OUTOFMEMORY);
        return false;
    }
    int rval = LoadStringW(hModuleMessages, resid, formatString, cchLen);
    if (!rval) return false;

    return (FormatMessageW (flags, formatString, 0, 0, buffer, cchMax, args2.Args() ? (va_list *)args2.Args() : &args) != 0);
}


/*
 * Helper function to load and format a message using ANSI functions.
 * Used as a backup when Unicode ones are not available.
 */
static bool LoadAndFormatA(HINSTANCE hModuleMessages, int resid, va_list args, VarArgList args2, __out_ecount(cchMax) PWSTR buffer, int cchMax)
{
    return false;
}

bool __cdecl LoadAndFormatMessage(int resid, __out_ecount(cchMax) PWSTR buffer, int cchMax, ...)
{
    va_list args;
    bool success;

    va_start(args, cchMax);

    if (W_IsUnicodeSystem())
        success = LoadAndFormatW(hModuleMessages, resid, args, NULL, buffer, cchMax);
    else
        success = LoadAndFormatA(hModuleMessages, resid, args, NULL, buffer, cchMax);

    va_end(args);

    return success;

}

bool __cdecl LoadAndFormatMessage(HINSTANCE hModuleMessages, int resid, __out_ecount(cchMax) PWSTR buffer, int cchMax, ...)
{
    va_list args;
    bool success;

    va_start(args, cchMax);

    if (W_IsUnicodeSystem())
        success = LoadAndFormatW(hModuleMessages, resid, args, NULL, buffer, cchMax);
    else
        success = LoadAndFormatA(hModuleMessages, resid, args, NULL, buffer, cchMax);

    va_end(args);

    return success;
}

////////////////////////////////////////////////////////////////////////////////
// CController::CheckDisplayWarning
//
// This function determines whether a warning should be displayed or suppressed,
// taking into account the warning level and "no warn" list.

BOOL CController::CheckDisplayWarning(long iErrorIndex, int warnLevel)
{
    ASSERT(warnLevel > 0);  // this must be a warning.

    ASSERT(errorInfo[iErrorIndex].level != 99);
    if (errorInfo[iErrorIndex].level == 99)
        return FALSE;   // This isn't a real warning so don't report it

    if (warnLevel > m_OptionData.warnLevel)
        return FALSE;   // warning level suppressed the warning.

    if (m_OptionData.IsNoWarnNumber(errorInfo[iErrorIndex].number))
        return FALSE;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// CController::CreateError
//
// This function creates a CError object from the given information.  The CError
// object is created with no location information.  The CError object is returned
// with a ref count of 0.
//
// If "warnOverride" is true (defaults to false), and this error is usually
// an error (not warning or fatal), then the error is wrapped with warning WRN_ErrorOverride
HRESULT CController::CreateError(long iErrorIndex, VarArgList args, CError **ppError, bool warnOverride)
{
    // Make some assertions...
    ASSERT (iErrorIndex < ERR_COUNT && iErrorIndex > 0);
    ASSERT (iErrorIndex != WRN_ErrorOverride);

    *ppError = NULL;

    if (FetchAtIndex(errorInfo, iErrorIndex).level == 0 && warnOverride)
    {
        if ( !CheckDisplayWarning(WRN_ErrorOverride, errorInfo[WRN_ErrorOverride].level))
            return S_FALSE;
    }
    else
    {
        // Clear this bit (since we clearly aren't overriding an error to a warning)
        warnOverride = false;

        // If it's a warning, does it meet the warning level criteria?
        if (FetchAtIndex(errorInfo, iErrorIndex).level > 0 && !CheckDisplayWarning(iErrorIndex, FetchAtIndex(errorInfo, iErrorIndex).level))
            return S_FALSE;
    }

    CComObject<CError>  *pObj;
    HRESULT             hr;

    // Okay, create the error node and return it
    if (SUCCEEDED (hr = CComObject<CError>::CreateInstance (&pObj)))
    {
        if (FAILED (hr = pObj->Initialize (hModuleMessages, iErrorIndex, args)))
            goto LERROR;
        *ppError = pObj;
    }

    // Do we need to wrap this Error in a warning?
    if (warnOverride && *ppError) {
        ASSERT(SUCCEEDED(hr));
        ASSERT(pObj->Kind() == ERROR_ERROR);

        PCWSTR rgpsz[2];
        rgpsz[0] = pObj->GetText();
        rgpsz[1] = (PCWSTR)(INT_PTR)pObj->ID();

        // Okay, re-init the the warning node and return it
        if (FAILED (hr = pObj->Initialize(hModuleMessages, WRN_ErrorOverride, rgpsz)))
            goto LERROR;
    }

    // Now check to see if we need to promote this warning to an error
    if (pObj && pObj->Kind() == ERROR_WARNING && m_OptionData.IsWarnAsError(pObj->ID()) && FAILED(hr = pObj->WarnAsError(hModuleMessages)))
        goto LERROR;

    return hr;

LERROR:
    m_spHost->OnCatastrophicError (FALSE, 0, NULL);
    if (pObj)
        delete pObj;
    *ppError = NULL;
    return hr;

}

////////////////////////////////////////////////////////////////////////////////
// CController::SubmitError
//
// This function places a fully-constructed CError object into an error container
// and sends it to the compiler host (this would be the place to batch these guys
// up if we decide to.
//
// Note that if the error can't be put into a container (if, for example, we
// can't create a container) the error is destroyed and the host is notified via
// OnCatastrophicError.

void CController::SubmitError (CError *pError)
{

    // Allow NULL -- this is often called with a function that returns an error as
    // an argument; it may not actually be an error.
    if (pError == NULL)
        return;

    // So we addref and release properly.
    CComPtr<CError> perr = pError;

    // Remember that we had an error (if this isn't a warning)
    if (pError->Kind() != ERROR_WARNING)
    {
        if (pError->WasWarning())
            m_iWarnAsErrorsReported++;
        m_iErrorsReported++;
    }

    if (m_fSuppressErrors && pError->Kind() != ERROR_FATAL)
        return;

    // Make sure we have an error container we can use.  Note that we (somewhat hackily)
    // check the ref count on any existing container, and if 1, re-use it.  (If it's
    // 1, it means we have the only ref on it, so nobody will be hurt by re-using it).
    if (m_pCompilerErrors != NULL)
    {
        if (m_pCompilerErrors->RefCount() == 1)
        {
            // This one can be re-used -- just empty it.
            m_pCompilerErrors->ReleaseAllErrors();
        }
        else
        {
            m_pCompilerErrors->Release();
            m_pCompilerErrors = NULL;
        }
    }

    if (m_pCompilerErrors == NULL)
    {
        // Create a new container for the errors
        if (FAILED (CErrorContainer::CreateInstance (EC_COMPILATION, 0, &m_pCompilerErrors)))
        {
            m_spHost->OnCatastrophicError (FALSE, 0, NULL);
            return;
        }
    }

    // We must have a container by now!  Add the error and push it to the host.
    ASSERT (m_pCompilerErrors != NULL);
    if (SUCCEEDED (m_pCompilerErrors->AddError (pError))) {
        m_spHost->ReportErrors (m_pCompilerErrors);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CController::ReportErrorsToHost
//
// This function reports non-empty error containers to the host. It also keeps
// track of whether non-empty containers have actual errors (i.e. something
// other than warnings).

void CController::ReportErrorsToHost (ICSErrorContainer *pErrors)
{
    if (pErrors == NULL)
        return;

    long    iCount, iWarnings, iWarnAsErrors;

    if (SUCCEEDED (pErrors->GetErrorCount (&iWarnings, NULL, NULL, &iCount)))
    {
        if (iCount > 0)
        {
            // This container has errors/warnings.  Give it to the host.
            m_spHost->ReportErrors (pErrors);

            // Count the ones that aren't warnings
            m_iErrorsReported += (iCount - iWarnings);

            if (SUCCEEDED(pErrors->GetWarnAsErrorCount( &iWarnAsErrors)))
                m_iWarnAsErrorsReported += iWarnAsErrors;
        }
    }
}

/*
 * ThrowFatalException. After a fatal error occurs, this calls to throw
 * an exception out to the outer-most code to abort the compilation.
 */
void COMPILER::ThrowFatalException()
{
    RaiseException(FATAL_EXCEPTION_CODE, 0, 0, NULL);
}

/*
 * Code has caught an exception. Handle it. If the exception code is
 * FATAL_EXCEPTION_CODE, this is the result of a previously reported
 * fatal error and we do nothing. Otherwise, we report an internal
 * compiler error.
 */
void COMPILER::HandleException(DWORD exceptionCode)
{
    Error (NULL, FTL_InternalError, exceptionCode);
}

/*
 * The following routines create "fill-in" strings to be used as insertion
 * strings in an error message. They allocate memory from a single static
 * buffer used for the purpose, and freed when the error message is
 * reported.
 * The buffer initially reserves 2MB worth of memory, but only commits
 * 1 page at a time.  It commits new pages as needed.  The
 * LoadAndFormat routines used to actually create the error message are
 * try a 4K buffer, a 32K buffer, and then a 2MB buffer.
 * Although ErrBufferLast does tell how much more memory can be used/allocated
 * We specifically don't ever commit the last page, so that most of this
 * code does not have to do accurate buffer-length calculations.
 * If the user really does have a 2MB error message, than an ICE is
 * acceptable.
 */

/*
 * Create a fill-in string describing an HRESULT.
 */
PCWSTR COMPILER::ErrHR(HRESULT hr, bool useGetErrorInfo)
{
    VERIFYLOCALHANDLER;

    START_ERR_STRING(this);

    do {
        if (useGetErrorInfo) {
            // See if there is more detailed error message available via GetErrorInfo.
            CComPtr<IErrorInfo> err;
            CComBSTR str;
            if (SUCCEEDED(GetErrorInfo( 0, &err)) && err != NULL &&
                SUCCEEDED(err->GetDescription(&str))) {
                ErrAppendString(str);
                break;
            }
        } 
        
        // Check for some well-known HRESULTS that aren't in the system database.
        if (hr >= CLDB_E_FILE_BADREAD && hr <= CLDB_E_BADUPDATEMODE) {
            ErrAppendId(IDS_CLB_ERROR_FIRST + (hr - CLDB_E_FILE_BADREAD));
        }

        DWORD dwFormatMessageFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

        // Use FormatMessage to get the error string for the HRESULT from the system.
        if (0 != (errBufferNext += FormatMessageW(dwFormatMessageFlags,
                            NULL, hr, 0,
                            errBufferNext, ErrBufferLeftTot(),
                            NULL))) {
            break;
        }


        // Check for errors, and possibly repeat.
        if (hr != E_FAIL) {
            hr = E_FAIL;
        }
        else {
            // Something really extreme. I don't understand why this would happen.
            ASSERT(0);      // investigate please.
            return L"";
        }
    } while (true);

    return END_ERR_STRING(this);
}


/*
 * Create a fill-in string describing the last Win32 error.
 */
PCWSTR COMPILER::ErrGetLastError()
{
    return ErrHR(HRESULT_FROM_WIN32(GetLastError()), false);
}

/*
 * For now, just return the text of the name...
 */
PCWSTR COMPILER::ErrName(PNAME name)
{
    START_ERR_STRING(this);
    ErrAppendName(name);
    return END_ERR_STRING(this);
}


/*
 * For now, just return the text of the access modifier
 */
PCWSTR COMPILER::ErrAccess(ACCESS acc)
{

    VERIFYLOCALHANDLER;

    switch (acc) {
    case ACC_PUBLIC:
        return CParser::GetTokenInfo( TID_PUBLIC)->pszText;

    case ACC_PROTECTED:
        return CParser::GetTokenInfo( TID_PROTECTED)->pszText;

    case ACC_INTERNAL:
        return CParser::GetTokenInfo( TID_INTERNAL)->pszText;

    case ACC_INTERNALPROTECTED:
    {
        START_ERR_STRING(this);

        ErrAppendString(CParser::GetTokenInfo( TID_PROTECTED)->pszText);
        ErrAppendChar(L' ');
        ErrAppendString(CParser::GetTokenInfo( TID_INTERNAL)->pszText);

        return END_ERR_STRING(this);
    }

    case ACC_PRIVATE:
        return CParser::GetTokenInfo( TID_PRIVATE)->pszText;

    default:
        ASSERT( !"Unknown access modifier");
        return L"";
    }
}

/*
 * Create a fill-in string describing a possibly fully qualified name.
 */
void COMPILER::ErrAppendNameNode(BASENODE *name)
{
    switch (name->kind) {
    case NK_NAME:
    case NK_ALIASNAME:
        // non-dotted name, just do the regular name thing
        ErrAppendName(name->asSingleName()->pName);
        break;

    case NK_GENERICNAME:
        ErrAppendName(name->asANYNAME()->pName);
        ErrAppendString(L"<...>");
        break;

    case NK_OPENNAME:
        ErrAppendName(name->asOPENNAME()->pName);
        ErrAppendChar(L'<');
        for (int carg = name->asOPENNAME()->carg; --carg > 0; )
            ErrAppendChar(L',');
        ErrAppendChar(L'>');
        break;

    case NK_DOT:
        {
            // now, find the first name:
            BASENODE *first = name->asDOT()->p1;
            while (first->kind == NK_DOT)
                first = first->asDOT()->p1;

            //
            // add the first name, unless this is a fully qualified name
            //
            if (first->asANYNAME()->pName != namemgr->GetPredefName(PN_EMPTY)) {
                ErrAppendNameNode(first->asANYNAME());
            }

            //
            // add the remaining names
            //
            do {
                // loop until we add all the names
                first = first->pParent;
                ASSERT(first->kind == NK_DOT && first->asDOT()->p2->IsAnyName());

                if (first->IsDblColon())
                    ErrAppendString(L"::");
                else
                    ErrAppendChar(L'.');
                ErrAppendNameNode(first->asDOT()->p2);

                // is this the rightmost name?
            } while (first != name);
        }
        break;

    default:
        VSFAIL("Bad node");
        break;
    }
}

/*
 * Create a fill-in string describing a parsed type node
 * Note: currently only works for predefined types, and named types
 *       does not work for arrays, pointers, etc.
 */
PCWSTR COMPILER::ErrTypeNode(TYPEBASENODE *type)
{
    START_ERR_STRING(this);
    ErrAppendTypeNode(type);
    return END_ERR_STRING(this);
}


void COMPILER::ErrAppendTypeNode(TYPEBASENODE *type)
{
    switch (type->kind) {
    case NK_PREDEFINEDTYPE:
        // NOTE: we may not have predefined types installed when we call this
        ErrAppendString(getBSymmgr().GetNiceName((PREDEFTYPE)type->asPREDEFINEDTYPE()->iType));
        break;

    case NK_NAMEDTYPE:
        ErrAppendNameNode(type->asNAMEDTYPE()->pName);
        break;

    case NK_OPENTYPE:
        ErrAppendNameNode(type->asOPENTYPE()->pName);
        break;

    case NK_ARRAYTYPE:
        ErrAppendTypeNode(type->asARRAYTYPE()->pElementType);
        ErrAppendChar(L'[');
        for (int i = type->asARRAYTYPE()->iDims; --i > 0; )
            ErrAppendChar(L',');
        ErrAppendChar(L']');
        break;

    case NK_POINTERTYPE:
        ErrAppendTypeNode(type->asPOINTERTYPE()->pElementType);
        ErrAppendChar(L'*');
        break;

    case NK_NULLABLETYPE:
        ErrAppendTypeNode(type->asNULLABLETYPE()->pElementType);
        ErrAppendChar(L'?');
        break;

    default:
        ASSERT(!"NYI. Handle other type node types");
        break;
    }
}


PCWSTR COMPILER::ErrId(int id)
{
    START_ERR_STRING(this);
    ErrAppendId(id);
    return END_ERR_STRING(this);
}


PCWSTR COMPILER::ErrSK(SYMKIND sk)
{
    int id;
    switch (sk) {
    case SK_METHSYM: id = IDS_SK_METHOD; break;
    case SK_NUBSYM: id = IDS_SK_CLASS; break;
    case SK_AGGSYM: id = IDS_SK_CLASS; break;
    case SK_AGGTYPESYM: id = IDS_SK_CLASS; break;
    case SK_NSSYM: id = IDS_SK_NAMESPACE; break;
    case SK_MEMBVARSYM: id = IDS_SK_FIELD; break;
    case SK_LOCVARSYM: id = IDS_SK_VARIABLE; break;
    case SK_PROPSYM: id = IDS_SK_PROPERTY; break;
    case SK_EVENTSYM: id = IDS_SK_EVENT; break;
    case SK_TYVARSYM: id = IDS_SK_TYVAR; break;
    case SK_ALIASSYM: id = IDS_SK_ALIAS; break;
    case SK_EXTERNALIASSYM: id = IDS_SK_EXTERNALIAS; break;
    case SK_NSAIDSYM:
        VSFAIL("Illegal sk");
        id = IDS_SK_ALIAS;
        break;
    default:
        VSFAIL("impossible sk");
        id = IDS_SK_UNKNOWN;
        break;
    }

    return ErrId(id);
}


PCWSTR COMPILER::ErrAggKind(AggKindEnum ak)
{
    int id;
    switch (ak) {
    case AggKind::Class: id = IDS_AK_CLASS; break;
    case AggKind::Delegate: id = IDS_AK_DELEGATE; break;
    case AggKind::Interface: id = IDS_AK_INTERFACE; break;
    case AggKind::Struct: id = IDS_AK_STRUCT; break;
    case AggKind::Enum: id = IDS_AK_ENUM; break;
    default:
        VSFAIL("impossible AggKind");
        id = IDS_AK_UNKNOWN;
        break;
    }

    return ErrId(id);
}

/*
 * Create a fill-in string describing a parameter list.
 * Does NOT include ()
 */
void COMPILER::ErrAppendParamList(TypeArray *params, bool isVarargs, bool isParamArray)
{
    if (!params)
        return;

    for (int i = 0; i < params->size; i++) {
        if (i > 0) {
            ErrAppendString(L", ");
        }

        if (isParamArray && i == params->size - 1) {
            ErrAppendString(L"params ");
        }

        // parameter type name
        ErrAppendSym(params->Item(i), NULL);
    }

    if (isVarargs) {
        if (params->size) {
            ErrAppendString(L", ");
        }

        ErrAppendString(L"...");
    }
}

/*
 * Create a fill-in string describing a delegate.
 * Different from ErrSym because this one also adds the return type and the arguments
 */
PCWSTR COMPILER::ErrDelegate(AGGTYPESYM * type)
{
    VERIFYLOCALHANDLER;

    ASSERT(type->isDelegateType());
    START_ERR_STRING(this);

    METHSYM * pInvoke = getBSymmgr().LookupInvokeMeth(type->getAggregate());
    if (!pInvoke) {
        // We can't find the Invoke method, so fall-back to reporting the plain delegate name
        ErrAppendSym(type, NULL);
    } else {
        SubstContext ctx(type);

        // return type
        ErrAppendSym(pInvoke->retType, &ctx);
        ErrAppendChar(L' ');

        // Delegate Name
        ErrAppendSym(type, NULL);

        // Parameter list
        ErrAppendChar(L'(');
        if (pInvoke->hasBogus() && !pInvoke->checkBogus()) {
            ErrAppendParamList(getBSymmgr().SubstTypeArray(pInvoke->params, &ctx), pInvoke->isVarargs, pInvoke->isParamArray);
        }
        ErrAppendChar(L')');
    }

    return END_ERR_STRING(this);
}



void COMPILER::ErrAppendString(PCWSTR str, size_t len)
{
    VERIFYLOCALHANDLER;

    if (len == (size_t) -1) {
        len = wcslen(str);
    }

    if (errBufferNext + 19 >= errBuffer + ERROR_BUFFER_MAX_WCHARS) {
        ASSERT( errBufferNext + 19 <= errBuffer + ERROR_BUFFER_MAX_WCHARS);
        // out of room do nothing
    } else if (errBufferNext + len + 1 >= errBuffer + ERROR_BUFFER_MAX_WCHARS) {
        wcsncpy_s(errBufferNext, ErrBufferLeft(13), str, 12);
        errBufferNext += 12;
        wcsncpy_s( errBufferNext, ErrBufferLeft(4), L"...", 3);
        errBufferNext += 3;
        wcsncpy_s( errBufferNext, ErrBufferLeft(4), str + len - 3, 3);
        errBufferNext += 3;
    } else {
        wcsncpy_s(errBufferNext, ErrBufferLeft(len + 1), str, len);
        errBufferNext += len;
    }
    *errBufferNext = L'\0';
}

void COMPILER::ErrAppendChar(WCHAR ch)
{
    VERIFYLOCALHANDLER;

    if (errBufferNext + 1 >= errBuffer + ERROR_BUFFER_MAX_WCHARS) {
        // just eat the character we have no room
    } else {
        *errBufferNext = ch;
        errBufferNext += 1;
    }
    
    *errBufferNext = L'\0';
}

void COMPILER::ErrAppendPrintf(PCWSTR format, ...)
{
    va_list args;
    va_start(args, format);
    
    StringCchVPrintfW (errBufferNext, ErrBufferLeftTot(), format, args);
    errBufferNext += wcslen(errBufferNext);
    
    va_end(args);
}

void COMPILER::ErrAppendName(NAME *name)
{
    if (name == namemgr->GetPredefName(PN_INDEXERINTERNAL)) {
        ErrAppendString(L"this");
    } else {
        ErrAppendString(name->text);
    }
}

void COMPILER::ErrAppendMethodParentSym(METHSYM * sym, SubstContext * pcxt, TypeArray ** substMethTyParams)
{
    if (substMethTyParams) {
        *substMethTyParams = NULL;
    }
    ErrAppendParentSym(sym, pcxt);
}

void COMPILER::ErrAppendParentSym(SYM * sym, SubstContext * pctx)
{
    PARENTSYM * parent = sym->parent;

    if (!parent)
        return;

    if (parent->isNSAIDSYM()) {
        int aid = parent->asNSAIDSYM()->GetAid();
        parent = parent->asNSAIDSYM()->GetNS();

        if (aid != kaidGlobal) {
            // Spit out the alias name
            ErrAppendSym(getBSymmgr().GetSymForAid(aid), NULL);
            ErrAppendChar(':');
            ErrAppendChar(':');
        }
    }

    if (parent == getBSymmgr().GetRootNS())
        return;

    if (pctx && !pctx->FNop() && parent->isAGGSYM() && parent->asAGGSYM()->typeVarsAll->size)
        parent = getBSymmgr().SubstType(parent->asAGGSYM()->getThisType(), pctx);
    ErrAppendSym(parent, NULL);
    ErrAppendChar(L'.');
}

void COMPILER::ErrAppendTypeParameters(TypeArray * params, SubstContext * pctx, bool forClass)
{
    if (params && params->size) {
        ErrAppendChar(L'<');
        ErrAppendSym(params->Item(0), pctx);
        for (int i = 1; i < params->size; i++) {
            ErrAppendString(L",");
            ErrAppendSym(params->Item(i), pctx);
        }
        ErrAppendChar(L'>');
    }
}

PCWSTR COMPILER::ErrNameNode(BASENODE *node)
{
    START_ERR_STRING(this);
    ErrAppendNameNode(node);
    return END_ERR_STRING(this);
}

PCWSTR COMPILER::ErrParamList(TypeArray *params, bool isVarargs, bool isParamArray)
{
    START_ERR_STRING(this);
    ErrAppendParamList(params, isVarargs, isParamArray);
    return END_ERR_STRING(this);
}

void COMPILER::ErrAppendMethod(METHSYM * meth, SubstContext * pctx, bool fArgs)
{
    if (meth->IsExpImpl() && meth->swtSlot) {
        ErrAppendParentSym(meth, pctx);

        // Get the type args from the explicit impl type and substitute using pctx (if there is one).
        SubstContext ctx(getBSymmgr().SubstType(meth->swtSlot.Type(), pctx)->asAGGTYPESYM());
        ErrAppendSym(meth->swtSlot.Sym(), &ctx, fArgs);

        // args already added
        return;
    }

    if (meth->isPropertyAccessor()) {
        PROPSYM * prop = meth->getProperty();

        // this includes the parent class
        ErrAppendSym(prop, pctx);

        // add accessor name
        if (prop->methGet == meth) {
            ErrAppendString(L".get");
        } else {
            ASSERT(meth == prop->methSet);
            ErrAppendString(L".set");
        }

        // args already added
        return;
    }

    if (meth->isEventAccessor()) {
        EVENTSYM * event = meth->getEvent();

        // this includes the parent class
        ErrAppendSym(event, pctx);

        // add accessor name
        if (event->methAdd == meth) {
            ErrAppendString(L".add");
        } else {
            ASSERT(meth == event->methRemove);
            ErrAppendString(L".remove");
        }

        // args already added
        return;
    }

    TypeArray * replacementTypeArray = NULL;
    ErrAppendMethodParentSym(meth, pctx, &replacementTypeArray);
    if (meth->isCtor()) {
        // Use the name of the parent class instead of the name "<ctor>".
        ErrAppendName(meth->getClass()->name);
    } else if (meth->isDtor()) {
        // Use the name of the parent class instead of the name "Finalize".
        ErrAppendChar(L'~');
        ErrAppendName(meth->getClass()->name);
    } else if (meth->isConversionOperator()) {
        // implicit/explicit
        ErrAppendString(meth->isImplicit() ? L"implicit" : L"explicit");
        ErrAppendString(L" operator ");

        // destination type name
        ErrAppendSym(meth->retType, pctx);
    } else if (meth->isOperator) {
        // handle user defined operators
        // map from CLS predefined names to "operator <X>"
        ErrAppendString(L"operator ");

        //
        //
        PCWSTR operatorName = CParser::GetTokenInfo((TOKENID) CParser::GetOperatorInfo(clsDeclRec.operatorOfName(meth->name))->iToken)->pszText;
        if (operatorName[0] == 0) {
            //
            // either equals or compare
            //
            if (meth->name == namemgr->GetPredefName(PN_OPEQUALS)) {
                operatorName = L"equals";
            } else {
                ASSERT(meth->name == namemgr->GetPredefName(PN_OPCOMPARE));
                operatorName = L"compare";
            }
        }
        ErrAppendString(operatorName);
    } else if (meth->IsExpImpl()) {
        if (meth->errExpImpl)
            ErrAppendSym(meth->errExpImpl, pctx, fArgs);
        else {
            // explicit impl that hasn't been prepared yet
            // can't be a property accessor
            ErrAppendNameNode(meth->parseTree->asMETHOD()->pName);
        }
    } else {
        // regular method
        {
            ErrAppendName(meth->name);
        }
    }

    if (!replacementTypeArray) {
        ErrAppendTypeParameters(meth->typeVars, pctx, false);
    }

    if (fArgs) {
        // append argument types
        ErrAppendChar(L'(');

        if (!CheckBogusNoEnsure(meth)) {
            ErrAppendParamList(getBSymmgr().SubstTypeArray(meth->params, pctx), meth->isVarargs, meth->isParamArray);
        }

        ErrAppendChar(L')');
    }
}

void COMPILER::ErrAppendIndexer(INDEXERSYM * indexer, SubstContext * pctx)
{
    ErrAppendString(L"this[");
    ErrAppendParamList(getBSymmgr().SubstTypeArray(indexer->params, pctx), false, indexer->isParamArray);
    ErrAppendChar(L']');
}

void COMPILER::ErrAppendProperty(PROPSYM * prop, SubstContext * pctx)
{
    ErrAppendParentSym(prop, pctx);
    if (prop->IsExpImpl() && prop->swtSlot.Sym()) {
        SubstContext ctx(getBSymmgr().SubstType(prop->swtSlot.Type(), pctx)->asAGGTYPESYM());
        ErrAppendSym(prop->swtSlot.Sym(), &ctx);
    } else if (prop->IsExpImpl()) {
        if (prop->errExpImpl)
            ErrAppendSym(prop->errExpImpl, pctx, false);
        else {
            // must be explicit impl not prepared yet.
            ErrAppendNameNode(prop->parseTree->asANYPROPERTY()->pName);
        }
        if (prop->isIndexer()) {
            ErrAppendChar(L'.');
            ErrAppendIndexer(prop->asINDEXERSYM(), pctx);
        }
    } else if (prop->isIndexer()) {
        ErrAppendIndexer(prop->asINDEXERSYM(), pctx);
    } else {
        ErrAppendName(prop->name);
    }
}

void COMPILER::ErrAppendEvent(EVENTSYM * event, SubstContext * pctx)
{
    // Qualify with parent symbol, if any.
    ErrAppendParentSym(event, pctx);

    if (event->ewtSlot.Sym() && event->IsExpImpl()) {
        SubstContext ctx(getBSymmgr().SubstType(event->ewtSlot.Type(), pctx)->asAGGTYPESYM());
        ErrAppendSym(event->ewtSlot.Event(), &ctx);
    } else if (event->IsExpImpl()) {
        // must be explicit impl not prepared yet.
        ErrAppendNameNode(event->parseTree->asPROPERTY()->pName);
    } else {
        ErrAppendName(event->name);
    }
}

void COMPILER::ErrAppendId(int id)
{
    VERIFYLOCALHANDLER;

    *errBufferNext = L'\0';
    for (int cchTrialSize = 128; cchTrialSize < ErrBufferLeftTot(); cchTrialSize *= 8) {
        ASSERT(cchTrialSize > 0); // We should never overflow
        if (LoadAndFormatMessage(getMessageDll(), id, errBufferNext, cchTrialSize)) {
            errBufferNext += wcslen(errBufferNext);
            return;
        }
    }
    if (LoadAndFormatMessage(getMessageDll(), id, errBufferNext, ErrBufferLeftTot())) {
        errBufferNext += wcslen(errBufferNext);
        return;
    }
    VSFAIL("A resource string too big to load!");
}

PCWSTR COMPILER::ErrSym(SYM * sym, SubstContext * pctx, bool fArgs)
{
    START_ERR_STRING(this);
    ErrAppendSym(sym, pctx, fArgs);
    return END_ERR_STRING(this);
}

/*
 * Create a fill-in string describing a symbol.
 */
void COMPILER::ErrAppendSym(SYM * sym, SubstContext * pctx, bool fArgs)
{
    VERIFYLOCALHANDLER;

    *errBufferNext = '\0';

    if (sym->isTYPESYM() && pctx) {
        if (!pctx->FNop())
            sym = getBSymmgr().SubstType(sym->asTYPESYM(), pctx);
        // We shouldn't use the SubstContext again so set it to NULL.
        pctx = NULL;
    }

    switch (sym->getKind()) {
    case SK_NSDECLSYM:
        // for namespace declarations just convert the namespace
        ErrAppendSym(sym->asNSDECLSYM()->NameSpace(), NULL);
        break;

    case SK_EXTERNALIASSYM:
    case SK_ALIASSYM:
        ErrAppendName(sym->name);
        break;

    case SK_GLOBALATTRSYM:
        ErrAppendName(sym->name);
        break;

    case SK_AGGDECLSYM:
        ErrAppendSym(sym->asAGGDECLSYM()->Agg(), pctx);
        break;

    case SK_AGGSYM: {
        // Check for a predefined class with a special "nice" name for
        // error reported.
        PCWSTR text = getBSymmgr().GetNiceName(sym->asAGGSYM());
        if (text) {
            // Found a nice name.
            ErrAppendString(text);
        } else {
            ErrAppendParentSym(sym, pctx);
            ErrAppendName(sym->name);
            ErrAppendTypeParameters(sym->asAGGSYM()->typeVarsThis, pctx, true);
        }
        break;
    }

    case SK_AGGTYPESYM: {
        // Check for a predefined class with a special "nice" name for
        // error reported.
        PCWSTR text = getBSymmgr().GetNiceName(sym->asAGGTYPESYM()->getAggregate());
        if (text) {
            // Found a nice name.
            ErrAppendString(text);
        }
        else {
            if (sym->asAGGTYPESYM()->outerType) {
                ErrAppendSym(sym->asAGGTYPESYM()->outerType, pctx);
                ErrAppendChar(L'.');
            } else {
                // In a namespace.
                ErrAppendParentSym(sym->asAGGTYPESYM()->getAggregate(), pctx);
            }
            ErrAppendName(sym->asAGGTYPESYM()->getAggregate()->name);
        }
        ErrAppendTypeParameters(sym->asAGGTYPESYM()->typeArgsThis, pctx, true);
        break;
    }

    case SK_METHSYM:
        ErrAppendMethod(sym->asMETHSYM(), pctx, fArgs);
        break;

    case SK_PROPSYM:
        ErrAppendProperty(sym->asPROPSYM(), pctx);
        break;

    case SK_EVENTSYM:
        ErrAppendEvent(sym->asEVENTSYM(), pctx);
        break;

    case SK_NSAIDSYM:
        {
            int aid = sym->asNSAIDSYM()->GetAid();
            sym = sym->asNSAIDSYM()->GetNS();

            if (aid != kaidGlobal) {
                // Spit out the alias name
                ErrAppendSym(getBSymmgr().GetSymForAid( aid), NULL);
                // If there's nothing besides the alias, stop here
                if (sym == getBSymmgr().GetRootNS())
                    break;
                // Otherwise  append ::Namespace
                ErrAppendChar(':');
                ErrAppendChar(':');
            }
        }
        // Fall through.
    case SK_NSSYM:
        if (sym == getBSymmgr().GetRootNS()) {
            ErrAppendId(IDS_GlobalNamespace);
        }
        else {
            ErrAppendParentSym(sym, NULL);
            ErrAppendName(sym->name);
        }
        break;

    case SK_MEMBVARSYM:
        ErrAppendParentSym(sym, pctx);
        ErrAppendName(sym->name);
        break;

    case SK_TYVARSYM:
        if (!sym->name) {
            // It's a standard type variable.
            if (sym->asTYVARSYM()->isMethTyVar)
                ErrAppendChar(L'!');
            ErrAppendChar(L'!');
            ErrAppendPrintf(L"%d", sym->asTYVARSYM()->indexTotal);
        }
        else
            ErrAppendName(sym->name);
        break;

    case SK_INFILESYM:
    case SK_MODULESYM:
    case SK_OUTFILESYM:
    case SK_LOCVARSYM:
    case SK_LABELSYM:
        // Generate symbol name.
        ErrAppendName(sym->name);
        break;

    case SK_ERRORSYM:
        if (sym->parent) {
            ASSERT(sym->asERRORSYM()->nameText && sym->asERRORSYM()->typeArgs);
            ErrAppendParentSym(sym, pctx);
            ErrAppendName(sym->asERRORSYM()->nameText);
            ErrAppendTypeParameters(sym->asERRORSYM()->typeArgs, pctx, true);
        }
        else {
            // Load the string "<error>".
            ASSERT(!sym->parent);
            ASSERT(!sym->asERRORSYM()->typeArgs);
            ErrAppendId(IDS_ERRORSYM);
        }
        break;

    case SK_NULLSYM:
        // Load the string "<null>".
        ErrAppendId(IDS_NULL);
        break;

    case SK_UNITSYM:
        // Leave blank.
        break;

    case SK_ANONMETHSYM:
        ErrAppendId(IDS_AnonMethod);
        break;

    case SK_METHGRPSYM:
        ErrAppendId(IDS_MethodGroup);
        break;

    case SK_ARRAYSYM: {
        TYPESYM * elementType;
        int rank;

        for (elementType = sym->asTYPESYM(); elementType && elementType->isARRAYSYM(); elementType = elementType->asARRAYSYM()->elementType())
            ;

        if (!elementType) {
            ASSERT(0);
            break;
        }

        ErrAppendSym(elementType, pctx);

        for (elementType = sym->asTYPESYM(); elementType && elementType->isARRAYSYM(); elementType = elementType->asARRAYSYM()->elementType()) {
            rank = elementType->asARRAYSYM()->rank;

            // Add [] with (rank-1) commas inside
            ErrAppendChar(L'[');

            // known rank.
            if (rank > 1)
                ErrAppendChar(L'*');
            for (int i = rank; i > 1; --i) {
                ErrAppendChar(L',');
                ErrAppendChar(L'*');
            }

            ErrAppendChar(L']');
        }
        break;
    }

    case SK_VOIDSYM:
        ErrAppendName(namemgr->KeywordName(TID_VOID));
        break;

    case SK_PARAMMODSYM:
        // add ref or out
        if (sym->asPARAMMODSYM()->isRef) {
            ErrAppendString(L"ref ");
        } else {
            ASSERT(sym->asPARAMMODSYM()->isOut);
            ErrAppendString(L"out ");
        }

        // add base type name
        ErrAppendSym(sym->asPARAMMODSYM()->paramType(), pctx);
        break;

    case SK_MODOPTTYPESYM:
        ErrAppendSym(sym->asMODOPTTYPESYM()->baseType(), pctx);
        break;

    case SK_PTRSYM:
        // Generate the base type.
        ErrAppendSym(sym->asPTRSYM()->baseType(), pctx);
        {

            // add the trailing *
            ErrAppendChar(L'*');
        }
        break;

    case SK_NUBSYM:
        ErrAppendSym(sym->asNUBSYM()->baseType(), pctx);
        ErrAppendChar(L'?');
        break;

    case SK_SCOPESYM:
    case SK_ANONSCOPESYM:
    default:
        // Shouldn't happen.
        ASSERT(!"Bad symbol kind");
        break;
    }

    ASSERT(!*errBufferNext);
}


PCWSTR COMPILER::FinishErrorString(PCWSTR start)
{
    ASSERT(start >= errBufferStart && start <= errBufferNext);
    // Nuke all FFFF characters since FormatStringW barfs on them.
    ClobberBadChars(const_cast<PWCH>(start), errBufferNext);
    ErrAppendChar(L'\0');
    return start;
}


CError * COMPILER::MakeErrorLocArgs(const ERRLOC * loc, int id, int carg, ErrArg * prgarg, bool fWarnOverride)
{
    // Create an arg array manually using the type information in the ErrArgs.
    PCWSTR * prgpsz = STACK_ALLOC(PCWSTR, carg);
    int * prgiarg = STACK_ALLOC(int, carg);

    PCWSTR * ppsz = prgpsz;
    int * piarg = prgiarg;
    int cargUnique = 0;

    for (int iarg = 0; iarg < carg; iarg++) {
        ErrArg * parg = prgarg + iarg;

        // If the NoStr bit is set we don't add it to prgpsz.
        if (parg->eaf & ErrArgFlags::NoStr)
            continue;

        PCWSTR psz;
        int iargRec = -1;

        switch (parg->eak) {
        case ErrArgKind::Int:
            psz = (PCWSTR)(INT_PTR)parg->n;
            break;

        case ErrArgKind::Ids:
            psz = ErrId(parg->n);
            break;

        case ErrArgKind::SymKind:
            psz = ErrSK(parg->sk);
            break;

        case ErrArgKind::AggKind:
            psz = ErrAggKind(parg->ak);
            break;

        case ErrArgKind::Sym:
            psz = ErrSym(parg->sym);
            iargRec = iarg;
            break;

        case ErrArgKind::Name:
            psz = ErrName(parg->name);
            break;

        case ErrArgKind::Str:
            psz = parg->psz;
            if (HasBadChars(psz)) {
                START_ERR_STRING(this);
                ErrAppendString(psz);
                psz = END_ERR_STRING(this);
            }
            break;

        case ErrArgKind::PredefName:
            psz = ErrName(namemgr->GetPredefName(parg->pdn));
            break;

        case ErrArgKind::NameNode:
            psz = ErrNameNode(parg->node);
            iargRec = iarg;
            break;

        case ErrArgKind::TypeNode:
            psz = ErrTypeNode(parg->node->asTYPEBASE());
            iargRec = iarg;
            break;

        case ErrArgKind::Ptr:
            psz = (PCWSTR)(INT_PTR)parg->ptr;
            break;

        case ErrArgKind::SymWithType:
            {
                SubstContext ctx(parg->pswt->ats, NULL);
                psz = ErrSym(parg->pswt->sym, &ctx);
                iargRec = iarg;
            }
            break;

        case ErrArgKind::MethWithInst:
            {
                SubstContext ctx(parg->pmpwi->ats, parg->pmpwi->typeArgs);
                psz = ErrSym(parg->pmpwi->sym, &ctx);
                iargRec = iarg;
            }
            break;

        default:
            VSFAIL("Bad arg kind");
            continue;
        }

        if (!(parg->eaf & ErrArgFlags::Unique))
            iargRec = -1;
        else if (iargRec >= 0)
            cargUnique++;

        *ppsz++ = psz;
        *piarg++ = iargRec;
    }

    int cpsz = (int)(ppsz - prgpsz);

    if (cargUnique > 1) {
        // Copy the strings over to another buffer.
        PCWSTR * prgpszNew = STACK_ALLOC(PCWSTR, cpsz);
        memcpy(prgpszNew, prgpsz, cpsz * sizeof(*prgpsz));

        for (int i = 0; i < cpsz; i++) {
            if (prgiarg[i] < 0 || prgpszNew[i] != prgpsz[i])
                continue;

            bool fSource;
            ErrArg * parg = prgarg + prgiarg[i];
            ASSERT((parg->eaf & ErrArgFlags::Unique) && !(parg->eaf & ErrArgFlags::NoStr));

            SYM * sym = NULL;
            BASENODE * node = NULL;

            switch (parg->eak) {
            case ErrArgKind::Sym:
                sym = parg->sym;
                break;
            case ErrArgKind::NameNode:
            case ErrArgKind::TypeNode:
                node = parg->node;
                break;
            case ErrArgKind::SymWithType:
                sym = parg->pswt->sym;
                break;
            case ErrArgKind::MethWithInst:
                sym = parg->pmpwi->sym;
                break;
            default:
                VSFAIL("Shouldn't be here!");
                continue;
            }
            ASSERT(!sym != !node);

            bool fMunge = false;

            for (int j = i + 1; j < cpsz; j++) {
                if (prgiarg[j] < 0)
                    continue;
                ASSERT(prgarg[prgiarg[j]].eaf & ErrArgFlags::Unique);
                if (wcscmp(prgpsz[i], prgpsz[j]))
                    continue;

                // The strings are identical. If they are the same symbol, leave them alone.
                // Otherwise, munge both strings. If j has already been munged, just make
                // sure we munge i.
                if (prgpszNew[j] != prgpsz[j]) {
                    fMunge = true;
                    continue;
                }

                ErrArg * parg2 = prgarg + prgiarg[j];
                ASSERT((parg2->eaf & ErrArgFlags::Unique) && !(parg2->eaf & ErrArgFlags::NoStr));

                SYM * sym2 = NULL;
                BASENODE * node2 = NULL;

                switch (parg2->eak) {
                case ErrArgKind::Sym:
                    sym2 = parg2->sym;
                    break;
                case ErrArgKind::NameNode:
                case ErrArgKind::TypeNode:
                    node2 = parg2->node;
                    break;
                case ErrArgKind::SymWithType:
                    sym2 = parg2->pswt->sym;
                    break;
                case ErrArgKind::MethWithInst:
                    sym2 = parg2->pmpwi->sym;
                    break;
                default:
                    VSFAIL("Shouldn't be here!");
                    continue;
                }
                ASSERT(!sym2 != !node2);

                if (sym2 == sym && node2 == node && !fMunge)
                    continue;

                prgpszNew[j] = ErrStrWithLoc(prgpsz[j], sym2, node2, &fSource);
                if (fSource)
                    parg2->eaf = parg2->eaf | ErrArgFlags::Ref;
                fMunge = true;
            }

            if (fMunge) {
                prgpszNew[i] = ErrStrWithLoc(prgpsz[i], sym, node, &fSource);
                if (fSource)
                    parg->eaf = parg->eaf | ErrArgFlags::Ref;
            }
        }

        prgpsz = prgpszNew;
    }

    CError * err;
    if (FAILED(pController->CreateError(id, prgpsz, &err, fWarnOverride)))
        return NULL;

    if (loc)
        AddLocationToError(err, *loc);

    // Add other locations as appropriate.
    for (ErrArg * parg = prgarg; parg < prgarg + carg; parg++) {
        if (!(parg->eaf & ErrArgFlags::Ref))
            continue;

        switch (parg->eak) {
        case ErrArgKind::Sym:
            AddRelatedSymLoc(err, parg->sym);
            break;

        case ErrArgKind::LocNode:
        case ErrArgKind::NameNode:
        case ErrArgKind::TypeNode:
            AddLocationToError(err, ERRLOC(&getBSymmgr(), parg->node));
            break;

        case ErrArgKind::SymWithType:
            AddRelatedSymLoc(err, parg->pswt->sym);
            break;

        case ErrArgKind::MethWithInst:
            AddRelatedSymLoc(err, parg->pmpwi->sym);
            break;

        default:
            break;
        }
    }

    return err;
}


PCWSTR COMPILER::ErrStrWithLoc(PCWSTR pszBase, SYM * sym, BASENODE * node, bool * pfSource)
{
    *pfSource = false;

    if (!node) {
        if (!sym)
            return pszBase;
        node = sym->GetSomeParseTree();
    }

    ERRLOC loc;

    if (node) {
        loc = ERRLOC(&getBSymmgr(), node);
        *pfSource = true;
    }
    else {
        INFILESYM * infile = sym->GetSomeInputFile();
        if (!infile)
            return pszBase;
        loc = ERRLOC(infile);
    }

    START_ERR_STRING(this);
    ErrAppendString(pszBase);
    ErrAppendString(L" [");
    ErrAppendString(loc.mapFile());
    if (loc.mapLine() >= 0)
        ErrAppendPrintf(L"(%d)", loc.mapLine() + 1);
    ErrAppendChar(L']');
    return END_ERR_STRING(this);
}


void COMPILER::AddRelatedSymLoc(CError * err, SYM * sym)
{
LRestart:
    switch (sym->getKind()) {
    default:
        if (sym->isTYPESYM()) {
            sym = sym->asTYPESYM()->GetNakedAgg();
            if (sym)
                goto LRestart;
        }
        else
            compiler()->AddLocationToError(err, ERRLOC(sym->getInputFile(), sym->getParseTree()));
        break;
    case SK_AGGTYPESYM:
        sym = sym->asAGGTYPESYM()->getAggregate();
        goto LRestart;
    case SK_AGGSYM:
        // For unresolved classes, use the module's input file for the location.
        if (sym->asAGGSYM()->IsUnresolved())
            compiler()->AddLocationToError(err, ERRLOC(sym->asAGGSYM()->AsUnresolved()->moduleErr->getInputFile(), NULL));
        else {
            // We have a class -- dump all declarations of it.
            FOREACHAGGDECL(sym->asAGGSYM(), decl)
                compiler()->AddLocationToError(err, ERRLOC(decl->getInputFile(), decl->getParseTree()));
            ENDFOREACHAGGDECL
        }
        break;
    case SK_NSSYM:
        // Dump all declarations of the namespace.
        for (NSDECLSYM * decl = sym->asNSSYM()->DeclFirst(); decl; decl = decl->DeclNext())
            AddRelatedSymLoc(err, decl);
        break;
    case SK_NSAIDSYM:
        {
            // Dump all declarations of the namespace that are in the aid.
            int aid = sym->asNSAIDSYM()->GetAid();
            for (NSDECLSYM * decl = sym->asNSAIDSYM()->GetNS()->DeclFirst(); decl; decl = decl->DeclNext()) {
                if (decl->getInputFile()->InAlias(aid))
                    AddRelatedSymLoc(err, decl);
            }
        }
        break;
    case SK_TYVARSYM:
        if (sym->getParseTree())
            compiler()->AddLocationToError(err, ERRLOC(& compiler()->getBSymmgr(), sym->getParseTree()));
        else if (sym->parent)
            AddRelatedSymLoc(err, sym->parent);
        break;
    case SK_LOCVARSYM:
        break;
    case SK_INFILESYM:
        compiler()->AddLocationToError(err, ERRLOC(sym->asINFILESYM()));
        break;
    case SK_MODULESYM:
        compiler()->AddLocationToError(err, ERRLOC(sym->getInputFile()));
        break;
    }
}


////////////////////////////////////////////////////////////////////////////////
// COMPILER::AddLocationToError
//
// This function adds the given ERRLOC data as a location to the given error.
// If there is any kind of failure, the host is told that things are toast via
// OnCatastrophicError().

void COMPILER::AddLocationToError (CError *pError, const ERRLOC *pErrLoc)
{
    // No file name means no location.
    if (pError == NULL || pErrLoc == NULL || pErrLoc->fileName() == NULL)
        return;

    POSDATA     posStart, posEnd, mapStart, mapEnd;

    // See if there's a line/column location
    if (pErrLoc->hasLocation())
    {
        posStart.iLine = pErrLoc->line();
        posStart.iChar = pErrLoc->column();
        posEnd.iLine = pErrLoc->line();
        posEnd.iChar = posStart.iChar + pErrLoc->extent();
        mapStart.iLine = pErrLoc->mapLine();
        mapStart.iChar = pErrLoc->column();
        mapEnd.iLine = pErrLoc->mapLine();
        mapEnd.iChar = mapStart.iChar + pErrLoc->extent();
    }

    if (FAILED (pError->AddLocation (pErrLoc->fileName(), &posStart, &posEnd, pErrLoc->mapFile(), &mapStart, &mapEnd)))
        host->OnCatastrophicError (FALSE, 0, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// COMPILER::SubmitError
//
// This function submits the given error to the controller, and if it's a fatal
// error, throws the fatal exception.

void COMPILER::SubmitError (CError * perr)
{
    CComPtr<CError> pError = perr;

    ResetErrorBuffer();

    if (pError != NULL)
    {
        bool shouldThrow = pError->Kind() == ERROR_FATAL && pError->ID() != FTL_StackOverflow;
        if (pError->LocationCount() > 0)
        {
            PCWSTR pszFileName = NULL;
            NAME * pName = NULL;
            INFILESYM * pIn = NULL;

            if (SUCCEEDED(pError->GetUnmappedLocationAt( 0, &pszFileName, NULL, NULL)) && pszFileName != NULL &&
                (pName = namemgr->LookupString(pszFileName)) != NULL &&
                (pIn = getBSymmgr().FindInfileSym(pName)) != NULL && pIn->pData != NULL &&
                pIn->pData->GetModule()->IsWarningDisabled(pError))
            {
                return;
            }
        }

        pController->SubmitError(pError);

#if DEBUG
        if (GetRegDWORD("Error"))
        {
            if (MessageBoxW(0, pError->GetText(), L"ASSERT?", MB_YESNO) == IDYES)
            {
                ASSERT(FALSE);
            }
        }
#endif

        // Stack overflow error is reported from inside the exception handler
        // so we don't need to raise another exception
        if (shouldThrow)
        {
            pError.Release();
            ThrowFatalException();
        }
    }
}

CError * COMPILER::MakeErrorTreeArgs(BASENODE * tree, int id, int carg, ErrArg * prgarg)
{
    if (tree) {
        ERRLOC errloc(&getBSymmgr(), tree);
        return MakeErrorLocArgs(&errloc, id, carg, prgarg);
    }
    else {
        return MakeErrorLocArgs(NULL, id, carg, prgarg);
    }
}

// NOTE: We'd like to skip creating the temp array (by making these __cdecl and passing &a instead of args),
// but we're not guaranteed that the compiler will do the right thing. Maybe do this on x86?
CError * COMPILER::MakeError(BASENODE * tree, int id, ErrArg a)                               {                                 return MakeErrorTreeArgs(tree, id, 1, &a); }
CError * COMPILER::MakeError(BASENODE * tree, int id, ErrArg a, ErrArg b)                     { ErrArg args[] = { a, b };       return MakeErrorTreeArgs(tree, id, 2, args); }
CError * COMPILER::MakeError(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c)           { ErrArg args[] = { a, b, c };    return MakeErrorTreeArgs(tree, id, 3, args); }
CError * COMPILER::MakeError(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d) { ErrArg args[] = { a, b, c, d }; return MakeErrorTreeArgs(tree, id, 4, args); }

CError * COMPILER::MakeError(ERRLOC loc, int id, ErrArg a)                               {                                 return MakeErrorLocArgs(&loc, id, 1, &a); }
CError * COMPILER::MakeError(ERRLOC loc, int id, ErrArg a, ErrArg b)                     { ErrArg args[] = { a, b };       return MakeErrorLocArgs(&loc, id, 2, args); }
CError * COMPILER::MakeError(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c)           { ErrArg args[] = { a, b, c };    return MakeErrorLocArgs(&loc, id, 3, args); }
CError * COMPILER::MakeError(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d) { ErrArg args[] = { a, b, c, d }; return MakeErrorLocArgs(&loc, id, 4, args); }

void COMPILER::Error(BASENODE * tree, int id, ErrArg a)                               {                                 ErrorTreeArgs(tree, id, 1, &a); }
void COMPILER::Error(BASENODE * tree, int id, ErrArg a, ErrArg b)                     { ErrArg args[] = { a, b };       ErrorTreeArgs(tree, id, 2, args); }
void COMPILER::Error(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c)           { ErrArg args[] = { a, b, c};     ErrorTreeArgs(tree, id, 3, args); }
void COMPILER::Error(BASENODE * tree, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d) { ErrArg args[] = { a, b, c, d }; ErrorTreeArgs(tree, id, 4, args); }

void COMPILER::Error(ERRLOC loc, int id, ErrArg a)                               {                                 ErrorLocArgs(&loc, id, 1, &a); }
void COMPILER::Error(ERRLOC loc, int id, ErrArg a, ErrArg b)                     { ErrArg args[] = { a, b };       ErrorLocArgs(&loc, id, 2, args); }
void COMPILER::Error(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c)           { ErrArg args[] = { a, b, c };    ErrorLocArgs(&loc, id, 3, args); }
void COMPILER::Error(ERRLOC loc, int id, ErrArg a, ErrArg b, ErrArg c, ErrArg d) { ErrArg args[] = { a, b, c, d }; ErrorLocArgs(&loc, id, 4, args); }

void COMPILER::ErrorRef(BASENODE * tree, int id, ErrArgRef a)                                         {                                 ErrorTreeArgs(tree, id, 1, &a); }
void COMPILER::ErrorRef(BASENODE * tree, int id, ErrArgRef a, ErrArgRef b)                            { ErrArg args[] = { a, b };       ErrorTreeArgs(tree, id, 2, args); }
void COMPILER::ErrorRef(BASENODE * tree, int id, ErrArgRef a, ErrArgRef b, ErrArgRef c)               { ErrArg args[] = { a, b, c };    ErrorTreeArgs(tree, id, 3, args); }
void COMPILER::ErrorRef(BASENODE * tree, int id, ErrArgRef a, ErrArgRef b, ErrArgRef c, ErrArgRef d)  { ErrArg args[] = { a, b, c, d }; ErrorTreeArgs(tree, id, 4, args); }


// only set the start if it is on the same line
// as we want to report the error on
void ERRLOC::SetStartInternal(LEXDATA &ld, BASENODE* node)
{
    ASSERT(!start.IsUninitialized());

    SetStartInternal(ld, node->tokidx);
}

void ERRLOC::SetStartInternal(LEXDATA &ld, long tokidx, long tokOffset)
{
    ASSERT(!start.IsUninitialized());

    if (tokidx >= 0) {
        tokidx = CParser::PeekTokenIndexFrom( ld, tokidx, tokOffset);
        if (ld.TokenAt(tokidx).iLine == start.iLine) {
            start = ld.TokenAt(tokidx);
        }
    }
}


/*
 * Recursively searches parse-tree for correct left-most node
 *
 * UNDONE:  REVIEW THIS FUNCTION THOROUGHLY
 */
void ERRLOC::SetStart (BASENODE *pNode)
{
    LEXDATA     ld;

    //EDMAURER SetLine () usually initializes 'start', but may not in some circumstances.
    if (start.IsUninitialized())
        return;

    if (pNode == NULL || pNode->tokidx == -1 || FAILED (m_sourceData->GetLexResults (&ld))) 
        return;

    SetStartInternal(ld, pNode);

    switch (pNode->kind) 
    {
    case NK_ACCESSOR:
        // Get and Set always are the token before the '{'
        SetStartInternal(ld, pNode->asACCESSOR()->iOpen, -1);
        return;

    case NK_ARROW:              // BINOPNODE
        if (pNode->asARROW()->p1)
            SetStart (pNode->asARROW()->p1);
        return;

    case NK_ATTR:               // ATTRNODE
        if (pNode->asATTR()->pName)
            SetStart (pNode->asATTR()->pName);
        return;

    case NK_ATTRDECL:
        if (pNode->asATTRDECL()->pNameNode)
            SetStart (pNode->asATTRDECL()->pNameNode); 
        return;

    case NK_LIST:
    case NK_CALL:
    case NK_DEREF:
    case NK_BINOP:              // BINOPNODE
        if (pNode->asANYBINOP()->p1 && pNode->asANYBINOP()->Op() != OP_CAST)
            SetStart (pNode->asANYBINOP()->p1);
        return;

    case NK_CLASS:              // CLASSNODE
    case NK_INTERFACE:
    case NK_STRUCT:
        if (pNode->asAGGREGATE()->pName)
            SetStart (pNode->asAGGREGATE()->pName);
        return;

    case NK_CTOR:
    case NK_DTOR:
    {
        long i;
        // Find the indentifier which must be the ctor/dtor name
        for (i=pNode->tokidx; i<ld.TokenCount() && ld.TokenAt(i).Token() != TID_IDENTIFIER; i++)
            ;

        SetStartInternal(ld, i);
        return;
    }

    case NK_DELEGATE:
        if (pNode->asDELEGATE()->pName)
            SetStart (pNode->asDELEGATE()->pName);
        return;

    case NK_DOT:                // BINOPNODE
        if (pNode->asDOT()->p1)
            SetStart (pNode->asDOT()->p1);
        return;

    case NK_ENUM:
        if (pNode->asENUM()->pName)
            SetStart (pNode->asENUM()->pName);
        return;

    case NK_INDEXER:
        if (pNode->asANYPROPERTY()->pName)
            SetStart (pNode->asANYPROPERTY()->pName);
        else 
        {
            long i;
            // Find 'this'
            for (i=pNode->tokidx; i<ld.TokenCount() && ld.TokenAt(i).Token() != TID_THIS; i++)
                ;

            SetStartInternal(ld, i);
        }
        return;

    case NK_METHOD:             // METHODNODE
        // use the name to keep the the same as fields
        if (pNode->asMETHOD()->pName)
            SetStart (pNode->asMETHOD()->pName);
        return;

    case NK_NAMESPACE:
        // use the name to keep the the same as fields
        if (pNode->asNAMESPACE()->pName)
            SetStart (pNode->asNAMESPACE()->pName);
        return;

    case NK_OPERATOR:
    {
        TOKENID tok;
        long i;

        for (i=pNode->tokidx; i<ld.TokenCount() && (tok = ld.TokenAt(i).Token()) != TID_OPERATOR && tok != TID_EXPLICIT && tok != TID_IMPLICIT; i++)
            ;
        SetStartInternal(ld, i);
        return;
    }

    case NK_PROPERTY:
        if (pNode->asANYPROPERTY()->pName)
            SetStart (pNode->asANYPROPERTY()->pName);
        return;

    case NK_UNOP:
        if ((pNode->Op() == OP_POSTINC || pNode->Op() == OP_POSTDEC) && pNode->asUNOP()->p1)
            SetStart (pNode->asUNOP()->p1);
        return;

    case NK_POINTERTYPE:
        SetStart (pNode->asPOINTERTYPE()->pElementType);
        return;

    case NK_NULLABLETYPE:
        SetStart (pNode->asNULLABLETYPE()->pElementType);
        return;

    default:
        break;
    }
}

/*
 * Sets start to correct line for given node
 *
 * UNDONE:  REVIEW THIS FUNCTION THOROUGHLY
 */
void ERRLOC::SetLine (BASENODE *pNode)
{
    LEXDATA     ld;

    if (pNode == NULL || pNode->tokidx == -1 || FAILED (m_sourceData->GetLexResults (&ld))) 
        return;

    // default if no special processing below.
    start = ld.TokenAt(pNode->tokidx);

    switch (pNode->kind) 
    {
    case NK_ACCESSOR:
        // Get and Set always are the token before the '{'
        start = ld.TokenAt(CParser::PeekTokenIndexFrom(ld, pNode->asACCESSOR()->iOpen, -1));
        return;

    case NK_CLASS:              // CLASSNODE
    case NK_INTERFACE:
    case NK_STRUCT:
        if (pNode->asAGGREGATE()->pName)
            SetLine (pNode->asAGGREGATE()->pName);
        return;

    case NK_CTOR:
    case NK_DTOR:
    {
        long i;
        // Find the indentifier which must be the ctor/dtor name
        for (i=pNode->tokidx; i<ld.TokenCount() && ld.TokenAt(i).Token() != TID_IDENTIFIER; i++)
            ;

        start = ld.TokenAt(i);
        return;
    }

    case NK_DELEGATE:
        if (pNode->asDELEGATE()->pName)
            SetLine (pNode->asDELEGATE()->pName);
        return;

    case NK_ENUM:
        if (pNode->asENUM()->pName)
            SetLine (pNode->asENUM()->pName);
        return;

    case NK_INDEXER:
        if (pNode->asANYPROPERTY()->pName)
            SetLine (pNode->asANYPROPERTY()->pName);
        else 
        {
            long i;
            for (i=pNode->tokidx; i<ld.TokenCount() && ld.TokenAt(i).Token() != TID_THIS; i++)
                ;
            start = ld.TokenAt(i);
        }
        return;

    case NK_METHOD:             // METHODNODE
        // use the name to keep the the same as fields
        if (pNode->asMETHOD()->pName)
            SetLine (pNode->asMETHOD()->pName);
        return;

    case NK_NAMESPACE:
        // use the name to keep the the same as fields
        if (pNode->asNAMESPACE()->pName)
            SetLine (pNode->asNAMESPACE()->pName);
        else if (pNode->asNAMESPACE()->pUsing)
            SetLine (pNode->asNAMESPACE()->pUsing);
        return;

    case NK_OPERATOR:
    {
        TOKENID tok;

        long i;
        for (i=pNode->tokidx; i<ld.TokenCount() && (tok = ld.TokenAt(i).Token()) != TID_OPERATOR && tok != TID_EXPLICIT && tok != TID_IMPLICIT; i++)
            ;
        start = ld.TokenAt(i);
        return;
    }

    case NK_NEW:
        SetLine (pNode->asNEW()->pType);
        return;

    case NK_PROPERTY:
        if (pNode->asANYPROPERTY()->pName)
            SetLine (pNode->asANYPROPERTY()->pName);
        return;

    case NK_PREDEFINEDTYPE:
        return;
        
    case NK_ARRAYTYPE:
        SetLine (pNode->asARRAYTYPE()->pElementType);
        return;
        
    case NK_NAMEDTYPE:
        SetLine (pNode->asNAMEDTYPE()->pName);
        return;

    case NK_POINTERTYPE:
        SetLine (pNode->asPOINTERTYPE()->pElementType);
        return;

    case NK_NULLABLETYPE:
        SetLine (pNode->asNULLABLETYPE()->pElementType);
        return;

    case NK_ATTRDECL:
        SetLine (pNode->asATTRDECL()->pNameNode);
        return;

    default:
        break;
    }
}

// only set the end if it is on the same line
// as we want to report the error on
void ERRLOC::SetEndInternal(LEXDATA &ld, BASENODE* node)
{
    ASSERT(!start.IsUninitialized());

    SetEndInternal(ld, node->tokidx);
}

void ERRLOC::SetEndInternal(LEXDATA & ld, long tokidx, long tokOffset)
{
    ASSERT(!start.IsUninitialized());

    if (tokidx >= 0) {
        tokidx = CParser::PeekTokenIndexFrom( ld, tokidx, tokOffset);
        if (ld.TokenAt(tokidx).iLine == start.iLine) {
            end = ld.TokenAt(tokidx).StopPosition();
        }
    }
}

/*
 * Recursively searches parse-tree for correct right-most node
 *
 *                                                          
 */
void ERRLOC::SetEnd (BASENODE *pNode)
{
    LEXDATA     ld;

    if (start.IsUninitialized())
        return;

    if (pNode == NULL || pNode->tokidx == -1 || FAILED (m_sourceData->GetLexResults (&ld))) 
        return;

    SetEndInternal(ld, pNode);

    switch (pNode->kind) 
    {
    case NK_ACCESSOR:
        // Get and Set always are the token before the '{'
        SetEndInternal(ld, pNode->asACCESSOR()->iOpen, -1);
        return;

    case NK_ARROW:              // BINOPNODE
        if (pNode->asARROW()->p2)
            SetEnd (pNode->asARROW()->p2);
        return;

    case NK_ATTR:               // ATTRNODE
        if (pNode->asATTR()->pName)
            SetEnd (pNode->asATTR()->pName);
        return;
    
    case NK_ATTRDECL:
        if (pNode->asATTRDECL()->pNameNode)
            SetEnd (pNode->asATTRDECL()->pNameNode);
        return;

    case NK_ANONBLOCK:
        if (pNode->asANONBLOCK()->iClose != -1)
        {
            // Try to grab the parameters
            SetEndInternal(ld, pNode->asANONBLOCK()->iClose);
            // Try to grab the open-curly
            SetEndInternal(ld, pNode->asANONBLOCK()->iClose, 1);
        }

        // And possibly the entire block
        SetEndInternal(ld, pNode->asANONBLOCK()->pBody->iClose);
        return;
        
    case NK_CALL:               // CALL
    case NK_DEREF:
        SetEndInternal(ld, pNode->asANYCALL()->iClose);
        return;

    case NK_BINOP:              // BINOPNODE
        if (pNode->asBINOP()->p2)
            SetEnd(pNode->asBINOP()->p2);
        return;

    case NK_LIST:
        SetEnd(pNode->asANYBINOP()->p2);
        return;

    case NK_CLASS:              //CLASSNODE
    case NK_INTERFACE:
    case NK_STRUCT:
        if (pNode->asAGGREGATE()->pName)
            SetEnd (pNode->asAGGREGATE()->pName);
        return;

    case NK_CTOR:
    case NK_DTOR:
    {
        long i;
        for (i=pNode->tokidx; i<ld.TokenCount() && ld.TokenAt(i).Token() != TID_IDENTIFIER; i++)
            ;
        SetEndInternal(ld, i);
        return;
    }

    case NK_DELEGATE:
        if (pNode->asDELEGATE()->pName)
            SetEnd (pNode->asDELEGATE()->pName);
        return;

    case NK_DOT:                // BINOPNODE
        if (pNode->asDOT()->p2)
            SetEnd (pNode->asDOT()->p2);
        return;

    case NK_ENUM:
        if (pNode->asENUM()->pName)
            SetEnd (pNode->asENUM()->pName);
        return;

    case NK_GENERICNAME:
        SetEndInternal(ld, pNode->asGENERICNAME()->iClose);
        return;

    case NK_INDEXER:
    {
        long i;
        for (i=pNode->tokidx; i<ld.TokenCount(); i++)
        {
            SetEndInternal(ld, i);
            if (ld.TokenAt(i).Token() == TID_THIS)
            {
                break;
            }
        }

        return;
    }

    case NK_METHOD:             // METHODNODE
        // use the name to keep the the same as fields
        if (pNode->asMETHOD()->pName)
            SetEnd (pNode->asMETHOD()->pName);
        return;

    case NK_NAMESPACE:
        // use the name to keep the the same as fields
        if (pNode->asNAMESPACE()->pName)
            SetEnd (pNode->asNAMESPACE()->pName);
        return;

    case NK_OPERATOR:
    {
        long i;
        for (i=pNode->tokidx; i<ld.TokenCount() - 1 && ld.TokenAt(i).Token() != TID_OPERATOR; i++) {
            SetEndInternal(ld, i);
        }

        SetEndInternal(ld, i);
        if (pNode->asOPERATOR()->iOp == OP_IMPLICIT || pNode->asOPERATOR()->iOp == OP_EXPLICIT) {
            // for conversion operators the type is the end of the name (or the token before the open paren)
            SetEndInternal( ld, pNode->asOPERATOR()->iOpenParen, -1);
        } else {
            // For non-conversion operators the token after 'operator' which is the end of the 'name'
            SetEndInternal(ld, i, 1);
        }

        return;
    }

    case NK_NEW:
        // The type is the most important
        SetEnd (pNode->asNEW()->pType);
        // But try to get the arguments/array indexes
        SetEndInternal (ld, pNode->asNEW()->iClose);
        return;

    case NK_PROPERTY:
        if (pNode->asANYPROPERTY()->pName)
            SetEnd (pNode->asANYPROPERTY()->pName);
        return;

    case NK_PREDEFINEDTYPE:
    case NK_POINTERTYPE:
    case NK_NULLABLETYPE:
        return;

    case NK_NAMEDTYPE:
        return SetEnd (pNode->asNAMEDTYPE()->pName);
        
    case NK_ARRAYTYPE:
        // For array types, the token index is the open '['.  Add the number of
        // dimensions and you land on the ']' (1==[], 2==[,], 3==[,,], etc).
        if (pNode->asARRAYTYPE()->iDims == -1)
            SetEndInternal( ld, pNode->tokidx, 2);          // unknown rank is [?]
        else
            SetEndInternal( ld, pNode->tokidx, pNode->asARRAYTYPE()->iDims);
        return;
    case NK_UNOP:
        switch (pNode->Op()) {
        case OP_PAREN:
        case OP_PREINC:
        case OP_PREDEC:
            SetEnd(pNode->asUNOP()->p1);
            break;
        default:
            break;
        }
        return;
    default:
        break;
    }

}


/*
 * This controls how we handle all fatal errors, asserts, and exceptions
 */
LONG CompilerExceptionFilter(EXCEPTION_POINTERS* exceptionInfo, LPVOID pvData)
{
    COMPILER *compiler = (COMPILER *)pvData;
    DWORD exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;

    // Don't stop here for fatal errors
    if (exceptionCode == FATAL_EXCEPTION_CODE)
        return EXCEPTION_CONTINUE_SEARCH;

    // If it's an AV in our error buffer range, it might be because we need to grow our error buffer.
    // If so, then just commit another page an allow execution to continue
    if (exceptionCode == EXCEPTION_ACCESS_VIOLATION && compiler && compiler->errBuffer &&
        (ULONG_PTR)compiler->errBuffer < exceptionInfo->ExceptionRecord->ExceptionInformation[1] && 
        (ULONG_PTR)(compiler->errBuffer + ERROR_BUFFER_MAX_WCHARS) >= exceptionInfo->ExceptionRecord->ExceptionInformation[1])
    {
        void * temp = NULL;
        if (((compiler->errBufferNextPage - (BYTE*)compiler->errBuffer) < (int) ERROR_BUFFER_MAX_BYTES-1) &&
            (NULL != (temp = VirtualAlloc( compiler->errBufferNextPage, compiler->pageheap.pageSize, MEM_COMMIT, PAGE_READWRITE)))) {
            compiler->errBufferNextPage += compiler->pageheap.pageSize;
            return EXCEPTION_CONTINUE_EXECUTION;
        } else {
            // We either ran out of reserved memory, or couldn't commit what we've already reserved!?!?!?
            // Normally we shouldn't throw another exception inside the exception filter
            // but this really is a fatal condition
            compiler->Error(NULL, FTL_NoMemory);
        }
    }

    if (compiler && compiler->pController)
        compiler->pController->SetExceptionData (exceptionInfo);

#ifdef _DEBUG

    if (COMPILER::GetRegDWORD("GPF"))
        return EXCEPTION_CONTINUE_SEARCH;

#endif

    WatsonOperationKindEnum howToReportWatsons = WatsonOperationKind::Queue;
    WCHAR bugreport[MAX_PATH];
    bugreport[0] = L'\0';
    if (compiler) {
        howToReportWatsons = compiler->options.m_howToReportWatsons;
        if (compiler->cmdHost) {
            if (FAILED(compiler->cmdHost->GetBugReportFileName(bugreport, lengthof(bugreport))))
                bugreport[0] = '\0';
        }
    }

    LONG result = EXCEPTION_EXECUTE_HANDLER;

    if (compiler && result == EXCEPTION_EXECUTE_HANDLER) {
        PAL_TRY
        {
            compiler->ReportICE(exceptionInfo);
        }
        PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
        }
        PAL_ENDTRY
    }
    return result;
}




/*
 *  Set a breakpoint here to control 2nd chance exceptions
 */
LONG GenericExceptionFilter(EXCEPTION_POINTERS * exceptionInfo, LPVOID pv)
{
    // If you want to do something different during debugging, slam the appropriate
    // value into eax.
    // EXCEPTION_CONTINUE_EXECUTION is -1
    // EXCEPTION_CONTINUE_SEARCH is 0
    // EXCEPTION_EXECUTE_HANDLER is 1
    return EXCEPTION_EXECUTE_HANDLER;
}


int __cdecl compareErrorId(const void * id, const void * err) { return *(const int *)id - ((const ERROR_INFO*)err)->number; }

// IsValidWarningNumber -- determine if a number is a valid warning number.

bool IsValidWarningNumber(int id)
{
    // Just search the whole error table for the id.
    ERROR_INFO * err = (ERROR_INFO*)bsearch( &id, errorInfo, lengthof(errorInfo), sizeof(errorInfo[0]), compareErrorId);

    return (err != NULL && err->level > 0);
}





////////////////////////////////////////////////////////////////////////////////
// CError::CError

CError::CError () :
    m_iLocations(0),
    m_pLocations(NULL),
    m_pMappedLocations(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
// CError::~CError

CError::~CError ()
{
    for (short i = 0; i<m_iLocations; i++)
    {
        if (m_pLocations[i].bstrFile != NULL)
            SysFreeString (m_pLocations[i].bstrFile);
        if (m_pMappedLocations[i].bstrFile != NULL)
            SysFreeString (m_pMappedLocations[i].bstrFile);
    }

    VSFree (m_pLocations);
    VSFree (m_pMappedLocations);
}

HRESULT CError::WarnAsError(HINSTANCE hModuleMessages)
{
    ASSERT(m_iKind == ERROR_WARNING);
    m_iKind = ERROR_ERROR;

    PCWSTR rgpsz[1] = { GetText() };

    BSTR bstr = CError::ComputeString(hModuleMessages, IDS_WarnAsError, rgpsz);
    if (!bstr)
        return E_OUTOFMEMORY;

    m_sbstrText.Attach(bstr);         // Attach will free any previous string.
    return S_OK;
}

BSTR CError::ComputeString(HINSTANCE hModuleMessages, long iResourceID, VarArgList args)
{
    CAutoFree<WCHAR>    mem;
    PWSTR               pszBuffer = SAFEALLOC (mem, bufferSize);
    bool (*pfnLoadAndFormat)(HINSTANCE,int,va_list,VarArgList,PWSTR,int);

    if (W_IsUnicodeSystem())
        pfnLoadAndFormat = LoadAndFormatW;
    else
        pfnLoadAndFormat = LoadAndFormatA;

    // Load the message and fill in arguments.
    if (!pfnLoadAndFormat (hModuleMessages, iResourceID, NULL, args, pszBuffer, bufferSize))
    {
        // Try twice more with bigger buffers (same size as the max compiler buffer)
        if (((GetLastError() == ERROR_INSUFFICIENT_BUFFER || GetLastError() == ERROR_MORE_DATA) &&
            NULL != (pszBuffer = SAFEALLOC (mem, bufferSize * 8)) &&
            pfnLoadAndFormat (hModuleMessages, iResourceID, NULL, args, pszBuffer, bufferSize * 8)) ||
            ((GetLastError() == ERROR_INSUFFICIENT_BUFFER || GetLastError() == ERROR_MORE_DATA) &&
            NULL != (pszBuffer = SAFEALLOC (mem, ERROR_BUFFER_MAX_WCHARS)) &&
            pfnLoadAndFormat (hModuleMessages, iResourceID, NULL, args, pszBuffer, ERROR_BUFFER_MAX_WCHARS))) {
        } else {
            // Not a lot we can do if we can't report an error. Assert in debug so we know
            // what is going on.
            VSFAIL("FormatMessage failed to load error message");
            return NULL;
        }
    }

    return SysAllocString(pszBuffer);
}

////////////////////////////////////////////////////////////////////////////////
// CError::UpdateLocations

BOOL CError::UpdateLocations (const POSDATA &posOld, const POSDATA &posNew)
{
    BOOL    fChanged = FALSE;

    for (short i=0; i<m_iLocations; i++)
    {
        LOCATION    *p = m_pLocations + i;
        LOCATION    *pMap = m_pMappedLocations + i;

        // Assert mapped location is sthg we can handle (even in the
        // presence of line maps)
        VSASSERT (p->posStart.iChar == pMap->posStart.iChar, "A line map should not imply a character position change");
        VSASSERT (p->posEnd.iChar   == pMap->posEnd.iChar, "A line map should not imply a character position change");
        VSASSERT ((p->posStart.iLine - pMap->posStart.iLine) == (p->posEnd.iLine - pMap->posEnd.iLine),
                  "The line delta between a mapped position and the original position should not "
                  "be different for the start and end position, even in the presence of a linemap "
                  "(that would mean an error can span over a #line directive)");

        // Update the location only if it falls after posOld
        if (p->posStart.IsUninitialized() || p->posEnd.IsUninitialized() || p->posEnd < posOld)
            continue;

        // If the error spans the old, this function is being misused.
        if (p->posStart < posOld)
        {
            VSFAIL ("Misuse of CError::UpdateLocations!  Can't update an error spanning a change...");
            continue;
        }

        // Adjust both positions, including the mapped ones
        {
            // Cheat when updating the mapped line (we need to revert back to original line #)
            // Note: This code is valid only if you don't change the 3 asserts above in this method
            long iLineDelta = (p->posStart.iLine - pMap->posStart.iLine);
            pMap->posStart.iLine = p->posStart.iLine;   // Lie
            pMap->posStart.Adjust(posOld, posNew);      // Adjust
            pMap->posStart.iLine += iLineDelta;         // Reapply delta to adjusted position
        }

        {
            // Cheat when updating the mapped line (we need to revert back to original line #)
            // Note: This code is valid only if you don't change the 3 asserts above in this method
            long iLineDelta = (p->posEnd.iLine - pMap->posEnd.iLine);
            pMap->posEnd.iLine = p->posEnd.iLine;   // Lie
            pMap->posEnd.Adjust(posOld, posNew);    // Adjust
            pMap->posEnd.iLine += iLineDelta;       // Reapply delta to adjusted position
        }

        if (p->posStart.Adjust (posOld, posNew))
            fChanged = TRUE;

        if (p->posEnd.Adjust (posOld, posNew))
            fChanged = TRUE;

    }

    return fChanged;
}


////////////////////////////////////////////////////////////////////////////////
// CError::Initialize

HRESULT CError::Initialize (HINSTANCE hModuleMessages, long iErrorIndex, VarArgList args)
{
    ERRORKIND kind;
    if (errorInfo[iErrorIndex].level == 0)
        kind = ERROR_ERROR;
    else if (errorInfo[iErrorIndex].level > 0)
        kind = ERROR_WARNING;
    else
        kind = ERROR_FATAL;
    return Initialize(hModuleMessages, kind, errorInfo[iErrorIndex].level > 0, errorInfo[iErrorIndex].number, errorInfo[iErrorIndex].resid, args);
}

////////////////////////////////////////////////////////////////////////////////
// CError::Initialize

HRESULT CError::Initialize (HINSTANCE hModuleMessages, ERRORKIND iKind, BOOL fWasWarning, short iErrorID, long iResourceID, VarArgList args)
{
    BSTR bstr;

    // Save the kind and error ID
    m_iKind = iKind;
    m_iID = iErrorID;
    m_fWasWarning = fWasWarning;

    bstr = CError::ComputeString(hModuleMessages, iResourceID, args);

    if (!bstr) {
        return E_OUTOFMEMORY;
    }

    m_sbstrText.Attach(bstr);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CError::AddLocation

HRESULT CError::AddLocation (PCWSTR pszFileName, const POSDATA *pposStart, const POSDATA *pposEnd, PCWSTR pszMapFile, const POSDATA *pMapStart, const POSDATA *pMapEnd)
{
    // All locations must have a file name
    if (pszFileName == NULL)
        return E_POINTER;

    // Check for duplicates before adding the location
    for (short i = 0; i < m_iLocations; i++) {
        const LOCATION * p = m_pLocations + i;
        const LOCATION * mp = m_pMappedLocations + i;
        // Match up the locations and then the filenames (hope for an early out)
        if (((!pposStart && p->posStart.IsUninitialized() && p->posEnd.IsUninitialized()) ||
             (p->posStart == *pposStart && p->posEnd == *pposEnd)) &&
            ((!pMapStart && mp->posStart.IsUninitialized() && mp->posEnd.IsUninitialized()) ||
             (mp->posStart == *pMapStart && mp->posEnd == *pMapEnd )) &&
            wcscmp(p->bstrFile, pszFileName) == 0 &&
            wcscmp(mp->bstrFile, pszMapFile) == 0) 
            return S_OK; // We've already got this location
    }

    void    *pNew = (m_pLocations == NULL) ? VSAlloc (sizeof (LOCATION)) : VSRealloc (m_pLocations, (m_iLocations + 1) * sizeof (LOCATION));
    if (pNew == NULL) {
        return E_OUTOFMEMORY;
    }
    m_pLocations = (LOCATION *)pNew;

    void    *pNewMap = (m_pMappedLocations == NULL) ? VSAlloc (sizeof (LOCATION)) : VSRealloc (m_pMappedLocations, (m_iLocations + 1) * sizeof (LOCATION));
    if (pNewMap == NULL) {
        return E_OUTOFMEMORY;
    }
    m_pMappedLocations = (LOCATION *)pNewMap;

    ZeroMemory(&m_pLocations[m_iLocations], sizeof(LOCATION)); 
    ZeroMemory(&m_pMappedLocations[m_iLocations], sizeof(LOCATION));
 
    m_pLocations[m_iLocations].bstrFile = SysAllocString (pszFileName);
    m_pMappedLocations[m_iLocations].bstrFile = SysAllocString (pszMapFile);
    if (m_pLocations[m_iLocations].bstrFile == NULL || m_pMappedLocations[m_iLocations].bstrFile == NULL)
        return E_OUTOFMEMORY;

    if (pposStart != NULL)
    {
        // Passing one means passing both!
        ASSERT (pposEnd != NULL);
        m_pLocations[m_iLocations].posStart = *pposStart;
        m_pLocations[m_iLocations].posEnd = *pposEnd;
    }
    else
    {
        ASSERT (pposEnd == NULL);
        m_pLocations[m_iLocations].posStart.SetUninitialized();
        m_pLocations[m_iLocations].posEnd.SetUninitialized();
    }

    if (pMapStart != NULL)
    {
        // Passing one means passing both!
        ASSERT (pMapEnd != NULL);
        m_pMappedLocations[m_iLocations].posStart = *pMapStart;
        m_pMappedLocations[m_iLocations].posEnd = *pMapEnd;
    }
    else
    {
        ASSERT (pMapEnd == NULL);
        m_pMappedLocations[m_iLocations].posStart.SetUninitialized();
        m_pMappedLocations[m_iLocations].posEnd.SetUninitialized();
    }

    m_iLocations++;
    return S_OK;

}

////////////////////////////////////////////////////////////////////////////////
// CError::GetErrorInfo

STDMETHODIMP CError::GetErrorInfo (long *piErrorID, ERRORKIND *pKind, PCWSTR *ppszText)
{
    if (piErrorID != NULL)
        *piErrorID = m_iID;
    if (pKind != NULL)
        *pKind = m_iKind;
    if (ppszText != NULL)
        *ppszText = m_sbstrText;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CError::GetLocationCount

STDMETHODIMP CError::GetLocationCount (long *piLocations)
{
    if (piLocations == NULL)
        return E_POINTER;

    *piLocations = m_iLocations;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CError::GetUnmappedLocationAt - returns the actual input location

STDMETHODIMP CError::GetUnmappedLocationAt (long iIndex, PCWSTR *ppszFileName, POSDATA *pposStart, POSDATA *pposEnd)
{
    if (iIndex < 0 || iIndex >= (long)m_iLocations)
        return E_INVALIDARG;

    if (ppszFileName != NULL)
        *ppszFileName = m_pLocations[iIndex].bstrFile;

    if (pposStart != NULL)
        *pposStart = m_pLocations[iIndex].posStart;

    if (pposEnd != NULL)
        *pposEnd = m_pLocations[iIndex].posEnd;

    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CError::GetLocationAt - returns the mapped (via #line) location

STDMETHODIMP CError::GetLocationAt (long iIndex, PCWSTR *ppszFileName, POSDATA *pposStart, POSDATA *pposEnd)
{
    if (iIndex < 0 || iIndex >= (long)m_iLocations)
        return E_INVALIDARG;

    if (ppszFileName != NULL)
        *ppszFileName = m_pMappedLocations[iIndex].bstrFile;

    if (pposStart != NULL)
        *pposStart = m_pMappedLocations[iIndex].posStart;

    if (pposEnd != NULL)
        *pposEnd = m_pMappedLocations[iIndex].posEnd;

    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::CErrorContainer

CErrorContainer::CErrorContainer () :
    m_ppErrors(NULL),
    m_iErrors(0),
    m_iWarnings(0),
    m_iWarnAsErrors(0),
    m_iCount(0),
    m_pReplacements(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::~CErrorContainer

CErrorContainer::~CErrorContainer ()
{
    if (m_ppErrors != NULL)
    {
        ASSERT(m_iCount <= cerrMax);

        for (long i=0; i<m_iCount; i++)
        {
            ASSERT(m_ppErrors[i] != NULL);
            if (m_ppErrors[i] != NULL)
                m_ppErrors[i]->Release();
        }

        VSFree(m_ppErrors);
        m_ppErrors = NULL;
    }

    if (m_pReplacements != NULL) {
        m_pReplacements->Release();
        m_pReplacements = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::CreateInstance

HRESULT CErrorContainer::CreateInstance (ERRORCATEGORY iCategory, DWORD_PTR dwID, CErrorContainer **ppContainer)
{
    CComObject<CErrorContainer> *pObj;
    HRESULT                     hr;

    if (SUCCEEDED (hr = CComObject<CErrorContainer>::CreateInstance (&pObj)))
    {
        if (FAILED (hr = pObj->Initialize (iCategory, dwID)))
        {
            delete pObj;
        }
        else
        {
            *ppContainer = pObj;
            pObj->AddRef();
        }
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::Initialize

HRESULT CErrorContainer::Initialize (ERRORCATEGORY iCategory, DWORD_PTR dwID)
{
    m_iCategory = iCategory;
    m_dwID = dwID;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::Clone

HRESULT CErrorContainer::Clone (CErrorContainer **ppContainer)
{
    // Can't clone a container while it is in replacement mode!
    if (m_pReplacements != NULL)
    {
        VSFAIL("Can't clone an error container while it is in replace mode!");
        return E_FAIL;
    }

    CErrorContainer *pNew;
    HRESULT hr;

    if (FAILED(hr = CreateInstance(m_iCategory, m_dwID, &pNew)))
        return hr;

    if (m_iCount) {
        ASSERT(m_iCount <= cerrMax);

        pNew->m_ppErrors = (CError **)VSAlloc(RoundUp8(m_iCount) * sizeof(CError *));
        if (pNew->m_ppErrors == NULL)
        {
            delete pNew;
            return E_OUTOFMEMORY;
        }

        pNew->m_iCount = m_iCount;
        for (long i=0; i<m_iCount; i++)
        {
            pNew->m_ppErrors[i] = m_ppErrors[i];
            pNew->m_ppErrors[i]->AddRef();
        }

        pNew->m_iErrors = m_iErrors;
        pNew->m_iWarnings = m_iWarnings;
        pNew->m_iWarnAsErrors = m_iWarnAsErrors;
    }
    else {
        ASSERT(!m_iErrors);
        ASSERT(!m_iWarnings);
        ASSERT(!m_iWarnAsErrors);
    }

    *ppContainer = pNew;            // NOTE:  Ref count is transferred here...
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::BeginReplacement
//
// This function turns "replace mode" on for this container.  All incoming
// errors get added to the replacement container instead of this one, and get
// retrofitted into this when EndReplacement is called (replacing errors that
// fall within the range supplied).

HRESULT CErrorContainer::BeginReplacement ()
{
    if (m_pReplacements != NULL)
    {
        VSFAIL ("Can't enter replace mode a second time!");
        return E_FAIL;
    }

    return CreateInstance (m_iCategory, m_dwID, &m_pReplacements);
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::EndReplacement
//
// This function is called to terminate "replace mode" and place all errors (if
// any) in the replacement container into this container, first removing all
// existing errors that fall within the given range.
//
// Returns S_FALSE if nothing changed.

HRESULT CErrorContainer::EndReplacement (const POSDATA &posStart, const POSDATA &posOldEnd, const POSDATA &posNewEnd)
{
    if (m_pReplacements == NULL) {
        VSFAIL ("Can't terminate a replacement that hasn't started!");
        return E_FAIL;
    }

    ASSERT(m_iCount <= cerrMax);

    long ierrMinDel = m_iCount; // The first current error to delete.
    long ierrLimDel = m_iCount; // One past the last current error to delete.
    POSDATA pos;

    // Okay, find the span of errors that fall within the given range.  It MUST
    // be a contiguous range (because errors are sorted by position when added).
    // Note that this is a linear search -- tolerable because the number of
    // errors coming from EC_TOKENIZATION (the only category that uses this so
    // far) is expected to be small, and because replacement doesn't happen often.
    if (posOldEnd.IsZero()) {
        // In this case, we replace all existing errors
        ierrMinDel = 0;
    }
    else {
        for (long ierr = 0; ierr < m_iCount; ierr++) {
            ASSERT (m_ppErrors[ierr] != NULL);
            if (FAILED(m_ppErrors[ierr]->GetUnmappedLocationAt(0, NULL, &pos, NULL)))
                continue;

            if (ierrMinDel == m_iCount && pos >= posStart)
                ierrMinDel = ierr;

            if (pos > posOldEnd) {
                ASSERT(ierrMinDel < m_iCount); // If this fires, then posStart > posOldEnd -- how'd that happen?!?
                ierrLimDel = ierr;
                break;
            }
        }
    }
    ASSERT(0 <= ierrMinDel && ierrMinDel <= ierrLimDel && ierrLimDel <= m_iCount);

    // If we're adding anything or deleting anything we're changing.
    bool fChanged = (m_pReplacements->m_iCount | (ierrLimDel - ierrMinDel)) != 0;

    if (ierrMinDel == 0 && ierrLimDel == m_iCount) {
        // Just swap the contents of the two containers.
        Swap(m_ppErrors, m_pReplacements->m_ppErrors);
        Swap(m_iCount, m_pReplacements->m_iCount);
        Swap(m_iErrors, m_pReplacements->m_iErrors);
        Swap(m_iWarnings, m_pReplacements->m_iWarnings);
        Swap(m_iWarnAsErrors, m_pReplacements->m_iWarnAsErrors);
    }
    else {
        // Determine the new size of our array (which is our current count, plus the
        // new ones, minus the ones between ierrMinDel and ierrLimDel).
        long cerrIns = m_pReplacements->m_iCount;
        long cerrNew = m_iCount - (ierrLimDel - ierrMinDel) + cerrIns;

        if (cerrNew > cerrMax) {
            // Don't keep more than cerrMax.
            cerrNew = cerrMax;
            if (ierrMinDel > cerrNew) {
                VSFAIL("Why is ierrMinDel > cerrMax?");
                ierrMinDel = cerrNew;
                ierrLimDel = m_iCount;
                cerrIns = 0;
            }
            else if (ierrMinDel + cerrIns > cerrNew) {
                cerrIns = cerrNew - ierrMinDel;
                ierrLimDel = m_iCount;
            }
            else {
                // Need to chop stuff off the end.
                long cerrCur = cerrNew - cerrIns + (ierrLimDel - ierrMinDel);
                ASSERT(0 <= cerrCur && cerrCur < m_iCount);
                ClearSomeErrors(cerrCur, m_iCount);
                m_iCount = cerrCur;
            }
        }
        ASSERT(0 <= ierrMinDel && ierrMinDel <= ierrLimDel && ierrLimDel <= m_iCount);
        ASSERT(cerrNew == m_iCount - (ierrLimDel - ierrMinDel) + cerrIns);

        // Grow the array if needed.
        if (cerrNew > m_iCount) {
            size_t cbNew = SizeMul(RoundUp8(cerrNew), sizeof(CError *));
            void * pvNew = (m_ppErrors == NULL) ? VSAlloc(cbNew) : VSRealloc(m_ppErrors, cbNew);

            if (!pvNew) {
                // Still need to end replacement mode.
                m_pReplacements->Release();
                m_pReplacements = NULL;
                return E_OUTOFMEMORY;
            }
            m_ppErrors = (CError **)pvNew;
        }

        // Release the errors that are being replaced, keeping track of warning/error/fatal counts.
        ClearSomeErrors(ierrMinDel, ierrLimDel);

        // Any errors beyond the edit need to have their locations updated.
        for (long ierr = ierrLimDel; ierr < m_iCount; ierr++) {
            if (m_ppErrors[ierr]->UpdateLocations(posOldEnd, posNewEnd))
                fChanged = true;
        }

        if (cerrNew != m_iCount) {
            // Move the errors beyond the edit to their new positions.
            memmove(m_ppErrors + ierrLimDel + (cerrNew - m_iCount), m_ppErrors + ierrLimDel, (m_iCount - ierrLimDel) * sizeof(CError *));
            if (cerrNew < m_iCount) {
                // We can shrink the block now. This shouldn't ever fail, but we don't care if it does.
                size_t cbNew = SizeMul(RoundUp8(cerrNew), sizeof(CError *));
                if (!cbNew) {
                    VSFree(m_ppErrors);
                    m_ppErrors = NULL;
                }
                else {
                    void *pvNew = VSRealloc(m_ppErrors, cbNew);
                    if (pvNew)
                        m_ppErrors = (CError **)pvNew;
                }
            }
        }

        // Adjust our count
        m_iCount = cerrNew;
        ASSERT(m_iCount <= cerrMax);

        // Copy the errors from the replacement container into the array, keeping counts of
        // warnings/errors/fatals.
        for (long ierr = 0; ierr < cerrIns; ierr++) {
            ERRORKIND kind;
            CError * perr = m_pReplacements->m_ppErrors[ierr];
            ASSERT(perr);
            m_ppErrors[ierrMinDel + ierr] = perr;
            perr->AddRef();
            perr->GetErrorInfo(NULL, &kind, NULL);
            if (kind == ERROR_ERROR) {
                m_iErrors++;
                if (perr->WasWarning())
                    m_iWarnAsErrors++;
            }
            else if (kind == ERROR_WARNING)
                m_iWarnings++;
        }

#ifdef DEBUG
        for (long ierr = 0; ierr < cerrNew; ierr++)
            ASSERT(m_ppErrors[ierr]);
#endif // !DEBUG
    }

    // Get rid of the replacement container
    m_pReplacements->Release();
    m_pReplacements = NULL;

    // Fin!
    return fChanged ? S_OK : S_FALSE;
}


void CErrorContainer::ClearSomeErrors(long ierrMin, long ierrLim)
{
    ASSERT(m_iCount <= cerrMax);
    ASSERT(0 <= ierrMin && ierrMin <= ierrLim && ierrLim <= m_iCount);

    // Release the errors that are being replaced, keeping track of warning/error/fatal counts.
    for (long ierr = ierrMin; ierr < ierrLim; ierr++) {
        ERRORKIND kind;
        m_ppErrors[ierr]->GetErrorInfo(NULL, &kind, NULL);
        if (kind == ERROR_ERROR) {
            ASSERT(m_iErrors > 0);
            m_iErrors--;
            if (m_ppErrors[ierr]->WasWarning()) {
                ASSERT(m_iWarnAsErrors > 0);
                m_iWarnAsErrors--;
            }
        }
        else if (kind == ERROR_WARNING) {
            ASSERT(m_iWarnings > 0);
            m_iWarnings--;
        }
        m_ppErrors[ierr]->Release();
        m_ppErrors[ierr] = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::AddError

HRESULT CErrorContainer::AddError (CError *pError)
{
    // If we're in replacement mode, this goes in our replacement container
    if (m_pReplacements != NULL)
        return m_pReplacements->AddError (pError);

    if ((m_iCount & 7) == 0)
    {
        if (m_iCount >= cerrMax)
            return S_OK;

        // Time to grow
        long    iSize = (m_iCount + 8) * sizeof (CError *);
        void    *pNew = (m_ppErrors == NULL) ? VSAlloc (iSize) : VSRealloc (m_ppErrors, iSize);

        if (!pNew)
            return E_OUTOFMEMORY;
        m_ppErrors = (CError **)pNew;
    }

    POSDATA pos, posNew;
    long    iInsert = m_iCount;

    if (SUCCEEDED (pError->GetLocationAt (0, NULL, &posNew, NULL)))
    {
        // Search backwards to see if we have to slide any errors down.  This doesn't
        // happen very often, but just in case...
        for (long i=m_iCount - 1; i >= 0; i--)
        {
            ASSERT (m_ppErrors[i] != NULL);
            if (FAILED (m_ppErrors[i]->GetLocationAt (0, NULL, &pos, NULL)))
                continue;

            if (pos <= posNew)
            {
                iInsert = i + 1;
                break;
            }
        }

        if (iInsert < m_iCount)
            memmove (m_ppErrors + iInsert + 1, m_ppErrors + iInsert, (m_iCount - iInsert) * sizeof (CError *));
    }

    m_ppErrors[iInsert] = pError;
    pError->AddRef();
    m_iCount++;
    ASSERT(m_iCount <= cerrMax);

    ERRORKIND   kind;

    // Keep track of errors/warnings/fatals (fatals are calculated)
    pError->GetErrorInfo (NULL, &kind, NULL);
    if (kind == ERROR_WARNING)
        m_iWarnings++;
    else if (kind == ERROR_ERROR)
    {
        if (pError->WasWarning())
            m_iWarnAsErrors++;
        m_iErrors++;
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::ReleaseAllErrors

void CErrorContainer::ReleaseAllErrors ()
{
    for (long i=0; i<m_iCount; i++)
    {
        ASSERT (m_ppErrors[i] != NULL);
        if (m_ppErrors[i] != NULL)
            m_ppErrors[i]->Release();
    }

    m_iCount = m_iErrors = m_iWarnings = m_iWarnAsErrors = 0;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::GetContainerInfo

STDMETHODIMP CErrorContainer::GetContainerInfo (ERRORCATEGORY *pCategory, DWORD_PTR *pdwID)
{
    if (pCategory != NULL)
        *pCategory = m_iCategory;

    if (pdwID != NULL)
        *pdwID = m_dwID;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::GetErrorCount

STDMETHODIMP CErrorContainer::GetErrorCount (long *piWarnings, long *piErrors, long *piFatals, long *piTotal)
{
    if (piWarnings != NULL)
        *piWarnings = m_iWarnings;

    if (piErrors != NULL)
        *piErrors = m_iErrors;

    // NOTE:  Fatals is calculated by the total minus the errors and warnings
    if (piFatals != NULL)
        *piFatals = m_iCount - (m_iWarnings + m_iErrors);

    if (piTotal != NULL)
        *piTotal = m_iCount;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::GetErrorCount

STDMETHODIMP CErrorContainer::GetWarnAsErrorCount (long *piWarnAsErrors)
{
    if (piWarnAsErrors != NULL)
        *piWarnAsErrors = m_iWarnAsErrors;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CErrorContainer::GetErrorAt

STDMETHODIMP CErrorContainer::GetErrorAt (long iIndex, ICSError **ppError)
{
    if (iIndex < 0 || iIndex >= m_iCount)
        return E_INVALIDARG;

    *ppError = m_ppErrors[iIndex];
    (*ppError)->AddRef();
    return S_OK;
}

ErrArgSymKind::ErrArgSymKind(SYM * sym)
{
    eak = ErrArgKind::SymKind;
    eaf = ErrArgFlags::None;
    sk = sym->getKind();
    if (sk == SK_NSAIDSYM) {
        if (sym->asNSAIDSYM()->GetNS()->name->text[0]) {
            // Non-empty namespace name means it's not the root
            // so treat it like a namespace instead of an alias
            sk = SK_NSSYM;
        }
        else {
            // An empty namespace name means it's just an alias for the root
            sk = SK_EXTERNALIASSYM;
        }
    }
}

ErrArgAggKind::ErrArgAggKind(AGGSYM * agg)
{
    eak = ErrArgKind::AggKind;
    eaf = ErrArgFlags::None;
    ak = agg->AggKind();
}

ErrArgAggKind::ErrArgAggKind(AGGTYPESYM * ats)
{
    eak = ErrArgKind::AggKind;
    eaf = ErrArgFlags::None;
    ak = ats->getAggregate()->AggKind();
}

