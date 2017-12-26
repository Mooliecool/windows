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
// File: options.cpp
//
// Code to parse and set the compiler options.
// ===========================================================================

#include "stdafx.h"


////////////////////////////////////////////////////////////////////////////////
// Create the options table

// _offsetof is exactly like the regular offsetof macro except that it has protection against 
// overriden operator&. Without the _offsetof macro, m_rgOptionTable would be initialized 
// at runtime instead of compile time because of overriden operator& on m_sbstr##id.
#define _offsetof(s,m)   PAL_safe_offsetof(s,m)

#define BOOLOPTDEF(id,descid,sh,lg,des,f) { sh, lg, des, OPTID_##id, offsetof (COptionData, m_f##id), IDS_OD_##descid, NULL, f|COF_BOOLEAN },
#define CORESTROPTDEF(id,descid,sh,lg,des,f) { sh, lg, des, OPTID_##id, _offsetof (COptionData, m_sbstr##id), IDS_OD_##descid, NULL, f },
#define STROPTDEF(id,descid,sh,lg,des,f) { sh, lg, des, OPTID_##id, _offsetof (COptionData, m_sbstr##id), IDS_OD_##descid, NULL, f },
OPTIONDEF   COptionData::m_rgOptionTable[] =
{
    #include "optdef.h"
    { NULL, NULL, NULL, 0, 0, NULL, 0 }
};

const int TOTAL_OPTIONS = lengthof(COptionData::m_rgOptionTable) - 1;

/*
 * Compiler warning numbers.
 */
#define ERRORDEF(num, name, strid)
#define WARNDEF(num, level, name, strid) num,
#define OLDWARN(num, name)               num,
#define FATALDEF(num, name, strid)

long COptionData::m_rgiWarningIDs [] =
{
    #include "errors.h"
};

#undef ERRORDEF
#undef WARNDEF
#undef OLDWARN
#undef FATALDEF

#define ERRORDEF(num, name, strid)
#define WARNDEF(num, level, name, strid) {strid, NULL},
#define OLDWARN(num, name)               {-1,    NULL},
#define FATALDEF(num, name, strid)

WARNINGDEF COptionData::m_rgWarningInfo [] =
{
    #include "errors.h"
};

#undef ERRORDEF
#undef WARNDEF
#undef OLDWARN
#undef FATALDEF

long    COptionData::m_rgiOptionIndexMap[LARGEST_OPTION_ID];
BOOL    COptionData::m_fMapCreated = FALSE;

const DWORD COptionData::CountOfWarnings() const { return lengthof(m_rgiWarningIDs); }

////////////////////////////////////////////////////////////////////////////////
// COptionData::CreateIndexMap

