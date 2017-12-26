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

// Tests KeepAlive() scopes

using System;

public class Test {

	public class Dummy {

		public static bool visited;
		~Dummy() {
			//Console.WriteLine("In Finalize() of Dummy");	
			visited=true;
		}
	}

	public static void Main() {

		Dummy obj = new Dummy();
		bool result=false;
		
		GC.Collect();
		GC.WaitForPendingFinalizers();
		
			
		if((Dummy.visited == false)) {  // has not visited the Finalize() yet
			result=true;
		}
		
		GC.KeepAlive(obj);	// will keep alive 'obj' till this point
		
		obj=null;
		GC.Collect();
		GC.WaitForPendingFinalizers();
		
		if(result==true && Dummy.visited==true) {
			Console.WriteLine("Test passed!");
			Environment.ExitCode = 0;
		}
		else {
			Console.WriteLine("Test failed!");
			Environment.ExitCode = 1;
		}		
	
	}
}



