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
public class Co2914CompareTo
{
 public static String strName = "TimeSpan.CompareTo";
 public static String strTest = "Co2914CompareTo";
 public static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
   TimeSpan	ts1;
   TimeSpan	ts2;
   String	strTime = null;
   int		iHours = -1;
   int		iMinutes = -1;
   int		iSeconds = -1;
   do
     {
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Compare timespan object less than" );
     try
       {
       strTime = "12:23:34";
       iHours = 12;
       iMinutes = 23;
       iSeconds = 34;
       ts1 = new TimeSpan( 23, 9, 30 );
       ts2 = new TimeSpan( 4, 12, 23, 34 );
       if ( ts1.CompareTo( ts2 ) >= 0 )
	 {
	 Console.WriteLine( strTest+ "E_101a: Expected ts1 to be less than ts2" );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_10001: Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Compare timespan object equal" );
     try
       {
       strTime = "12:23:34";
       iHours = 12;
       iMinutes = 23;
       iSeconds = 34;
       ts1 = new TimeSpan( 4, 12, 23, 34 );
       ts2 = new TimeSpan( 4, 12, 23, 34 );
       if ( ts1.CompareTo( ts2 ) != 0 )
	 {
	 Console.WriteLine( strTest+ "E_202a: Expected ts1 to be equal to ts2" );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_20202: Unexpected Exception: " + ex.ToString() );
       ++iCountErrors;
       break;
       }
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Compare timespan object greater than" );
     try
       {
       strTime = "12:23:34";
       iHours = 12;
       iMinutes = 23;
       iSeconds = 34;
       ts1 = new TimeSpan( 4, 12, 23, 34 );
       ts2 = new TimeSpan( 23, 9, 30 );
       if ( ts1.CompareTo( ts2 ) <= 0 )
	 {
	 Console.WriteLine( strTest+ "E_303a: Expected ts1 to be greater than ts2" );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_30303: Unexpected Exception: " + ex.ToString() );
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
   Co2914CompareTo oCbTest = new Co2914CompareTo();
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
