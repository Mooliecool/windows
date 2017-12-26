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
public class Co1517AddYears
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.AddYears(int)";
 public static readonly String s_strTFName        = "Co1517AddYears.cs";
 public static readonly String s_strTFAbbrev      = "Co1517";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source at "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   DateTime dt3;
   double do8a = -1.2;
   double do8b = -1.2;
   int in4a = -2;
   int in4b = -2;
   int in4c = -2;
   int in4NewYear = -3;
   int in4PrevYear = -4;
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
       dt3 = dt2.AddYears( in4a );
       in4PrevYear = dt2.Year;
       in4NewYear = dt3.Year;
       ++iCountTestcases;
       if ( in4NewYear != ( in4PrevYear + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_730iy!  in4NewYear=="+ in4NewYear );
	 }
       strLoc="Loc_104hb";
       in4SetYear = 2001;
       in4SetMonth = 1;
       in4SetDayOfMonth = 2;
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
       in4a = -3;
       dt3 = dt2.AddYears( in4a );
       in4PrevYear = dt2.Year;
       in4NewYear = dt3.Year;
       ++iCountTestcases;
       if ( in4NewYear != ( in4PrevYear + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_734ry!  in4NewYear=="+ in4NewYear );
	 }
       strLoc="Loc_120cc";
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
       in4a = -3;
       dt3 = dt2.AddYears( in4a );
       in4PrevYear = dt2.Year;
       in4NewYear = dt3.Year;
       ++iCountTestcases;
       if ( in4NewYear != ( in4PrevYear + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_771qm!  in4NewYear=="+ in4NewYear );
	 }
       strLoc="Loc_160ee";
       in4SetYear = 2000;
       in4SetMonth = 2;
       in4SetDayOfMonth = 29;
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
       in4a = 1;
       dt3 = dt2.AddYears( in4a );
       in4b = dt2.Day;
       in4c = dt3.Day;
       ++iCountTestcases;
       if ( in4c != ( in4b - 1 ) )   
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_770ts!  in4c=="+ in4c );
	 }
       iCountTestcases++;
       in4SetYear = 2000;
       in4SetMonth = 11;
       in4SetDayOfMonth = 27;
       in4SetHour = 13;
       in4SetMinute = 12;
       in4SetSecond = 11;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       strLoc="Loc_200aa";
       in4a = 9997;
       try
	 {
	 ++iCountTestcases;
	 dt3 = dt2.AddYears( in4a );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_744cb!  Should have thrown Excep.  dt3.GetYear()=="+ dt3.Year );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_744eu!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       strLoc="Loc_210bb";
       in4a = -( in4SetYear );
       ++iCountTestcases;
       try
	 {
	 dt3 = dt2.AddYears( in4a );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_731cb!  Should have thrown Excep.  dt3.GetYear()=="+ dt3.Year );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_731eu!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       strLoc="Loc_220cc";
       in4a = -( in4SetYear + 7 );
       ++iCountTestcases;
       try
	 {
	 dt3 = dt2.AddYears( in4a );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_756cb!  Should have thrown Excep.  dt3.GetYear()=="+ dt3.Year );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_756eu!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
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
   Co1517AddYears cbA = new Co1517AddYears();
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
