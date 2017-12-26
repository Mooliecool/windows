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
// File: main.cpp
// 
// Reads and verifies PE File Format and COM+ header using the PEverf* classes
// ===========================================================================
#include "stdpch.h"
#include "utilcode.h"
#include <time.h>
#include <limits.h>
#include <mscoree.h>
#include <corhost.h>
#include <corhlpr.h>
#include "peverify.h"
#include "malloc.h"
#include "ivehandler.h"
#include "ivalidator.h"
#include "resource.h"
#include "sstring.h"
#include <ex.h>

#include <palstartupw.h>

#undef WszLoadString
#define WszLoadString   PAL_LoadSatelliteStringW
HSATELLITE      g_hResourceInst = NULL;

#define WSZ_MSCORLIB L"mscorlib.dll"
#define SZ_MSCORLIB   "mscorlib.dll"
#define LEN_MSCORLIB ((sizeof(SZ_MSCORLIB) / sizeof(CHAR)) - 1)

// Actions
#define PEVERIFY_IL         0x001
#define PEVERIFY_METADATA   0x002
#define PEVERIFY_IL_ANYWAY  0x004
#define PEV_ACTION_MASK     (PEVERIFY_IL | PEVERIFY_METADATA | PEVERIFY_IL_ANYWAY)

// Flags
#define PEVERIFY_VERBOSE    0x008
#define PEVERIFY_NOSYMBOLS  0x010

