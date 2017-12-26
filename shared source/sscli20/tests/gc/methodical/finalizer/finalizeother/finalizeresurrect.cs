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

public class Application {
	
    	static public ResurrectObj ResObjHolder;    
	static public int count=0;

	public class ResurrectObj{

	    ~ResurrectObj() {
        	
            	//Console.WriteLine("In Finalize()");
		//Resurrecting the object
	        Application.ResObjHolder = this;
	
        	if(count<2) GC.ReRegisterForFinalize(this);
	    	
		else {
			Environment.ExitCode=0;
			return;
		}
		count++;        
  	    }
	}

    
   	 public static void ResurrectionDemo() {
		ResurrectObj obj = new ResurrectObj();
               	obj = null;
	        GC.Collect();
        	GC.WaitForPendingFinalizers(); 

		// calling the finalizer again on the resurrected object
	       	ResObjHolder = null;
	        GC.Collect();
        	GC.WaitForPendingFinalizers(); 
    }

}

public class Test {

	public static void Main() {
		Environment.ExitCode=1;
		Application.ResurrectionDemo();
		//Console.WriteLine(Application.count);
		GC.Collect();
	}

}
