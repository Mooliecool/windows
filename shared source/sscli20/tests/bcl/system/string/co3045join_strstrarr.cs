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
public class Co3045Join_StrStrArr
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "String\\Co3045Join_StrStrArr. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String [] strArray = {"abcde","12345","ABCDEF","!@#$%"};
   String separator = null;
   String result = null;
   String check = null;
   IntlStrings intl = new IntlStrings();
   String stra = intl.GetString(1, true, true);
   String strb = intl.GetString(8, true, true);
   String strc = intl.GetString(3, true, true);
   String strd = intl.GetString(15, true, true);
   String[] values = {stra, strb, strc, strd};
   String seperator = "!@#$%";
   check = String.Concat(stra, seperator);
   check = String.Concat(check, strb);
   check = String.Concat(check, seperator);
   check = String.Concat(check, strc);
   check = String.Concat(check, seperator);
   check = String.Concat(check, strd);
   result = String.Join(seperator, values);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_14mm (Co3045Join_StrStrArr.)  result==" + result);
     }
   separator = String.Empty;
   check = "abcde12345ABCDEF!@#$%";
   result = String.Join(separator, strArray);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_32tt (Co3045Join_StrStrArr.)  result==" + result);
     }
   separator = "!@#$%";
   check = "abcde!@#$%12345!@#$%ABCDEF!@#$%!@#$%";
   result = String.Join(separator, strArray);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_22ww (Co3045Join_StrStrArr.)  result==" + result);
     }
   separator = "ABcdE";
   check = "abcdeABcdE12345ABcdEABCDEFABcdE!@#$%";
   result = String.Join(separator, strArray);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_88uu (Co3045Join_StrStrArr.)  result==" + result);
     }
   separator = "h!1K";
   strArray[1] = null;
   check = "abcdeh!1Kh!1KABCDEFh!1K!@#$%";
   result = String.Join(separator, strArray);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_238y (Co3045Join_StrStrArr.)  result==" + result);
     }
   strArray[1] = "12345";  
   separator = null;
   check = "abcde12345ABCDEF!@#$%";
   result = String.Join(separator, strArray);
   iCountTestcases++;
   if(!(result.Equals(check)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_111h (Co3045Join_StrStrArr.)  result==" + result);
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3045Join_StrStrArr cb0 = new Co3045Join_StrStrArr();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3045Join_StrStrArr." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
