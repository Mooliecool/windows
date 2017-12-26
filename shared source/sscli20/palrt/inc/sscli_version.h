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
// ===========================================================================
// File: sscli_version.h
// 
// =========================================================================== 


#ifndef __SSCLI_VERSION_H__
#define __SSCLI_VERSION_H__

#ifndef __RC_STRINGIZE__
#define __RC_STRINGIZE__AUX(x)      #x
#define __RC_STRINGIZE__(x)         __RC_STRINGIZE__AUX(x)
#endif

#ifndef __RC_STRINGIZE_WSZ__
#define __RC_STRINGIZE_WSZ__AUX(x)  L###x
#define __RC_STRINGIZE_WSZ__(x)     __RC_STRINGIZE_WSZ__AUX(x)
#endif

#define SSCLI_VERSION_MAJOR 2
#define SSCLI_VERSION_MINOR 0
#define SSCLI_VERSION_RELEASE 0001

#define SSCLI_VERSION_STR __RC_STRINGIZE__(SSCLI_VERSION_MAJOR) "." __RC_STRINGIZE__(SSCLI_VERSION_MINOR) "." __RC_STRINGIZE__(SSCLI_VERSION_RELEASE)

#define SSCLI_VERSION_STRW __RC_STRINGIZE_WSZ__(SSCLI_VERSION_MAJOR) L"." __RC_STRINGIZE_WSZ__(SSCLI_VERSION_MINOR) L"." __RC_STRINGIZE_WSZ__(SSCLI_VERSION_RELEASE)
#endif // __SSCLI_VERSION_H__
