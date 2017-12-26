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
// File: stdafx.h
//
//*****************************************************************************
#include <rotor_palrt.h>

#include <switches.h>
#include <winwrap.h>

#ifdef DACCESS_COMPILE
#include <specstrings.h>
#endif

#include <util.hpp>

#include <cordbpriv.h>
#include <dbgipcevents.h>
#include "debugger.h"
#include "controller.h"
#include "walker.h"
#include "frameinfo.h"
#include <corerror.h>
#include "../inc/common.h"

