//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Shared implementation for the /LangVersion switch.  Contains constants and data used by
//  components of the compiler that need access to which features were introduced in version 10
//  and above.
// 
//-------------------------------------------------------------------------------------------------

#pragma once

// We use this structure to track which features were introduced in version 10 and beyond.
struct LanguageFeatureMap
{
    int m_FeatureIDIndex;
    LANGVERSION m_Introduced;
};

// Used to map a featureid (as defined in errors.inc) to an array index into the g_LanguageFeatureMap array.
#define FEATUREID_TO_INDEX(x) (x - FEATUREID_First)

