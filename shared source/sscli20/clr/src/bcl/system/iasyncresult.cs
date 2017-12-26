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
** Interface: IAsyncResult
**
** Purpose: Interface to encapsulate the results of an async
**          operation
**
===========================================================*/
namespace System {
    
	using System;
	using System.Threading;
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IAsyncResult
    {
        bool IsCompleted { get; }

        WaitHandle AsyncWaitHandle { get; }


        Object     AsyncState      { get; }

        bool       CompletedSynchronously { get; }
   
    
    }

}
