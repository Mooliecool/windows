//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  RESIDs should be defined in the following ranges:
// 
//   1     -   999   - non localized RESID (main DLL)
//   30000 - 59999   - localized RESID     (intl DLL)
//   60000 - 65535       - reserved
// 
//  Use RESID macro to define localized resources
//  Use RESID_NOLOC macro to define non-localized resources
//  Use RESID_NODEF macro to define localized resources where the resource
//    definition is in the RC file
//  Use RESID_NODEFNOLOC macro to define non-localized resources where the
//    resource definition is in the RC file.
//
//-------------------------------------------------------------------------------------------------

#if !defined(VBC)

//-------------------------------------------------------------------------------------------------
//
// CTMENU resources
//

RESID(CTMENU,    1,     CTMENU "menus.cto")

#endif

//-------------------------------------------------------------------------------------------------
//
// TYPELIB resources
//
// ...the NTB_USED flag, when true, allows us to build under the new NT Build process.
//    once we've switched exclusively to NT Build, we can remove the conditional.

RESID_NOLOC(TYPELIB_MAIN,   1,      TYPELIB "vbidl.tlb")

//-------------------------------------------------------------------------------------------------
//
// Dialog Resources
//

#define RESID_DLG_REFERENCES 30000
#define RESID_DLG_RESET 30001
