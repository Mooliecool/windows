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
// ---------------------------------------------------------------------------
// SString_com.cpp
// ---------------------------------------------------------------------------

#include "stdafx.h"
#include "sstring.h"
#include "ex.h"
#include "holder.h"

#define DEFAULT_RESOURCE_STRING_SIZE 255

//----------------------------------------------------------------------------
// Load the string resource into this string.
//----------------------------------------------------------------------------
BOOL SString::LoadResource(int resourceID, BOOL quiet) {
    return !FAILED(LoadResourceAndReturnHR(resourceID, quiet));
}

HRESULT SString::LoadResourceAndReturnHR(int resourceID, BOOL quiet)
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        NOTHROW;
    }
    CONTRACT_END;

    HRESULT hr = E_FAIL;

    CCompRC *pResourceDLL = CCompRC::GetDefaultResourceDll();
    if (pResourceDLL != NULL)
    {
 
        int size = 0;

        EX_TRY
        {
            if (GetRawCount() == 0)
                Resize(DEFAULT_RESOURCE_STRING_SIZE, REPRESENTATION_UNICODE);

            while (TRUE)
            {
                // First try and load the string in the amount of space that we have.
                // In fatal error reporting scenarios, we may not have enough memory to 
                // allocate a larger buffer.
            
                hr = pResourceDLL->LoadString(resourceID, GetRawUnicode(), GetRawCount()+1, quiet, &size);
                if (FAILED(hr))
                {
                    Clear();
                    break;
                }

                // Although we cannot directly detect truncation, we can tell if we
                // used up all the space (in which case we will assume truncation.)
                if (size < (int)GetRawCount())
                    break;

                // Double the size and try again.
                Resize(size*2, REPRESENTATION_UNICODE);

            }

            if(SUCCEEDED(hr)) 
                Truncate(Begin() + (COUNT_T) wcslen(GetRawUnicode()));
        }
        EX_CATCH
        {
            hr = E_FAIL;
        }
        EX_END_CATCH(SwallowAllExceptions);
    }
    RETURN hr;
}

BOOL SString::LoadEnglishResource(int resourceID, BOOL quiet) {
    return !FAILED(LoadEnglishResourceAndReturnHR(resourceID, quiet));
}

HRESULT SString::LoadEnglishResourceAndReturnHR(int resourceID, BOOL quiet)
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        NOTHROW;
    }
    CONTRACT_END;


    HRESULT hr = E_FAIL;

    int size = DEFAULT_RESOURCE_STRING_SIZE;

    EX_TRY
    {
        CCompRC *pResourceDLL = CCompRC::GetDefaultResourceDll();
        while (TRUE)
        {
            Resize(size, REPRESENTATION_UNICODE);

            hr = pResourceDLL->LoadString(LANG_ENGLISH, resourceID, GetRawUnicode(), GetRawCount()+1, quiet, &size);
            if (FAILED(hr))
            {
                Clear();
                break;
            }

            // Although we cannot directly detect truncation, we can tell if we
            // used up all the space (in which case we will assume truncation.)
            if (size < (int)GetRawCount())
                break;

            // Double the size and try again.
            size *= 2;
        }

        if(SUCCEEDED(hr)) 
            Truncate(Begin() + (COUNT_T) wcslen(GetRawUnicode()));
    }
    EX_CATCH
    {
        hr = E_FAIL;
    }
    EX_END_CATCH(SwallowAllExceptions);

    RETURN hr;
}

