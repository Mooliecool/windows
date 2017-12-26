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
public class Co1511get_Hour
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1511get_Hour  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   int in4a = -2;
   int in4SetYear = 1988;
   int in4SetMonth = 7;
   int in4SetDayOfMonth = 20;
   int in4SetHour = 14;
   int in4SetMinute = 15;
   int in4SetSecond = 16;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="100bb";
       in4SetYear = 1988;
       in4SetMonth = 7;
       in4SetDayOfMonth = 20;
       in4SetHour = 11;
       in4SetMinute = 15;
       in4SetSecond = 16;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = dt2.Hour;
       ++iCountTestcases;
       if ( in4a != in4SetHour )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_330fj!  in4a=="+ in4a  );
	 }
       strLoc="110cc";
       in4SetYear = 1988;
       in4SetMonth = 7;
       in4SetDayOfMonth = 20;
       in4SetHour = 0;
       in4SetMinute = 15;
       in4SetSecond = 16;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = dt2.Hour;
       ++iCountTestcases;
       if ( in4a != in4SetHour )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_331dq!  in4a=="+ in4a  );
	 }
       strLoc="110cc";
       in4SetYear = 1988;
       in4SetMonth = 7;
       in4SetDayOfMonth = 20;
       in4SetHour = 14;
       in4SetMinute = 15;
       in4SetSecond = 16;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = dt2.Hour;
       ++iCountTestcases;
       if ( in4a != in4SetHour )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_332nb!  in4a=="+ in4a  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1511get_Hour) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co1511get_Hour.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co1511get_Hour.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1511get_Hour cbA = new Co1511get_Hour();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1511get_Hour) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1511get_Hour.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
