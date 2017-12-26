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

// Tests Pinned handle
// Should throw an InvalidOperationException for accessing the AddrOfPinnedObject()
// for a different type of handle.

using System;
using System.Runtime.InteropServices;

public class Test {

	public static void Main() {

		Environment.ExitCode=1;
		int[] arr = new int[100];
		GCHandle handle = GCHandle.Alloc(arr,GCHandleType.Pinned);
		GCHandle hhnd = GCHandle.Alloc(handle,GCHandleType.Normal);

		GC.Collect();
		GC.WaitForPendingFinalizers();
		
		Console.WriteLine("Address of obj: {0}",handle.AddrOfPinnedObject());
		try {
			Console.WriteLine("Address of handle {0}",hhnd.AddrOfPinnedObject());
		}catch(Exception e) {
			Console.WriteLine("Caught: {0}",e.ToString());
			Environment.ExitCode=0;
			Console.WriteLine("Test passed");
		}	
		
	}
}
