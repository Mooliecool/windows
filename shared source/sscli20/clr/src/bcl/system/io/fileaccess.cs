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
** Enum:   FileAccess
**
**
** Purpose: Enum describing whether you want read and/or write
** permission to a file.
**
**
===========================================================*/

using System;

namespace System.IO {
    // Contains constants for specifying the access you want for a file.
    // You can have Read, Write or ReadWrite access.
    // 
    [Serializable, Flags]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum FileAccess
    {
        // Specifies read access to the file. Data can be read from the file and
        // the file pointer can be moved. Combine with WRITE for read-write access.
        Read = 1,
    
        // Specifies write access to the file. Data can be written to the file and
        // the file pointer can be moved. Combine with READ for read-write access.
        Write = 2,
    
        // Specifies read and write access to the file. Data can be written to the
        // file and the file pointer can be moved. Data can also be read from the 
        // file.
        ReadWrite = 3,
    }
}
