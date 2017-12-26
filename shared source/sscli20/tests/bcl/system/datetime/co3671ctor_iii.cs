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
public class Co3671ctor_iii
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.DateTime()";
 public static readonly String s_strTFName        = "Co3671ctor_iii.cs";
 public static readonly String s_strTFAbbrev      = "Co3671";
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
   int in4SetYear;
   int in4SetMonth;
   int in4SetDayOfMonth;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       in4SetYear = 1999;
       in4SetMonth = 2;
       in4SetDayOfMonth = 8;
       dt1 = new DateTime(in4SetYear, in4SetMonth, in4SetDayOfMonth);
       ++iCountTestcases;
       if ( dt1.Year != in4SetYear )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_320ye!  dt1.GetYear()=="+ dt1.Year );
	 }
       ++iCountTestcases;
       if ( dt1.Month != in4SetMonth )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_206xr!  dt1.GetMonth()=="+ dt1.Month );
	 }
       ++iCountTestcases;
       if ( dt1.Day != in4SetDayOfMonth )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_832se!  dt1.GetDay()=="+ dt1.Day );
	 }
       in4SetYear = 0;
       in4SetMonth = 2;
       in4SetDayOfMonth = 8;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(in4SetYear, in4SetMonth, in4SetDayOfMonth);
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
       in4SetYear = 10000;
       in4SetMonth = 2;
       in4SetDayOfMonth = 8;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(in4SetYear, in4SetMonth, in4SetDayOfMonth);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_034ce!  dt1=="+ dt1 );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_027wa!  , Wrong Exception thrown == " + ex  );
	 }
       in4SetYear = 1999;
       in4SetMonth = 0;
       in4SetDayOfMonth = 8;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(in4SetYear, in4SetMonth, in4SetDayOfMonth);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_964xe!  dt1=="+ dt1 );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_263rl!  , Wrong Exception thrown == " + ex  );
	 }
       in4SetYear = 1999;
       in4SetMonth = 13;
       in4SetDayOfMonth = 8;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(in4SetYear, in4SetMonth, in4SetDayOfMonth);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_734eo!  dt1=="+ dt1 );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_902ct!  , Wrong Exception thrown == " + ex  );
	 }
       in4SetYear = 1999;
       in4SetMonth = 2;
       in4SetDayOfMonth = 0;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(in4SetYear, in4SetMonth, in4SetDayOfMonth);
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
       in4SetYear = 1999;
       in4SetMonth = 2;
       in4SetDayOfMonth = 29;
       try
	 {
	 ++iCountTestcases;
	 dt1 = new DateTime(in4SetYear, in4SetMonth, in4SetDayOfMonth);
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_367qp!  dt1=="+ dt1 );
	 }
       catch (ArgumentException ex)
	 {
	 }
       catch (Exception ex)
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_035cr!  , Wrong Exception thrown == " + ex  );
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
   Co3671ctor_iii oCbTest = new Co3671ctor_iii();
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
