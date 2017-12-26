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
** Class:  FileAttributes
**
**
** Purpose: File attribute flags corresponding to NT's flags.
**
** 
===========================================================*/
using System;

namespace System.IO {
    // File attributes for use with the FileEnumerator class.
    // These constants correspond to the constants in WinNT.h.
    // 
    [Flags,Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum FileAttributes
    {
    	// From WinNT.h (FILE_ATTRIBUTE_XXX)
    	ReadOnly = 0x1,
    	Hidden = 0x2,
    	System = 0x4,
    	Directory = 0x10,
    	Archive = 0x20,
    	Device = 0x40,
    	Normal = 0x80,
    	Temporary = 0x100,
    	SparseFile = 0x200,
    	ReparsePoint = 0x400,
    	Compressed = 0x800,
    	Offline = 0x1000,
    	NotContentIndexed = 0x2000,
    	Encrypted = 0x4000,
    }
}
