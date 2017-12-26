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

#ifndef __COMISOLATEDSTORAGE_h__
#define __COMISOLATEDSTORAGE_h__

// Dependency in managed : System.IO.IsolatedStorage.IsolatedStorage.cs
#define ISS_ROAMING_STORE   0x08
#define ISS_MACHINE_STORE   0x10

class COMIsolatedStorage
{
public:
    static FCDECL0(Object*, GetCaller);
    static void ThrowISS(HRESULT hr);

private:

    static StackWalkAction StackWalkCallBack(CrawlFrame* pCf, PVOID ppv);
};

class COMIsolatedStorageFile
{
public:
    static FCDECL1(Object*, GetRootDir, DWORD dwFlags);

    static FCDECL1(UINT64, GetUsage, LPVOID handle);

    static FCDECL4(void, Reserve, LPVOID handle, UINT64* pqwQuota, UINT64* pqwReserve, CLR_BOOL fFree);

    static FCDECL2(LPVOID, Open, StringObject* fileNameUNSAFE, StringObject* syncNameUNSAFE);

    static FCDECL1(void, Close, LPVOID handle);

    static FCDECL2(void, Lock, LPVOID handle, CLR_BOOL fLock);

    // create the machine store root directory and apply the correct DACL
    static FCDECL1(void, CreateDirectoryWithDacl, StringObject* pathUNSAFE);
private:

    static void GetRootDirInternal(DWORD dwFlags, __in_ecount(cPath) WCHAR *path, DWORD cPath);
    static void CreateDirectoryIfNotPresent(__in_z WCHAR *path, LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);
};

// --- [ Structure of data that gets persisted on disk ] -------------(Begin)

// non-standard extension: 0-length arrays in struct
#ifdef _MSC_VER
#pragma warning(disable:4200)
#endif
#include <pshpack1.h>

typedef unsigned __int64 QWORD;

typedef QWORD ISS_USAGE;

// Accounting Information
typedef struct
{
    ISS_USAGE   cUsage;           // The amount of resource used

    QWORD       qwReserved[7];    // For future use, set to 0

} ISS_RECORD;

#include <poppack.h>
#ifdef _MSC_VER
#pragma warning(default:4200)
#endif

// --- [ Structure of data that gets persisted on disk ] ---------------(End)

class AccountingInfo
{
public:

    // The file name is used to open / create the file.
    // A synchronization object will also be created using the sync name

    AccountingInfo(WCHAR *wszFileName, WCHAR *wszSyncName);

    // Init should be called before Reserve / GetUsage is called.

    HRESULT Init();             // Creates the file if necessary

    // Reserves space (Increments qwQuota)
    // This method is synchrinized. If quota + request > limit, method fails

    HRESULT Reserve(
        ISS_USAGE   cLimit,     // The max allowed
        ISS_USAGE   cRequest,   // amount of space (request / free)
        BOOL        fFree);     // TRUE will free, FALSE will reserve

    // Method is not synchronized. So the information may not be current.
    // This implies "Pass if (Request + GetUsage() < Limit)" is an Error!
    // Use Reserve() method instead.

    HRESULT GetUsage(
        ISS_USAGE   *pcUsage);  // [out] The amount of space / resource used

    // Frees cached pointers, Closes handles

    ~AccountingInfo();

    static void AcquireLock(AccountingInfo *pAI) {
        WRAPPER_CONTRACT;
        HRESULT hr = pAI->Lock();
        if (FAILED(hr)) COMIsolatedStorage::ThrowISS(hr);
    }
    static void ReleaseLock(AccountingInfo *pAI) { 
        WRAPPER_CONTRACT;
        pAI->Unlock(); 
    }
    typedef Holder<AccountingInfo *, AccountingInfo::AcquireLock, AccountingInfo::ReleaseLock> AccountingInfoLockHolder;

private:
    HRESULT Lock();     // Machine wide Lock
    void    Unlock();   // Unlock the store

    HRESULT Map();      // Maps the store file into memory
    void    Unmap();    // Unmaps the store file from memory
    void    Close();    // Close the store file, and file mapping

    WCHAR          *m_wszFileName;  // The file name
    HANDLE          m_hFile;        // File handle for the file
    HANDLE          m_hMapping;     // File mapping for the memory mapped file

    // members used for synchronization
    WCHAR          *m_wszName;      // The name of the mutex object
    HANDLE          m_hLock;        // Handle to the Mutex object

#ifdef _DEBUG
    ULONG           m_dwNumLocks;   // The number of locks owned by this object
#endif

    union {
        PBYTE       m_pData;        // The start of file stream
        ISS_RECORD *m_pISSRecord;
    };
};

#endif  // __COMISOLATEDSTORAGE_h__
