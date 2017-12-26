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
using System.IO;
using System.Text;
using System;
public class BugAddYears
{
 public virtual void runTest()
   {
   DateTime dt2;
   DateTime dt3;
   dt3 = DateTime.Now;
   try
     {
     dt2 = dt3.AddYears(Int32.MinValue);
     Console.WriteLine("Bug??? Exception not thrown, {0}", dt2);
     }
   catch(ArgumentOutOfRangeException)
     {
     }
   catch(Exception ex)
     {
     Console.WriteLine("Wrong exception thrown, " + ex.GetType().Name);
     }
   try
     {
     dt2 = dt3.AddYears(Int32.MaxValue);
     Console.WriteLine("Bug??? Exception not thrown, {0}", dt2);
     }
   catch(ArgumentOutOfRangeException)
     {
     }
   catch(Exception ex)
     {
     Console.WriteLine("Wrong exception thrown, " + ex.GetType().Name);
     }
   }
 public static void Main( String[] args )
   {
   BugAddYears cbA = new BugAddYears();
   cbA.runTest();
   }
}
