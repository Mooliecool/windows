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

// Tests Pinning of objects
// Cannot pin objects or array of objects

using System;
using System.Runtime.InteropServices;

public class Test {

	public static void Main() {
		
		Object[] arr = new Object[100];
		Object obj = new Object();
		Environment.ExitCode=1;

		try {
			GCHandle handle1 = GCHandle.Alloc(arr,GCHandleType.Pinned);
			GCHandle handle2 = GCHandle.Alloc(obj,GCHandleType.Pinned);
		} catch(Exception e) {
			Console.WriteLine("Caught: {0}",e);
			Environment.ExitCode=0;
			Console.WriteLine("Test passed");
		}
	}
}
