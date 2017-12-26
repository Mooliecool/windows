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

// Tests Thread.Abort in Finalize() 

using System;
using System.Threading;

public class Test {

	public class Dummy {

		public static bool visited;
		~Dummy() {
			Thread id = Thread.CurrentThread;
			Console.WriteLine("In Finalize() of Dummy");	
			id.Abort();	// should abort the thread here..
			visited=true;
		}
	}

	public static void Main() {

		Dummy obj = new Dummy();
			
		obj=null;
		GC.Collect();
		
		GC.WaitForPendingFinalizers();  // makes sure Finalize() is called.

		if(Dummy.visited == false) {
			Environment.ExitCode = 0;
			Console.WriteLine("Test for Thread.Abort in Finalize() passed!");
		}
		else {
			Environment.ExitCode = 1;
			Console.WriteLine("Test for Thread.Abort in Finalize() failed!");
		}
	}
}
