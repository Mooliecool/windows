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

//============================================================
//
// Class: COMIsolatedStorage
//
// Created By: Shajan Dasan
// Contact: Tarik Soulami
//
// Purpose: Native Implementation of IsolatedStorage
//
// Date:  Feb 14, 2000
//
//============================================================

#include "common.h"
#include "excep.h"
#include "eeconfig.h"
#include "comstring.h"
#include "comstringcommon.h"    // RETURN()  macro
#include "comisolatedstorage.h"


#define IS_ROAMING(x)   ((x) & ISS_ROAMING_STORE)

void COMIsolatedStorage::ThrowISS(HRESULT hr)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    static MethodTable * pMT = NULL;

    if (pMT == NULL)
        pMT = g_Mscorlib.GetClass(CLASS__ISSEXCEPTION);

    _ASSERTE(pMT && "Unable to load the throwable class !");

    if ((hr >= ISS_E_ISOSTORE_START) && (hr <= ISS_E_ISOSTORE_END))
    {
        switch (hr)
        {
        case ISS_E_ISOSTORE :
        case ISS_E_OPEN_STORE_FILE :
        case ISS_E_OPEN_FILE_MAPPING :
        case ISS_E_MAP_VIEW_OF_FILE :
        case ISS_E_GET_FILE_SIZE :
        case ISS_E_CREATE_MUTEX :
        case ISS_E_LOCK_FAILED :
        case ISS_E_FILE_WRITE :
        case ISS_E_SET_FILE_POINTER :
        case ISS_E_CREATE_DIR :
        case ISS_E_CORRUPTED_STORE_FILE :
        case ISS_E_STORE_VERSION :
        case ISS_E_FILE_NOT_MAPPED :
        case ISS_E_BLOCK_SIZE_TOO_SMALL :
        case ISS_E_ALLOC_TOO_LARGE :
        case ISS_E_USAGE_WILL_EXCEED_QUOTA :
        case ISS_E_TABLE_ROW_NOT_FOUND :
        case ISS_E_DEPRECATE :
        case ISS_E_CALLER :
        case ISS_E_PATH_LENGTH :
        case ISS_E_MACHINE :
        case ISS_E_STORE_NOT_OPEN :
        case ISS_E_MACHINE_DACL :
            COMPlusThrowHR(hr);
            break;

        default :
            _ASSERTE(!"Unknown hr");
        }
    }

    COMPlusThrowHR(hr);
}

StackWalkAction COMIsolatedStorage::StackWalkCallBack(
        CrawlFrame* pCf, PVOID ppv)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    static MethodTable *s_pIsoStore = NULL;
    if (s_pIsoStore == NULL)
        s_pIsoStore = g_Mscorlib.GetClass(CLASS__ISS_STORE);

    static MethodTable *s_pIsoStoreFile = NULL;
    if (s_pIsoStoreFile == NULL)
        s_pIsoStoreFile = g_Mscorlib.GetClass(CLASS__ISS_STORE_FILE);

    static MethodTable *s_pIsoStoreFileStream = NULL;
    if (s_pIsoStoreFileStream == NULL)
        s_pIsoStoreFileStream = g_Mscorlib.GetClass(CLASS__ISS_STORE_FILE_STREAM);

    // Get the function descriptor for this frame...
    MethodDesc *pMeth = pCf->GetFunction();
    MethodTable *pMT = pMeth->GetMethodTable();

    // Skip the Isolated Store and all it's sub classes..

    if ((pMT == s_pIsoStore)     ||
        (pMT == s_pIsoStoreFile) ||
        (pMT == s_pIsoStoreFileStream))
    {
        LOG((LF_STORE, LL_INFO10000, "StackWalk Continue %s\n",
            pMeth->m_pszDebugMethodName));
        return SWA_CONTINUE;
    }

    *(PVOID *)ppv = pMeth->GetModule()->GetAssembly();

    return SWA_ABORT;
}

FCIMPL0(Object*, COMIsolatedStorage::GetCaller)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    OBJECTREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    Assembly *pAssem = NULL;

    if (StackWalkFunctions(GetThread(), StackWalkCallBack, (VOID*)&pAssem)
        == SWA_FAILED)
        ThrowISS(ISS_E_CALLER);

    if (pAssem == NULL)
        ThrowISS(ISS_E_CALLER);

    PREFIX_ASSUME(pAssem != NULL);
