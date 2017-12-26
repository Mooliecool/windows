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

#include "rotor_pal.h"
#include "rotor_palrt.h"
#include "syms.h"
#include "libs.h"
#include "sscoree_int.h"



/***************************************************************************/
/* Temporary definitions for argument types                                */
/***************************************************************************/
#ifndef SNAPI
#  define SNAPI BOOLEAN
#  define SNAPI_(_type) _type
#endif

#ifndef __fusion_h__ 
enum ASM_CACHE_FLAGS {};

typedef void * LPAPPLICATIONCONTEXT;
typedef void * LPASSEMBLYNAME;

typedef void * IAssembly;
typedef void * IAssemblyName;
typedef void * IAssemblyCache;
typedef void * IAssemblyEnum;
typedef void * IInstallReferenceEnum;
typedef void * IHistoryReader;
typedef void * IHistoryAssembly;
typedef void * IMetaDataAssembly;
typedef void * IMetaDataAssemblyImport;
typedef void * IApplicationContext;
#endif // __fusion_h__

#ifndef __csiface_h__
typedef void * ICSCompilerFactory;
struct COMPILEPARAMS;
#endif // __csiface_h__

#define SSCOREE_SHIM_BODY(FUNC,RET_COMMAND,SIG_RET,SIG_ARGS,ARGS)       \
do {                                                                    \
    SSCOREE_SHIM_CUSTOM_INIT                                            \
    FARPROC proc_addr = SscoreeShimGetProcAddress (                     \
                        SHIMSYM_ ## FUNC,                               \
                        #FUNC);                                         \
    _ASSERTE (proc_addr);                                               \
    if (proc_addr) {                                                    \
        RET_COMMAND ((SIG_RET (STDMETHODCALLTYPE *)SIG_ARGS)proc_addr)ARGS; \
    }                                                                   \
} while (0)


#define SSCOREE_SHIM_RET(SIG_RET,FUNC,SIG_ARGS,ARGS,ONERROR)            \
extern "C"                                                              \
SIG_RET STDMETHODCALLTYPE FUNC SIG_ARGS                                 \
{                                                                       \
    SSCOREE_SHIM_BODY (FUNC, return, SIG_RET, SIG_ARGS, ARGS);          \
    return ONERROR;                                                     \
}

#define SSCOREE_SHIM_NORET(FUNC,SIG_ARGS,ARGS)                          \
extern "C"                                                              \
void STDMETHODCALLTYPE FUNC SIG_ARGS                                    \
{                                                                       \
    SSCOREE_SHIM_BODY (FUNC, ; ,void, SIG_ARGS, ARGS);                  \
}

#include "sscoree_shims.h"
