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

// Tests 2 sec timeout for each object in Finalize() 

using System;
using System.Threading;

public class Test {

	public class Dummy {

		public static bool visited;
		internal Thread thd;

		public virtual void ThreadStart(){
			Console.WriteLine("In ThreadStart()");
		}			

		~Dummy() {
			Console.WriteLine("In Finalize() of Dummy");
		
                        thd = new Thread( new ThreadStart (this.ThreadStart) );
    			thd.Start();
			Thread.Sleep(1000);
			Console.WriteLine("After 1st Sleep..Test Passed!");
			Environment.ExitCode=0;

			Thread.Sleep(1500);
			Console.WriteLine("After 2nd Sleep..Test Failed");	// shouldn't come here as 2sec timeout is finished for this object
			Environment.ExitCode=1;
		
		}
	}

	public static void Main() {

		Dummy obj = new Dummy();
			
		obj=null;
		GC.Collect();
	}
}
