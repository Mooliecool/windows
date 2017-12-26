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
public class Co4225ToString
{
 static String strName = "SignedByte.ToString";
 static String strTest = "Co4225ToString";
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   String strMade = null;
   String strKnown = null;;
   SByte SByteVal = (SByte)1;
   do
     {
     try
       {
       ++iCountTestcases;
       SByteVal = (SByte)(-3);
       strMade = SByteVal.ToString();
       Console.Out.WriteLine( strMade );
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_hg45");
	 }
       strKnown = "-3";
       if ( strMade.CompareTo( strKnown ) != 0 )
	 {
	 ++iCountErrors;
	 print ("E_k565");
	 }
       ++iCountTestcases;
       SByteVal = 3;
       strMade = SByteVal.ToString();
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_kt50");
	 }
       iCountTestcases++;
       strKnown = ( "3" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_kfy4");
	 }
       ++iCountTestcases;
       SByteVal = 0;
       strMade = SByteVal.ToString();
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_ot6");
	 }
       iCountTestcases++;
       strKnown = ( "0" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_hr8");
	 }
       ++iCountTestcases;
       SByteVal = unchecked((SByte)0x80);
       strMade = SByteVal.ToString();
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_fi");
	 }
       strKnown = ( "-128" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_fi2");
	 }
       ++iCountTestcases;
       SByteVal = (SByte)0x7F;
       strMade = SByteVal.ToString();
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_k387");
	 }
       strKnown = ( "127" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_fho");
	 }
       ++iCountTestcases;
       SByteVal = unchecked((SByte)0xFF);
       strMade = SByteVal.ToString();
       if ( strMade == null )
	 {
	 ++iCountErrors;
	 print ("E_ro");
	 }
       strKnown = ( "-1" );
       if ( strMade.Equals( strKnown ) == false )
	 {
	 ++iCountErrors;
	 print ("E_e3reu");
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
   Co4225ToString oCbTest = new Co4225ToString();
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
