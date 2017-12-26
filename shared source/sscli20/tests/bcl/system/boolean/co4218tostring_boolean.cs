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
public class Co4218ToString_Boolean
{
 static String strName = "Boolean.ToString";
 static String strTest = "Co4218ToString_Boolean";
 static String strEEVT = "\u0003";
 static String strEMSG = "\u001a";
 static String strDEBUGINFO =  "DEBUGINFO" ;
 static String strDEBUGINFOEEVT = strDEBUGINFO + strEEVT ;
 static void debugPoint( String strPoint )
   {
   Console.Error.Write( strDEBUGINFOEEVT );
   Console.Error.Write( "POINTTOBREAK: find " );
   Console.Error.Write( strPoint );
   Console.Error.Write( " (" );
   Console.Error.Write( strTest );
   Console.Error.Write( ".cs)" );
   Console.Error.WriteLine( strEMSG );
   }
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   Boolean[] boolValues = { true, false, true, false, true, false, true, false }; 
   String[] strResults = 
   {
     Boolean.TrueString ,
     Boolean.FalseString ,
     Boolean.TrueString ,
     Boolean.FalseString,
     Boolean.TrueString,
     Boolean.FalseString ,
     Boolean.TrueString ,
     Boolean.FalseString,
   };
   for ( int ii = 0; ii < boolValues.Length; ++ii )
     {
     iCountTestcases++;
     if ( boolValues[ii].ToString( null ).Equals( strResults[ii] ) != true )
       {
       Console.Error.Write( "ToString error: " );
       Console.Error.Write( "Test <" );
       Console.Error.Write( ii );
       Console.Error.Write( "> " );
       Console.Error.Write( "Expected Result = <" );
       Console.Error.Write( strResults[ii] );
       Console.Error.Write( "> " );
       Console.Error.Write( "Returned Result = <" );
       Console.Error.Write( boolValues[ii].ToString( null ) );
       Console.Error.WriteLine( "> " );
       debugPoint( "E_202" );
       iCountErrors++;
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
   Co4218ToString_Boolean oCbTest = new Co4218ToString_Boolean();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.Error.Write( strTest );
     Console.Error.WriteLine(  ": Main() Uncaught exception" );
     Console.Error.WriteLine( ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
