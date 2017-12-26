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
public class Co6008FromString
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "DateTime.Parse(String)";
 public static String s_strTFName        = "Co6008FromString.cs";
 public static String s_strTFAbbrev      = "Co6008";
 public static String s_strTFPath        = "";
 public static Int32 DBGLEV = 0;  
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 Boolean m_verbose = false;
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   String strBaseLoc;
   StringBuilder sblMsg = new StringBuilder( 99 );
   m_verbose = false;
   DateTime dexpected = new DateTime();
   String dstr  = "";
   String dstr2 = "";
   try
     {
     m_strLoc = "Loc_normalTests";
     int Y = 100;    
     int M = 2;      
     int D = 3;      
     int h = 4;      
     int m = 5;      
     int s = 42;     
     int ampm = 0;
     int Days = 1;
     for( Y=1999; Y < 2000; Y++ ) {
     for( M=11; M<=12; M++) {
     switch( M ) {
     case 1:  Days = 31; break;
     case 2:
       Days = 28;
       if ( Y%4 == 0 ) {
       if( Y%100 == 0 ) {
       if ( Y%400 == 0 ) Days = 29;
       }
       else Days = 29;
       }
       break;
     case 3:  Days = 31; break;
     case 4:  Days = 30; break;
     case 5:  Days = 31; break;
     case 6:  Days = 30; break;
     case 7:  Days = 31; break;
     case 8:  Days = 31; break;
     case 9:  Days = 30; break;
     case 10: Days = 31; break;
     case 11: Days = 30; break;
     case 12: Days = 31; break;
     }
     for( D=1; D<=Days;D+=5 ) {
     for( h = 0; h < 24; h++ ) {
     for ( m = 0; m < 60; m += 5 ) {
     dexpected = new DateTime( Y,M,D,h,m,s );
     dstr =  M + "/" + D + "/" + Y + " " + h + ":" + m + ":" + s ;
     ampm = h;
     if ( h < 12 ) {
     if (h == 0) ampm = 12;
     dstr2 =  M + "/" + D + "/" + Y + " " + ampm + ":" + m + ":" + s + " AM";
     }
     else {
     ampm = h - 12;
     dstr2 =  M + "/" + D + "/" + Y + " " + ampm + ":" + m + ":" + s + " PM";
     }
     TestValue( "             " + dstr, dexpected, "<" );
     TestValue( dstr + "             ", dexpected, ">" );
     TestValue( "             " + dstr + "             ", dexpected, "<>" );
     TestValue( dstr, dexpected, "." );
     TestValue( dstr2, dexpected, "/" );
     }       
     }           
     }               
     }                   
     }                       
     m_strLoc = "Loc_exepTests";
     TestException( null, "System.ArgumentNullException", "A" );
     TestException( "2/29/1900 12:29:30 AM", "System.FormatException", "A" );
     TestException( "", "System.FormatException", "F" );
     DateTime dta;
     DateTime dtb;
     DateTime dtc;
     DateTime dtd;
     TimeSpan ts1;
     DateTime dc1;
     String str1;
     Double da;
     Int64 i8a;
     dtc = new DateTime(1900, 1, 1, 0, 0, 0);
     dtd = new DateTime(1999, 12, 31, 23, 59, 0);
     i8a = (dtd.Ticks - dtc.Ticks)/10;
     ts1 = new TimeSpan(i8a);
     dta = dtc;
     strBaseLoc="1300_";
     for(int ii = 0; ii < 10; ii++)
       {
       m_strLoc = (new StringBuilder(strBaseLoc)).Append(ii).ToString();
       ++iCountTestcases;
       if (dta.ToString().Equals(DateTime.Parse(dta.ToString()).ToString()) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( (new StringBuilder( s_strTFAbbrev)).Append("Err_320ye_").Append(ii).Append("  expected==").Append(dta.ToString()).Append(" Returned== ").Append(DateTime.Parse(dta.ToString()).ToString()));
	 }
       dta = dta.Add(ts1);
       }
     Console.Write(Environment.NewLine);
     Console.WriteLine( "Total Tests Ran: " + iCountTestcases + " Failed Tests: " + iCountErrors );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( "Error Err_8888yyy ("+ s_strTFAbbrev +")!  Unexpected exception thrown sometime after m_strLoc=="+ m_strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
     return true;
     }
   else
     {
     Console.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 private void TestValue( String test, DateTime expected, String moniker ) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestValue'"+test+"'";
 try {
 DateTime dtest = DateTime.Parse( test );
 if ( dtest != expected ) {
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"'-> "+dtest+", expected "+expected );
 }
 } catch ( Exception e ) {
 Console.WriteLine( e +"->"+ m_strLoc );
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"' expected "+expected );
 }
 }
 private void TestException( String test, String expected, String moniker) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestException'"+test+"'";
 try {
 DateTime dtest = DateTime.Parse( test );
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "No Exception Thrown " +"->"+ m_strLoc );
 Console.WriteLine( "Failed: '"+ expected +"' expected. '"+ test +"' did not throw an exception!"); 
 }
 catch ( Exception e ) {
 if( e.GetType().FullName.Equals(expected) ) return;
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + e +"->"+ m_strLoc );
 Console.WriteLine( "Failed: '"+ expected +"' expected, '"+e.ToString()+"' received. "); 
 return;
 }
 }
 public static void Main( String[] args ) 
   {
   Environment.ExitCode = 1;  
   Boolean bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co6008FromString cbX = new Co6008FromString();
   try
     {
     bResult = cbX.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( "FAiL!  Error Err_9999zzz ("+ s_strTFAbbrev +")!  Uncaught Exception caught fell to Main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFPath + s_strTFName );
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
} 
