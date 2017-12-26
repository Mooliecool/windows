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
public class Co4215Equals
{
 internal static String strName = "Boolean.Equals";
 internal static String strTest = "";
 public virtual bool runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   bool b2 = false;
   Boolean boo2 = false;
   Boolean boo3 = false;
   iCountTestcases++;
   b2 = true;
   boo2 = 	 b2;
   boo3 = 	 b2 ;
   if ( ! boo2.Equals( boo3 ) )
     {
     iCountErrors += 1;
     print( "E_185jz!" );
     }
   iCountTestcases++;
   b2 = true;
   boo2 = 	 b2 ;
   boo3 = 	 ! b2 ;
   if ( boo2.Equals( boo3 ) )
     {
     iCountErrors += 1;
     print ( "E_258em!" );
     }
   iCountTestcases++;
   b2 = false;
   boo2 = 	 b2 ;
   boo3 = 	 ! b2 ;
   if ( boo2.Equals( boo3 ) )
     {
     iCountErrors += 1;
     print ( "E_231wn!" );
     }
   iCountTestcases++;
   b2 = false;
   boo2 = 	 b2 ;
   boo3 = 	 b2 ;
   if ( ! boo2.Equals( boo3 ) )
     {
     iCountErrors += 1;
     print ( "E_311ah!" );
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
   bool bResult = false;	
   Co4215Equals oCbTest = new Co4215Equals();
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
