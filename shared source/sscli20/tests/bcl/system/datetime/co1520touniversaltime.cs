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
public class Co1520ToUniversalTime
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.ToUniversalTime()";
 public static String s_strTFName        = "Co1520ToUniversalTime.cs";
 public static String s_strTFAbbrev      = "Co1520";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source at "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   DateTime dt3;
   int in4CompareLocalVersusUniversal = 0;
   int in4a = 0;
   try
     {
     do
       {
       strLoc = "Loc_110hh";
       dt2 = new DateTime( 2000,2,28 ,23,59,59 );  
       ++iCountTestcases;
       if ( DateTime.IsLeapYear( dt2.Year ) != true )
	 {
	 ++iCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_066ly!" );
	 }
       dt3 = dt2.ToUniversalTime();
       in4CompareLocalVersusUniversal = DateTime.Compare( dt2 ,dt3 );
       if ( in4CompareLocalVersusUniversal < 0 )
	 {
	 in4a = dt3.Day;
	 ++iCountTestcases;
	 if ( in4a != 29 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_027vz!  in4a=="+ in4a );
	   }
	 }
       if ( in4CompareLocalVersusUniversal > 0 )
	 {
	 in4a = dt3.Day;
	 ++iCountTestcases;
	 if ( in4a != 28 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_026tz!  in4a=="+ in4a );
	   }
	 }
       if ( in4CompareLocalVersusUniversal == 0 )
	 {
	 in4a = dt3.Day;
	 ++iCountTestcases;
	 if ( in4a != 28 )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_025mz!  in4a=="+ in4a );
	   }
	 }
       DateTime dtMin;
       DateTime dtGMTMin = new DateTime();
       DateTime dtMax;
       DateTime dtGMTMax = new DateTime();
       strLoc = "Loc_130ey";
       dtMin = new DateTime( 1,1,1 ,0,0,0 );
       ++iCountTestcases;
       try{
       dtGMTMin = dtMin.ToUniversalTime();
       }catch ( Exception ex){
       ++iCountErrors;
       Console.WriteLine( "Err_7482354dsg! Exception thrown!" + ex );
       }
       dtMax = new DateTime( 9999,12,31 ,23,59,59 );
       ++iCountTestcases;
       try{
       dtGMTMax = dtMax.ToUniversalTime();
       Console.WriteLine(dtMax);
       }catch (Exception ex){
       ++iCountErrors;
       Console.WriteLine("Err_049sc!  Exception thrown, " + ex.GetType().Name);
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
   Boolean bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1520ToUniversalTime cbA = new Co1520ToUniversalTime();
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
