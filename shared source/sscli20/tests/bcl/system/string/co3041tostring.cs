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
using System.Globalization;
using GenStrings;
using System;
public class Co3041ToString
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "String\\Co3041ToString. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str1 = null;
   iCountTestcases++;
   IntlStrings intl = new IntlStrings();
   str1 = intl.GetString(17, false, true);
   if(!(str1.ToString().Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_498o (Co3041ToString.)");
     }
   iCountTestcases++;
   str1 = "4874378828457y34";
   if(!(str1.ToString().Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_433r (Co3041ToString.)");
     }
   iCountTestcases++;
   str1 = "#$%@#$%@*#_@_!__!_!($#%JWE(@JA)(*#@$";
   if(!(str1.ToString().Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_879u (Co3041ToString.)");
     }
   iCountTestcases++;
   str1 = "\0";
   if(!(str1.ToString().Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_899 (Co3041ToString.)");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3041ToString cb0 = new Co3041ToString();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3041ToString." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