#ifdef _DEBUG
    LOG((LF_STORE, LL_INFO10000, "StackWalk Found %s\n", pAssem->GetSimpleName()));
#endif

    refRetVal = pAssem->GetExposedObject();

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(UINT64, COMIsolatedStorageFile::GetUsage, LPVOID handle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    UINT64 retVal = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();

    HRESULT hr      = S_OK;
    AccountingInfo  *pAI = (AccountingInfo*) handle;

    if (pAI == NULL)
        COMIsolatedStorage::ThrowISS(ISS_E_STORE_NOT_OPEN);

    PREFIX_ASSUME(pAI != NULL);

    hr = pAI->GetUsage(&retVal);

    if (FAILED(hr))
        COMIsolatedStorage::ThrowISS(hr);

    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

FCIMPL1(void, COMIsolatedStorageFile::Close, LPVOID handle)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    AccountingInfo *pAI = (AccountingInfo*) handle;

    if (pAI != NULL)
        delete pAI;

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL2(void, COMIsolatedStorageFile::Lock, LPVOID handle, CLR_BOOL fLock)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    AccountingInfo *pAI = (AccountingInfo*) handle;

    _ASSERTE(pAI);

    if (fLock)
        AccountingInfo::AcquireLock(pAI);
    else
        AccountingInfo::ReleaseLock(pAI);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL2(LPVOID, COMIsolatedStorageFile::Open, StringObject* fileNameUNSAFE, StringObject* syncNameUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    LPVOID retVal = NULL;
    STRINGREF fileName = (STRINGREF) fileNameUNSAFE;
    STRINGREF syncName = (STRINGREF) syncNameUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_2(fileName, syncName);

    HRESULT hr;
    AccountingInfo *pAI = new AccountingInfo(fileName->GetBuffer(), syncName->GetBuffer());

    hr = pAI->Init();

    if (FAILED(hr))
        COMIsolatedStorage::ThrowISS(hr);

    retVal = pAI;

    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND

FCIMPL4(void, COMIsolatedStorageFile::Reserve, LPVOID handle, UINT64* pqwQuota, UINT64* pqwReserve, CLR_BOOL fFree)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    HRESULT   hr;
    AccountingInfo *pAI = (AccountingInfo*) handle;

    if (pAI == NULL)
        COMIsolatedStorage::ThrowISS(ISS_E_STORE_NOT_OPEN);

    PREFIX_ASSUME(pAI != NULL);
    hr = pAI->Reserve(*(pqwQuota), *(pqwReserve), fFree);

    if (FAILED(hr))
    {
#ifdef _DEBUG
        if (fFree) {
            LOG((LF_STORE, LL_INFO10000, "free 0x%x failed\n",
                (long)(*(pqwReserve))));
    } else {
            LOG((LF_STORE, LL_INFO10000, "reserve 0x%x failed\n",
                (long)(*(pqwReserve))));
        }
#endif
        COMIsolatedStorage::ThrowISS(hr);
    }

#ifdef _DEBUG
    if (fFree) {
        LOG((LF_STORE, LL_INFO10000, "free 0x%x\n",
            (long)(*(pqwReserve))));
    } else {
        LOG((LF_STORE, LL_INFO10000, "reserve 0x%x\n",
            (long)(*(pqwReserve))));
    }
#endif

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(Object*, COMIsolatedStorageFile::GetRootDir, DWORD dwFlags)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);

    WCHAR path[MAX_PATH + 1];

    GetRootDirInternal(dwFlags, path, MAX_PATH + 1);

    refRetVal = COMString::NewString(path);

    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL1(void, COMIsolatedStorageFile::CreateDirectoryWithDacl, StringObject* pathUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    STRINGREF pathSAFE = (STRINGREF) pathUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_1(pathSAFE);

    WCHAR* wszPath = pathSAFE->GetBuffer();
    SECURITY_ATTRIBUTES *pSecAttrib = NULL;


    CreateDirectoryIfNotPresent(wszPath, pSecAttrib);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND


// Throws on error
void COMIsolatedStorageFile::CreateDirectoryIfNotPresent(__in_z WCHAR *path, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    LONG  lresult;

    // Check if the directory is already present
    lresult = WszGetFileAttributes(path);

    if (lresult == -1)
    {
        if (!WszCreateDirectory(path, lpSecurityAttributes))
            COMPlusThrowWin32();
    }
    else if ((lresult & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        COMIsolatedStorage::ThrowISS(ISS_E_CREATE_DIR);
    }
}

// Synchronized by the managed caller
const WCHAR* g_relativePath[] = {
    L"\\IsolatedStorage"
};

#define nRelativePathLen       (  \
    sizeof("\\IsolatedStorage") + 1)

#define nSubDirs (sizeof(g_relativePath)/sizeof(g_relativePath[0]))

void COMIsolatedStorageFile::GetRootDirInternal(
        DWORD dwFlags, __in_ecount(cPath) WCHAR *path, DWORD cPath)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(cPath > 1);
        PRECONDITION(cPath <= MAX_PATH + 1);
    } CONTRACTL_END;

    ULONG len;

    --cPath;    // To be safe.
    path[cPath] = 0;

    // Get roaming or local App Data locations
    if (!PAL_GetUserConfigurationDirectory(path, cPath))
        COMIsolatedStorage::ThrowISS(ISS_E_CREATE_DIR);

    len = (ULONG)wcslen(path);

    if ((len + nRelativePathLen + 1) > cPath)
        COMIsolatedStorage::ThrowISS(ISS_E_PATH_LENGTH);

    CreateDirectoryIfNotPresent(path);

    // Create the store directory if necessary
    for (unsigned int i=0; i<nSubDirs; ++i)
    {
        wcscat_s(path, cPath, g_relativePath[i]);
        CreateDirectoryIfNotPresent(path);
    }

    wcscat_s(path, cPath, L"\\");
}

