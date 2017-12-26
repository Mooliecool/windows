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
/* ------------------------------------------------------------------------- *
 * cordbpriv.h -- header file for private Debugger data shared by various
 *                Runtime components.
 * ------------------------------------------------------------------------- */

#ifndef _cordbpriv_h_
#define _cordbpriv_h_

#include "corhdr.h"
#include <unknwn.h>

//
// Environment variable used to control the Runtime's debugging modes.
// This is PURELY FOR INTERNAL, NONSHIPPING USAGE!! (ie, for the test team)
//
#define CorDB_CONTROL_ENV_VAR_NAME      "Cor_Debugging_Control_424242"
#define CorDB_CONTROL_ENV_VAR_NAMEL    L"Cor_Debugging_Control_424242"

//
// Environment variable used to controll the Runtime's debugging modes.
//
#define CorDB_REG_KEY                 FRAMEWORK_REGISTRY_KEY_W L"\\"
#define CorDB_REG_DEBUGGER_KEY       L"DbgManagedDebugger"
#define CorDB_REG_QUESTION_KEY       L"DbgJITDebugLaunchSetting"
#define CorDB_ENV_DEBUGGER_KEY       L"COMPLUS_DbgManagedDebugger"


//
// Initial value for EnC versions
//
#define CorDB_DEFAULT_ENC_FUNCTION_VERSION    1

//
// We split the value of DbgJITDebugLaunchSetting between the value for whether or not to ask the user and between a
// mask of places to ask. The places to ask are specified in the UnhandledExceptionLocation enum in excep.h.
//
enum DebuggerLaunchSetting
{
    DLS_ASK_USER          = 0,
    DLS_TERMINATE_APP     = 1,
    DLS_ATTACH_DEBUGGER   = 2,
    DLS_QUESTION_MASK     = 3,
    DLS_ASK_WHEN_SERVICE  = 16,
    DLS_LOCATION_MASK     = 0xFFFFFF00,
    DLS_LOCATION_SHIFT    = 8 // Shift right 8 bits to get a UnhandledExceptionLocation value from the location part.
};


//
// Flags used to control the Runtime's debugging modes. These indicate to
// the Runtime that it needs to load the Runtime Controller, track data
// during JIT's, etc.
//
enum DebuggerControlFlag
{
    DBCF_NORMAL_OPERATION           = 0x0000,

    DBCF_USER_MASK                  = 0x00FF,
    DBCF_GENERATE_DEBUG_CODE        = 0x0001,
    DBCF_ALLOW_JIT_OPT              = 0x0008,
    DBCF_PROFILER_ENABLED           = 0x0020,
//    DBCF_ACTIVATE_REMOTE_DEBUGGING  = 0x0040,  Deprecated.  DO NOT USE

    DBCF_INTERNAL_MASK              = 0xFF00,
    DBCF_ATTACHED                   = 0x0200,
    DBCF_FIBERMODE                  = 0x0400
};

//
// Flags used to control the debuggable state of modules and
// assemblies.
//
enum DebuggerAssemblyControlFlags
{
    DACF_NONE                       = 0x00,
    DACF_USER_OVERRIDE              = 0x01,
    DACF_ALLOW_JIT_OPTS             = 0x02,
    DACF_OBSOLETE_TRACK_JIT_INFO    = 0x04, // obsolete in V2.0, we're always tracking.
    DACF_ENC_ENABLED                = 0x08,
    DACF_IGNORE_PDBS                = 0x20,
    DACF_CONTROL_FLAGS_MASK         = 0x2F,

    DACF_PDBS_COPIED                = 0x10,
    DACF_MISC_FLAGS_MASK            = 0x10,
};

#endif /* _cordbpriv_h_ */
