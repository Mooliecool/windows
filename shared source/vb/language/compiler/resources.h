//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Macros and functions related to resource ids.
//
//  How to add resources
//  - String resource:
//    Edit Strings.inc to include the string definition.  A STRID_ constant will
//    automatically be defined for you, and you can use ResLoadString to load
//    the string from resources.
//
//  - Error string resource:
//    Edit Errors.inc to include the error definition.  An ERRID_ constant will
//    automatically be defined for you, and you can use HrMake to construct
//    an HRESULT from it.
//
//  - Other resources:
//    Edit resid.h to include the resource definition.  A RESID_ constant will
//    automatically be defined for you, and you can use use the HINSTOFRESID
//    macro to get the HINSTANCE of the DLL containing the resource.
//
//  Resource ID allocations
//    Non localized resources (located in main DLL):
//    1        -    999    - non localized resources
//    Localized resources (located in international DLL):
//    1000   -    59999    - localized resources
//    60000  -    65535    - reserved (no resources with these ID's)
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Global handle for localized resource lookup
extern HINSTANCE g_hinstDll;

//-------------------------------------------------------------------------------------------------
//
// Typedefs for resource ID's
//

// Generic resource id typedef
typedef UINT RESID;

// Typedef for a resource id of a string
typedef RESID STRID;

// Typedef for a resource id of an error message
typedef RESID ERRID;

typedef RESID FEATUREID;

//-------------------------------------------------------------------------------------------------
//
// Macros and constants
//

// A block of resource ID's are reserved for non-localized resources (main DLL)
// All resource ID's greater than or equal to this number are localized.
#define RESID_INTL_START    1000

// Return whether a particular resource ID comes from the intl DLL
#define FLOCALIZEDRESID(RESID)    (RESID >= RESID_INTL_START)

// Maximum length of a string before replacing arguments
#define CCH_MAX_LOADSTRING    512

// Does this errid have an error message associated with it?
#define HasErridMsg(errid)      (errid < 60000)

// Macros to convert an HRESULT to an ERRID
#define ERRIDOFHR(HR)        HRESULT_CODE(HR)

// Macro to cnovert an ERRID to an HRESULT
#define HROFERRID(ERRID)    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, ERRID)

//-------------------------------------------------------------------------------------------------
//
// Functions
//

// Load the international DLL
HRESULT ResLoadDll();

// Load a string from the appropriate DLL
HRESULT ResLoadString(
    RESID resid,
    _Out_z_cap_(cchBuf)WCHAR * wszBuf,
    unsigned cchBuf);

// The same, load a string from the appropriate DLL, but return a StringBuffer
WCHAR *ResLoadString(RESID resid, StringBuffer *buf);

// Load a string and replace embedded placeholders with supplied arguments.
// ResLoadStringRepl() requires at least 1 (WCHAR *) arg be passed after pbuf.
HRESULT __cdecl ResLoadStringRepl(
    RESID resid,
    StringBuffer * pbuf,
    ...);

// A version of ResLoadStringRepl that takes a va_list
HRESULT ResLoadStringReplArgs(
    RESID resid,
    _In_z_ StringBuffer * pbuf,
    _In_opt_z_ WCHAR * wszZero,
    va_list ap);

// A helper to replace strings. Uses ResStringReplArgs, different from it in that it takes variable
// number of arguments.
// const in a cdecl ? - needed to get around the C++ strict type checking when passing in const WCHAR* s
HRESULT __cdecl ResStringRepl(
    _In_z_ const WCHAR * wszIn,
    StringBuffer * pbuf,
    ...);

// Helper used to replace strings.
HRESULT ResStringReplArgs(
    _In_z_ WCHAR * wszIn,
    StringBuffer * pbuf,
    _In_opt_z_ WCHAR * wszRepl0,
    va_list ap);

//-------------------------------------------------------------------------------------------------
//
// ID Definitions
//

// Special resource ID's
#define RESID_None    0
#define STRID_None    0
#define ERRID_None    0

// Define the STRID constants
#define STRID(NAME, ID, STRING) \
  const STRID STRID_ ## NAME = ID;
#define STRID_NOLOC(NAME, ID, STRING) \
  STRID(NAME, ID, STRING)
#include "Strings.inc"
#undef STRID
#undef STRID_NOLOC

// Define the ERRID constants
#define ERRID(NAME, ID, STRING) \
  const ERRID ERRID_ ## NAME = ID;
#define ERRID_NOLOC(NAME, ID, STRING) \
  ERRID(NAME, ID, STRING)
#define WRNID(NAME, ID, STRING) \
  const ERRID WRNID_ ## NAME = ID;
#define FEATUREID(NAME, ID, STRING) \
  const ERRID FEATUREID_ ## NAME = ID;
#include "Errors.inc"
#define FERRID(errid) (errid >= ERRID_First && errid <= ERRID_Last)
#define FWRNID(wrnid) (wrnid >= WRNID_First && wrnid <= WRNID_Last)
#define FFEATUREID(featureid) (featureid >= FEATUREID_First && featureid <= FEATUREID_Last)
#if IDE
#include "EncErrors.inc"
#define FENCID(encid) (encid >= ENCID_First && encid <= ENCID_Last)
#endif
#undef ERRID
#undef ERRID_NOLOC
#undef WRNID
#undef FEATUREID

// Define the RESID constants
#define RESID(NAME, ID, DEFINITION) \
  const RESID RESID_ ## NAME = ID;
#define RESID_NOLOC(NAME, ID, DEFINITION) \
  const RESID RESID_ ## NAME = ID;
#define RESID_NODEF(NAME, ID) \
  const RESID RESID_ ## NAME = ID;
#define RESID_NODEFNOLOC(NAME, ID) \
  const RESID RESID_ ## NAME = ID;
#include "ResourceIds.h"
#undef RESID
#undef RESID_NOLOC
#undef RESID_NODEF
#undef RESID_NODEFNOLOC
