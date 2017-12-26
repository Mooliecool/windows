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

using System;

public class Test {

	public class Dummy {
		public static int count=0;
		~Dummy() {
			count++;
		}
	}

	public static void Main() {

		Dummy[] obj = new Dummy[10000];

		for(int i=0;i<10000;i++) {
			obj[i] = new Dummy();
		}
		
		obj=null;     // making sure collect is called even with /debug
		GC.Collect();
		GC.WaitForPendingFinalizers();
		
		if(Dummy.count == 10000) {     // all objects in array finalized!
			Environment.ExitCode = 0;
			Console.WriteLine("Test for Finalize() for array of objects passed!");
		}
		else {
			Environment.ExitCode = 1;
			Console.WriteLine("Test for Finalize() for array of objects failed!");
		}
		
		

	}
}
