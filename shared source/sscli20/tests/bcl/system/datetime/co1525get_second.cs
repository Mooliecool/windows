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
public class Co1525get_Second
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.GetSecond()";
 public static readonly String s_strTFName        = "Co1525get_Second.cs";
 public static readonly String s_strTFAbbrev      = "Co1525";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   String strBaseLoc;
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   DateTime dt3;
   int in4SetCrucialTimeUnit = -1;
   int in4GottenTimeUnit = -2;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_330ye";
       in4SetCrucialTimeUnit = 2000;
       dt2 = new DateTime( in4SetCrucialTimeUnit ,2,29 ,1,12,23 );
       in4GottenTimeUnit = dt2.Year;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_320ye!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_331mo";
       in4SetCrucialTimeUnit = 12;
       dt2 = new DateTime( 2000 ,in4SetCrucialTimeUnit ,29 ,1,12,23 );
       in4GottenTimeUnit = dt2.Month;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_321mo!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_332da";
       in4SetCrucialTimeUnit = 29;
       dt2 = new DateTime( 2000,2 ,in4SetCrucialTimeUnit   ,1,12,23 );
       in4GottenTimeUnit = dt2.Day;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_322da!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_333ho";
       in4SetCrucialTimeUnit = 1;
       dt2 = new DateTime( 2000,2,29 ,in4SetCrucialTimeUnit ,12,23 );
       in4GottenTimeUnit = dt2.Hour;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_323ho!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_334mi";
       in4SetCrucialTimeUnit = 12;
       dt2 = new DateTime( 2000,2,29 ,1  ,in4SetCrucialTimeUnit ,23 );
       in4GottenTimeUnit = dt2.Minute;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_324mi!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_335si";
       in4SetCrucialTimeUnit = 23;
       dt2 = new DateTime( 2000,2,29 ,1,12 ,in4SetCrucialTimeUnit );
       in4GottenTimeUnit = dt2. Second;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_325se!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_776ms";
       in4SetCrucialTimeUnit = 0;  
       dt2 = new DateTime( 2000,2,29 ,1,12,23 );
       in4GottenTimeUnit = dt2.Millisecond;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_076ms!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_786ms";
       in4SetCrucialTimeUnit = 987;  
       dt3 = dt2.AddMilliseconds( (double)in4SetCrucialTimeUnit );
       in4GottenTimeUnit = dt3.Millisecond;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_176ms!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
	 }
       strLoc="Loc_037dy";
       in4SetCrucialTimeUnit = 60;  
       dt2 = new DateTime( 2000,2,29 ,1,12,23 );
       in4GottenTimeUnit = dt2.DayOfYear;
       ++iCountTestcases;
       if ( in4GottenTimeUnit != in4SetCrucialTimeUnit )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +" "+"Err_027dy!  ("+ in4SetCrucialTimeUnit
			    +")  in4GottenTimeUnit=="+ in4GottenTimeUnit );
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
   Co1525get_Second cbA = new Co1525get_Second();
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