#define WSZ_GLOBAL L"Global\\"

//--------------------------------------------------------------------------
// The file name is used to open / create the file.
// A synchronization object will also be created using this name
// with '\' replaced by '-'
//--------------------------------------------------------------------------
AccountingInfo::AccountingInfo(WCHAR *wszFileName, WCHAR *wszSyncName) :
        m_hFile(INVALID_HANDLE_VALUE),
        m_hMapping(NULL),
        m_hLock(NULL),
        m_pData(NULL)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

#ifdef _DEBUG
    m_dwNumLocks = 0;
#endif

    static WCHAR* g_wszGlobal = WSZ_GLOBAL;

    int buffLen;
    BOOL fGlobal;

    buffLen = (int)wcslen(wszFileName) + 1;

    NewArrayHolder<WCHAR> pwszFileName(new WCHAR[buffLen]);

    // String length is known, using a memcpy here is faster, however, this
    // makes the code here and below less readable, this is not a very frequent
    // operation. No real perf gain here. Same comment applies to the strcpy
    // following this.

    wcscpy_s(pwszFileName, buffLen, wszFileName);

    _ASSERTE(((int)wcslen(pwszFileName) + 1) <= buffLen);

    // Allocate the Mutex name
    buffLen = (int)wcslen(wszSyncName) + 1;

    // Use "Global\" prefix for Win2K server running Terminal Server.
    // If TermServer is not running, the Global\ prefix is ignored.

    fGlobal = NeedGlobalObject();

    if (fGlobal)
        buffLen += sizeof(WSZ_GLOBAL)/sizeof(WCHAR);

    NewArrayHolder<WCHAR> pwszName(new WCHAR[buffLen]);

    if (fGlobal)
    {
        wcscpy_s(pwszName, buffLen, g_wszGlobal);
        wcscat_s(pwszName, buffLen, wszSyncName);
    }
    else
    {
        wcscpy_s(pwszName, buffLen, wszSyncName);
    }

    _ASSERTE(((int)wcslen(pwszName) + 1) <= buffLen);

    pwszFileName.SuppressRelease();
    pwszName.SuppressRelease();

    // Now publish the strings
    m_wszFileName = pwszFileName;
    m_wszName = pwszName;
}

//--------------------------------------------------------------------------
// Frees memory, and open handles
//--------------------------------------------------------------------------
AccountingInfo::~AccountingInfo()
{
    CONTRACTL 
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }CONTRACTL_END;

    if (m_pData)
        CLRUnmapViewOfFile(m_pData);

    if (m_hMapping != NULL)
        CloseHandle(m_hMapping);

    if (m_hFile != INVALID_HANDLE_VALUE)
        CloseHandle(m_hFile);

    if (m_hLock != NULL)
        CloseHandle(m_hLock);

    if (m_wszFileName)
        delete [] m_wszFileName;

    if (m_wszName)
        delete [] m_wszName;

    _ASSERTE(m_dwNumLocks == 0);
}

