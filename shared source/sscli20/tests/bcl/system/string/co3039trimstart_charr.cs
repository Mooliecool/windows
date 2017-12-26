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
using System.Globalization;
using GenStrings;
public class Co3039TrimStart_ChArr
{
 public Boolean runTest()
   {
   Console.Out.WriteLine( "String\\Co3039TrimStart_ChArr. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str1 = null;
   String str2 = null;
   String str3 = null;
   char[] cArr=null; 
   char[] cArr1= new Char[]{ 'e', 'F', 'G', 'H' }; 
   char[] cArr2= new Char[]{'E', 'F', 'G', 'H'}; 
   char[] cArr3= new Char[]{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}; 
   char[] cArr4= new Char[]{'a', 'b'}; 
   char[] cArr5= new Char[]{'!', '?', '(', '@', '#'}; 
   str1 = "               abcdefg";
   str2 = "abcdefg";
   str3 = str1.TrimStart(cArr);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_92si (Co3039TrimStart_ChArr.)");
     }
   str1 = "                 e           abcde     e      ";
   str2 = "e           abcde     e      ";
   str3 = str1.TrimStart(cArr);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_982q (Co3039TrimStart_ChArr.)");
     }
   str1 = "           \n\n\n\t\t\t\t\t\t\t\r\r\r\r\r\n\n\t\t\t   abcdefgh";
   str2 = "abcdefgh";
   str3 = str1.TrimStart(cArr);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_828i (Co3039TrimStart_ChArr.)");
     }
   str1 = "EEEEEEFGHABCD";
   str2 = "ABCD";
   str3 = str1.TrimStart(cArr1);
   iCountTestcases++;
   if(!(str3.Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_53si (Co3039TrimStart_ChArr.)");
     }
   str3 = str1.TrimStart(cArr2);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_32ww (Co3039TrimStart_ChArr.)");
     }
   str1 = "AAAAAAAAAAABCCCBBBCDDDDDEFGH";
   str2 = "";
   str3 = str1.TrimStart(cArr3);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_72as (Co3039TrimStart_ChArr.)");
     }
   str1 = "ababbababbbababcababbabbabbaba";
   str2 = "cababbabbabbaba";
   str3 = str1.TrimStart(cArr4);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_438t (Co3039TrimStart_ChArr.)");
     }
   IntlStrings intl = new IntlStrings();
   str2 = intl.GetString(15, false, true);
   // Make sure to add a character that's not in the TrimStart Array
   str1 = String.Concat("!!!!!!@#", "A" , str2);
   str3 = str1.TrimStart(cArr5);		
   if(!(str3.Equals("A" + str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_2550u (Co3039TrimStart_ChArr.)");
     }
   str1 = "!!!!!!!!?((@#!!$?%*%@!!??";
   str2 = "$?%*%@!!??";
   str3 = str1.TrimStart(cArr5);
   iCountTestcases++;
   if(!(str3.Equals(str2)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_280u (Co3039TrimStart_ChArr.)");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   Co3039TrimStart_ChArr cb0 = new Co3039TrimStart_ChArr();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3039TrimStart_ChArr." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
