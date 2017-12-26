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
public class Co1519ctor_iiiiii
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime(int,int,int,int,int,int)";
 public static readonly String s_strTFName        = "Co1519ctor_iiiiii.cs";
 public static readonly String s_strTFAbbrev      = "Co1519";
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
   double do8c = -1.2;
   int in4a = -2;
   int in4b = -2;
   int in4c = -2;
   long lo8a = -2L;
   long lo8b = -2L;
   long lo8c = -2L;
   int in4NewUnit = -3;
   int in4PrevUnit = -4;
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
       in4SetHour = 15;
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
       in4a = dt2.Year;
       ++iCountTestcases;
       if ( in4SetYear != in4a )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_900ye!  in4a=="+ in4a );
	 }
       in4a = dt2.Month;
       ++iCountTestcases;
       if ( in4SetMonth != in4a )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_901mo!  in4a=="+ in4a );
	 }
       in4a = dt2.Day;
       ++iCountTestcases;
       if ( in4SetDayOfMonth != in4a )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_902da!  in4a=="+ in4a );
	 }
       in4a = dt2.Hour;
       ++iCountTestcases;
       if ( in4SetHour != in4a )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_903ho!  in4a=="+ in4a );
	 }
       in4a = dt2.Minute;
       ++iCountTestcases;
       if ( in4SetMinute != in4a )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_904mi!  in4a=="+ in4a );
	 }
       in4a = dt2. Second;
       ++iCountTestcases;
       if ( in4SetSecond != in4a )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_905se!  in4a=="+ in4a );
	 }
       in4SetYear = 12345;  
       in4SetMonth = 11;
       in4SetDayOfMonth = 27;
       in4SetHour = 13;
       in4SetMinute = 32;
       in4SetSecond = 41;
       try
	 {
	 ++iCountTestcases;
	 dt2 = new DateTime
	   (
	    in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	    ,in4SetHour ,in4SetMinute ,in4SetSecond
	    );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_940ye!  Should have thrown Excep.  dt2.GetYear()=="+ dt2.Year );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_941ye!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       in4SetYear = 0;  
       in4SetMonth = 11;
       in4SetDayOfMonth = 27;
       in4SetHour = 13;
       in4SetMinute = 32;
       in4SetSecond = 41;
       try
	 {
	 ++iCountTestcases;
	 dt2 = new DateTime
	   (
	    in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	    ,in4SetHour ,in4SetMinute ,in4SetSecond
	    );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_942ye!  Should have thrown Excep.  dt2.GetYear()=="+ dt2.Year );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_943ye!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       in4SetYear = 1999;
       in4SetMonth = 13;  
       in4SetDayOfMonth = 27;
       in4SetHour = 13;
       in4SetMinute = 32;
       in4SetSecond = 41;
       try
	 {
	 ++iCountTestcases;
	 dt2 = new DateTime
	   (
	    in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	    ,in4SetHour ,in4SetMinute ,in4SetSecond
	    );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_940mo!  Should have thrown Excep.  dt2.GetMonth()=="+ dt2.Month );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_941mo!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       in4SetYear = 1999;
       in4SetMonth = 0;  
       in4SetDayOfMonth = 27;
       in4SetHour = 13;
       in4SetMinute = 32;
       in4SetSecond = 41;
       try
	 {
	 ++iCountTestcases;
	 dt2 = new DateTime
	   (
	    in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	    ,in4SetHour ,in4SetMinute ,in4SetSecond
	    );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_942mo!  Should have thrown Excep.  dt2.GetMonth()=="+ dt2.Month );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_943mo!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       in4SetYear = 1999;
       in4SetMonth = 2;
       in4SetDayOfMonth = 29;  
       in4SetHour = 13;
       in4SetMinute = 32;
       in4SetSecond = 41;
       try
	 {
	 ++iCountTestcases;
	 dt2 = new DateTime
	   (
	    in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	    ,in4SetHour ,in4SetMinute ,in4SetSecond
	    );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_940da!  Should have thrown Excep.  dt2.GetDay()=="+ dt2.Day );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_941da!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       in4SetYear = 1999;
       in4SetMonth = 2;
       in4SetDayOfMonth = 0;  
       in4SetHour = 13;
       in4SetMinute = 32;
       in4SetSecond = 41;
       try
	 {
	 ++iCountTestcases;
	 dt2 = new DateTime
	   (
	    in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	    ,in4SetHour ,in4SetMinute ,in4SetSecond
	    );
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_942da!  Should have thrown Excep.  dt2.GetDay()=="+ dt2.Day );
	 }
       catch ( ArgumentException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_943da!  Generic excep show specific excep type was other than expected.  excep1=="+ excep1 );
	 }
       in4SetYear = 1999;
       in4SetMonth = 2;
       in4SetDayOfMonth = 9;
       in4SetHour = 24;  
       in4SetMinute = 3;
       in4SetSecond = -2;
       try
	 {
	 ++iCountTestcases;
	 dt2 = new DateTime
	   (
	    in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	    ,in4SetHour ,in4SetMinute ,in4SetSecond
	    );
	 if ( dt2.Hour != 0  ||  dt2.Day != 10 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_940ho!  Should have thrown Excep.  dt2.GetHour()=="+ dt2.Hour );
	   }
	 }
       catch ( ArgumentOutOfRangeException argexc )
	 {}
       catch ( Exception excep1 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_941ho!  No Exception expected here.  excep1=="+ excep1 );
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
   Co1519ctor_iiiiii cbA = new Co1519ctor_iiiiii();
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
