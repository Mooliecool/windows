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

// unilib.h - UniLib Unicode library

//-----------------------------------------------------------------
// Note: Portions of the library is designed to be usable
//       without vsmem/vsassert, so only a primitive DebugBreak
//       UASSERT is used in these portions of the library.
//
//-----------------------------------------------------------------

#ifdef _MSC_VER
#pragma once
#endif

#ifndef __UNILIB_H__
#define __UNILIB_H__


#include "unichar.h"    // Unicode character constants
#include "uniprop.h"    // Unicode character properties
#include "unicase.h"    // Case changing, Unicode string comparisons
#include "unistr.h"     // Unicode string copy, catenate, URL detection, string munging
#include "unimisc.h"    // SignOf, Swap, InRange, CMinimalTextStream
//#include "commacro.h"   // COM macros
#include "utf.h"        // UTF-8 detection, Unicode<->UTF-8 conversion
//#include "regexp.h"     // OBSOLETE: Old Regular Expression engine
#include "unum.h"       // Integer conversions and fullwidth->halfwidth utils
#include "uniapi.h"     // Unicode Win API wrappers (Win9x/NT)
//#include "uniesc.h"     // Escape/Unescape Unicode strings as ASCII

// The folowing are part of Unilib, but not included here to avoid collisions 
// between vsmem and other allocators such as VB*.

// Text files, IVsTextLines, IVsTextImage implementations and helpers
//
//#include "textpst.h"      // Text file persistence
//#include "txeventsink.h"  // text event sinks
//#include "txfactory.h"    // Create IVsTextImage
//#include "tximage.h"      // IVsTextImage
//#include "tximghlp.h"     // helpers for using IVsTextImage
//#include "txlinehlp.h"    // helpers for using IvsTextLines

#endif // __UNILIB_H__
