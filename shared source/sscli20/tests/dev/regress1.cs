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

class C 
{
    private string s = "This is private";
}

class B {
    public string t = "This is safe";
}

class Class1
{
	static void Main(string[] args)
	{
             try {
        		B[,] ab = new B[1,1];
        		object[,] ao = ab;
        		ao[0,0] = new C();
        		Console.WriteLine(ab[0,0].t);
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
