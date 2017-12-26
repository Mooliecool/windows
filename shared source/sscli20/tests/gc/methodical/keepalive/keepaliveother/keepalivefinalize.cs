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

// Tests KeepAlive() in Finalize

using System;
using System.Collections;

public class Test {
	
	public class Dummy1 {
		public static bool visited;
		~Dummy1() {
			//Console.WriteLine("In Finalize() of Dummy1");
			Dummy2 temp = new Dummy2();
			visited=true;
			
			GC.Collect();
			GC.WaitForPendingFinalizers();
			GC.KeepAlive(temp);
		}
	}

	public class Dummy2 {
		public static bool visited;
		~Dummy2() {
			//Console.WriteLine("In Finalize() of Dummy2");
			visited=true;
		}
	}

	public static void Main() {

		Dummy1 obj = new Dummy1();
		
		GC.Collect();
		GC.WaitForPendingFinalizers();

		if((Dummy1.visited == false) && (Dummy2.visited == false)) {  // has not visited the Finalize()
			Environment.ExitCode = 0;
			Console.WriteLine("Test passed!");
		}
		else {
			Environment.ExitCode = 1;
			Console.WriteLine("Test failed!");
		}
		
		GC.KeepAlive(obj);

	}
}
