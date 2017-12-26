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
public class Co2936get_Ticks
{
 public static String strName = "TimeSpan.GetTicks";
 public static String strTest = "Co2936get_Ticks";
 public static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
   TimeSpan	ts;
   do
     {
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Construct positive TimeSpan object, obtain and verify tick count" );
     try
       {
       ts = TimeSpan.Parse( "04:12:23.5" );
       long lTicks = ts.Days * TimeSpan.TicksPerDay;
       lTicks += ts.Hours * TimeSpan.TicksPerHour;
       lTicks += ts.Minutes * TimeSpan.TicksPerMinute;
       lTicks += ts.Seconds * TimeSpan.TicksPerSecond;
       lTicks += ts.Milliseconds * TimeSpan.TicksPerMillisecond;
       if ( lTicks != ts.Ticks )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Ticks <" + lTicks + "> ";
	 strInfo = strInfo + "Returned Ticks <" + ts.Ticks + "> ";
	 Console.WriteLine( strTest+ "E_101c: " + strInfo );
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
     Console.Error.WriteLine( "[]  Construct another TimeSpan object where milliseconds contains a nonzero value" );
     try
       {
       ts = TimeSpan.Parse( "00:00:00.547" );
       long lTicks = ts.Days * TimeSpan.TicksPerDay;
       lTicks += ts.Hours * TimeSpan.TicksPerHour;
       lTicks += ts.Minutes * TimeSpan.TicksPerMinute;
       lTicks += ts.Seconds * TimeSpan.TicksPerSecond;
       lTicks += ts.Milliseconds * TimeSpan.TicksPerMillisecond;
       if ( lTicks != ts.Ticks )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Ticks <" + lTicks + "> ";
	 strInfo = strInfo + "Returned Ticks <" + ts.Ticks + "> ";
	 Console.WriteLine( strTest+ "E_202c: " + strInfo );
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
     Console.Error.WriteLine( "[]  Construct TimeSpan object using negative hours, month and seconds" );
     try
       {
       ts = TimeSpan.Parse( "-04:12:23.25" );
       long lTicks = -ts.Days * TimeSpan.TicksPerDay;
       lTicks += -ts.Hours * TimeSpan.TicksPerHour;
       lTicks += -ts.Minutes * TimeSpan.TicksPerMinute;
       lTicks += -ts.Seconds * TimeSpan.TicksPerSecond;
       lTicks += -ts.Milliseconds * TimeSpan.TicksPerMillisecond;
       if ( -lTicks != ts.Ticks )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Ticks <" + -lTicks + "> ";
	 strInfo = strInfo + "Returned Ticks <" + ts.Ticks + "> ";
	 Console.WriteLine( strTest+ "E_303c: " + strInfo );
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
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Construct TimeSpan object using zero time" );
     try
       {
       ts = TimeSpan.Parse( "00:00:00.0" );
       long lTicks = -ts.Days * TimeSpan.TicksPerDay;
       lTicks += -ts.Hours * TimeSpan.TicksPerHour;
       lTicks += -ts.Minutes * TimeSpan.TicksPerMinute;
       lTicks += -ts.Seconds * TimeSpan.TicksPerSecond;
       lTicks += -ts.Milliseconds * TimeSpan.TicksPerMillisecond;
       if ( lTicks != ts.Ticks )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Ticks <" + lTicks + "> ";
	 strInfo = strInfo + "Returned Ticks <" + ts.Ticks + "> ";
	 Console.WriteLine( strTest+ "E_404c: " + strInfo );
	 ++iCountErrors;
	 break;
	 }
       }
     catch (Exception ex)
       {
       Console.WriteLine( strTest+ "E_40404: Unexpected Exception: " + ex.ToString() );
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
   Co2936get_Ticks oCbTest = new Co2936get_Ticks();
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
