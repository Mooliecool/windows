/*=============================================================
**
** Source: commonconsts.h
**
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**============================================================*/

#ifndef _COMMONCONSTS_H_
#define _COMMONCONSTS_H_

#include <rotor_pal.h>

const int TIMEOUT = 60000;

char *szcHelperProcessStartEvName =  "start";
char *szcHelperProcessReadyEvName =  "ready";
char *szcHelperProcessFinishEvName =  "finish";

/* PEDANTIC and PEDANTIC0 is a helper macro that just grumps about any
 * zero return codes in a generic way. with little typing */
#define PEDANTIC(function, parameters) \
{ \
   if (! (function parameters) ) \
   { \
    Trace("%s: NonFatal failure of %s%s for reasons %u and %u\n", \
          __FILE__, #function, #parameters, GetLastError(), errno); \
   } \
} 
#define PEDANTIC1(function, parameters) \
{ \
   if ( (function parameters) ) \
   { \
    Trace("%s: NonFatal failure of %s%s for reasons %u and %u\n", \
          __FILE__, #function, #parameters, GetLastError(), errno); \
   } \
} 

#endif
