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
public class Test {
	public static void Main() {
		Int32 basesize;
		Int32[] largeobjarr;
		int loop=0;
	
		Console.WriteLine("Test should pass with ExitCode 0");

		while(loop<50) { 
			Console.WriteLine("loop: {0}",loop);
			basesize = 4096; 
    			try {
			for(int i=0;i<100;i++) { 
				//Console.WriteLine("In loop {0}, Allocating array of {1} bytes\n",i,basesize*4);
        			largeobjarr = new Int32[basesize]; 
        			basesize+=4096; 
    			}
			} catch(Exception e) {
				Console.WriteLine("Exception caught: {0}",e);
				Environment.ExitCode=1;
				return;
			}
			loop++;
		}

		Environment.ExitCode = 0;
		Console.WriteLine("Test Passed");
	} 
}

