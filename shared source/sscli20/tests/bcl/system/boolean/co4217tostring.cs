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
using System.Text;
public class Co4217ToString
{
 static String strName = "Boolean.ToString";
 static String strTest = "";
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   String strMsg = null;
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   Boolean BooleanObj = false;
   Boolean[] boolValues = { true, false, true, false, true, false, true, false }; 
   String[] strResults = 
   {
     Boolean.TrueString,
     Boolean.FalseString,
     Boolean.TrueString,
     Boolean.FalseString,
     Boolean.TrueString,
     Boolean.FalseString,
     Boolean.TrueString,
     Boolean.FalseString,
   };
   for ( int ii = 0; ii < boolValues.Length; ++ii )
     {
     iCountTestcases++;
     BooleanObj =  boolValues[ii] ;
     if ( BooleanObj.ToString().Equals( strResults[ii]) != true )
       {
       Console.Error.WriteLine(  "POINTTOBREAK: find error E_47ys (Co4217ToString)"  );
       strMsg = "EXTENDEDINFO: In loop ii==";
       strMsg = strMsg + ii.ToString() ;
       Console.Error.WriteLine(  strMsg );
       iCountErrors++;
       break;
       }
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strTest + "   iCountTestCases == " + iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL. " + strTest + "    iCountErrors==" + iCountErrors);
     return false;
     }
   }
 private void printexc(Exception exc)
   {
   String output = "EXTENDEDINFO: "+exc.ToString();
   Console.Error.WriteLine(output);
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Cb4201Put.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   String strMsg = null;
   Co4217ToString oCbTest = new Co4217ToString();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4217ToString.cs"  );
     strMsg = "EXTENDEDINFO: ";
     strMsg = strMsg + ex.ToString() ;
     Console.Error.WriteLine( strMsg  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
