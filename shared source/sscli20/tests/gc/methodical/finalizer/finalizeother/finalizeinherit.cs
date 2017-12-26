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

namespace One {
abstract class A
{
		
}

class B: A
{
	~B() {
		Console.WriteLine("In Finalize of B");
	}
}

class C: B
{
	public static int count=0;
	~C() {
		Console.WriteLine("In Finalize of C");
		count++;
	}
}
}

namespace Two {
using One;
class D: C
{
	
}
}

namespace Three {
using One;
using Two;
class Test
{
	static void Main() {
		
		B b = new B();
		C c = new C();
		D d = new D();

		A a = c;
		
		d=null;
		b=null;
		a=null;
		c=null;

		GC.Collect();
		GC.WaitForPendingFinalizers();
		
		if(C.count == 2) {
			Environment.ExitCode=0;
			Console.WriteLine("Test Passed!");
		}
		else {
			Environment.ExitCode=1;
			Console.WriteLine("Test Failed!");
		}
		
	}
}

}