#define MAKE_WIDE(ptrname, utf8str) \
    long __l##ptrname; \
    LPWSTR ptrname; \
    __l##ptrname = MultiByteToWideChar(CP_ACP, 0, utf8str, -1, 0, 0); \
    ptrname = (LPWSTR) _alloca(__l##ptrname*sizeof(WCHAR));  \
    MultiByteToWideChar(CP_ACP, 0, utf8str, -1, ptrname, __l##ptrname); 

#define MAKE_ANSI(ptrname, wstr) \
    long __l##ptrname; \
    LPSTR ptrname; \
    __l##ptrname = WideCharToMultiByte(CP_ACP, 0, wstr, -1, 0, 0, NULL, NULL); \
    ptrname = (LPSTR) _alloca(__l##ptrname);  \
    if (WideCharToMultiByte(CP_ACP, 0, wstr, -1, ptrname, __l##ptrname, NULL, NULL) != __l##ptrname) \
        memset( ptrname, 0, __l##ptrname );

bool g_bQuiet;
bool g_bNoCLS;
bool g_bCLSOnly;
bool g_bUniqueOnly;
bool g_bShowHres;
bool g_bClock;
bool g_bSymbols;
unsigned g_uMaxErrors;

unsigned g_uErrorCount;
unsigned g_uWarningCount;

#define HR_TABLE_SIZE 16384
HRESULT  g_HR[HR_TABLE_SIZE];
HRESULT  g_HRignore[HR_TABLE_SIZE];

unsigned g_uIgnoreCount;

HRESULT MDWarning[] = {
    VLDTR_E_TR_HASTYPEDEF,
    VLDTR_E_TD_DUPGUID,
    VLDTR_E_TD_IFACEGUIDNULL,
    VLDTR_E_TD_ENUMNOVALUE,
    VLDTR_E_TD_ENUMNOLITFLDS,
    VLDTR_E_TD_EXTTYPESPEC,
    VLDTR_E_MR_DUP,
    VLDTR_E_MR_VARARGCALLINGCONV,
    VLDTR_E_MODREF_DUP,
    VLDTR_E_TD_EXTTRRES,
    VLDTR_E_MD_MULTIPLESEMANTICS,
    VLDTR_E_MD_PARAMOUTOFSEQ,
    VLDTR_E_FMD_GLOBALITEM,
    VLDTR_E_FD_FLDINIFACE,
    VLDTR_E_FMD_BADIMPLMAP,
    VLDTR_E_AS_HASHALGID,
    VLDTR_E_AS_BADLOCALE,
    VLDTR_E_AR_PROCID,
    VLDTR_E_CT_NOTYPEDEFID,
    VLDTR_E_IFACE_METHNOTIMPLTHISMOD
};
unsigned Nmdw = sizeof(MDWarning)/sizeof(HRESULT);
bool IsMdWarning(HRESULT hr)
{
    for(unsigned i = 0; i < Nmdw; i++) if(hr == MDWarning[i]) return true;
    return false;
}


#define MESSAGE_DLL L"peverify.satellite"

static HSATELLITE LoadSatelliteResource()
{
    WCHAR path[MAX_PATH];
    WCHAR * pEnd;
 
    if (!GetModuleFileNameW(NULL, path, MAX_PATH))
        return 0;
 
#ifndef PLATFORM_UNIX
    pEnd = wcsrchr(path, L'\\');
#else
    pEnd = wcsrchr(path, L'/');
#endif
    if (!pEnd)
        return 0;
    ++pEnd;  // point just beyond.
 
    // Append message DLL name.
    if ((int) sizeof(MESSAGE_DLL) + pEnd - path > (int) sizeof(path) - 1)
        return 0;
    wcscpy(pEnd, MESSAGE_DLL);
 
    return PAL_LoadSatelliteResourceW(path);
}
 

void Output(__in_z const WCHAR* pwszOutput)
{
    wprintf(pwszOutput);
}

void rprintf(DWORD dwResourceID, ...)
{
    EX_TRY
    {
        // Load resource string
        WCHAR wszFormat[512];
        if(WszLoadString(g_hResourceInst, dwResourceID, wszFormat, 511))
        {
            wszFormat[511] = L'\0';

            // Format the other args
            va_list args;
            va_start(args, dwResourceID);
            SString s;
            s.VPrintf(wszFormat, args);
            va_end(args);

            // Print the string
            Output(s.GetUnicode());
        }
        else
        {
            Output(L"\nfailed to load resource string\n");
        }
    }
    EX_CATCH
    {
        Output(L"\nError formatting output\n");
    }
    EX_END_CATCH(SwallowAllExceptions);
}

class CVEHandler :
    public IVEHandler
{
private:
    ICLRValidator  *m_val;
public:
    CVEHandler() : m_val(0) {}
    void SetValidator(ICLRValidator *val) { m_val = val; }

    // *** IUnknown methods ***
    STDMETHODIMP_(ULONG) AddRef(void)
    { return E_NOTIMPL; }
    STDMETHODIMP_(ULONG) Release(void)
    { return E_NOTIMPL; }
    STDMETHODIMP    QueryInterface(REFIID riid, void** ppv) 
    { return E_NOTIMPL; }

    HRESULT STDMETHODCALLTYPE VEHandler( 
        /* [in] */ HRESULT VECode,
        /* [in] */ VEContext Context,
        /* [in] */ SAFEARRAY __RPC_FAR * psa)
    {
        for(unsigned i=0; i < g_uIgnoreCount; i++)
        {
            if(g_HRignore[i] == VECode) return S_OK;
        }
        if(g_bUniqueOnly)
        {
            int i;
            for(i=0; (i < HR_TABLE_SIZE) && g_HR[i]; i++)
            {
                if(g_HR[i] == VECode) return S_OK;
            }

	    if (i < HR_TABLE_SIZE) {
                g_HR[i] = VECode;
	    }
        }
        ++g_uErrorCount;
        
        if(g_bQuiet) return S_OK;

        if (m_val)
        {
#define MSG_LEN 1000
            WCHAR msg[MSG_LEN + 1];

            if (SUCCEEDED(m_val->FormatEventInfo(VECode, Context, 
                    &msg[0], MSG_LEN, psa)))
            {
                if(!g_bSymbols)
                {
                    rprintf(VER_IL);
                    rprintf(VER_ERROR1);
                    fflush(stdout);
                }
                Output(msg);

                // show hresult
                if(g_bShowHres)
                    rprintf(VER_ERROR2, VECode);

                Output(L"\n");
                fflush(stdout);
                return g_uErrorCount >= g_uMaxErrors ? E_FAIL : S_OK;
            }
        }

        return E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE SetReporterFtn( 
        /* [in] */ __int64 lFnPtr)
    {
        return E_NOTIMPL;
    }
};



//-----------------------------------------------------------------------------
// CompactMsg
// suitable for scripts
//-----------------------------------------------------------------------------
void CompactMsg(__in_z WCHAR *pwszFilename, unsigned uNumErrors)
{
    Output(pwszFilename);

    if (uNumErrors == 0)
        rprintf(VER_PASS);
    else
        rprintf(VER_FAIL, uNumErrors);
}

//-----------------------------------------------------------------------------
// Error/warning reporting function, replacing the standard one of IVEHandler
//-----------------------------------------------------------------------------
HRESULT PEVerifyReporter( // Return status.
    LPCWSTR     szMsg,                  // Error message.
    VEContext   Context,                // Error context (offset,token)
    HRESULT     hr)                     // Original HRESULT
{
    for(unsigned i=0; i < g_uIgnoreCount; i++)
    {
        if(g_HRignore[i] == hr) return S_OK;
    }

    if(g_bUniqueOnly)
    {
        int i;
        for(i=0; (i < HR_TABLE_SIZE) && g_HR[i]; i++)
        {
            if(g_HR[i] == hr) return S_OK;
        }

	if (i < HR_TABLE_SIZE) {
            g_HR[i] = hr;
	}
    }
    if(wcsstr(szMsg,L"[CLS]"))
    {
        if(g_bNoCLS) return S_OK; // ignore CLS warnings
    }
    else
    {
        if(g_bCLSOnly) return S_OK; // ignore everything but CLS warnings
    }
    if(!g_bQuiet)
    {
        if(szMsg)
        {
            rprintf(VER_MD);
            if(g_bShowHres)
                rprintf(VER_HEX, hr);
            Output(L": ");
            fflush(stdout);
            Output(szMsg);

            // include token and offset from Context
            if(Context.Token)
                rprintf(VER_TOKEN, Context.Token);
            if(Context.uOffset)
                rprintf(VER_AT, Context.uOffset);
            Output(L"\n");
        }
    }
    if(IsMdWarning(hr)) g_uWarningCount++;
    else g_uErrorCount++;
    return (g_uErrorCount >= g_uMaxErrors) ? E_FAIL : S_OK;
}

#ifndef EXTERNAL_VE_HANDLER_FOR_MD_VALIDATION

typedef HRESULT (*REPORTFCTN)(LPCWSTR, VEContext, HRESULT);

class MDVEHandlerClass : public IVEHandler
{
public: 
    LONG        m_refCount;
    REPORTFCTN  m_fnReport;

    MDVEHandlerClass() { m_refCount=0; m_fnReport=PEVerifyReporter; };
    virtual ~MDVEHandlerClass() { };

    //-----------------------------------------------------------
    // IUnknown support
    //-----------------------------------------------------------
    HRESULT STDMETHODCALLTYPE    QueryInterface(REFIID id, void** pInterface) 
    {
        if (id == IID_IVEHandler)
            *pInterface = (IVEHandler*)this;
        else if (id == IID_IUnknown)
            *pInterface = (IUnknown*)(IVEHandler*)this;
        else
        {
            *pInterface = NULL;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() 
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() 
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) delete this;
        return (refCount);
    }

    //-----------------------------------------------------------
    // IVEHandler support
    //-----------------------------------------------------------
    HRESULT STDMETHODCALLTYPE   SetReporterFtn(__int64 lFnPtr)
    {
        m_fnReport = lFnPtr ? reinterpret_cast<REPORTFCTN>(lFnPtr) 
                             : PEVerifyReporter;
        return S_OK;
    };

//*****************************************************************************
// The Verification Event Handler itself. Declared in VEHandler.h as virtual, may be overridden
//*****************************************************************************
    HRESULT STDMETHODCALLTYPE VEHandler(HRESULT hrRpt, VEContext Context, SAFEARRAY *psa)
    {
    // The following code is copied from Utilcode\PostError.cpp with minor additions
        WCHAR       rcBuf[1024];             // Resource string.
        WCHAR       rcMsg[1024];             // Error message.
        BYTE       *marker;                  // User text.
        HRESULT     hr;
        VARIANT    *pVar;
        ULONG       nVars,i,k;
        BYTE       *pval;
        WCHAR      *pWsz[1024], *pwsz; // is more than 1024 string arguments likely?

        // Return warnings without text.
        if (!FAILED(hrRpt))
            return (hrRpt);

        memset(pWsz,0,sizeof(pWsz));
        // Convert safearray of variants into va_list
        if(psa && (nVars = psa->rgsabound[0].cElements))
        {
            _ASSERTE(psa->fFeatures & FADF_VARIANT);
            _ASSERTE(psa->cDims == 1);
            marker = new BYTE[nVars*sizeof(double)]; // double being the largest variant element
            if (marker == NULL)
                return E_FAIL;
            for(i=0,pVar=(VARIANT *)(psa->pvData),pval=marker; i < nVars; pVar++,i++)
            {
                switch(V_VT(pVar))
                {
                    case VT_I1:
                        *(int *)pval = V_I1(pVar);
                        pval += sizeof(int);
                        break;
                        
                    case VT_UI1:
                        *(int *)pval = V_UI1(pVar);
                        pval += sizeof(int);
                        break;
                        

                    case VT_I2:
                        *(int *)pval = V_I2(pVar);
                        pval += sizeof(int);
                        break;
                        
                    case VT_UI2:
                        *(int *)pval = V_UI2(pVar);
                        pval += sizeof(int);
                        break;
                    
                    case VT_I8:
                    case VT_UI8:
                        *(INT64 *)pval = V_I8(pVar);
                        pval += sizeof(INT64);
                        break;
                        

                    case VT_BYREF|VT_I1:
                    case VT_BYREF|VT_UI1: // it's ASCII string, convert it to UNICODE
                        {
                        PBYTE pb = V_UI1REF(pVar);
                        size_t len, m;
                        len = (ULONG32)strlen((char *)pb)+1;
                        pwsz = new WCHAR[len];
                        for(m=0; m<len; m++) pwsz[m] = pb[m];
                        for(k=0; pWsz[k]; k++);
                        pWsz[k] = pwsz;
                        
                        *(WCHAR **)pval = pwsz;
                        pval += sizeof(WCHAR *);
                        break;
                        }

                    default:
                        *(int *)pval = V_I4(pVar);
                        pval += sizeof(int);
                        break;
                }
            }
        }
        else
            marker = NULL;

        // If this is one of our errors, then grab the error from the rc file.
        if (HRESULT_FACILITY(hrRpt) == FACILITY_URT)
        {
            hr = UtilLoadStringRC(LOWORD(hrRpt), rcBuf, NumItems(rcBuf), true);
            if (hr == S_OK)
            {
                // Format the error.
                if (marker != NULL)
                    _vsnwprintf_s(rcMsg, _countof(rcMsg), _TRUNCATE, rcBuf, (va_list) marker);
                else
                    _snwprintf_s(rcMsg, _countof(rcMsg), _TRUNCATE, L"%s", rcBuf);
            }
        }
        // Otherwise it isn't one of ours, so we need to see if the system can
        // find the text for it.
        else
        {
            if (WszFormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                    0, hrRpt, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    rcMsg, NumItems(rcMsg), 0))
            {
                hr = S_OK;

                // System messages contain a trailing \r\n, which we don't want normally.
                int iLen = lstrlenW(rcMsg);
                if (iLen > 3 && rcMsg[iLen - 2] == '\r' && rcMsg[iLen - 1] == '\n')
                    rcMsg[iLen - 2] = '\0';
            }
            else
                hr = HRESULT_FROM_WIN32(GetLastError());
        }
        if(marker) delete[] marker;

        // If we failed to find the message anywhere, then issue a hard coded message.
        if (FAILED(hr))
        {
            _snwprintf_s(rcMsg,_countof(rcMsg), _TRUNCATE, L".NET Framework Internal error: 0x%08x", hrRpt);
            //DEBUG_STMT(DbgWriteEx(rcMsg));
        }

        // delete WCHAR buffers allocated above (if any)
        for(k=0; pWsz[k]; k++) delete pWsz[k];

        return (m_fnReport(rcMsg, Context,hrRpt) == S_OK ? S_OK : E_FAIL);
    };

};

#endif // !EXTERNAL_VE_HANDLER_FOR_MD_VALIDATION


#define ERRORMAX 100
//-----------------------------------------------------------------------------
// VerifyPEformat (driver function for file format verification)
// returns true if the file correctly verifies
//-----------------------------------------------------------------------------
bool VerifyPEformat(__in_z WCHAR *pwszFilename, DWORD dwFlags, bool quiet)
{
    bool retval = false;
    bool bILverified = false;
    HRESULT hr = S_OK;
    DWORD dwValidatorFlags = 0;


    DWORD cBegin=0,cEnd=0,cMDBegin=0,cMDEnd=0,cMDVBegin=0,cMDVEnd=0,cILBegin=0,cILEnd=0,cILVBegin=0,cILVEnd=0;

    if(g_bClock) cBegin = GetTickCount();

    g_bQuiet = quiet;

    g_uErrorCount = 0;
    g_uWarningCount = 0;
    if (dwFlags & PEVERIFY_METADATA)
    { // Meta Data Validation Segment:
        IMetaDataDispenserEx *pDisp;
        IMetaDataImport *pImport;
        IMetaDataValidate *pValidate = 0;
#ifndef EXTERNAL_VE_HANDLER_FOR_MD_VALIDATION
        MDVEHandlerClass    *pVEHandler = 0;
#else
        IVEHandler  *pVEHandler = 0;
#endif
        if(g_bClock) cMDBegin = GetTickCount();

        g_uErrorCount = 1; // just in case we don't make it to ValidateMetaData
        if(SUCCEEDED(hr = CoCreateInstance(CLSID_CorMetaDataDispenser, 
                      NULL, CLSCTX_INPROC_SERVER, 
                      IID_IMetaDataDispenserEx, (void **) &pDisp)))
        {
            if(SUCCEEDED(hr = pDisp->OpenScope(pwszFilename,0,
                        IID_IMetaDataImport, (IUnknown**)&pImport)))
            {
                if(SUCCEEDED(hr=pImport->QueryInterface(IID_IMetaDataValidate, 
                    (void**) &pValidate)))
                {
#ifndef EXTERNAL_VE_HANDLER_FOR_MD_VALIDATION
                    if((pVEHandler = new MDVEHandlerClass()))
#else
                    if(SUCCEEDED(hr = CoCreateInstance(CLSID_VEHandlerClass,
                                                        NULL,
                                                        CLSCTX_INPROC_SERVER,
                                                        IID_IVEHandler,
                                                        (void **)&pVEHandler)))
#endif
                    {
                        pVEHandler->SetReporterFtn((__int64) (size_t) PEVerifyReporter);

                        if(SUCCEEDED(hr = pValidate->ValidatorInit(
                            ValidatorModuleTypePE,  pVEHandler)))
                        {
                            g_uErrorCount = 0;
                            if(g_bClock) cMDVBegin = GetTickCount();
                            hr = pValidate->ValidateMetaData();
                            if(g_bClock) cMDVEnd = GetTickCount();
                            retval = (g_uErrorCount == 0);
                        }
                        else
                            rprintf(VER_INIT_VALIDATOR_ERROR, hr);

#ifndef EXTERNAL_VE_HANDLER_FOR_MD_VALIDATION
                    }
                    else
                        rprintf(VER_CREATE_VEHANDLER_ERROR);
#else
                        pVEHandler->Release();
                    }
                    else
                        rprintf(VER_CCI_OF_IVEHANDLER_ERROR, hr);
#endif
                    pValidate->Release();
                }
                else
                    rprintf(VER_CANT_GET_IMDVALIDATE, hr);

                pImport->Release();
            }
            else
            {
                rprintf(VER_CANT_OPENSCOPEONMEM);
            }
            pDisp->Release();
        }
        else
            rprintf(VER_CANT_COCREATE_MDD, hr); 
        if(g_bClock) cMDEnd = GetTickCount();
    }  // End Of Meta Data Validation Segment
    fflush(stdout);
    if (((dwFlags & PEVERIFY_IL)&&(g_uErrorCount == 0))
        ||((dwFlags & PEVERIFY_IL_ANYWAY)&&(g_uMaxErrors > g_uErrorCount)))
    {
        CVEHandler  veh;
        ICLRRuntimeHost *pClrHost = NULL;
    
        bILverified = true;
        if(g_bClock) cILBegin = GetTickCount();

        // Here we are going to get the hosting interface to the runtime and get 
        // it to verify the methods.
        hr =  CoCreateInstance(CLSID_CLRRuntimeHost,
                NULL,CLSCTX_INPROC_SERVER,
                IID_ICLRRuntimeHost,
                (void**)&pClrHost);

        if (FAILED(hr)) {
            rprintf(VER_INST_ERR);
            goto Exit;
        }
    
        hr = pClrHost->Start();
    
        if(FAILED(hr)) {
            rprintf(VER_CANT_START_FRAMEWORK);
            goto Exit;
        }
    
#ifdef PEVERIFY_USE_CORHOST_
        LONG naryErrors[ERRORMAX];
        DWORD errorCount;
        
        errorCount = 0;
        ZeroMemory(naryErrors, sizeof(naryErrors));
    
        hr = pClrHost->
            VerifyAssemblyFile(pwszFilename, naryErrors, ERRORMAX, &errorCount);
        g_uErrorCount += errorCount;
#else
        ICLRValidator  *pVal;
        HANDLE      hFile, hMapping;
        PBYTE       pFile;
        DWORD       dwFileSize, dwHigh;

        pVal        = NULL;
        pFile       = NULL;
        hMapping    = NULL;
        hFile       = INVALID_HANDLE_VALUE;
        dwFileSize  = dwHigh = 0;

        hFile = WszCreateFile(
            pwszFilename,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_RANDOM_ACCESS,
            NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            hr = GetLastError();
            rprintf(VER_ERR_OPENING_FILE, pwszFilename, hr);
            g_uErrorCount++;
            goto exit_val;
        }

        // Get the file size
    
        dwFileSize = ::GetFileSize(hFile, &dwHigh);
    
        if ((dwFileSize == 0xFFFFFFFF) && (hr = ((GetLastError() != NO_ERROR)?E_FAIL:S_OK)))
        {
            rprintf(VER_CANT_GET_FILE_SIZE, pwszFilename, hr);
            g_uErrorCount++;
            goto exit_val;
        }
    
        if (dwHigh != 0)
        {
            hr = E_FAIL;
            rprintf(VER_FILE_TOO_BIG);
        }

        hMapping = WszCreateFileMapping(
            hFile,
            NULL,
            PAGE_READONLY,
            0,
            0,
            NULL);

        if (hMapping == NULL)
        {
            hr = GetLastError();
            rprintf(VER_CANT_CREATE_MAPPING, pwszFilename, hr);
            g_uErrorCount++;
            goto exit_val;
        }

        pFile = (PBYTE) MapViewOfFile(
            hMapping,
            FILE_MAP_READ,
            0,
            0,
            0);


        if (pFile == NULL)
        {
            hr = GetLastError();
            rprintf(VER_CANT_MAP_FILE, pwszFilename, hr);
            g_uErrorCount++;
            goto exit_val;
        }

        // CorHost requires full path.
        WCHAR wszFullPath[_MAX_PATH + 1];
        BOOL fFullName;

        fFullName = (BOOL) WszGetFullPathName(
                            pwszFilename,  _MAX_PATH,  wszFullPath,  NULL);

        wszFullPath[_MAX_PATH] = 0;

        hr = pClrHost->QueryInterface(IID_ICLRValidator, (void **)&pVal);

        if (FAILED(hr))
        {
            rprintf(VER_CANT_GET_IVALIDATOR, hr);
            g_uErrorCount++;
            goto exit_val;
        }

        // All error exits from here on will be to Exit_val:

        veh.SetValidator(pVal);
    
        // Set Validator Flags
        if(dwFlags & PEVERIFY_VERBOSE)
            dwValidatorFlags |= VALIDATOR_EXTRA_VERBOSE;
        if(dwFlags & PEVERIFY_NOSYMBOLS)
            g_bSymbols = false;
        else
        {
            dwValidatorFlags |= VALIDATOR_SHOW_SOURCE_LINES;
            g_bSymbols = true;
        }
        if(g_bClock) cILVBegin = GetTickCount();

        DWORD dwAppDomainId;
        hr = pClrHost->GetCurrentAppDomainId(&dwAppDomainId);
        if (FAILED(hr))
        {
            rprintf(VER_FAILED_TO_GET_DOMAIN_ID, hr);
            g_uErrorCount++;
            goto exit_val;
        }

        hr = pVal->Validate(
            &veh, 
            dwAppDomainId, 
            dwValidatorFlags, 
            g_uMaxErrors - g_uErrorCount, 
            0, 
            fFullName ? wszFullPath : pwszFilename, 
            pFile, 
            dwFileSize);

        if(g_bClock) cILVEnd = GetTickCount();

exit_val:
        if (pFile != NULL)
            UnmapViewOfFile(pFile);

        if (hMapping != NULL)
            CloseHandle(hMapping);

        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);

        if (pVal)
            pVal->Release();

#endif
Exit:
        if (pClrHost)
            pClrHost->Release();
        if(g_bClock) cILEnd = GetTickCount();
    }

    if (FAILED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            rprintf(VER_ERR_OPENING_FILE2);
        }

#ifdef PEVERIFY_USE_CORHOST_
        if(errorCount == 0)
        {
            if(bILverified)
            {
                if(!quiet)
                    rprintf(VER_IL_VERIF_FAILED, hr);
                g_uErrorCount++;
            }
        }
#endif
    }

    retval = (g_uErrorCount == 0);

    if (quiet) {
        CompactMsg(pwszFilename, g_uErrorCount);
    }
    else {

#ifdef PEVERIFY_USE_CORHOST_
        for(DWORD i = 0; i < errorCount; i++) {
            rprintf(VER_ERROR3, naryErrors[i]);
        }

        if (errorCount > 0)
            Output(L"\n");
#endif

        if(retval)
        {
            rprintf(VER_ALL_DONE1);
            Output(pwszFilename);
            rprintf(VER_ALL_DONE2);
        }
        else
        {
            rprintf(VER_ERROR4, (unsigned __int32)g_uErrorCount, (g_uErrorCount == 1) ? L"" : L"s");
            Output(pwszFilename);
            printf("\n");
        }
        if(g_uWarningCount)
            rprintf(VER_WARNINGS, g_uWarningCount);
    }


    if(g_bClock)
    {
        cEnd = GetTickCount();
        rprintf(VER_TIMING, (cEnd-cBegin));
        if(dwFlags & PEVERIFY_METADATA)
        {
            rprintf(VER_MD_CYCLE, (cMDEnd-cMDBegin));
            rprintf(VER_MD_PURE, (cMDVEnd-cMDVBegin));
        }
        if(bILverified)
        {
            rprintf(VER_IL_CYCLE, (cILEnd-cILBegin));
            rprintf(VER_IL_PURE, (cILVEnd-cILVBegin));
        }
    }

    return retval;
}



//-----------------------------------------------------------------------------
// Usage
//-----------------------------------------------------------------------------
//#define CLS_OPTIONS_ENABLED
void Usage()
{
    rprintf(VER_USAGE_01);
    rprintf(VER_USAGE_02);
    rprintf(VER_USAGE_03);
    rprintf(VER_USAGE_04);
    rprintf(VER_USAGE_07);
    rprintf(VER_USAGE_08);
    rprintf(VER_USAGE_09);
    rprintf(VER_USAGE_10);
    rprintf(VER_USAGE_11);
    rprintf(VER_USAGE_14);
    rprintf(VER_USAGE_15);
    rprintf(VER_USAGE_17);
    rprintf(VER_USAGE_18);
    rprintf(VER_USAGE_19);
    rprintf(VER_USAGE_20);
    Output(L"\n");
}


static FILE* PEVerifyFOpen( const WCHAR *filename, const WCHAR *mode )
{
    FILE* f;
    errno_t fError;

    
    f = NULL;

    {
        fError = _wfopen_s(&f, filename, mode);
        if (fError != 0) {
            f = NULL;
        }

    }

    return f;
}

//-----------------------------------------------------------------------------
// OutputLogo
//-----------------------------------------------------------------------------
void OutputLogo()
{
    rprintf(VER_PRODUCT_STR_L);
    Output(L" ");
    Output(VER_FILEVERSION_STR_L);
    Output(L"\n");
    Output(VER_LEGALCOPYRIGHT_LOGO_STR_L);
    Output(L"\n\n");
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
extern "C" int _cdecl wmain(int argc, __in_ecount(argc) WCHAR **argv)
{
    bool    quiet = false;
    bool    logo = true;
    DWORD   dwFlags = 0;
    int     filenameIndex = -1;
    WCHAR*  pch;
    WCHAR*  mscorlib;
    bool    fCheckMscorlib = true;
    size_t  offset;
    bool    fInvalidOption = false;

    if (!PAL_RegisterLibraryW(L"rotor_palrt"))
    {
        Output(L"Failed to register libraries\n");
        exit(1);
    }
 
    if (!(g_hResourceInst = LoadSatelliteResource()))
    {
        Output(L"Failed to load resource strings\n");
        exit(1);
    }

    OnUnicodeSystem();      // Init the Wsz Wrappers.

    g_bCLSOnly = false;
    g_bNoCLS = true;
    g_bUniqueOnly = false;
    g_bShowHres = false;
    g_bClock = false;
    g_uMaxErrors = 0xFFFFFFFF;
    memset(g_HR,0,sizeof(g_HR));
    memset(g_HRignore,0,sizeof(g_HRignore));
    g_uIgnoreCount = 0;

    for (int i=1; i<argc; ++i)
    {
        if ((*(argv[i]) != L'/') && (*(argv[i]) != L'-'))
            filenameIndex = i;        
        else if ((_wcsicmp(argv[i], L"/?") == 0) ||
                 (_wcsicmp(argv[i], L"-?") == 0))
        {
            OutputLogo();
            Usage();
            exit(0);
        }
        else if ((_wcsicmp(argv[i], L"/QUIET") == 0) ||
                 (_wcsicmp(argv[i], L"-QUIET") == 0))
                     quiet = true;
        else if ((_wcsicmp(argv[i], L"/nologo") == 0) ||
                 (_wcsicmp(argv[i], L"-nologo") == 0))
                     logo = false;
        else if ((_wcsicmp(argv[i], L"/IL") == 0) ||
                 (_wcsicmp(argv[i], L"-IL") == 0))
                     dwFlags |= PEVERIFY_IL | PEVERIFY_IL_ANYWAY;
        else if ((_wcsicmp(argv[i], L"/MD") == 0) ||
                 (_wcsicmp(argv[i], L"-MD") == 0))
                     dwFlags |= PEVERIFY_METADATA;
        else if ((_wcsicmp(argv[i], L"/verbose") == 0) ||
                 (_wcsicmp(argv[i], L"-verbose") == 0))
                     dwFlags |= PEVERIFY_VERBOSE;
        else if ((_wcsicmp(argv[i], L"/UNIQUE") == 0) ||
                 (_wcsicmp(argv[i], L"-UNIQUE") == 0))
                     g_bUniqueOnly = true;
        else if ((_wcsicmp(argv[i], L"/HRESULT") == 0) ||
                 (_wcsicmp(argv[i], L"-HRESULT") == 0))
                     g_bShowHres = true;
        else if ((_wcsicmp(argv[i], L"/CLOCK") == 0) ||
                 (_wcsicmp(argv[i], L"-CLOCK") == 0))
                     g_bClock = true;
        else if ((_wcsnicmp(argv[i], L"/BREAK",6) == 0) ||
                 (_wcsnicmp(argv[i], L"-BREAK",6) == 0))
        {
            g_uMaxErrors = 1;
            pch = wcschr(argv[i],L'=');
            if(pch)
            {
                int dummy;
                int nResultThatIDontCareAbout;
                nResultThatIDontCareAbout = swscanf_s( pch+1, L"%d", &dummy );
                if(dummy > 1) g_uMaxErrors=(unsigned)dummy;
            }
        }
        else if ((_wcsnicmp(argv[i], L"/IGNORE",7) == 0) ||
                 (_wcsnicmp(argv[i], L"-IGNORE",7) == 0))
        {
            pch = wcschr(argv[i],L'=');
            if(pch)
            {
                WCHAR* pch1 = pch;
                if(*(pch+1) == L'@')
                {
                    if(wcslen(pch+2))
                    {
                        FILE* pF = PEVerifyFOpen(pch+2,L"rt");
                        if(pF)
                        {
                            char sz[2048];
                            unsigned dummy;
                            while(!feof(pF))
                            {
                                memset(sz,0,2048);
                                char* pResultThatIDontCareAbout;
                                pResultThatIDontCareAbout = fgets(sz,2048,pF);
                                char *spch1 = &sz[0] - 1;
                                do
                                {
                                    char* spch = spch1+1;
                                    spch1 = strchr(spch, ',');
                                    if(spch1) *spch1 = 0;
                                    dummy = atoi( spch );
                                    if(!dummy)
                                    {
                                        int nResultThatIDontCareAbout;
                                        nResultThatIDontCareAbout = sscanf_s(spch,"%x",&dummy);
                                    }
                                    if(dummy && g_uIgnoreCount < HR_TABLE_SIZE)
                                    {
                                        g_HRignore[g_uIgnoreCount++] = dummy;
                                        //printf("0x%08x\n",g_HRignore[g_uIgnoreCount-1]);
                                    }
                                } while(spch1);
                            }
                            fclose(pF);
                        }
                        else
                            rprintf(VER_FAILED_TO_OPEN_FILE, pch+2);
                    }
                    else
                    {
                        rprintf(VER_INVALID_OPTION, argv[i]);
                        fInvalidOption = true;
                    }
                }
                else
                {
                    do
                    {
                        pch = pch1+1;
                        pch1 = wcschr(pch, ',');
                        if(pch1) *pch1 = 0;
                        int nResultThatIDontCareAbout;
                        nResultThatIDontCareAbout = swscanf_s(pch,L"%x",&g_HRignore[g_uIgnoreCount++]);
                        //printf("0x%08x\n",g_HRignore[g_uIgnoreCount-1]);
                    } while(pch1);
                }
            }
        }
        else
        {
#if PLATFORM_UNIX
            if ((*(argv[i]) == L'/'))
            {
              filenameIndex = i;        
            }
            else
#endif
            {
            rprintf(VER_INVALID_OPTION, argv[i]);
            fInvalidOption = true;
        }
    }
    }

    // Make sure we're verifying something
    if ((dwFlags & PEV_ACTION_MASK) == 0)
        dwFlags |= (PEVERIFY_METADATA | PEVERIFY_IL);

    if(!(dwFlags & PEVERIFY_METADATA))
    {
        g_bCLSOnly = false;
        g_bNoCLS = false;
    }
    if (!quiet && logo)
    {
        OutputLogo();
    }

    if (argc < 2 || filenameIndex < 1 || fInvalidOption)
    {
        Usage();
        exit(argc<2? 0:1);
    }
    fflush(stdout);
    // PEVerify cannot be used to verify some system libraries.
    // compare with ignorecase the last LEN_OF_MSCORLIB chars will be sufficient

    if ((dwFlags&PEVERIFY_IL) != 0)
    {
    	if (wcslen(argv[filenameIndex]) >= LEN_MSCORLIB)
  		{

	        offset = wcslen(argv[filenameIndex]) - LEN_MSCORLIB;
	    
	        if (offset >= 0)
	        {
	            mscorlib = argv[filenameIndex];
	    
	            if (offset > 0)
	            {
	                // check for files names that end with mscorlib.dll, 
	                // but not mscorlib.dll eg. MyMscorlib.dll
	                if ((mscorlib[offset - 1] != '.') &&
	                    (mscorlib[offset - 1] != '\\') &&
	                    (mscorlib[offset - 1] != '/'))
	                    fCheckMscorlib = false;
	    
	                // let my.mscorlib.dll go thru.., but prevent .mscorlib.dll
	                if ((mscorlib[offset - 1] == '.') && (offset != 1))
	                    fCheckMscorlib = false;
	    
	                mscorlib += offset;
	            }
	    
	            if (fCheckMscorlib && _wcsicmp(mscorlib, WSZ_MSCORLIB) == 0)
	            {
	                rprintf(VER_NO_SYSTEM_LIB, argv[filenameIndex]);
	                if(dwFlags & PEVERIFY_METADATA) dwFlags = PEVERIFY_METADATA;
	                else exit(1);
	            }
	        }
        }
    }
    fflush(stdout);
    if (!VerifyPEformat(argv[filenameIndex], dwFlags, quiet))
        exit(1);

    exit(0);        // return success
    return 0;
}

HINSTANCE GetModuleInst()
{
    return (NULL);
}
