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
public class Co1515AddSeconds
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer = "";
 public static readonly String s_strClassMethod = "DateTime.AddSeconds(double)";
 public static readonly String s_strTFName = "Co1515AddSeconds.cs";
 public static readonly String s_strTFAbbrev = "Co1515";
 public static readonly String s_strTFPath = "";
 public const int s_in4NumInNextHigherUnit = 60;  
 public const int s_in4NumSecondsPerDay = 86400;
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source at "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   DateTime dt3;
   int in4a = -2;
   int in4b = -2;
   int in4c = -2;
   int in4NewSecond = -3;
   int in4PrevSecond = -4;
   int in4SetYear = 1988;
   int in4SetMonth = 7;
   int in4SetDayOfMonth = 20;
   int in4SetHour = 14;
   int in4SetMinute = 15;
   int in4SetSecond = 16;
   try
     {
     do
       {
       strLoc="Loc_100bb";
       in4SetYear = 1999;
       in4SetMonth = 7;
       in4SetDayOfMonth = 10;
       in4SetHour = 11;
       in4SetMinute = 12;
       in4SetSecond = 13;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = 2;
       dt3 = dt2.AddSeconds( (double)in4a );
       in4PrevSecond = dt2. Second;
       in4NewSecond = dt3. Second;
       ++iCountTestcases;
       if ( in4NewSecond != ( in4PrevSecond + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_530iy!  in4NewSecond=="+ in4NewSecond );
	 }
       strLoc="Loc_184wg";
       in4SetYear = 1999;
       in4SetMonth = 12;
       in4SetDayOfMonth = 31;
       in4SetHour = 23;
       in4SetMinute = 59;
       in4SetSecond = 13;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = 60;
       dt3 = dt2.AddSeconds( (double)in4a );
       in4PrevSecond = dt2.Second;
       in4NewSecond = dt3. Second;
       ++iCountTestcases;
       if ( in4NewSecond != in4PrevSecond )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_530iya!  in4NewSecond=="+ in4NewSecond );
	 }
       ++iCountTestcases;
       if ( dt3.Day != 1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_4y8c3!  in4NewSecond=="+ in4NewSecond );
	 }
       strLoc="Loc_184wg";
       in4SetYear = 2000;
       in4SetMonth = 2;
       in4SetDayOfMonth = 28;
       in4SetHour = 23;
       in4SetMinute = 59;
       in4SetSecond = 13;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = 60;
       dt3 = dt2.AddSeconds( (double)in4a );
       in4PrevSecond = dt2. Second;
       in4NewSecond = dt3. Second;
       ++iCountTestcases;
       if ( in4NewSecond != in4PrevSecond )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_530iya!  in4NewSecond=="+ in4NewSecond );
	 }
       ++iCountTestcases;
       if ( dt3.Day != 29 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_534tya!  in4NewSecond=="+ in4NewSecond );
	 }
       strLoc="Loc_184wg";
       in4SetYear = 2000;
       in4SetMonth = 2;
       in4SetDayOfMonth = 29;
       in4SetHour = 23;
       in4SetMinute = 59;
       in4SetSecond = 13;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = 60;
       dt3 = dt2.AddSeconds( (double)in4a );
       in4PrevSecond = dt2. Second;
       in4NewSecond = dt3. Second;
       ++iCountTestcases;
       if ( in4NewSecond != in4PrevSecond )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_544iyb!  in4NewSecond=="+ in4NewSecond );
	 }
       ++iCountTestcases;
       Console.WriteLine(dt3.Day);
       if ( dt3.Day != 1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_545tyb!  in4NewSecond=="+ in4NewSecond );
	 }
       strLoc="Loc_120cca";
       in4SetYear = 3000;   
       in4SetMonth = 1;
       in4SetDayOfMonth = 1;
       in4SetHour = 0;
       in4SetMinute = 0;
       in4SetSecond = 2;
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
       dt3 = dt2.AddSeconds( (double)in4a );
       in4PrevSecond = dt2. Second;
       in4NewSecond = dt3. Second;
       ++iCountTestcases;
       if ( in4NewSecond != 57 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_571qma!  in4NewSecond=="+ in4NewSecond );
	 }
       strLoc="Loc_140dd";
       in4SetYear = 1999;
       in4SetMonth = 7;
       in4SetDayOfMonth = 10;
       in4SetHour = 11;
       in4SetMinute = 12;
       in4SetSecond = 13;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = s_in4NumInNextHigherUnit + 2;
       dt3 = dt2.AddSeconds( (double)in4a );
       in4PrevSecond = dt2. Second;
       in4NewSecond = dt3. Second;
       ++iCountTestcases;
       if ( in4NewSecond != ( in4PrevSecond + (in4a % s_in4NumInNextHigherUnit) ) )  
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_595pk!  in4NewSecond=="+ in4NewSecond );
	 }
       in4b = dt2.Minute;
       in4c = dt3.Minute;
       ++iCountTestcases;
       if ( in4c != ( in4b + 1 ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_531jo!  in4c=="+ in4c );
	 }
       strLoc="Loc_160ee";
       in4SetYear = 2000;
       in4SetMonth = 2;
       in4SetDayOfMonth = 28;
       in4SetHour = 11;
       in4SetMinute = 12;
       in4SetSecond = 13;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       in4a = s_in4NumInNextHigherUnit * 60 * 24 * 3;
       dt3 = dt2.AddSeconds( (double)in4a );
       in4b = dt2. Second;
       in4c = dt3. Second;
       ++iCountTestcases;
       if ( in4c != ( in4b ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_570ts!  in4c=="+ in4c );
	 }
       in4c = dt3.Day;
       ++iCountTestcases;
       if ( in4c != 2 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_526eg!  in4c=="+ in4c );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1515AddSeconds cbA = new Co1515AddSeconds();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev +"FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFName +s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