//--------------------------------------------------------------------------
// Init should be called before Reserve / GetUsage is called.
// Creates the file if necessary
//--------------------------------------------------------------------------
HRESULT AccountingInfo::Init()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(m_hLock == NULL); // Init was called multiple times on this object without calling Close
        SO_TOLERANT;
    } CONTRACTL_END;

    // Create the synchronization object

    HRESULT hr = S_OK;
    BEGIN_SO_INTOLERANT_CODE(GetThread())
    m_hLock = WszCreateMutex(NULL, FALSE /* Initially not owned */, m_wszName);

    if (m_hLock == NULL)
        IfFailGo(ISS_E_CREATE_MUTEX);

    // Init was called multiple times on this object without calling Close

    _ASSERTE(m_hFile == INVALID_HANDLE_VALUE);

    {
        // The default DACL is fine here since we've already set the DACL on the root
        GCX_PREEMP();
        m_hFile = WszCreateFile(m_wszFileName,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_FLAG_RANDOM_ACCESS,
                                NULL);
        
        if (m_hFile == INVALID_HANDLE_VALUE)
            IfFailGo(ISS_E_OPEN_STORE_FILE);
    }

    // If this file was created for the first time, then create the accounting
    // record and set to zero
    {
        AccountingInfoLockHolder pAI(this);

        DWORD   dwLow = 0, dwHigh = 0;    // For checking file size
        QWORD   qwSize;

        dwLow = ::GetFileSize(m_hFile, &dwHigh);

        if ((dwLow == 0xFFFFFFFF) && (GetLastError() != NO_ERROR))
        {
            IfFailGo(ISS_E_GET_FILE_SIZE);
        }

        qwSize = ((QWORD)dwHigh << 32) | dwLow;

        if (qwSize < sizeof(ISS_RECORD))
        {
            DWORD dwWrite;

            // Need to create the initial file
            NewArrayHolder<BYTE> pb(new BYTE[sizeof(ISS_RECORD)]);

            memset(pb, 0, sizeof(ISS_RECORD));

            dwWrite = 0;

            if ((WriteFile(m_hFile, pb, sizeof(ISS_RECORD), &dwWrite, NULL)
                == 0) || (dwWrite != sizeof(ISS_RECORD)))
            {
                IfFailGo(ISS_E_FILE_WRITE);
            }
        }

        // Lock out of scope here will be released
    }
ErrExit:
    ;
    END_SO_INTOLERANT_CODE;
    return hr;
}

//--------------------------------------------------------------------------
// Reserves space (Increments qwQuota)
// This method is synchronized. If quota + request > limit, method fails
//--------------------------------------------------------------------------
HRESULT AccountingInfo::Reserve(
            ISS_USAGE   cLimit,     // The max allowed
            ISS_USAGE   cRequest,   // amount of space (request / free)
            BOOL        fFree)      // TRUE will free, FALSE will reserve
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_SO_INTOLERANT_CODE(GetThread());
    {
        AccountingInfoLockHolder pAI(this);

        hr = Map();

        if (SUCCEEDED(hr))
        {
            if (fFree)
            {
                if (m_pISSRecord->cUsage > cRequest)
                    m_pISSRecord->cUsage -= cRequest;
                else
                    m_pISSRecord->cUsage = 0;
            }
            else
            {
                if ((m_pISSRecord->cUsage + cRequest) > cLimit)
                    hr = ISS_E_USAGE_WILL_EXCEED_QUOTA;
                else
                    // Safe to increment quota.
                    m_pISSRecord->cUsage += cRequest;
            }

            Unmap();
        }
        // Lock out of scope here will be released
    }

    END_SO_INTOLERANT_CODE;
    return hr;
}

//--------------------------------------------------------------------------
// Method is not synchronized. So the information may not be current.
// This implies "Pass if (Request + GetUsage() < Limit)" is an Error!
// Use Reserve() method instead.
//--------------------------------------------------------------------------
HRESULT AccountingInfo::GetUsage(ISS_USAGE *pcUsage)  // pcUsage - [out]
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    BEGIN_SO_INTOLERANT_CODE(GetThread());
    {
        AccountingInfoLockHolder pAI(this);

        hr = Map();

        if (! FAILED(hr))
        {
            *pcUsage = m_pISSRecord->cUsage;

            Unmap();
        }
        // Lock out of scope here will be released
    }
    END_SO_INTOLERANT_CODE;
    return hr;
}

