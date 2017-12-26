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
using System.Threading;
using System.Globalization;
using System;
public class Co1518AddTicks
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.AddTicks(int)";
 public static readonly String s_strTFName        = "Co1518AddTicks.cs";
 public static readonly String s_strTFAbbrev      = "Co1518";
 public static readonly String s_strTFPath        = "";
 public const int s_in4NumInNextHigherUnit = 10000;  
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source at "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;  
   DateTime dt3;
   String str2 = null;
   int in4a = -1;
   int in4b = -1;
   int in4c = -1;
   long lo8a = -1L;
   long lo8b = -1L;
   long lo8c = -1L;
   long lo8NewTick = -3L;
   long lo8PrevTick = -4L;
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
       strLoc="Loc_103bd";
       in4SetYear = 1;
       in4SetMonth = 1;
       in4SetDayOfMonth = 1;
       in4SetHour = 0;
       in4SetMinute = 0;
       in4SetSecond = 0;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       ++iCountTestcases;
       try
	 {
	 str2 = dt2.ToString();  
	 }
       catch ( Exception excep1 )
	 {
	 if(Thread.CurrentThread.CurrentCulture.LCID==0x0409){
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_876ts!" );
	 }
	 }
       lo8a = (long)( 1 );
       dt3 = dt2.AddTicks( lo8a );
       lo8PrevTick = dt2.Ticks;
       lo8NewTick = dt3.Ticks;
       ++iCountTestcases;
       if ( 0L != ( lo8PrevTick ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_803xu!  lo8PrevTick=="+ lo8PrevTick );
	 }
       ++iCountTestcases;
       if ( 1L != ( lo8NewTick ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_804bu!  lo8NewTick=="+ lo8NewTick );
	 }
       strLoc="Loc_102bc";
       in4SetYear = 1999;
       in4SetMonth = 7;
       in4SetDayOfMonth = 10;
       in4SetHour = 12;
       in4SetMinute = 13;
       in4SetSecond = 14;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       lo8a = (long)( s_in4NumInNextHigherUnit * 1000 );  
       dt3 = dt2.AddTicks( lo8a );
       in4a = dt2. Second;
       in4b = dt3. Second;
       ++iCountTestcases;
       if ( in4b != ( in4a + 1 ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_805np!  lo8PrevTick=="+ lo8PrevTick );
	 }
       strLoc="Loc_100bb";
       in4SetYear = 1999;
       in4SetMonth = 7;
       in4SetDayOfMonth = 10;
       in4SetHour = 12;
       in4SetMinute = 13;
       in4SetSecond = 14;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,in4SetHour
	  ,in4SetMinute
	  ,in4SetSecond
	  );
       lo8PrevTick = dt2.Ticks;
       ++iCountTestcases;
       if ( lo8PrevTick < 630672055940000000L )  
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_830my!  lo8PrevTick=="+ lo8PrevTick );
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
       lo8a = -3L;
       dt3 = dt2.AddTicks( lo8a );
       lo8PrevTick = dt2.Ticks;
       lo8NewTick = dt3.Ticks;
       ++iCountTestcases;
       if ( lo8NewTick != ( lo8PrevTick + lo8a ) )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_871qm!  lo8NewTick=="+ lo8NewTick );
	 }
       strLoc="Loc_120cc";
       ++iCountTestcases;
       if(Thread.CurrentThread.CurrentCulture.LCID==0x0409){
       in4SetYear = 1;
       in4SetMonth = 1;
       in4SetDayOfMonth = 1;
       in4SetHour = 0;
       in4SetMinute = 0;
       in4SetSecond = 0;
       dt2 = new DateTime
	 (
	  in4SetYear ,in4SetMonth ,in4SetDayOfMonth
	  ,in4SetHour ,in4SetMinute ,in4SetSecond
	  );
       strLoc="Loc_121cd";
       str2 = null;
       str2 = dt2.ToString();
       if ( str2 == null )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_894en!  str2 should Not be null here." );
	 }
       Console.Error.WriteLine( "Inf_581dx, str2.Length=="+ str2.Length );
       Console.Error.WriteLine( "Inf_582mx, Int64.MaxValue ticks is dt2.ToString()=="+ str2 );
       strLoc="Loc_122ce";
       lo8a = Int64.MaxValue;
       try {
       dt3 = dt2.AddTicks( lo8a );
       ++iCountErrors;
       Console.WriteLine( s_strTFAbbrev +"Err_871qm!  ");
       } catch (ArgumentOutOfRangeException aexc) {
       Console.WriteLine( "Info_4988v! Caught expected exception, aexc=="+aexc.Message);
       } catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine( "Error_498yv! Incorrect exception thrown, exc=="+exc);
       }
       }
       strLoc = "Loc_472uw";
       dt2 = DateTime.Now;
       try
	 {
	 dt2.AddTicks(Int64.MaxValue);
	 iCountErrors++;
	 Console.WriteLine("Err_34987ts! Unexpected exception thrown,");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_34987ts! Wrong exception thrown, {0}", ex.GetType().Name);
	 }
       dt2 = DateTime.Now;
       try
	 {
	 dt2.AddTicks(Int64.MinValue);
	 iCountErrors++;
	 Console.WriteLine("Err_34987ts! Unexpected exception thrown,");
	 }
       catch(ArgumentOutOfRangeException){}
       catch(Exception ex)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_34987ts! Wrong exception thrown, {0}", ex.GetType().Name);
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
   Co1518AddTicks cbA = new Co1518AddTicks();
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
