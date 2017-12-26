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
// regutil.cpp
//
// This module contains a set of functions that can be used to access the
// registry.
//
//*****************************************************************************


#include "stdafx.h"
#include "utilcode.h"
#include "mscoree.h"
#include "sstring.h"

#define COMPLUS_PREFIX L"COMPlus_"
#define LEN_OF_COMPLUS_PREFIX 8

//*****************************************************************************
// Reads from the environment setting
//*****************************************************************************
LPWSTR REGUTIL::EnvGetString(LPCWSTR name, BOOL fPrependCOMPLUS)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    WCHAR buff[64];
    
    if(wcslen(name) > (size_t)(64 - 1 - (fPrependCOMPLUS ? LEN_OF_COMPLUS_PREFIX : 0)))
    {
        return(0);
    }


    if (fPrependCOMPLUS)
    {
        wcscpy_s(buff, _countof(buff), COMPLUS_PREFIX);
    }
    else
    {
        *buff = 0;
    }

    wcscat_s(buff, _countof(buff), name);

    FAULT_NOT_FATAL(); // We don't report OOM errors here, we return a default value.

    int len = WszGetEnvironmentVariable(buff, 0, 0);
    
    if (len == 0)
    {
        return(0);
    }

    // If we can't get memory to return the string, then will simply pretend we didn't find it.
    NewArrayHolder<WCHAR> ret(new (nothrow) WCHAR [len]); 

    if (ret != NULL)
    {
        WszGetEnvironmentVariable(buff, ret, len);
    }

    ret.SuppressRelease();
    return ret;
}



BOOL REGUTIL::UseRegistry()
{
    return TRUE;
}// UseRegistry


//*****************************************************************************
// Reads a DWORD from the COR configuration according to the level specified
// Returns back defValue if the key cannot be found
//*****************************************************************************
DWORD REGUTIL::GetConfigDWORD(LPCWSTR name, DWORD defValue, CORConfigLevel level, BOOL fPrependCOMPLUS)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    DWORD rtn;
    WCHAR val[16];

    FAULT_NOT_FATAL(); // We don't report OOM errors here, we return a default value.

    OnUnicodeSystem();

    if (level & COR_CONFIG_ENV)
    {
        WCHAR buff[64];
        _snwprintf(buff, 64, L"%s%s", fPrependCOMPLUS ? COMPLUS_PREFIX : L"", name);

        // don't allocate memory here - this is used to initialize memory checking for FEATURE_PAL
        if (WszGetEnvironmentVariable(buff, val, sizeof(val) / sizeof(WCHAR)))
        {
            LPWSTR endPtr;
            rtn = wcstoul(val, &endPtr, 16);         // treat it has hex
            if (endPtr != val)                      // success
                return(rtn);
        }
    }

    // Early out if no registry access, simplifies following code.
    //
    if (!UseRegistry() || !(level & COR_CONFIG_REGISTRY))
        return(defValue);


    if (level & COR_CONFIG_USER)
    {
        if (PAL_FetchConfigurationString(FALSE, name, val, sizeof(val) / sizeof(WCHAR)))
        {
            LPWSTR endPtr;
            rtn = wcstoul(val, &endPtr, 16);         // treat it has hex
            if (endPtr != val)                      // success
                return(rtn);
        }
    }

    if (level & COR_CONFIG_MACHINE)
    {
        if (PAL_FetchConfigurationString(TRUE, name, val, sizeof(val) / sizeof(WCHAR)))
        {
            LPWSTR endPtr;
            rtn = wcstoul(val, &endPtr, 16);         // treat it has hex
            if (endPtr != val)                      // success
                return(rtn);
        }
    }

    return(defValue);
}

#define FUSION_REGISTRY_KEY_W L"Software\\Microsoft\\Fusion"

//*****************************************************************************
// Reads a string from the COR configuration according to the level specified
// The caller is responsible for deallocating the returned string
//*****************************************************************************
LPWSTR REGUTIL::GetConfigString(LPCWSTR name, BOOL fPrependCOMPLUS, CORConfigLevel level)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    
    NewArrayHolder<WCHAR> ret(NULL);

    FAULT_NOT_FATAL(); // We don't report OOM errors here, we return a default value.

    if (level & COR_CONFIG_ENV)
    {
        ret = EnvGetString(name, fPrependCOMPLUS);  // try getting it from the environement first
        if (ret != 0) {
            if (*ret != 0) 
            {
                ret.SuppressRelease();
                return(ret);
            }
            ret.Clear();
        }
    }

    // Early out if no registry access, simplifies following code.
    //
    if (!UseRegistry() || !(level & COR_CONFIG_REGISTRY))
    {
        return(ret);
    }


    if (level & COR_CONFIG_USER)
    {

        // Allocate the return value
        ret = new (nothrow) WCHAR [_MAX_PATH];
        if (!ret)
        {
            return NULL;
        }

        if (PAL_FetchConfigurationString(TRUE, name, ret, _MAX_PATH))
        {
            ret.SuppressRelease();
            return(ret);
        }
        
    }

    if (level & COR_CONFIG_MACHINE)
    {
        // Allocate the return value
        ret = new (nothrow) WCHAR [_MAX_PATH];
        if (!ret)
        {
            return NULL;
        }

        if (PAL_FetchConfigurationString(FALSE, name, ret, _MAX_PATH))
        {
            ret.SuppressRelease();
            return(ret);
        }

    }

    if (level & COR_CONFIG_FUSION)
    {
        // Allocate the return value
        ret = new (nothrow) WCHAR [_MAX_PATH];
        if (!ret)
        {
            return NULL;
        }

        if (PAL_FetchConfigurationString(FALSE, name, ret, _MAX_PATH))
        {
            ret.SuppressRelease();
            return(ret);
        }

    }
    
    return NULL;
}

void REGUTIL::FreeConfigString(__in_z LPWSTR str)
{
    LEAF_CONTRACT;
    
    delete [] str;
}

//*****************************************************************************
// Reads a BIT flag from the COR configuration according to the level specified
// Returns back defValue if the key cannot be found
//*****************************************************************************
DWORD REGUTIL::GetConfigFlag(LPCWSTR name, DWORD bitToSet, BOOL defValue)
{
    WRAPPER_CONTRACT;
    
    return(GetConfigDWORD(name, defValue) != 0 ? bitToSet : 0);
}



















