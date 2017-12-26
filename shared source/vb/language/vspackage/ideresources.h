//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Define the STRID constants
#define STRID(NAME, ID, STRING)             const STRID STRID_ ## NAME = ID;
#define STRID_NOLOC(NAME, ID, STRING)       STRID(NAME, ID, STRING)
#include "IDEStrings.h"
#undef STRID
#undef STRID_NOLOC
