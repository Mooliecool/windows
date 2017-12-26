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
using System.Security;
[assembly:AllowPartiallyTrustedCallersAttribute] 

public class TestClass{

	public static int Main(string[] args){
          try {
          Console.WriteLine("initial field value: " + A.getField()) ;
          B.C.foo(5);		
          Console.WriteLine("final field value: " + A.getField()) ;
          return 0;
          }
          catch {
          return 1;
          }
	}

}

public class A{

protected static int protInt=0;
public static int getField(){
	return protInt;
}

}

public class B : A{


public class C{

  public static void foo(int i){
	A.protInt=i;
  }

}

}

