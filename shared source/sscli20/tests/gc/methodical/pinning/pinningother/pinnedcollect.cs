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

// Tests Collect with pinned objects
// Arr has both Normal and Pinned handles

using System;
using System.Runtime.InteropServices;

public class Test {
	
		public static void Main() {
			float[] arr = new float[100];
			GCHandle handle1 = GCHandle.Alloc(arr,GCHandleType.Pinned);
			GCHandle handle2 = GCHandle.Alloc(arr,GCHandleType.Normal);

			Environment.ExitCode=1;
			IntPtr oldaddr, newaddr;
			
			oldaddr=handle1.AddrOfPinnedObject();
			Console.WriteLine("Address of obj: {0}",oldaddr);	
			
			GC.Collect();
			GC.WaitForPendingFinalizers();

			handle1.Free();		// arr should only have normal handle now
			GC.Collect();
			GC.WaitForPendingFinalizers();

			GC.Collect();
			GC.WaitForPendingFinalizers();
		
			try {
				Console.WriteLine("Address of obj: {0}",handle1.AddrOfPinnedObject());
			}catch(Exception e) {
				Console.WriteLine("Caught: " + e);
			}

			arr=null;
			GC.Collect();
			
			// Pinning the arr again..it should have moved
			GCHandle handle3 = GCHandle.Alloc(arr,GCHandleType.Pinned);
			newaddr=handle3.AddrOfPinnedObject();

			Console.WriteLine("Address of obj: {0}",newaddr);
		
			if(oldaddr==newaddr) {
				Console.WriteLine("Test failed!");
				Environment.ExitCode=1;
			}else {
				Console.WriteLine("Test passed!");
				Environment.ExitCode=0;
			}		
			
		}
	}
	
