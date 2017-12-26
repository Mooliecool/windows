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
public class Co1512AddMinutes
{
 public const int s_in4MinutesPerYear = 525600;
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1512AddMinutes  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   DateTime dt3;
   int in4a = -2;
   int in4b = -2;
   int in4c = -2;
   int in4NewMinute = -3;
   int in4PrevMinute = -4;
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
       in4a = 5;
       dt3 = dt2.AddMinutes( (double)in4a );
       in4PrevMinute = dt2.Minute;
       in4NewMinute = dt3.Minute;
       ++iCountTestcases;
       if ( in4NewMinute != ( in4PrevMinute + in4a ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_330fj!  in4NewMinute=="+ in4NewMinute  );
	 }
       strLoc="120cc";
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
       in4a = -5;
       dt3 = dt2.AddMinutes( (double)in4a );
       in4PrevMinute = dt2.Minute;
       in4NewMinute = dt3.Minute;
       ++iCountTestcases;
       if ( in4NewMinute != ( in4PrevMinute + in4a ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_478sm!  in4NewMinute=="+ in4NewMinute  );
	 }
       strLoc="140dd";
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
       in4a = 60 + 5;
       dt3 = dt2.AddMinutes( (double)in4a );
       in4PrevMinute = dt2.Minute;
       in4NewMinute = dt3.Minute;
       ++iCountTestcases;
       if ( in4NewMinute != ( in4PrevMinute + (in4a % 60) ) )  
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_512fz!  in4NewMinute=="+ in4NewMinute  );
	 }
       in4b = dt2.Hour;
       in4c = dt3.Hour;
       ++iCountTestcases;
       if ( in4c != ( in4b + 1 ) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_513rz!  in4c=="+ in4c  );
	 }
       strLoc="160ee";
       in4SetYear = 1999;
       in4SetMonth = 6;
       in4SetDayOfMonth = 4;
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
       in4a = s_in4MinutesPerYear;
       dt3 = dt2.AddMinutes( (double)in4a );
       in4b = dt2.Day;
       in4c = dt3.Day;
       ++iCountTestcases;
       if ( in4c != ( in4b - 1 ) )  
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_906mc!  in4c=="+ in4c  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1512AddMinutes) strLoc=="+ strLoc +" ,exc_general==" + exc_general  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co1512AddMinutes.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co1512AddMinutes.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1512AddMinutes cbA = new Co1512AddMinutes();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1512AddMinutes) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1512AddMinutes.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
