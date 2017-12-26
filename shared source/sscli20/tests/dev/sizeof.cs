using System;
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

//
// This is .cs file on which the actual test, sizeof.il, is based on.
// You cannot simply compile this, ildasm the .exe and use that .il file
// directly. You must manually change the IL as indicated by the inline comments
//

public struct ValType
{
   int a;
   int b;
   int c;
}

public class RefType
{
   int a = 0;
   int b = 0;
   int c = 0;
   int d = 0;
}

public class SizeOfTests
{
   private static int s_failed = 0;
   public unsafe static int Main()
   {
      try
      {
         Container<int> Cint = new Container<int>();
         Container<double> Cdouble = new Container<double>();
         Container<ValType> CValType = new Container<ValType>();
         Container<RefType> CRefType = new Container<RefType>();

         SizeOfTests.Test(sizeof(System.Int32) == 4, "sizeof(System.Int32)");
         SizeOfTests.Test(sizeof(ValType) == 12, "sizeof(ValType)");
         SizeOfTests.Test(sizeof(ValType) == 4, "sizeof(RefType)"); // replace with RefType in IL

         SizeOfTests.Test(Cint.sizeofT() == 4, "Cint.sizeofT()");
         SizeOfTests.Test(Cdouble.sizeofT() == 8, "Cdouble.sizeofT()");
         SizeOfTests.Test(CValType.sizeofT() == 12, "CValType.sizeofT()");
         SizeOfTests.Test(CRefType.sizeofT() == 4, "CRefType.sizeofT()");
      }
      catch (Exception e)
      {
         Console.WriteLine("Caught Unexpected Exception");
         Console.WriteLine(e.ToString());
         return 1;
      }

      return s_failed;
   }

   public static void Test(bool test, string msg)
   {
      if(test)
      {
         Console.WriteLine("PASSED " + msg);
      }
      else
      {
         Console.WriteLine("FAILED " + msg);
         s_failed = 1;
      }
   }
}

public class Container<T>
{
   public unsafe int sizeofT()
   {
      return 4; // replace with sizeof !0 in IL
   }
}
