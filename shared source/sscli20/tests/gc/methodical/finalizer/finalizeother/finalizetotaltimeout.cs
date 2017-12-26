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

// Tests 40 sec total timeout for running finalizers 

using System;
using System.Threading;

public class Test {

	public class Dummy {
		public static int count=0;
		~Dummy() {
			count++;
			Thread.Sleep(1000);
			if(count==42) {		// time is more than 40 sec
				Environment.ExitCode=1;
			}
		}
	}

	public static void Main() {

		Dummy[] obj = new Dummy[42];

		for(int i=0;i<42;i++) {
			obj[i] = new Dummy();
		}
		
		obj=null;     // making sure collect is called even with /debug
		GC.Collect();
		Environment.ExitCode=0;
		
	}
}