//--------------------------------------------------------------------------
// Maps the store file into memory
//--------------------------------------------------------------------------
HRESULT AccountingInfo::Map()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    GCX_PREEMP();
    // Mapping will fail if filesize is 0
    if (m_hMapping == NULL)
    {
        m_hMapping = WszCreateFileMapping(
            m_hFile,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            NULL);

        if (m_hMapping == NULL)
            return ISS_E_OPEN_FILE_MAPPING;
    }

    _ASSERTE(m_pData == NULL);

    m_pData = (PBYTE) CLRMapViewOfFile(
        m_hMapping,
        FILE_MAP_WRITE,
        0,
        0,
        0);

    if (m_pData == NULL)
        return ISS_E_MAP_VIEW_OF_FILE;

    return S_OK;
}

//--------------------------------------------------------------------------
// Unmaps the store file from memory
//--------------------------------------------------------------------------
void AccountingInfo::Unmap()
{
    CONTRACTL 
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }CONTRACTL_END;


    if (m_pData)
    {
        CLRUnmapViewOfFile(m_pData);
        m_pData = NULL;
    }
}

//--------------------------------------------------------------------------
// Close the store file, and file mapping
//--------------------------------------------------------------------------
void AccountingInfo::Close()
{
    WRAPPER_CONTRACT;
    Unmap();

    if (m_hMapping != NULL)
    {
        CloseHandle(m_hMapping);
        m_hMapping = NULL;
    }

    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }

    if (m_hLock != NULL)
    {
        CloseHandle(m_hLock);
        m_hLock = NULL;
    }

#ifdef _DEBUG
    _ASSERTE(m_dwNumLocks == 0);
#endif
    }

//--------------------------------------------------------------------------
// Machine wide Lock
//--------------------------------------------------------------------------
HRESULT AccountingInfo::Lock()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    // Lock is intented to be used for inter process/thread synchronization.

#ifdef _DEBUG
    _ASSERTE(m_hLock);

    LOG((LF_STORE, LL_INFO10000, "Lock %S, thread 0x%x start..\n",
            m_wszName, GetCurrentThreadId()));
#endif

    DWORD dwRet;
    {
        GCX_PREEMP();
        // m_hLock is a mutex
        Thread::BeginThreadAffinity();
        dwRet = WaitForSingleObject(m_hLock, INFINITE);
    }

#ifdef _DEBUG
    InterlockedIncrement((LPLONG)&m_dwNumLocks);

    switch (dwRet)
    {
    case WAIT_OBJECT_0:
        LOG((LF_STORE, LL_INFO10000, "Loc %S, thread 0x%x - WAIT_OBJECT_0\n",
            m_wszName, GetCurrentThreadId()));
        break;

    case WAIT_ABANDONED:
        LOG((LF_STORE, LL_INFO10000, "Loc %S, thread 0x%x - WAIT_ABANDONED\n",
            m_wszName, GetCurrentThreadId()));
        break;

    case WAIT_FAILED:
        LOG((LF_STORE, LL_INFO10000, "Loc %S, thread 0x%x - WAIT_FAILED\n",
            m_wszName, GetCurrentThreadId()));
        break;

    case WAIT_TIMEOUT:
        LOG((LF_STORE, LL_INFO10000, "Loc %S, thread 0x%x - WAIT_TIMEOUT\n",
            m_wszName, GetCurrentThreadId()));
        break;

    default:
        LOG((LF_STORE, LL_INFO10000, "Loc %S, thread 0x%x - 0x%x\n",
            m_wszName, GetCurrentThreadId(), dwRet));
        break;
    }

#endif

    if ((dwRet == WAIT_OBJECT_0) || (dwRet == WAIT_ABANDONED))
        return S_OK;

    return ISS_E_LOCK_FAILED;
}

//--------------------------------------------------------------------------
// Unlock the store
//--------------------------------------------------------------------------
void AccountingInfo::Unlock()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

#ifdef _DEBUG
    _ASSERTE(m_hLock);
    _ASSERTE(m_dwNumLocks >= 1);

    LOG((LF_STORE, LL_INFO10000, "UnLoc %S, thread 0x%x\n",
        m_wszName, GetCurrentThreadId()));
#endif

    ReleaseMutex(m_hLock);
    Thread::EndThreadAffinity();

#ifdef _DEBUG
    InterlockedDecrement((LPLONG)&m_dwNumLocks);
#endif
}


