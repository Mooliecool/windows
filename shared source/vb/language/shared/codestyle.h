//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Macros to support C++ coding guidelines standard.
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Use these macros to mark unreachable 'default' case inside 'switch' statements.
#define UNREACHABLE()           VSFAIL("Unexecpted value in a 'switch' statement.")
#define UNREACHABLE_MSG(msg)    VSFAIL(msg)

// Used to explicitly handle possible alignment problems on Win64 or ARM
#if _WIN64 || ARM
#define USE_UNALIGNED __unaligned
#else // _WIN64
#define USE_UNALIGNED
#endif // _WIN64

// LCID used for many operations.
#define LCID_US_ENGLISH 0x409
