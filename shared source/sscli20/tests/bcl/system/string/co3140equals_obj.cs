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
using System.Globalization;
using GenStrings;
using System;
using System.Collections;
public class Co3140Equals_Obj
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "String\\Co3140Equals_Obj. runTest() started." );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str1 = null;
   Object obj = null;
   str1 = "test";
   obj = null;
   iCountTestcases++;
   if(str1.Equals(obj))
     {
     iCountErrors++;
     print("E_83ia");
     }
   str1 = "test";
   obj = ('c');
   iCountTestcases++;
   if(str1.Equals(obj))
     {
     iCountErrors++;
     print("E_83sk");
     }
   str1 = "obj";
   obj = ('c');
   iCountTestcases++;
   if(str1.Equals(obj))
     {
     iCountErrors++;
     print("E_9sik");
     }
   str1 = "test test";
   obj = new StringBuilder("test test");
   iCountTestcases++;
   if(str1.Equals(obj))
     {
     iCountErrors++;
     print("E_38aa");
     }
   str1 = "test";
   obj = "test";
   iCountTestcases++;
   if(!str1.Equals(obj))
     {
     iCountErrors++;
     print("E_48qi");
     }
   IntlStrings intl = new IntlStrings();
   str1 = intl.GetString(3, true, true);
   obj = (object) str1;
   iCountTestcases++;
   if(!str1.Equals(obj))
     {
     iCountErrors++;
     print("E_48qi");
     }
   str1 = "TeSt";
   obj = "TeSt";
   iCountTestcases++;
   if(!str1.Equals(obj))
     {
     iCountErrors++;
     print("E_29di");
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Co3140Equals_Obj.)");
   Console.Out.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co3140Equals_Obj cb0 = new Co3140Equals_Obj();
   try
     {
     bResult = cb0.runTest();
     }
   catch ( System.Exception exc )
     {
     bResult = false;
     System.Console.Error.WriteLine( "Co3140Equals_Obj." );
     System.Console.Error.WriteLine( exc.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
