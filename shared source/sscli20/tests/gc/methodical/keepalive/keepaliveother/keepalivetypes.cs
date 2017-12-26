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

// Tests KeepAlive() with different types of inputs

using System;

public class Test {

	public class Dummy {

		public static bool visited;
		~Dummy() {
			//Console.WriteLine("In Finalize() of Dummy");	
			visited=true;
		}
	}

	public struct StrDummy {
		public int val;
		public static bool flag;

		public StrDummy(int v) {
			val=v;
			flag=true;
		}
	}

	public enum Color
	{
		Red, Blue, Green
	}

	public static void Main() {

		Dummy obj = new Dummy();
		StrDummy strobj = new StrDummy(999);
		Color enumobj = new Color();
	
		GC.Collect();
		GC.WaitForPendingFinalizers();
		
			
		if((Dummy.visited == false) && (StrDummy.flag==true)) {  // has not visited the Finalize()
			Environment.ExitCode = 0;
			Console.WriteLine("Test passed!");
		}
		else {
			Environment.ExitCode = 1;
			Console.WriteLine("Test failed!");
		}

		GC.KeepAlive(obj);	// will keep alive 'obj' till this point
		GC.KeepAlive(1000000);
		GC.KeepAlive("long string for testing");
		GC.KeepAlive(-12345678);
		GC.KeepAlive(3456.8989);
		GC.KeepAlive(true);
		GC.KeepAlive(strobj);
		GC.KeepAlive(enumobj);
		
	}
}



