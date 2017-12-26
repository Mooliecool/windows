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
using System.Security; 
class Decl1
{
 Decl1()
   {
   }
 Boolean foo(int i1, int i2, int i3, int i4, int i5)
   {
   if(i1 != 1 ||
      i2 != 2 ||
      i3 != 3 ||
      i4 != 4 ||
      i5 != 5)
     return false;
   try {
   (new UIPermission(PermissionState.Unrestricted)).Demand();
   return false;
   }
   catch(SecurityException) {
   return true;
   }
   }
 [UIPermissionAttribute(SecurityAction.Deny, Unrestricted=true)]
   public Boolean callthrough()
   {
   return foo(1,2,3,4,5);
   }
 static public void Main(String[] args)  
   {
   Decl1 fileTest = new Decl1();
   if(fileTest.callthrough())
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
