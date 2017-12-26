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

// Tests multiple handles for same object


using System;
using System.Runtime.InteropServices;

public class Test {

	public static void Main() {
		
		int[] arr = new int[1000];
		GCHandle[] arrhandle = new GCHandle[10000];	// array of handles to the same object
		IntPtr[] oldaddress = new IntPtr[10000];		// store old addresses
		IntPtr[] newaddress = new IntPtr[10000];		// store new addresses
		
		for(int i=0;i<10000; i++) {
		   arrhandle[i]=GCHandle.Alloc(arr,GCHandleType.Pinned);
		   oldaddress[i]=arrhandle[i].AddrOfPinnedObject();
		}

		GC.Collect();
		GC.WaitForPendingFinalizers();

		for(int i=0;i<10000; i++) {
		     newaddress[i]=arrhandle[i].AddrOfPinnedObject();
		}

		for(int i=0;i<10000;i++) {
			if(oldaddress[i]!=newaddress[i]) {
				Environment.ExitCode=1;
				Console.WriteLine("Test failed");
			}
		}
		Environment.ExitCode=0;
		Console.WriteLine("Test passed");	
		
		
	}
}
