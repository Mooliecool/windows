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

// Tests Pinning of Int

using System;
using System.Runtime.InteropServices;

public class Test {

	public static void Main() {
		
		int i=10;
		Object temp1,temp2;

		GCHandle handle = GCHandle.Alloc(i,GCHandleType.Pinned);
		Console.WriteLine(handle.Target);

		temp1=handle.Target;
		GC.Collect();
		GC.WaitForPendingFinalizers();
		
		Console.WriteLine(handle.Target);
		temp2=handle.Target;

		if(temp1==temp2) {
			Environment.ExitCode=0;
			Console.WriteLine("Test passed");
		} else {
			Environment.ExitCode=1;
			Console.WriteLine("Test failed");
		}


		
	}
}
