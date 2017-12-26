//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Simple Utility Functions 
//
//-------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------
//
// Call GetLastError and construct an HRESULT for it.  If no last error has
// been set, then use E_OUTOFMEMORY.  This is useful when calling system
// functions that return BOOL's, but use SetLastError for more detailed info.
// Call GetLastError and construct an HRESULT for it.  If no last error has
// been set, then use E_OUTOFMEMORY.  This is useful when calling system
// functions that return BOOL's, but use SetLastError for more detailed info.
//
//-------------------------------------------------------------------------------------------------

VBAPI
HRESULT
GetLastHResultError();


