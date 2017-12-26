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
public class Co2940get_TotalHours
{
 public static String strName = "TimeSpan.GetTotalHours";
 public static String strTest = "Co2940get_TotalHours";
 public static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
   TimeSpan	ts;
   int iDays = -1;
   int iHours = -1;
   int iMinutes = -1;
   int iSeconds = -1;
   do
     {
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Verify GetTotalHours using a whole number of hours" );
     try
       {
       iDays = 4;
       iHours = 0;
       iMinutes = 0;
       iSeconds = 0;
       double dblHours = 24.0 * iDays;
       ts = new TimeSpan( iDays, iHours, iMinutes, iSeconds );
       if ( dblHours != ts.TotalHours )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Hours <" + dblHours + "> ";
	 strInfo = strInfo + "Returned Hours <" + ts.TotalHours + "> ";
	 Console.WriteLine( strTest+ "E_101a: " + strInfo );
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
     Console.Error.WriteLine( "[]  Verify GetTotalHours using fractional hours" );
     try
       {
       iDays = 4;
       iHours = 12;
       iMinutes = 0;
       iSeconds = 0;
       double dblHours = 24.0 * iDays + iHours;
       ts = new TimeSpan( iDays, iHours, iMinutes, iSeconds );
       if ( dblHours != ts.TotalHours )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Hours <" + dblHours + "> ";
	 strInfo = strInfo + "Returned Hours <" + ts.TotalHours + "> ";
	 Console.WriteLine( strTest+ "E_202a: " + strInfo );
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
     Console.Error.WriteLine( "[]  Verify GetTotalHours using negative time" );
     try
       {
       iDays = -4;
       iHours = -6;
       iMinutes = -0;
       iSeconds = -0;
       double dblHours = -( 24.0 * -iDays + -iHours);
       ts = new TimeSpan( iDays, iHours, iMinutes, iSeconds );
       if ( dblHours != ts.TotalHours )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Hours <" + dblHours + "> ";
	 strInfo = strInfo + "Returned Hours <" + ts.TotalHours + "> ";
	 Console.WriteLine( strTest+ "E_202a: " + strInfo );
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
     Console.Error.WriteLine( "[]  Verify GetTotalHours using zero time" );
     try
       {
       iDays = 0;
       iHours = 0;
       iMinutes = 0;
       iSeconds = 0;
       double dblHours = 24.0 * iDays;
       ts = new TimeSpan( iDays, iHours, iMinutes, iSeconds );
       if ( dblHours != ts.TotalHours )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Hours <" + dblHours + "> ";
	 strInfo = strInfo + "Returned Hours <" + ts.TotalHours + "> ";
	 Console.WriteLine( strTest+ "E_202a: " + strInfo );
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
   Co2940get_TotalHours oCbTest = new Co2940get_TotalHours();
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
