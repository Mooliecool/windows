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
public class Co1521ToLongDateString
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.ToLongDateString()";
 public static readonly String s_strTFName        = "Co1521ToLongDateString.cs";
 public static readonly String s_strTFAbbrev      = "Co1521";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source at "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   String strBaseLoc;
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Co1521_cons_strUdt[] cb_cs2Arr =
   {
     new Co1521_cons_strUdt( 1999,1,9 ,13,24,35 ,"Saturday, January 09, 1999" )
     ,new Co1521_cons_strUdt( 1,1,1 ,0,0,0 ,"Monday, January 01, 0001" )   
     ,new Co1521_cons_strUdt( 100,1,1 ,0,0,0 ,"Friday, January 01, 0100" )
     ,new Co1521_cons_strUdt( 9999,12,31 ,23,59,59 ,"Friday, December 31, 9999" )
   };
   DateTime dt2;
   String str2;
   try
     {
     do
       {
       strBaseLoc="Loc_100bb_";
       strLoc=strBaseLoc;
       for ( int ia = 0 ;ia < cb_cs2Arr.Length ;ia++ )
	 {
	 strLoc = strBaseLoc + ia.ToString();
	 cb_cs2Arr[ia].ConstructDateTime();
	 }
       strBaseLoc="Loc_120cc_";
       strLoc=strBaseLoc;
       for ( int ib = 0 ;ib < cb_cs2Arr.Length ;ib++ )
	 {
	 strLoc= strBaseLoc + ib.ToString();
	 try
	   {
	   strLoc= strBaseLoc + ib.ToString() + "_A";
	   dt2 = cb_cs2Arr[ib].GetDateTime();
	   strLoc= strBaseLoc + ib.ToString() + "_B";
	   str2 = dt2.ToLongDateString();   
	   strLoc= strBaseLoc + ib.ToString() + "_C";
	   ++iCountTestcases;
	   if ( cb_cs2Arr[ib].o_strUDT.Equals( str2 ) == false )
	     {
	     ++iCountErrors;
	     Console.WriteLine( s_strTFAbbrev +"Err_028hs! (ib=="+ ib +")  str2=="+ str2 );
	     }
	   }
	 catch ( Exception )
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_034pa! (ib=="+ ib +")  ,strLoc=="+ strLoc );
	   }
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
   Co1521ToLongDateString cbA = new Co1521ToLongDateString();
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
internal class Co1521_cons_strUdt
{
 public int o_in4Year = -1;
 public int o_in4Month = -1;
 public int o_in4Day = -1;
 public int o_in4Hour = -1;
 public int o_in4Minute = -1;
 public int o_in4Second = -1;
 public String o_strUDT = "Not yet initialized properly.";
 public DateTime o_dtPrimary;
 private Co1521_cons_strUdt() {}   
 public Co1521_cons_strUdt   
   (
    int p_in4Year ,int p_in4Month ,int p_in4Day
    ,int p_in4Hour ,int p_in4Minute ,int p_in4Second
    ,String p_strUDT
    )
   {
   o_in4Year = p_in4Year;
   o_in4Month = p_in4Month;
   o_in4Day = p_in4Day;
   o_in4Hour = p_in4Hour;
   o_in4Minute = p_in4Minute;
   o_in4Second = p_in4Second;
   o_strUDT = p_strUDT;
   }
 public virtual DateTime GetDateTime()
   {
   return o_dtPrimary;
   }
 public virtual void ConstructDateTime()
   {
   o_dtPrimary = new DateTime
     (
      o_in4Year
      ,o_in4Month
      ,o_in4Day
      ,o_in4Hour
      ,o_in4Minute
      ,o_in4Second
      );
   }
} 