void COptionData::CreateIndexMap ()
{
    long i;
    for (i=0; i<LARGEST_OPTION_ID; i++)
        m_rgiOptionIndexMap[i] = -1;

    for (i=0; i<TOTAL_OPTIONS; i++)
        m_rgiOptionIndexMap[m_rgOptionTable[i].iOptionID] = i;

    m_fMapCreated = TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// COptionData::ResetToDefaults

void COptionData::ResetToDefaults ()
{
    for (long i=0; i<TOTAL_OPTIONS; i++)
    {
        if (m_rgOptionTable[i].dwFlags & COF_BOOLEAN)
        {
            *((BOOL *)(((BYTE *)this) + m_rgOptionTable[i].iOffset)) = m_rgOptionTable[i].dwFlags & COF_DEFAULTON;
        }
        else if (m_rgOptionTable[i].dwFlags & COF_HASDEFAULT)
        {
            // These are special cased...
            if (m_rgOptionTable[i].iOptionID == OPTID_WARNINGLEVEL)
                m_sbstrWARNINGLEVEL = L"4";
            else
                VSFAIL ("HEY!  A string option has a default value that isn't handled!");
        }
        else
        {
            ((CComBSTR *)(((BYTE *)this) + m_rgOptionTable[i].iOffset))->Empty();
        }
    }

    // Set the post-commit options
    warnLevel = 4;
    pdbOnly = false;
    m_howToReportWatsons = WatsonOperationKind::Queue;
    m_platform = platformAgnostic;
    noWarnNumbers.Empty();
    compatMode = CompatibilityNone;

    unknownTestSwitch.Empty();

    m_fNOCODEGEN = false;
    ASSERT(m_fDELAYSIGN != TRUE && m_fDELAYSIGN != FALSE);
}

////////////////////////////////////////////////////////////////////////////////
// CCoreOptionData::IsNoWarnNumber

bool CCoreOptionData::IsNoWarnNumber(WORD id)
{
    int cNoWarn = noWarnNumbers.Length();

    for (int i = 0; i < cNoWarn; ++i) {
        if (id == noWarnNumbers[i])
            return true; // warning suppressed by noWarn list
    }
    return false;
}

int __cdecl CompareWarnId(const void * p1, const void * p2)
{
    return *(WORD*)p1 - *(long*)p2;
}

////////////////////////////////////////////////////////////////////////////////
// COptionData::IsWarnAsError

bool COptionData::IsWarnAsError(WORD id)
{
    long * p = (long*)bsearch( &id, m_rgiWarningIDs, lengthof(m_rgiWarningIDs), sizeof(m_rgiWarningIDs[0]), CompareWarnId);
    
    ASSERT(p && (WORD)*p == id);
    if ((size_t)(p - m_rgiWarningIDs) >= (size_t)m_sbstrWARNASERRORLIST.Length()) {
        return false;
    }

    return !!(m_sbstrWARNASERRORLIST[p - m_rgiWarningIDs]);
}

////////////////////////////////////////////////////////////////////////////////
// COptionData::LoadCachedBSTR

HRESULT COptionData::LoadCachedBSTR(BSTR* pbstr, long iResID)
{
    CComBSTR    sbstr;

    sbstr.LoadString (_Module.GetResourceInstance(), iResID);
    if (sbstr == NULL)
        return E_UNEXPECTED;

    // Swap the pointers in a thread safe manner
    if (InterlockedCompareExchangePointer((PVOID *)pbstr, (PVOID)(BSTR)sbstr, NULL) == NULL)
        sbstr.Detach();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// COptionData::Shutdown

void COptionData::Shutdown()
{
    int i;

    for (i=0; i<TOTAL_OPTIONS; i++) {
        OPTIONDEF *pDef = m_rgOptionTable + i;
        BSTR bstr = pDef->bstrDesc;
        if (bstr != NULL) {           
            pDef->bstrDesc = NULL;
            SysFreeString(bstr);
        }
    }

    for (i = 0; i<(int)lengthof(m_rgWarningInfo); i++) {
        WARNINGDEF *pDef = m_rgWarningInfo + i;
        BSTR bstr = pDef->bstrDesc;
        if (bstr != NULL) {           
            pDef->bstrDesc = NULL;
            SysFreeString(bstr);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::CCompilerConfig

CCompilerConfig::CCompilerConfig () :
    m_pController(NULL),
    m_pData(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::~CCompilerConfig

CCompilerConfig::~CCompilerConfig ()
{
    if (m_pController != NULL) {
        m_pController->Release();
        m_pController = NULL;
    }

    if (m_pData != NULL) {
        delete m_pData;
        m_pData = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::Initialize

HRESULT CCompilerConfig::Initialize (CController *pController, COptionData *pData)
{
    m_pController = pController;
    pController->AddRef();

    m_pData = new COptionData (*pData);
    if (m_pData == NULL)
        return E_OUTOFMEMORY;

    // The command-line compiler defaults to  headless mode.
    // Note that we don't need to add this code to ResetAllOptions since the command-line compiler never calls it
    // So if ResetAllOptions is ever called, we know it's the IDE.
    CComPtr<ICSCommandLineCompilerHost> cmdHost;
    if (SUCCEEDED(m_pController->GetHost()->QueryInterface(IID_ICSCommandLineCompilerHost, (void**)&cmdHost)))
    {
        m_pData->m_howToReportWatsons = WatsonOperationKind::Queue;
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::GetOptionCount

STDMETHODIMP CCompilerConfig::GetOptionCount (long *piCount)
{
    int ioptLast;

    // Hide the ones with no names.
    for (ioptLast = TOTAL_OPTIONS; --ioptLast >= 0 && !COptionData::GetOptionDef(ioptLast)->pszLongSwitch; ) {
        ASSERT(!COptionData::GetOptionDef(ioptLast)->pszShortSwitch);
        ASSERT(!COptionData::GetOptionDef(ioptLast)->pszDescSwitch);
        ASSERT(!COptionData::GetOptionDef(ioptLast)->bstrDesc);
    }
    *piCount = ioptLast + 1;

#ifdef DEBUG
    // Make sure all the nameless ones are at the end!
    while (--ioptLast >= 0) {
        ASSERT(COptionData::GetOptionDef(ioptLast)->pszLongSwitch);
    }
#endif // DEBUG

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::GetOptionInfoAt
//
// Note that *ppszSwitchName and *ppszDescription are modified to point
// to memory that is held by the compiler until shutdown.

STDMETHODIMP CCompilerConfig::GetOptionInfoAt (long iIndex, long *piOptionID, PCWSTR *ppszSwitchName, PCWSTR *ppszDescription, DWORD *pdwFlags)
{
    if (iIndex < 0 || iIndex >= TOTAL_OPTIONS)
        return E_INVALIDARG;

    OPTIONDEF   *pDef = COptionData::GetOptionDef (iIndex);

    // Nameless ones are also invalid.
    if (!pDef->pszLongSwitch)
        return E_INVALIDARG;

    // Provide the option ID
    if (piOptionID != NULL)
        *piOptionID = pDef->iOptionID;

    // Provide the switch name
    if (ppszSwitchName != NULL) {
        if (pDef->pszShortSwitch)
            *ppszSwitchName = pDef->pszShortSwitch;
        else
            *ppszSwitchName = pDef->pszLongSwitch;
    }

    // Provide the description string
    if (ppszDescription != NULL)
    {
        // Make sure the description string is loaded
        if (pDef->bstrDesc == NULL && (pDef->iDescID != IDS_OD_HIDDEN))
        {
            HRESULT hr;
            if (FAILED(hr = COptionData::LoadCachedBSTR(&pDef->bstrDesc, pDef->iDescID)))
                return hr;
        }

        *ppszDescription = pDef->bstrDesc;
        if (*ppszDescription == NULL)
            *ppszDescription = L"";
    }

    // Provide flags
    if (pdwFlags != NULL)
        *pdwFlags = pDef->dwFlags;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::GetOptionInfoAtEx

STDMETHODIMP CCompilerConfig::GetOptionInfoAtEx (long iIndex, long *piOptionID, PCWSTR *ppszShortSwitchName, PCWSTR *ppszLongSwitchName, PCWSTR *ppszDescSwitchName, PCWSTR *ppszDescription, DWORD *pdwFlags)
{
    if (iIndex < 0 || iIndex >= TOTAL_OPTIONS)
        return E_INVALIDARG;

    OPTIONDEF   *pDef = COptionData::GetOptionDef (iIndex);

    // Nameless ones are also invalid.
    if (!pDef->pszLongSwitch)
        return E_INVALIDARG;

    // Provide the option ID
    if (piOptionID != NULL)
        *piOptionID = pDef->iOptionID;

    // Provide the switch names
    if (ppszShortSwitchName != NULL)
        *ppszShortSwitchName = pDef->pszShortSwitch;
    if (ppszLongSwitchName != NULL)
        *ppszLongSwitchName = pDef->pszLongSwitch;
    if (ppszDescSwitchName != NULL)
        *ppszDescSwitchName = pDef->pszDescSwitch;

    // Provide the description string
    if (ppszDescription != NULL)
    {
        // Make sure the description string is loaded
        if (pDef->bstrDesc == NULL && (pDef->iDescID != IDS_OD_HIDDEN))
        {
            HRESULT hr;
            if (FAILED(hr = COptionData::LoadCachedBSTR(&pDef->bstrDesc, pDef->iDescID)))
                return hr;
        }

        *ppszDescription = pDef->bstrDesc;
        if (*ppszDescription == NULL)
            *ppszDescription = L"";
    }

    // Provide flags
    if (pdwFlags != NULL)
        *pdwFlags = pDef->dwFlags;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::ResetAllOptions

STDMETHODIMP CCompilerConfig::ResetAllOptions ()
{
    m_pData->ResetToDefaults ();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::GetOption

STDMETHODIMP CCompilerConfig::GetOption (long iOptionID, VARIANT *pvtValue)
{
    if (iOptionID < 0 || iOptionID >= LARGEST_OPTION_ID)
        return E_INVALIDARG;

    long        iIndex = COptionData::GetOptionIndex (iOptionID);
    if (iIndex == -1)
        return E_INVALIDARG;

    OPTIONDEF   *pOpt = COptionData::GetOptionDef (iIndex);
    void        *pMember = (void *)(((BYTE *)m_pData) + pOpt->iOffset);

    if (pOpt->dwFlags & COF_BOOLEAN)
    {
        V_VT(pvtValue) = VT_BOOL;
        V_BOOL(pvtValue) = (!!*((BOOL *)pMember)) ? VARIANT_TRUE : VARIANT_FALSE;
        return S_OK;
    }
    else
    {
        return ((CComBSTR*)pMember)->CopyTo(pvtValue);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::SetOption

STDMETHODIMP CCompilerConfig::SetOption (long iOptionID, VARIANT vtValue)
{
    if (iOptionID < 0 || iOptionID >= LARGEST_OPTION_ID)
        return E_INVALIDARG;

    long        iIndex = COptionData::GetOptionIndex (iOptionID);
    if (iIndex == -1)
        return E_INVALIDARG;

    OPTIONDEF   *pOpt = COptionData::GetOptionDef (iIndex);
    void        *pMember = (void *)(((BYTE *)m_pData) + pOpt->iOffset);

    // Check for booleans first...
    if (pOpt->dwFlags & COF_BOOLEAN)
    {
        if (V_VT(&vtValue) != VT_BOOL)
            return E_INVALIDARG;

        *(BOOL *)pMember = V_BOOL(&vtValue);

        if (iOptionID == OPTID_WARNINGSAREERRORS)
        {
            m_pData->m_sbstrWARNASERRORLIST = CComBSTR(lengthof(m_pData->m_rgiWarningIDs));
            memset(m_pData->m_sbstrWARNASERRORLIST, V_UI1 (&vtValue), m_pData->m_sbstrWARNASERRORLIST.ByteLength());
        }

        return S_OK;
    }

    // All other options are string...
    if (V_VT(&vtValue) != VT_BSTR)
        return E_INVALIDARG;

    // Special-case the 'test' switch used for undocumented internal/test switches.
    if (iOptionID == OPTID_INTERNALTESTS)
    {
        WCHAR* opt;
        WCHAR* next = V_BSTR(&vtValue);
        DWORD chLen = 0;

        for(next = V_BSTR(&vtValue); next != NULL; opt = next) {
            opt = next;
            next = wcschr(opt, L';');
            if (next == NULL)
                chLen = (int)wcslen(opt);
            else {
                chLen = (DWORD)(next - opt);
                if ( !*(++next))
                    next = NULL;
            }
            bool fBoolSet = false;
            bool fBoolVal = true;
            if (chLen > 0 && (opt[0] == L'+' || opt[0] == L'-')) {
                fBoolSet = true;
                fBoolVal = (opt[0] == L'+');
                chLen--;
                opt++;
            }

            m_pData->unknownTestSwitch = CComBSTR(chLen, opt);
        }

        m_pData->m_sbstrINTERNALTESTS += L";";
        m_pData->m_sbstrINTERNALTESTS += V_BSTR(&vtValue);
        return S_OK;
    }

    // Setting the debug type automatically turns debugging on.
    if (iOptionID == OPTID_DEBUGTYPE)
        m_pData->m_fEMITDEBUGINFO = true;
    
    // Set all others...
    return ((CComBSTR *)pMember)->AssignBSTR(V_BSTR(&vtValue));
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::GetWarnNumbers

STDMETHODIMP CCompilerConfig::GetWarnNumbers (long *piCount, const long ** pWarnIDs)
{
    if (!piCount || !pWarnIDs)
        return E_INVALIDARG;

    *piCount = lengthof(m_pData->m_rgiWarningIDs);
    *pWarnIDs = m_pData->m_rgiWarningIDs;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::GetWarnInfo

STDMETHODIMP CCompilerConfig::GetWarnInfo (long iWarnIndex, PCWSTR *ppszWarningDescription)
{
    if (iWarnIndex < 0 || iWarnIndex >= (int)lengthof(m_pData->m_rgiWarningIDs) || !ppszWarningDescription)
        return E_INVALIDARG;

    WARNINGDEF  *pDef = COptionData::m_rgWarningInfo + iWarnIndex;

    // Make sure the description string is loaded
    if (pDef->bstrDesc == NULL && (pDef->iWarnDescID != -1))
    {
        HRESULT hr;
        if (FAILED(hr = COptionData::LoadCachedBSTR(&pDef->bstrDesc, pDef->iWarnDescID)))
            return hr;
    }

    *ppszWarningDescription = pDef->bstrDesc;
    if (*ppszWarningDescription == NULL)
        *ppszWarningDescription = L"";

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::ReportOptionError -- report an error with an option.

HRESULT CCompilerConfig::ReportOptionErrorArgs(ICSError **ppError, int id, int carg, ErrArg * prgarg)
{
    HRESULT     hr = S_OK;
    CError      *pError;

    if (*ppError != NULL) {
        (*ppError)->Release();
        (*ppError) = NULL;
    }

    // Create an arg array manually using the type information in the ErrArgs.
    PCWSTR * prgpsz = STACK_ALLOC(PCWSTR, carg);
    PCWSTR * ppsz = prgpsz;

    for (ErrArg * parg = prgarg; parg < prgarg + carg; parg++) {
        ConvertAndCleanArgToStack(parg, ppsz);
        ppsz++;
    }

    //EDMAURER Not sure why, but CError is returned with refcnt = 0. So if QI fails,
    //AddRef/Release to get this object to go away.
    if (SUCCEEDED (hr = m_pController->CreateError(id, prgpsz, &pError)))
    {
        if (pError && FAILED (hr = pError->QueryInterface (IID_ICSError, (void **)ppError)))
        {
            pError->AddRef ();
            pError->Release ();
        }
    }
    return hr;
}

HRESULT CCompilerConfig::ReportOptionError(ICSError **ppError, int id, ErrArg a)           {                           return ReportOptionErrorArgs(ppError, id, 1, &a); }
HRESULT CCompilerConfig::ReportOptionError(ICSError **ppError, int id, ErrArg a, ErrArg b) { ErrArg args[] = { a, b }; return ReportOptionErrorArgs(ppError, id, 2, args); }

//EDMAURER manage the one error object. It should point to the first error found.
//If there is no error, then point to the first warning. Construct an error object
//for use in the comparison of warning/error. Construction may demote an error to a warning.

void SetErrorIfWorse (ICSError** curerr, ICSError** nuerr)
{
    bool replace;

    replace = true;

    if (*curerr)
    {
        ERRORKIND ckind;
        ERRORKIND nukind;
        if (S_OK == (*curerr)->GetErrorInfo (NULL, &ckind, NULL) && S_OK == (*nuerr)->GetErrorInfo (NULL, &nukind, NULL))
            replace = (ERROR_WARNING == ckind && ERROR_ERROR == nukind);
        else
            replace = false;
    }

    if (!replace)
        (*nuerr)->Release ();
    else
    {
        if (*curerr)
            (*curerr)->Release ();

        *curerr = *nuerr;
    }

    *nuerr = NULL;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::CommitChanges
//
// Note: to fix VS7:545705, this function has the semantics that it always
// commits changes, even if an error occurs. If it returns S_FALSE,
// one or more options were incorrect, and ICSError has a message about
// one of them. If there are errors and warnings, this message will refer to 
// an error. All correct options are committed anyway.

//EDMAURER Returns  S_OK:       no warnings or errors
//                  S_FALSE:    warning or error was generated and contents 
//                              of [out] parameter are valid.
//                  FAILED ():  There was a problem constructing the error object 
//                              or worse. Don't proceed.

STDMETHODIMP CCompilerConfig::CommitChanges (ICSError **ppError)
{
    //EDMAURER return the worst error of all. 
    HRESULT     hr = S_OK;

    CComBSTR    &pW = m_pData->m_sbstrWARNINGLEVEL;
    CComBSTR    &pDebugType = m_pData->m_sbstrDEBUGTYPE;
    CComBSTR    &pPlatform = m_pData->m_sbstrPLATFORM;
    CComBSTR    &pCompatMode = m_pData->m_sbstrCOMPATIBILITY;

    *ppError = NULL;

    ICSError* possible_error = NULL;
    
    // Before committing, we must verify that the options that have been set so
    // far don't "conflict".  Also, certain string options that should boil down
    // to a numeric range (for example) is checked here (rather than in
    // SetOption, because we have a way to describe the error).
    if (pW == NULL || wcslen (pW) != 1 || pW[0] < '0' || pW[0] > '4')
    {
        hr = ReportOptionError(ppError, ERR_BadWarningLevel);

        pW = L"4";  // default value on error.
    }

    m_pData->warnLevel = pW[0] - '0';

    if (pDebugType == NULL || wcslen(pDebugType) == 0 || _wcsicmp(pDebugType, L"full") == 0)
        m_pData->pdbOnly = false;
    else if (_wcsicmp(pDebugType, L"pdbonly") == 0)
        m_pData->pdbOnly = true;
    else {
        if (!FAILED (hr) && (S_OK == (hr = ReportOptionError(&possible_error, ERR_BadDebugType, (BSTR)pDebugType))))
            SetErrorIfWorse (ppError, &possible_error);

        m_pData->pdbOnly = false;
        pDebugType.Empty();
    }

    if (pPlatform == NULL || wcslen(pPlatform) == 0 || _wcsicmp(pPlatform, L"anycpu") == 0)
        m_pData->m_platform = platformAgnostic;
    else if (_wcsicmp(pPlatform, L"x86") == 0)
        m_pData->m_platform = platformX86;
    else if (_wcsicmp(pPlatform, L"itanium") == 0)
        m_pData->m_platform = platformIA64;
    else if (_wcsicmp(pPlatform, L"x64") == 0)
        m_pData->m_platform = platformAMD64;
    else {
        if (!FAILED (hr) && (S_OK == (hr = ReportOptionError (&possible_error, ERR_BadPlatformType, (BSTR)pPlatform))))
            SetErrorIfWorse (ppError, &possible_error);

        m_pData->m_platform = platformAgnostic;
        pPlatform.Empty();
    }

    if (pCompatMode == NULL || wcslen(pCompatMode) == 0)
        m_pData->compatMode = CompatibilityNone;
#define COMPATNAME( name, text) \
    else if ( _wcsicmp(pCompatMode, text) == 0) \
        m_pData->compatMode = Compatibility ## name ;
#define COMPATDEF( name, value, text) \
    else if ( _wcsicmp(pCompatMode, text) == 0) \
        m_pData->compatMode = Compatibility ## name ;
#include "compat.h"
    else
    {
        if (!FAILED (hr) && (S_OK == (hr = ReportOptionError (&possible_error, ERR_BadCompatMode, (BSTR)pCompatMode))))
            SetErrorIfWorse (ppError, &possible_error);
    }

    // Validate and set the "no warn" list. Use wcstok_s on a copy of the option,
    // because wcstok_s modifies the string.
    if (m_pData->m_sbstrNOWARNLIST.Length() > 0) {
        PWSTR noWarnText;
        PWSTR warnNumberStr, endPtr;
        unsigned long warnNumber;
        unsigned short * warnNumberList;
        int cWarnings;
        const WCHAR delimiters[] = { ' ', ',', ';', '\0' };  // delimiters in the warning list.

        int cchNoWarnList = m_pData->m_sbstrNOWARNLIST.Length() + 1;
        noWarnText = STACK_ALLOC(WCHAR, cchNoWarnList);
        warnNumberList = STACK_ALLOC(unsigned short, cchNoWarnList); // Can't have more warnings than the length of the string
        StringCchCopyW(noWarnText, cchNoWarnList, m_pData->m_sbstrNOWARNLIST);

        // First count how many numbers, and validate them.
        cWarnings = 0;  
        WCHAR *pchContext = NULL;
        for (warnNumberStr = wcstok_s(noWarnText, delimiters, &pchContext); warnNumberStr; warnNumberStr = wcstok_s(NULL, delimiters, &pchContext))
        {
            warnNumber = wcstoul(warnNumberStr, &endPtr, 10);
            if ((endPtr != warnNumberStr + wcslen(warnNumberStr)) || ! IsValidWarningNumber(warnNumber)) {
                if (!FAILED (hr) && (S_OK == (hr = ReportOptionError (&possible_error, WRN_BadWarningNumber, warnNumberStr))))
                    SetErrorIfWorse (ppError, &possible_error);
            }
            else {
                warnNumberList[cWarnings] = (unsigned short) warnNumber;
                ++cWarnings;
            }
        }

        // Now alloc the array of warning numbers, which is stored in a BSTR for convenience sake.
        m_pData->noWarnNumbers = CComBSTR(cWarnings, (OLECHAR *)warnNumberList);
    }
    else {
        m_pData->noWarnNumbers.Empty();
    }

    if (m_pData->unknownTestSwitch.Length() > 0)
    {
        if (!FAILED (hr) && (S_OK == (hr = ReportOptionError (&possible_error, ERR_UnknownTestSwitch, (PCWSTR) m_pData->unknownTestSwitch))))
            SetErrorIfWorse (ppError, &possible_error);

        m_pData->unknownTestSwitch.Empty();
    }

    WCHAR canon[MAX_PATH];


    if (m_pData->m_sbstrKEYFILE != NULL)
    {
        if (!GetCanonFilePath(m_pData->m_sbstrKEYFILE, WCBuffer(canon), false)) {
            if (!FAILED (hr) && (S_OK == (hr = ReportOptionError (&possible_error, FTL_InputFileNameTooLong, (PCWSTR)m_pData->m_sbstrKEYFILE))))
                SetErrorIfWorse (ppError, &possible_error);

            m_pData->m_sbstrKEYFILE.Empty();
        }
        else
            m_pData->m_sbstrKEYFILE = canon;
    }

    if(m_pData->m_sbstrCCSYMBOLS != NULL)
    {
        // Verify that all CCSYMBOLS are identifiers
        CComBSTR    sbstr(m_pData->m_sbstrCCSYMBOLS);

        // Lexical Grammar here is:
        // token
        //      spaces-opt Id spaces-opt 
        // separator
        //     ; ,
        // CC-List
        //     token
        //     token separator CC-List
        //
        PWSTR pszTokenStart = sbstr;
        PWSTR pszStringEnd = pszTokenStart + wcslen(pszTokenStart);
        while (pszTokenStart < pszStringEnd)
        {
            // end of current token
            PWSTR pszTokenEnd = wcspbrk(pszTokenStart, L";,");
            if (pszTokenEnd == NULL)
                pszTokenEnd = pszTokenStart + wcslen(pszTokenStart);

            // trim whitespace to get to id
            PWSTR pszIdStart = pszTokenStart;
            while (*pszIdStart == L' ' && pszIdStart < pszTokenEnd)
                pszIdStart ++;
            PWSTR pszIdEnd = pszTokenEnd;
            while (pszIdEnd > pszIdStart && pszIdEnd[-1] == L' ')
                pszIdEnd --;

            // report error for missing id or invalid id
            if (pszIdStart == pszIdEnd || !m_pController->GetNameMgr()->IsValidIdentifier(pszIdStart, (int)(pszIdEnd - pszIdStart), m_pData->compatMode, CheckIdentifierFlags::Simple))
            {
                *pszIdEnd = L'\0';
                if (!FAILED (hr) && (S_OK == (hr = ReportOptionError (&possible_error, WRN_DefineIdentifierRequired, pszIdStart))))
                    SetErrorIfWorse (ppError, &possible_error);

                // Check for bad options in original data when turning these strings into defines later.
            }

            // next token
            pszTokenStart = pszTokenEnd + 1;
        }
    }


    // No error -- commit the change to the controller (which will fire the
    // notifications to the host if actual changes are made).
    m_pController->SetConfiguration (m_pData);


    if (!FAILED (hr))
    {
        if (*ppError)
            hr = S_FALSE;
        else //'hr' could have been set to S_FALSE by ReportOptionError () and there is no error object.
            hr = S_OK;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CCompilerConfig::GetCompiler

STDMETHODIMP CCompilerConfig::GetCompiler (ICSCompiler **ppCompiler)
{
    *ppCompiler = m_pController;
    (*ppCompiler)->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CController::SetConfiguration

void CController::SetConfiguration (COptionData *pData)
{
    CTinyGate gate(&m_lockOptions);
    BOOL    rgfChangedIndex[TOTAL_OPTIONS];

    // The configuration has already been verified.  What we need to do is
    // perform a "diff" of each option as we copy them over, sending the
    // OnConfigChange event back to the host for each one that's different.
    // First, do a comparison, setting corresponding entries in rgfChangedIndex
    // for differing option values.
    long i;
    for (i=0; i<TOTAL_OPTIONS; i++)
    {
        OPTIONDEF   *pDef = COptionData::GetOptionDef(i);
        void        *pNewMember = (void *)(((BYTE *)pData) + pDef->iOffset);
        void        *pCurMember = (void *)(((BYTE *)&m_OptionData) + pDef->iOffset);

        if (pDef->dwFlags & COF_BOOLEAN)
        {
            rgfChangedIndex[i] = (!*((BOOL *)pCurMember) != !*((BOOL *)pNewMember));
        }
        else
        {
            PCWSTR  pszNew = *((CComBSTR *)pNewMember), pszCur = *((CComBSTR *)pCurMember);

            if ((pszNew == NULL || pszNew[0] == 0) && (pszCur == NULL || pszCur[0] == 0))
                rgfChangedIndex[i] = FALSE;     // Both null/empty -- no change
            else if (pszNew != NULL && pszCur != NULL && wcscmp (pszNew, pszCur) == 0)
                rgfChangedIndex[i] = FALSE;     // They're the same
            else
                rgfChangedIndex[i] = TRUE;      // They're different
        }
    }

    // Okay, copy over the goods
    m_OptionData = *pData;

    // Now, fire the events
    for (i=0; i<TOTAL_OPTIONS; i++)
    {
        if (rgfChangedIndex[i])
        {
            VARIANT vt;
            OPTIONDEF   *pDef = COptionData::GetOptionDef(i);
            void        *pMember = (void *)(((BYTE *)&m_OptionData) + pDef->iOffset);

            if (pDef->dwFlags & COF_BOOLEAN)
            {
                V_VT(&vt) = VT_BOOL;
                V_BOOL(&vt) = !!(*((BOOL *)pMember)) ? VARIANT_TRUE : VARIANT_FALSE;
                m_spHost->OnConfigChange (pDef->iOptionID, vt);
            }
            else
            {
                V_VT(&vt) = VT_BSTR;
                V_BSTR(&vt) = SysAllocString(*((CComBSTR *)pMember));
                m_spHost->OnConfigChange (pDef->iOptionID, vt);
                SysFreeString(V_BSTR(&vt));
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
// COMPILER::AddInputSet

HRESULT COMPILER::AddInputSet (CInputSet *pSet)
{
    HRESULT     hr = S_OK;

    PAL_TRY {
        PAL_TRY {
            hr = AddInputSetWorker(pSet);
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

HRESULT COMPILER::AddInputSetWorker (CInputSet *pSet)
{
    size_t            iSources = pSet->GetSourceTable()->Count();
    size_t            iResources = pSet->GetResourceTable()->Count();
    INFILESYM       *pInFile = NULL;
    OUTFILESYM      *pOutFile = NULL;

    CSourceFileInfo **ppSrcFiles = STACK_ALLOC(CSourceFileInfo *, iSources);
    CResourceFileInfo **ppResFiles = STACK_ALLOC(CResourceFileInfo *, iResources);

    if (iSources == 0 && pSet->GetOutputName() == NULL) {
        Error( NULL, ERR_OutputNeedsName);
        return S_FALSE;
    }

    //
    // create the output file
    //
    pOutFile = getCSymmgr().CreateOutFile(
                    pSet->GetOutputName(),
                    pSet->DLL(),
                    pSet->WinApp(),
                    pSet->GetMainClass(),
                    pSet->GetWin32Resource(),
                    pSet->GetWin32Icon(),
                    pSet->GetPDBFile());

    pOutFile->imageBase = pSet->ImageBase();
    pOutFile->fileAlign = pSet->FileAlignment();
    pOutFile->isManifest = !!pSet->MakeAssembly();

    m_fAssemble |= pOutFile->isManifest;

    // Get a module id for this output file
    pOutFile->SetModuleID(getBSymmgr().AidAlloc(pOutFile));

    //
    // create the input files
    //
    if (! pSet->CopySources (ppSrcFiles, iSources))
        return E_UNEXPECTED;    // Someone must have changes the inputset asyncronously!
        
    for (size_t i = 0; i < iSources; i++)
    {
        pInFile = getCSymmgr().CreateSourceFile (ppSrcFiles[i]->m_pName->text, pOutFile);
    }

    RESFILESYM      *pResFile;
    if (!pSet->CopyResources (ppResFiles, iResources))
        return E_UNEXPECTED;    // Someone must have changes the inputset asyncronously!

    for (size_t i = 0; i < iResources; i++)
    {
        const WCHAR *szName = ppResFiles[i]->m_pFileName->text;
        const WCHAR *szIdent = ppResFiles[i]->m_pIdent->text;

        if (ppResFiles[i]->m_fEmbed)
        {
            pResFile = getCSymmgr().CreateEmbeddedResFile (szName, szIdent, !!ppResFiles[i]->m_fVisible);
        }
        else
        {
            OUTFILESYM *pOutFile = getCSymmgr().CreateOutFile(
                                    szName,
                                    true,
                                    false,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL);
            pOutFile->isResource = true;

            pResFile = getCSymmgr().CreateSeperateResFile (szName, pOutFile, szIdent, !!ppResFiles[i]->m_fVisible);
        }
        if ( pResFile == NULL)
            Error( NULL, ERR_ResourceNotUnique, szIdent);
    }

    return S_OK;
}

