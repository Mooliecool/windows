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
using System;
using GenStrings;
using System.Globalization;
public class Co3038TrimEnd_ChArr
{
 public Boolean runTest()
   {
   Console.Out.WriteLine( "String\\Co3038TrimEnd_ChArr. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str1 = null;
   String str2 = null;
   String str3 = null;
   String str4 = null;
   char[] cArr=null; 
   char[] cArr1= new Char[]{ 'E', 'F', 'g', 'h' }; 
   char[] cArr2= new Char[]{'E', 'F', 'G', 'H'}; 
   char[] cArr3= new Char[]{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}; 
   char[] cArr4= new Char[]{'a', 'b'}; 
   char[] cArr5= new Char[]{'#', '$', '%', '@', '!', '*', '?'}; 
   char[] cArr6= new Char[]{}; 
   str1 = "  GeneMi  ";
   str2 = "  GeneMi";
   str3 = str1.TrimEnd(cArr6);
   str4 = str1.TrimEnd(null);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find Error_zer703 (Co3038TrimEnd_ChArr)");
     }
   iCountTestcases++;
   if(!(str3.Equals(str4)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find Error_zen603 (Co3038TrimEnd_ChArr)");
     }
   str1 = "abcdefg     ";
   str2 = "abcdefg";
   str3 = str1.TrimEnd(cArr);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_92si (Co3038TrimEnd_ChArr.)");
     }
   str1 = "abcde     e                     ";
   str2 = "abcde     e";
   str3 = str1.TrimEnd(cArr);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_982q (Co3038TrimEnd_ChArr.)");
     }
   IntlStrings intl = new IntlStrings();
   str1 = intl.GetString(8, false, true).TrimEnd();
   str2 = str1;
   str1 = String.Concat(str1, "           \n\n\n\t\t\t\t\t\t\t\r\r\r\r\r\n\n\t\t\t   ");
   str3 = str1.TrimEnd(cArr);
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_3228i (Co3038TrimEnd_ChArr.)");
     }
   str1 = "abcdefgh           \n\n\n\t\t\t\t\t\t\t\r\r\r\r\r\n\n\t\t\t   ";
   str2 = "abcdefgh";
   str3 = str1.TrimEnd(cArr);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_828i (Co3038TrimEnd_ChArr.)");
     }
   str1 = "ABCDEEEEEEFGH";
   str2 = "ABCD";
   str3 = str1.TrimEnd(cArr1);
   iCountTestcases++;
   if(!(str3.Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_280u (Co3038TrimEnd_ChArr.)");
     }
   str3 = str1.TrimEnd(cArr2);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_32ww (Co3038TrimEnd_ChArr.)");
     }
   str1 = "ABCDEFGH";
   str2 = "";
   str3 = str1.TrimEnd(cArr3);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_72as (Co3038TrimEnd_ChArr.)");
     }
   str1 = "ababbababbbababcababbabbabbaba";
   str2 = "ababbababbbababc";
   str3 = str1.TrimEnd(cArr4);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_438t (Co3038TrimEnd_ChArr.)");
     }
   str1 = "!!!!!!!!?((@#$?%*%@!!??";
   str2 = "!!!!!!!!?((";
   str3 = str1.TrimEnd(cArr5);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_279j (CB3038TrimTail.)");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   Co3038TrimEnd_ChArr cb0 = new Co3038TrimEnd_ChArr();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine(  "Co3038TrimEnd_ChArr." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
