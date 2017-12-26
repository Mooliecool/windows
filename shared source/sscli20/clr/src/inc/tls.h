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
// TLS.H -
//
// Encapsulates TLS access for maximum performance. 
//


#ifndef __tls_h__
#define __tls_h__


#define POPTIMIZEDTLSGETTER PAL_POPTIMIZEDTLSGETTER
#define MakeOptimizedTlsGetter PAL_MakeOptimizedTlsGetter
#define FreeOptimizedTlsGetter PAL_FreeOptimizedTlsGetter

enum TLSACCESSMODE {
   TLSACCESS_GENERIC      = 1,   // Make no platform assumptions: use the API
};

inline TLSACCESSMODE GetTLSAccessMode(DWORD tlsIndex) {
    return TLSACCESS_GENERIC;
}


#endif // __tls_h__
