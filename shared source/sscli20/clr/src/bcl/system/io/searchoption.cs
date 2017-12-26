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
** Enum:   SearchOption
**
**
** Purpose: Enum describing whether the search operation should  
** retrieve files/directories from the current directory alone  
** or should include all the subdirectories also.
**
**
===========================================================*/

using System;

namespace System.IO {
    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum SearchOption
    {
        // Include only the current directory in the search operation
    	TopDirectoryOnly,

        // Include the current directory and all the sub-directories
        // underneath it including reparse points in the search operation. 
        // This will traverse reparse points (i.e, mounted points and symbolic links)
        // recursively. If the directory structure searched contains a loop
        // because of hard links, the search operation will go on for ever. 
    	AllDirectories,
    }
}
