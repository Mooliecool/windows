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

// Tests Pinning many objects
// Here we create 2500 arrays and pin them all

using System;
using System.Runtime.InteropServices;
public class Test {
public static void Main() {

int NUM = 2500;
int[][] arr = new int[NUM][];
GCHandle[] handle = new GCHandle[NUM];

IntPtr[] oldaddr = new IntPtr[NUM];

for(int i=0;i<NUM;i++) {
	arr[i] = new int[NUM];
	handle[i] = GCHandle.Alloc(arr[i],GCHandleType.Pinned);
	oldaddr[i] = handle[i].AddrOfPinnedObject();
}

GC.Collect();
GC.WaitForPendingFinalizers();

for(int i=0;i<NUM;i++) {
	if(handle[i].AddrOfPinnedObject() != oldaddr[i]) {
		Environment.ExitCode=1;
		Console.WriteLine("Test failed!");
		return;
	}
}

GC.Collect();
GC.WaitForPendingFinalizers();

for(int i=0;i<NUM;i++) {
	if(handle[i].IsAllocated != true) {
		Environment.ExitCode=1;
		Console.WriteLine("Test failed!");
		return;
	}
}

Environment.ExitCode=0;
Console.WriteLine("Test passed!");


}
}
