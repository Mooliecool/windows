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
// stdafx.h
//
// Common include file for utility code.
//*****************************************************************************
#include <stdlib.h>		// for qsort
#include <windows.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>

#define FEATURE_NO_HOST     // Do not use host interface
#include <utilcode.h>

#include <corpriv.h>

#include "pewriter.h"
#include "ceegen.h"
#include "ceefilegenwriter.h"
#include "ceesectionstring.h"
