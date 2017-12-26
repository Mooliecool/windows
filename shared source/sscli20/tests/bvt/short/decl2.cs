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
using System.Security.Permissions; 
[assembly:UIPermissionAttribute(SecurityAction.RequestMinimum, Unrestricted=true)]
  [assembly:PermissionSetAttribute(SecurityAction.RequestOptional, Name="Nothing")]
  class Decl2
{
 Decl2()
   {
   }
 Boolean foo2(String value)
   {
   if(value.Equals("hello world"))
     return true;
   else
     return false;
   }
 [UIPermissionAttribute(SecurityAction.Demand, Unrestricted=true)]
   Boolean foo(int i1, int i2, int i3, int i4, int i5)
   {
   if(i1 != 1 ||
      i2 != 2 ||
      i3 != 3 ||
      i4 != 4 ||
      i5 != 5 ||
      !foo2("hello world"))
     return false;
   else
     return true;
   }
 static public void Main(String[] args)  
   {
   Decl2 fileTest = new Decl2();
   if(fileTest.foo(1,2,3,4,5)) 
     {
     Console.Out.WriteLine("Success");
     Environment.ExitCode = 0;
     }
   else {
   Console.Out.WriteLine("Failed");
   Environment.ExitCode = 101;
   }
   }
}
