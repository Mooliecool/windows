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

// this is the template used to generate regress4.il

using System;
using System.Security;

public class A
{
	protected int m_secureField;
        public A()
	{
		m_secureField = 0;
	}

	public int GetSecureField() //change to family in IL
	{
		return m_secureField ;
	}

}

public class B : A
{

	public B()
	{
		m_secureField = 1;
	}

}

delegate int Dlg();

public class C : A
{
	public static void Main()
	{
                try {
  		    Dlg d = new Dlg(new B().GetSecureField);
                    Console.WriteLine(d());
                    Console.WriteLine("FAILED: Verification exception not thrown");
                    Environment.Exit(1);
                }
                catch (VerificationException) {
                    Console.WriteLine("PASSED: Verification exception thrown");
                    Environment.Exit(0);
                }
                Console.WriteLine("FAILED: unexpected");
                Environment.Exit(1);
	}
}
