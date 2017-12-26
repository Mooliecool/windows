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

// LargeObjectAlloc2.cs
// Allocate nested objects of ~ 25 MB
// If memory is low, after every loop, the large objects should be collected
// and committed from the LargeObjectHeap
// The Finalizer makes sure that the GC is actually collecting the large objects


using System;

namespace LargeObjectTest {

public class OtherLargeObject {
	private int[] otherarray;
	public OtherLargeObject() {
		otherarray = new int[5000]; // 20 KB
	}
}
	
public class LargeObject {
	private int[] array;
	private OtherLargeObject[] olargeobj;

	public LargeObject() {
		array = new int[1250000]; // 5 MB
		olargeobj = new OtherLargeObject[1000];		//20 MB
		for(int i=0;i< 1000;i++) {	  
		    olargeobj[i] = new OtherLargeObject(); 
		}
	}

	~LargeObject() {
		Console.WriteLine("In finalizer");
		Environment.ExitCode=0;
		
	}
}

public class Test {

	public static void Main() {
		int loop = 0;
		LargeObject largeobj;

		Console.WriteLine("Test should pass with ExitCode 0\n");
		Environment.ExitCode=1;
		
		while(loop<=200) {
		  	loop++;
		  	Console.Write("LOOP: {0}\n",loop);
			try {
			   largeobj=new LargeObject();
			   Console.WriteLine("Allocated LargeObject");
			}catch (Exception e) {
			    Console.WriteLine("Failure to allocate at loop {0}\n",loop);
			    Console.WriteLine("Caught Exception: {0}",e);
			    return;
			 }
		largeobj=null;
		GC.Collect();
		GC.WaitForPendingFinalizers();
		Console.WriteLine("LargeObject Collected\n");
		}
		Console.WriteLine("Test Passed");
		GC.Collect();
	}
}
}
