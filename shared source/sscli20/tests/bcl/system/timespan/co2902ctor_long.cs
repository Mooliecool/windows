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
using System.Threading;
using System;
using System.IO;
public class Co2902ctor_long
{
 public static String strName = "TimeSpan.Constructor_long";
 public static String strTest = "Co2902ctor_long";
 public static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
   TimeSpan	ts;
   String		strDefTime = "00:00:00";
   long[]		lTicks = { 0L, 10000L, 1000*10000L, -1 };
   String[]	strResults =
   {
     "00:00:00",
     "00:00:00.0010000",
     "00:00:01",
     "-00:00:00.0000001",
   };
   do
     {
     Console.Error.WriteLine( "[]  Construct and verify TimeSpan objects" );
     try
       {
       for ( int ii = 0; ii < lTicks.Length; ++ii )
	 {
	 ++iCountTestcases;
	 ts = new TimeSpan( lTicks[ii] );
	 if ( String.Equals( ts.ToString(), strResults[ii] ) != true )
	   {
	   String strInfo = strTest + " error: ";
	   strInfo = strInfo + "Expected TimeSpan string <" + strResults[ii] + "> ";
	   strInfo = strInfo + "Returned TimeSpan string <" + ts.ToString() + "> ";
	   Console.WriteLine( strTest+ "E_101b: " + strInfo );
	   ++iCountErrors;
	   break;
	   }
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_10001: Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
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
   Co2902ctor_long oCbTest = new Co2902ctor_long();
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
