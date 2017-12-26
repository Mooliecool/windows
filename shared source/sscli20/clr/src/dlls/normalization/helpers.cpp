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

// Helpers.cpp
//
// WARNING: This .DLL is supposed to be the same in managed and native code,
// starting with the Longhorn & Whidbey releases.  Please make sure they stay in sync.
//
// This file contains nifty helper methods that can help us process unicode characters.
// Things like Hangul and surrogate character parsing.
//

#include "normalizationprivate.h"
#include "normalizationhelp.h"
#include "helpers.h"
