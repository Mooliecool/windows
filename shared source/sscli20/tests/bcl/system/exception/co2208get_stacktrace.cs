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
using System;
using System.IO;
public class Co2208get_StackTrace
{
 public static String strName = "Exception.PrintStackTrace";
 public static String strTest = "Co2208get_StackTrace";
 public static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
   Exception		myException = null;
   String			strException = strTest + " Exception Test";
   do
     {
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Contruct exception object" );
     try
       {
       myException = new Exception( strException );
       if ( myException == null )
	 {
	 Console.WriteLine( strTest+ "E_101: Failed to create exception object" );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_10101: Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Throw the exception object" );
     try
       {
       throw myException;
       }
     catch ( Exception ex )
       {
       if ( ex.Message.CompareTo( strException ) != 0 )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected exception message <" + strException + "> ";
	 strInfo = strInfo + "Returned exception message <" + ex.Message + "> ";
	 Console.WriteLine( strTest+ "E_202a: " + strInfo );
	 ++iCountErrors;
	 break;
	 }
       String strStackTrace = ex.StackTrace;
       if ( strStackTrace.IndexOf( "runTest", 0 ) == -1 ||
	    strStackTrace.IndexOf( "Co2208get_StackTrace", 0 ) == -1 )
	 {
	 Console.WriteLine( strTest+ "E_202b: Invalid Stack Trace\n" + strStackTrace );
	 ++iCountErrors;
	 break;
	 }
       }
     }
   while ( false );
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( strTest + " iCountTestcases==" + iCountTestcases + " paSs" );
     return true;
     }
   else
     {
     System.String strFailMsg = null;
     Console.WriteLine( strTest+ strPath );
     Console.WriteLine( strTest+ "FAiL" );
     Console.Error.WriteLine( strTest + " iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co2208get_StackTrace oCbTest = new Co2208get_StackTrace();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.WriteLine( strTest+ strPath );
     Console.WriteLine( strTest+ "E_1000000" );
     Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
     Console.WriteLine( strTest+ ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
