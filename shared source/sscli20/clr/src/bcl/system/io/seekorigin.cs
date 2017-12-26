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
** Enum:   SeekOrigin
**
**
** Purpose: Enum describing locations in a stream you could
** seek relative to.
**
**
===========================================================*/

using System;

namespace System.IO {
    // Provides seek reference points.  To seek to the end of a stream,
    // call stream.Seek(0, SeekOrigin.End).
	  [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum SeekOrigin
    {
    	// These constants match Win32's FILE_BEGIN, FILE_CURRENT, and FILE_END
    	Begin = 0,
    	Current = 1,
    	End = 2,
    }
}
