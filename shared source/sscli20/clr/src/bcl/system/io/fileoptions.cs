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
** Enum:   FileOptions
**
**
** Purpose: Additional options to how to create a FileStream.
**    Exposes the more obscure CreateFile functionality.
**
**
===========================================================*/

using System;
using System.Runtime.InteropServices;

namespace System.IO {
    // Maps to FILE_FLAG_DELETE_ON_CLOSE and similar values from winbase.h.
    // We didn't expose a number of these values because we didn't believe 
    // a number of them made sense in managed code, at least not yet.
    [Serializable, Flags]
    [ComVisible(true)]
    public enum FileOptions
    {
        // NOTE: any change to FileOptions enum needs to be 
        // matched in the FileStream ctor for error validation
        None = 0,
        WriteThrough = unchecked((int)0x80000000),
        Asynchronous = unchecked((int)0x40000000), // FILE_FLAG_OVERLAPPED
        // NoBuffering = 0x20000000,
        RandomAccess = 0x10000000,
        DeleteOnClose = 0x04000000,
        SequentialScan = 0x08000000,
        // AllowPosix = 0x01000000,
        // BackupOrRestore,
        // DisallowReparsePoint = 0x00200000,
        // NoRemoteRecall = 0x00100000,
        Encrypted = 0x00004000, // FILE_ATTRIBUTE_ENCRYPTED
    }
}

