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
public class Co2922Equals
{
 public static String strName = "TimeSpan.Equals";
 public static String strTest = "Co2922Equals";
 public static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
   TimeSpan	ts1;
   TimeSpan	ts2;
   do
     {
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Verify Equality for positive TimeSpan" );
     try
       {
       ts1 = TimeSpan.Parse( "04:12:23.5" );
       long lTicks = ts1.Days * TimeSpan.TicksPerDay;
       lTicks += ts1.Hours * TimeSpan.TicksPerHour;
       lTicks += ts1.Minutes * TimeSpan.TicksPerMinute;
       lTicks += ts1.Seconds * TimeSpan.TicksPerSecond;
       lTicks += ts1.Milliseconds * TimeSpan.TicksPerMillisecond;
       ts2 = TimeSpan.FromTicks( lTicks );
       if ( TimeSpan.Equals( ts1, ts2 ) != true )
	 {
	 Console.WriteLine( strTest+ "E_101c: ts2 does not equal ts1" );
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
     Console.Error.WriteLine( "[]  Verify Equality for negative TimeSpan" );
     try
       {
       ts1 = TimeSpan.Parse( "-04:12:23.25" );
       long lTicks = -ts1.Days * TimeSpan.TicksPerDay;
       lTicks += -ts1.Hours * TimeSpan.TicksPerHour;
       lTicks += -ts1.Minutes * TimeSpan.TicksPerMinute;
       lTicks += -ts1.Seconds * TimeSpan.TicksPerSecond;
       lTicks += -ts1.Milliseconds * TimeSpan.TicksPerMillisecond;
       ts2 = TimeSpan.FromTicks( -lTicks );
       if ( TimeSpan.Equals( ts1, ts2 ) != true )
	 {
	 Console.WriteLine( strTest+ "E_202c: ts2 does not equal ts1" );
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
     Console.Error.WriteLine( "[]  Verify Equality for zero time" );
     try
       {
       ts1 = TimeSpan.Parse( "00:00:00.0" );
       long lTicks = ts1.Days * TimeSpan.TicksPerDay;
       lTicks += ts1.Hours * TimeSpan.TicksPerHour;
       lTicks += ts1.Minutes * TimeSpan.TicksPerMinute;
       lTicks += ts1.Seconds * TimeSpan.TicksPerSecond;
       lTicks += ts1.Milliseconds * TimeSpan.TicksPerMillisecond;
       ts2 = TimeSpan.FromTicks( lTicks );
       if ( TimeSpan.Equals( ts1, ts2 ) != true )
	 {
	 Console.WriteLine( strTest+ "E_303c: ts2 does not equal ts1" );
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
   Co2922Equals oCbTest = new Co2922Equals();
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
