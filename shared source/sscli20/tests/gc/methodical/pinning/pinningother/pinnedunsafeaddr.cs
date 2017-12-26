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

// Tests pinned objects

using System;
using System.Runtime.InteropServices;

public class Test {
	
		public static void Main() {
			int[] arr = new int[10];
			GCHandle handle = GCHandle.Alloc(arr,GCHandleType.Pinned);
			IntPtr temp;
			
			IntPtr addr = handle.AddrOfPinnedObject();
			Console.WriteLine("Address of obj: {0}",addr);	
			
			GC.Collect();
			GC.WaitForPendingFinalizers();
		
			Console.WriteLine("Address of obj: {0}",handle.AddrOfPinnedObject());
			
			for(int i=1;i<=10;i++) {
				temp=(Marshal.UnsafeAddrOfPinnedArrayElement(arr,i-1));
				Console.WriteLine("temp={0}",temp);
				int t = (int)addr+(4*(i-1));
				Console.WriteLine("t={0}",t);
				if(t != (int)temp) {
					Environment.ExitCode=1;
					Console.WriteLine("Test failed");
					return;
				}
			}
			Environment.ExitCode=0;
			Console.WriteLine("Test passed");
	}
}
