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
public class Co3673ctor_long
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.DateTime(String)";
 public static readonly String s_strTFName        = "Co3673ctor_long.cs";
 public static readonly String s_strTFAbbrev      = "Co3673";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   DateTime dt1;
   DateTime dt2;
   int in4Year;
   int in4Month;
   int in4DayOfMonth;
   long lTicks;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       lTicks = DateTime.MaxValue.Ticks;
       dt1 = new DateTime(lTicks);
       in4Year = 9999;
       ++iCountTestcases;
       if ( dt1.Year != in4Year )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_320ye!  dt1.GetYear()=="+ dt1.Year );
	 }
       in4Month = 12;
       ++iCountTestcases;
       if ( dt1.Month != in4Month )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_206xr!  dt1.GetMonth()=="+ dt1.Month );
	 }
       in4DayOfMonth = 31;
       ++iCountTestcases;
       if ( dt1.Day != in4DayOfMonth )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_832se!  dt1.GetDay()=="+ dt1.Day );
	 }
       lTicks = DateTime.MinValue.Ticks;
       dt1 = new DateTime(lTicks);
       in4Year = 1;
       ++iCountTestcases;
       if ( dt1.Year != in4Year )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_421ze!  dt1.GetYear()=="+ dt1.Year );
	 }
       in4Month = 1;
       ++iCountTestcases;
       if ( dt1.Month != in4Month )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_935xe!  dt1.GetMonth()=="+ dt1.Month );
	 }
       in4DayOfMonth = 1;
       ++iCountTestcases;
       if ( dt1.Day != in4DayOfMonth )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_034xe!  dt1.GetDay()=="+ dt1.Day );
	 }
       dt2 = new DateTime(1999, 2, 9);
       lTicks = dt2.Ticks;
       dt1 = new DateTime(lTicks);
       in4Year = 1999;
       ++iCountTestcases;
       if ( dt1.Year != in4Year )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_106cw!  dt1.GetYear()=="+ dt1.Year );
	 }
       in4Month = 2;
       ++iCountTestcases;
       if ( dt1.Month != in4Month )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_294zz!  dt1.GetMonth()=="+ dt1.Month );
	 }
       in4DayOfMonth = 9;
       ++iCountTestcases;
       if ( dt1.Day != in4DayOfMonth )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_935qp!  dt1.GetDay()=="+ dt1.Day );
	 }
       lTicks = DateTime.MinValue.Ticks-1;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(lTicks);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_835xr!  dt1=="+ dt1 );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  , Wrong Exception thrown == " + ex  );
	 }
       lTicks = DateTime.MaxValue.Ticks+1;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(lTicks);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_165dd!  dt1=="+ dt1 );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_934xr!  , Wrong Exception thrown == " + ex  );
	 }
       } while ( false );
     }
   catch (Exception exc_general)
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
   Co3673ctor_long oCbTest = new Co3673ctor_long();
   try
     {
     bResult = oCbTest.runTest();
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
