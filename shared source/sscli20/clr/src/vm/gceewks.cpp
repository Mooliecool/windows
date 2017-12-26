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

#include "common.h"
#include "object.inl"

#ifdef GC_PROFILING
#include "profilepriv.h"
#endif

#include "perfcounters.h"
#include "eventtrace.h"
#include "log.h"
#include "eeconfig.h"
#include "excep.h"

#include "dbginterface.h"

#include "remoting.h"
#include "comsynchronizable.h"
#include "comsystem.h"

#include "syncclean.hpp"


#include "corhost.h"
#include "eepolicy.h"


namespace WKS { 
using ::Object;
using ::ArrayBase;

#include "gcimpl.h"
#include "gcee.cpp"

}

