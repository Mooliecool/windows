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
/*********************************************************************
 **                                                                 **
 ** CorExcep.h - lists the exception codes used by the CLR.         **
 **                                                                 **
 *********************************************************************/


#ifndef __COREXCEP_H__
#define __COREXCEP_H__

// All COM+ exceptions are expressed as a RaiseException with this exception
// code.  If you change this value, you must also change
// bcl\src\system\Exception.cs's _COMPlusExceptionCode value.

#define EXCEPTION_MSVC    0xe06d7363    // 0xe0000000 | 'msc'
#define EXCEPTION_COMPLUS 0xe0524f54    // 0xe0000000 | 'ROT'

#define EXCEPTION_HIJACK  0xe0434f4e    // 0xe0000000 | 'COM'+1

#define EXCEPTION_SOFTSO  0xe053534f    // 0xe0000000 | 'SSO'
                                        // We can not throw internal C++ exception through managed frame.
                                        // At boundary, we will raise an exception with this error code

#endif // __COREXCEP_H__
