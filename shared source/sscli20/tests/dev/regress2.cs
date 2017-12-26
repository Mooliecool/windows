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

class A 
	{
	
	}

	class B
	{
		public int q;

           public static void Main() {
             try {
			object[,] oa = new B[1,1];
			oa[0,0] = new A();
			B[,] ba = (B[,])oa; // this should throw
			Console.WriteLine(ba[0,0].q);  // this is really an A which was newed 2 lines above
                        Environment.ExitCode = 1;
            }
            catch ( System.ArrayTypeMismatchException )
                 {
                   Console.WriteLine("TEST PASSED");
                   Environment.ExitCode = 0; 
                 }
            catch(Exception ex)
		 {
		   Console.WriteLine("TEST FAILED," + ex.ToString());
                   Environment.ExitCode = 1;
		 }
		}	
	}
