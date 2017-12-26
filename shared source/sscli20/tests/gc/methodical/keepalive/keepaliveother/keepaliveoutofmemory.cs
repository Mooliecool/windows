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

// Tests KeepAlive() when out of memory

using System;

public class Test {

	public static int FACTOR=1024;
	public class Dummy {

		public static bool visited=false;
		public int[] data;
		public int value;
		
		public Dummy(){
			data = new int[FACTOR];
			value=99;
		}

		~Dummy() {
			visited=true;
		}
	}

	public static void Main() {

		Dummy[] arr = new Dummy[1000];

		for(int i=0;i<1000;i++) {
			try {
				arr[i] = new Dummy();
			}catch(Exception e) {
				Console.WriteLine("Caught: {0}",e);

				FACTOR=FACTOR/4;
				Console.WriteLine("FACTOR: {0}",FACTOR);

				GC.Collect();
				GC.WaitForPendingFinalizers();
			}
		}
		
		for(int i=0;i<1000;i++) {
			
			if(arr[i].value!=99) {
				Environment.ExitCode=1;
				Console.WriteLine("Test failed");
				return;
			}
		}
		Environment.ExitCode=0;
		Console.WriteLine("Test passed");
		
		GC.KeepAlive(arr);	// arr should not be collected till here
	}
}
