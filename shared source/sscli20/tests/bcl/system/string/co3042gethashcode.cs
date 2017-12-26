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
public class Co3042GetHashCode
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "String\\Co3042GetHashCode. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str1 = null;
   String str2 = null;
   IntlStrings intl = new IntlStrings();
   str1 = intl.GetString(12, false, true);
   str2 = str1;
   if(str1.GetHashCode() != str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_498o (Co3042GetHashCode.)");
     }
   iCountTestcases++;
   str1 = "sdfjlksjdfhsdf ;flks ;lldk fas";
   str2 = "sdfjlksjdfhsdf ;flks ;lldk fas";
   if(str1.GetHashCode() != str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_498o (Co3042GetHashCode.)");
     }
   iCountTestcases++;
   str1 = "4874378828457y34";
   str2 = "4874378828457y34";
   if(str1.GetHashCode() != str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_433r (Co3042GetHashCode.)");
     }
   iCountTestcases++;
   str1 = "#$%@#$%@*#_@_!__!_!($#%JWE(@JA)(*#@$";
   str2 = "#$%@#$%@*#_@_!__!_!($#%JWE(@JA)(*#@$";
   if(str1.GetHashCode() != str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_879u (Co3042GetHashCode.)");
     }
   iCountTestcases++;
   str1 = "\0";
   str2 = "\0";
   if(str1.GetHashCode() != str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_899q (Co3042GetHashCode.)");
     }
   str1 = "\n\n\n\n\n\n\n\r\r\r\r\t\t\t\t\tuuuu\t\t\t\t\r\r\r\n%%%%%%%%^heyhw";
   str2 = "\n\n\n\n\n\n\n\r\r\r\r\t\t\t\t\tuuuu\t\t\t\t\r\r\r\n%%%%%%%%^heyhw";
   iCountTestcases++;
   if(str1.GetHashCode() != str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_888q (Co3042GetHashCode.)");
     }
   str1 = "\n\n\n\n\r\r\r\rt";
   str2 = "\n\n\n\n\r\r\r\r\t";
   iCountTestcases++;
   if(str1.GetHashCode() == str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_999p (Co3042GetHashCode.)");
     }
   str1 = "t";
   str2 = "T";
   iCountTestcases++;
   if(str1.GetHashCode() == str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_999r (Co3042GetHashCode.)");
     }
   str1 = "\0";
   str2 = "0";
   iCountTestcases++;
   if(str1.GetHashCode() == str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_111q (Co3042GetHashCode.)");
     }
   str1 = "111111111111111111111111111111111111111111111111111111111111111";
   str2 = "111111111111111111111111111111111111111111111111111111111111112";
   iCountTestcases++;
   if(str1.GetHashCode() == str2.GetHashCode())
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666a (Co3042GetHashCode.)");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3042GetHashCode cb0 = new Co3042GetHashCode();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3042GetHashCode." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
