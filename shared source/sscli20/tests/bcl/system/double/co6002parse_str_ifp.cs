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
using System.Globalization;
public class Co6002FromString
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "Double.ToString(String, CultureInfo.InvariantCulture)";
 public static String s_strTFName        = "Co6002FromString.cs";
 public static String s_strTFAbbrev      = "Co6002";
 public static String s_strTFPath        = "";
 public static Int32 DBGLEV = 0;  
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 Boolean m_verbose = false;
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   StringBuilder sblMsg = new StringBuilder( 99 );
   m_verbose = false;
   try
     {
     m_strLoc = "Loc_normalTests";
     TestValue( "-10.32", -10.32, "." );
     TestValue( "       10.32        ", 10.32, "." );
     TestValue( "10        ", 10, "." );
     TestValue( "0", 0, "." );
     TestValue( "       10.5E6", 10.5E6, "." );
     TestValue( "1.79769313486232E307", 1.79769313486232E307, "^" );
     TestValue( "-1.79769313486232E307", -1.79769313486232E307, "^" );
     TestValue( "85403985042930928590489205840e-28594275904287690710985403985042930928590489205840285942759042876907109854039850429309285904892058402859427590428769071098540398504293092859048920584028594275904287690710985403985042930928590489205840285942759042876907109854039850429309285904892058402859427590428769071098540398", 0, "^" );
     TestValue( "    -8540398504293092859048920e-5840285942759042876907109854039850429309285904892058402859427590428769071098540398504293092859048920584028594275904287690710985403985042930928590489205840285942759042876907109854039850429309285904892058402859427590428769071098540398504293092859048920584028594275904287690710985", 0, "^" );
     TestValue( "4e00000000000000000001", 40, "." );
     TestValue( Double.PositiveInfinity.ToString(CultureInfo.InvariantCulture ), Double.PositiveInfinity, "^" );
     TestValue( Double.NegativeInfinity.ToString(CultureInfo.InvariantCulture ), Double.NegativeInfinity, "^" );
     TestValue( Double.NaN.ToString( CultureInfo.InvariantCulture ), Double.NaN, ":" );  
     TestValue( "    " + Double.NaN.ToString(CultureInfo.InvariantCulture ) + "        ", Double.NaN, ":" );
     TestValue( Double.NaN.ToString(CultureInfo.InvariantCulture) + "    ", Double.NaN, ":" );
     TestValue( "8540398504293092859048920584028594275904287690710985403933333333333333333333333333333300000000000000000000000000000000000000000000000000000000000084888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888855555555555555", 8.54039850429309E+306, "^" );
     TestValue( "    -8540398504293092859048920584028594275904287690710985403985042930928590489205840285942759042876907109854039850429309285904892058402859427590428769071098540398504293092859048920584028594275904287690710985403985042930928590489205840285942759042876907109854039850429309285904892058402859427590428769071098540", -8.54039850429309E+306, "^" );
     m_strLoc = "Loc_exepTests";
     TestException("exception", "System.FormatException", "f" );
     TestException("1-1.5", "System.FormatException", "f" );
     TestException("  ", "System.FormatException", "f" );
     TestException(null, "System.ArgumentNullException", "A" );
     TestException( "1.79769313486232E308", "System.OverflowException", "^" );
     TestException( "-1.79769313486232E308", "System.OverflowException", "^" );
     TestException( "85403985042930928590489205840e285942759042876907109854039850429309285904892058402859427590428769071098540398504293092859048920584028594275904287690710985403985042930928590489205840285942759042876907109854039850429309285904892058402859427590428769071098540398504293092859048920584028594275904287690710985403985", "System.OverflowException", "^" );
     TestException( "    -8540398504293092859048920e58402859427590428769071098540398504293092859048920584028594275904287690710985403985042930928590489205840285942759042876907109854039850429309285904892058402859427590428769071098540398504293092859048920584028594275904287690710985403985042930928590489205840285942759042876907109854", "System.OverflowException", "^" );
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
 private void TestValue( String test, Double expected, String moniker ) {
 iCountTestcases++;
 if ( m_verbose ) Console.Write(moniker);
 m_strLoc = "Loc_TestValue'"+test+"'";
 try {
 Double dtest = Double.Parse( test, CultureInfo.InvariantCulture );
 if ( Double.IsNaN( expected ) )
   {
   if ( ! Double.IsNaN( dtest ) )
     {
     Console.WriteLine( Environment.NewLine + "Failed: '"+test+"'-> "+dtest+", expected "+expected );
     }
   }
 else if ( Double.IsInfinity( expected ) )
   {
   if ( Double.IsPositiveInfinity( expected ) )
     {
     if ( ! Double.IsPositiveInfinity( dtest ) )
       {
       Console.WriteLine( Environment.NewLine + "Failed: '"+test+"'-> "+dtest+", expected "+expected );
       }
     }
   else if ( Double.IsNegativeInfinity( expected ) )
     {
     if ( ! Double.IsNegativeInfinity( dtest ) )
       {
       Console.WriteLine( Environment.NewLine + "Failed: '"+test+"'-> "+dtest+", expected "+expected );
       }
     }
   }
 else {
 Double max = Math.Max(dtest, expected);
 Double min = Math.Max(dtest, expected);
 if ( (min/max) < 0.00000001 ) {
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "Failed: '"+test+"'-> "+dtest+", expected "+expected );
 }
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
 Double.Parse( test, CultureInfo.InvariantCulture );
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + "No Exception Thrown " +"->"+ m_strLoc );
 Console.WriteLine( "Failed: '"+ expected +"' expected. '"+ test +"' did not throw an exception!"); 
 }
 catch ( Exception e ) {
 if( e.GetType().FullName.Equals( expected ) ) return;
 iCountErrors++;
 Console.WriteLine( Environment.NewLine + e +"->"+ m_strLoc );
 Console.WriteLine( "Failed: '"+ expected +"' expected, '"+e.ToString()+"' received. "); 
 return;
 }
 }
 public static void Main( String[] args ) 
   {
   Environment.ExitCode=1;  
   Boolean bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co6002FromString cbX = new Co6002FromString();
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
