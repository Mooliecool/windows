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

//Tests KeepAlive with WeakReference and WeakReference Exceptions 


using System;

public class Test {

	static public Application resurrect;
	static public WeakReference weak;
        static bool flag;

	public static void Print(Exception e) {
		//Console.WriteLine("Caught " + e);
		
	}

	public class Application {
			public static bool visited=false;
  			
			~Application() {
				
				if((visited==false) && (Test.flag==false)) {	 // object is being finalized
				   GC.ReRegisterForFinalize(this);
				   visited=true;
				   Environment.ExitCode=1;
				 }
				else {			// trying to resurrect object that has been finalized
				   try {	
					   Test.resurrect=(Application)weak.Target;
					   GC.ReRegisterForFinalize(Test.resurrect);
				   } catch(Exception e) {
   					        if(Test.flag==false) Environment.ExitCode=1;
						Print(e);
			   	   }
					 
				}
			}
	}

	public static void Main() {

		Test.flag=false;
		Environment.ExitCode=0;

		Application obj = new Application();
		weak = new WeakReference(obj,true);

		//obj=null;
		GC.Collect();
		GC.WaitForPendingFinalizers();	// forcing a GC to collect the obj
	
		Test.resurrect=null;
		GC.Collect();
		GC.WaitForPendingFinalizers();	// resurrecting the obj

		GC.KeepAlive(obj);	// keep alive the object

		Test.flag=true;
		Console.WriteLine("Done");
	}
}
