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

#define _CRT_DEPENDENCY_  //this code depends on the crt file functions
#include <crtwrap.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>		// for qsort
#include <windows.h>
#include <time.h>

#include <corerror.h>
#include <utilcode.h>

#include <corpriv.h>

#include <sighelper.h>

#include "pesectionman.h"

#include "ceegen.h"
#include "ceesectionstring.h"
