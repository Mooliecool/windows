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
public class Co1513AddMonths
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strClassMethod = "DateTime.AddMonths(int)";
 public static readonly String s_strTFName = "Co1513AddMonths.cs";
 public static readonly String s_strTFAbbrev = "Co1513";
 public static readonly String s_strTFPath = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   DateTime dt3;
   int in4a = -2;
   int in4b = -2;
   int in4c = -2;
   int in4NewMonth = -3;
   int in4PrevMonth = -4;
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
       in4a = 2;
       dt3 = dt2.AddMonths( in4a );
       in4PrevMonth = dt2.Month;
       in4NewMonth = dt3.Month;
       ++iCountTestcases;
       if ( in4NewMonth != ( in4PrevMonth + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_482sw!  in4NewMonth=="+ in4NewMonth );
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
       in4a = -3;
       dt3 = dt2.AddMonths( in4a );
       in4PrevMonth = dt2.Month;
       in4NewMonth = dt3.Month;
       ++iCountTestcases;
       if ( in4NewMonth != ( in4PrevMonth + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_707dw!  in4NewMonth=="+ in4NewMonth );
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
       in4a = 12 + 3;
       dt3 = dt2.AddMonths( in4a );
       in4PrevMonth = dt2.Month;
       in4NewMonth = dt3.Month;
       ++iCountTestcases;
       if ( in4NewMonth != ( in4PrevMonth + (in4a % 12) ) )  
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_142nw!  in4NewMonth=="+ in4NewMonth );
	 }
       in4b = dt2.Year;
       in4c = dt3.Year;
       ++iCountTestcases;
       if ( in4c != ( in4b + 1 ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_288gw!  in4c=="+ in4c );
	 }
       strLoc="160ee";
       in4SetYear = 2000;
       in4SetMonth = 1;
       in4SetDayOfMonth = 31;
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
       in4a = 1;
       dt3 = dt2.AddMonths( in4a );
       in4b = dt2.Month;
       in4c = dt3.Month;
       ++iCountTestcases;
       if ( in4c != ( in4b + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_906mc!  in4c=="+ in4c );
	 }
       in4c = dt3.Day;
       ++iCountTestcases;
       if ( in4c != 29 )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_917uc!  in4c=="+ in4c );
	 }
       strLoc="170ff";
       in4SetYear = 2000;
       in4SetMonth = 2;
       in4SetDayOfMonth = 29;
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
       in4a = 1;
       dt3 = dt2.AddMonths( in4a );
       in4b = dt2.Month;
       in4c = dt3.Month;
       ++iCountTestcases;
       if ( in4c != ( in4b + in4a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_666mc!  in4c=="+ in4c );
	 }
       in4c = dt3.Day;
       ++iCountTestcases;
       if ( in4c != 29 )   
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_667uc!  in4c=="+ in4c );
	 }
       dt3 = DateTime.Now;
       try
	 {
	 dt3.AddMonths(Int32.MinValue);
	 ++iCountErrors;
	 Console.WriteLine("Err_3947tsfg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine("Err_0745rtgf! Wrong exception thrown, " + ex.GetType().Name);
	 }
       try
	 {
	 dt3.AddMonths(Int32.MaxValue);
	 ++iCountErrors;
	 Console.WriteLine("Err_3947tsfg! Exception not thrown");
	 }
       catch(ArgumentOutOfRangeException)
	 {
	 }
       catch(Exception ex)
	 {
	 ++iCountErrors;
	 Console.WriteLine("Err_0745rtgf! Wrong exception thrown, " + ex.GetType().Name);
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
   Co1513AddMonths cbA = new Co1513AddMonths();
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
