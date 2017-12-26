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
/*============================================================
**
** Class:  COMStreams
**
**                                                    
**
** Purpose: Streams native implementation
**
** Date:  June 29, 1998
**
===========================================================*/
#include "common.h"
#include "excep.h"
#include "object.h"
#include <winbase.h>
#include "comstreams.h"
#include "field.h"
#include "eeconfig.h"
#include "comstring.h"
#include "winwrap.h"

