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
public class Co2954Negate
{
 public static String strName = "TimeSpan.Negate";
 public static String strTest = "Co2954Negate";
 public static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Error.WriteLine( strName + ": " + strTest + " runTest started..." );
   TimeSpan	ts1;
   TimeSpan	ts2;
   TimeSpan	tsResult;
   do
     {
     ++iCountTestcases;
     Console.Error.WriteLine( "[]  Negate positive time to yield negative time" );
     try
       {
       ts1 = new TimeSpan(10*TimeSpan.TicksPerHour);
       tsResult = ts1.Negate();
       if ( tsResult.Ticks != -10*TimeSpan.TicksPerHour )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Ticks <" + -10*TimeSpan.TicksPerHour + "> ";
	 strInfo = strInfo + "Returned Ticks <" + tsResult.Ticks + "> ";
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
     Console.Error.WriteLine( "[]  Negate negative time to yield positive time" );
     try
       {
       ts1 = new TimeSpan(-10*TimeSpan.TicksPerHour);
       tsResult = ts1.Negate();
       if ( tsResult.Ticks != 10*TimeSpan.TicksPerHour )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Ticks <" + 10*TimeSpan.TicksPerHour + "> ";
	 strInfo = strInfo + "Returned Ticks <" + tsResult.Ticks + "> ";
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
     Console.Error.WriteLine( "[]  Negate zero time" );
     try
       {
       ts1 = new TimeSpan(0);
       tsResult = ts1.Negate();
       if ( tsResult.Ticks != 0 )
	 {
	 String strInfo = strTest + " error: ";
	 strInfo = strInfo + "Expected Ticks <" + 0 + "> ";
	 strInfo = strInfo + "Returned Ticks <" + tsResult.Ticks + "> ";
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
   Co2954Negate oCbTest = new Co2954Negate();
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
