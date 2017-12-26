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

#ifndef __LOCK_H_
#define __LOCK_H_

#include "helpers.h"
#include "fusionp.h"

class CCriticalSection
{
    public:
        CCriticalSection(CRITSEC_COOKIE csCookie)
        : _csCookie(csCookie)
        , _bEntered(FALSE)
        {
            _ASSERTE(csCookie);
        }

        ~CCriticalSection()
        {
            if (_bEntered) {
                ClrLeaveCriticalSection(_csCookie);
            }
        }

        // Don't try to catch when entering or leaving critical
        // sections! We used to do this, but this is absolutely not the
        // right behaviour.
        //
        // Enter/Leave will raise exceptions when they can't allocate the
        // event used to signal waiting threads when there is contention.
        // However, if the event can't be allocated, other threads
        // that were waiting on the event never get signalled, so they'll
        // spin forever.
        //
        // If Enter/Leave ever raise an exception, you should just bubble
        // the exception up, and not try to do anything.

        HRESULT Lock()
        {
            HRESULT                          hr = S_OK;

            if (_bEntered) {
                return E_UNEXPECTED;
            }

            ClrEnterCriticalSection(_csCookie);
            _bEntered = TRUE;

            return hr;
        }

        HRESULT Unlock()
        {
            HRESULT                      hr = S_OK;
            
            if (_bEntered) {
                _bEntered = FALSE;
                ClrLeaveCriticalSection(_csCookie);
            }
            else {
                _ASSERTE(0);
                hr = E_UNEXPECTED;
            }

            return hr;
        }

    private:
        CRITSEC_COOKIE                       _csCookie;
        BOOL                                 _bEntered;
};

HRESULT LockCacheDir(CACHE_FLAGS dwCacheFlags, LPCWSTR pwzLockFile, HANDLE *phLock);
HRESULT UnlockCacheDir(HANDLE hLock);

// The underline mechanism is file lock 
class CMutex
{
    public:
        CMutex(MUTEX_COOKIE hMutex)
        {
            _ASSERTE(hMutex);
            _pwzLockFile = (LPWSTR) hMutex;
            _hLock = INVALID_HANDLE_VALUE;
        }

        ~CMutex()
        {
            if (_hLock != INVALID_HANDLE_VALUE)
                Unlock(); 
        }

        HRESULT Lock()
        {
            HRESULT hr = S_OK;

            if(_pwzLockFile == (LPWSTR) INVALID_HANDLE_VALUE)   // no need to take lock.
                goto Exit;

            if (_hLock != INVALID_HANDLE_VALUE) {
                hr = E_UNEXPECTED;
                goto Exit;
            }

            hr = LockCacheDir(CACHE_DOWNLOAD, _pwzLockFile, &_hLock);
                                
        Exit :
            return hr;
        }

        HRESULT Unlock()
        {
            HRESULT hr = S_OK;

            if(_pwzLockFile == (LPWSTR) INVALID_HANDLE_VALUE)
                goto Exit;

            if (_hLock != INVALID_HANDLE_VALUE) {
                hr = UnlockCacheDir(_hLock);
                _hLock = INVALID_HANDLE_VALUE;
            } else {
                _ASSERTE(!"Unlock on invalid handle\n");
                hr = E_UNEXPECTED;
            }

        Exit:
            return hr;
        }

    private:
        LPWSTR          _pwzLockFile;
        HANDLE          _hLock;
};

#endif  // __LOCK_H_
