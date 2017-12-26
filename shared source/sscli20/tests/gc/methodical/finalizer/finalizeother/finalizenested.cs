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
using System.Threading;

public class Test {

	public class D {
		public D() {
		}
		~D() {
			Console.WriteLine("In Finalize() of D");
			Thread.Sleep(1000);	
		}
	}

	public class C {
		public D d;

		public C() {
			d = new D();
		}
		~C() {
			Console.WriteLine("In Finalize() of C");
			d=null;	
			Thread.Sleep(1000);	
		}
	}

	public class B {
		public C c;

		public B() {
			c = new C();
		}
		~B() {
			Console.WriteLine("In Finalize() of B");
			c=null;	
			Thread.Sleep(1000);	
		}
	}

	public class A {
		public B b;

		public A() {
			b = new B();
		}

		~A() {
			Console.WriteLine("In Finalize() of A");	
			b=null;
			Thread.Sleep(1000);	
		}
	}

	public class Dummy {

		public A a;
		public static bool visited;

		public Dummy() {
			a = new A();
		}

		~Dummy() {
			Console.WriteLine("In Finalize() of Dummy");	
			a=null;
			visited=true;
		}
	}

	public static void Main() {

		Dummy obj = new Dummy();
			
		obj=null;
		GC.Collect();
		
		GC.WaitForPendingFinalizers();  // makes sure Finalize() is called.

		if(Dummy.visited == true) {
			Environment.ExitCode = 0;
			Console.WriteLine("Test for Nested Finalize() passed!");
		}
		else {
			Environment.ExitCode = 1;
			Console.WriteLine("Test for Nested Finalize() failed!");
		}
	}
}
