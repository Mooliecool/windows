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
public class Co4227ToString_Byte
{
 static String strName = "Byte.ToString_Byte";
 static String strTest = "Co4227ToString_Byte";
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   String strMade = null;
   String strKnown = null;;
   do
     {
     try
       {
       ++iCountTestcases;
       strMade = ((IConvertible)unchecked((SByte)0x80)).ToString( null );
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_lkyt8");
	 }
       strKnown = ( "-128" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_dh4");
	 }
       ++iCountTestcases;
       strMade = ((IConvertible)(SByte)0x7F).ToString( null );
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_kj4");
	 }
       strKnown = ( "127" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_hu3");
	 }
       ++iCountTestcases;
       strMade = ((IConvertible)(SByte)0x00).ToString( null );
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_k347");
	 }
       strKnown = ( "0" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_gfj43");
	 }
       ++iCountTestcases;
       strMade = ((IConvertible)unchecked((SByte)0xFF)).ToString( null );
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_wsh35");
	 }
       strKnown = ( "-1" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_fdk3");
	 }
       }
     catch ( Exception exc )
       {
       ++iCountErrors;
       print ("E_ej2");
       printexc (exc);
       }
     }
   while ( false );
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
   Co4227ToString_Byte oCbTest = new Co4227ToString_Byte();
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
