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
using GenStrings;
using System.Globalization;
using System.Text;
using System;
using System.Collections;
public class Co3146ctor_CharArr
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "String\\Co3146ctor_CharArr. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strError = null;
   char[] cArr2 = null;
   String str8 = null;
   String str9 = null;
   iCountTestcases++;
   try {
   str9 = new String(cArr2);
   if (!str9.Equals ("" )) {
   iCountErrors++;
   print("E_39jf");
   }
   } catch (ArgumentException   ) {}
   catch (Exception exc2)
     {
     iCountErrors++;
     print("E_27ai");
     strError = "EXTENDEDINFO: ";
     strError = strError + exc2.ToString();
     Console.Error.WriteLine(strError);
     }
   cArr2 = new char[10];
   for(int i=0;i<10;i++)
     cArr2[i] = '\0';
   StringBuilder strb = new StringBuilder("\0\0\0\0\0\0\0\0\0\0");
   str9 = new String(cArr2);
   str8 = "\0\0\0\0\0\0\0\0\0\0";
   iCountTestcases++;
   if(!str9.Equals(str8))  
     {
     iCountErrors++;
     print("E_57ai");
     }
   cArr2 = new char[10];
   cArr2[0] = 'a';
   cArr2[1] = 'b';
   cArr2[2] = 'c';
   cArr2[3] = 'd';
   cArr2[4] = 'e';
   cArr2[5] = 'f';
   cArr2[6] = 'g';
   cArr2[7] = 'h';
   cArr2[8] = 'i';
   cArr2[9] = 'j';
   str9 = new String( cArr2 );
   str8 = "abcdefghij";
   iCountTestcases++;
   if ( ! str9.Equals( str8 ) )
     {
     iCountErrors ++;
     print("E_89is");
     }
   IntlStrings intl = new IntlStrings();
   String testString = intl.GetString(8, false, true);
   cArr2 = testString.ToCharArray();
   str9 = new String(cArr2);
   if(! str9.Equals(testString)) {
   ++iCountErrors;
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3146ctor_CharArr.)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3146ctor_CharArr cb0 = new Co3146ctor_CharArr();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3146ctor_CharArr." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
