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

// LargeObjectAlloc3.cs
// Allocate nested objects of increasing size ranging from 200 KB to ~ 25 MB
// If memory is low, after every loop, the large objects should be collected
// and committed from the LargeObjectHeap
// The Finalizer makes sure that the GC is actually collecting the large objects


using System;

namespace LargeObjectTest {

public class OtherLargeObject {
	private int[] otherarray;
	public OtherLargeObject() {
		otherarray = new int[1024*50];
	}
}
	
public class LargeObject {
	private OtherLargeObject[] olargeobj;

	public LargeObject(int size) {
		olargeobj = new OtherLargeObject[size];		
		for(int i=0;i< size;i++) {	  
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
		int size = 1;
		int loop=1;
		LargeObject largeobj;

		
		Console.WriteLine("Test should pass with ExitCode 0\n");
		Environment.ExitCode=1;
		
		while(loop<100) {
		Console.WriteLine("Loop: {0}",loop);
		for(int i=0;i<=7;i++) {
		
		try {
		   largeobj=new LargeObject(size);
		   Console.WriteLine("Allocated LargeObject: {0} bytes",size*4*1024*50);
		}catch (Exception e) {
		    Console.WriteLine("Failure to allocate in loop {0}\n",loop);
		    Console.WriteLine("Caught Exception: {0}",e);
		    return;
		 }
		largeobj=null;
		GC.Collect();
		GC.Collect();
		Console.WriteLine("LargeObject Collected");
		size*=2;
                }
		size=1;
		loop++;
		}
		
		Console.WriteLine("Test Passed");
		GC.Collect();
	}
}
}

