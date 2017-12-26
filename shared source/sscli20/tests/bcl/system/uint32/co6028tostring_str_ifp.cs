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
using System.Globalization;
public class Co6028Format_str_ifp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "UInt32.ToString( String, IFormatProvider )";
 public static String s_strTFName        = "Co6028Format_str_ifp.cs";
 public static String s_strTFAbbrev      = "Co6028";
 public static String s_strTFPath        = "";
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   Console.WriteLine("");
   try
     {
     m_strLoc = "Loc_normalTests";
     UInt32 primativeUIntA = 100;
     UInt32 primativeUIntB = 200;
     UInt32 testUIntA = (UInt32)primativeUIntA ;
     UInt32 testUIntAA = (UInt32) primativeUIntA;
     UInt32 testUIntB = (UInt32) primativeUIntB;
     NumberFormatInfo nfi = new NumberFormatInfo();
     nfi.CurrencySymbol =			"[$US dollars]";
     nfi.CurrencyDecimalDigits =		4;
     nfi.NegativeSign =				"_unsigned??_";
     nfi.NumberDecimalDigits =		4;
     nfi.NumberGroupSeparator =      "";
     nfi.CurrencyGroupSeparator =    "";
     UInt32[] primativeUInt = { 
       0,
       100,
       1000,
       10000,
       100,
       UInt32.MinValue,
       UInt32.MaxValue, 
     };
     String[] currencyResults = new String[ primativeUInt.Length ]; 
     String[] decimalResults = new String[ primativeUInt.Length ]; 
     String[] fixedResults = new String[ primativeUInt.Length ]; 
     String[] generalResults = new String[ primativeUInt.Length ]; 
     String[] numberResults = new String[ primativeUInt.Length ]; 
     String[] sciResults = new String[ primativeUInt.Length ]; 
     String[] hexResults = new String[ primativeUInt.Length ]; 
     UInt32 bb;
     UInt32 div;
     UInt32 remainder;
     Char ch;
     int[] Cgroups = nfi.CurrencyGroupSizes; 
     int[] Ngroups = nfi.NumberGroupSizes; 
     String currencyTemp;
     String numberTemp;
     String sciTemp;
     String sciExp;
     int sciExponent;
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     sciTemp =  "" + primativeUInt[i] ;
     sciResults[i] = sciTemp[0] + nfi.NumberDecimalSeparator;
     sciTemp = sciTemp.Substring( 1 ); 
     sciExponent = sciTemp.Length;
     sciTemp = sciTemp.PadRight( 6, '0' );
     sciTemp = sciTemp.Substring( 0 , 6 );
     sciExp =  "" + sciExponent ;
     sciExp = sciExp.PadLeft( 3, '0' );
     sciExp = sciExp.Substring( sciExp.Length - 3 );
     sciResults[i] = sciResults[i] + sciTemp + "E" + nfi.PositiveSign + sciExp;
     currencyTemp =  "" + primativeUInt[i] ;
     currencyResults[i] = currencyTemp;
     if ( Cgroups[0] > 0 ) {
     if ( currencyTemp.Length > Cgroups[0] ) { 
     currencyResults[i] = "";
     for( int j = (currencyTemp.Length - Cgroups[0]); j > 0; j = j - Cgroups[0] ) {
     currencyResults[i] = nfi.CurrencyGroupSeparator +
       currencyTemp.Substring( j, Cgroups[0] ) + currencyResults[i];
     currencyTemp = currencyTemp.Substring(0,j);
     }
     currencyResults[i] = currencyTemp + currencyResults[i];
     }
     }
     currencyResults[i] =  nfi.CurrencySymbol + currencyResults[i] + nfi.CurrencyDecimalSeparator + "".PadRight( nfi.CurrencyDecimalDigits, '0' ) ;
     numberTemp =  "" + primativeUInt[i] ;
     numberResults[i] = numberTemp;
     if ( Ngroups[0] > 0 ) {
     if ( numberTemp.Length > Ngroups[0] ) { 
     numberResults[i] = "";
     for( int j = (numberTemp.Length - Ngroups[0]); j > 0; j = j - Ngroups[0] ) {
     numberResults[i] = nfi.NumberGroupSeparator +
       numberTemp.Substring( j, Ngroups[0] ) + numberResults[i];
     numberTemp = numberTemp.Substring(0,j);
     }
     numberResults[i] = numberTemp + numberResults[i];
     }
     }
     numberResults[i] =  numberResults[i] + nfi.NumberDecimalSeparator + "".PadRight( nfi.NumberDecimalDigits, '0' );
     decimalResults[i] =  primativeUInt[i] + "";
     fixedResults[i] =  primativeUInt[i] + "." + "".PadRight( nfi.NumberDecimalDigits, '0' );
     generalResults[i] =  primativeUInt[i] + "";
     bb = primativeUInt[i];
     hexResults[i] =  "" ;
     while ( bb > 0 ) {
     div = (UInt32)(bb/16);
     remainder = (UInt32)(bb%16);
     if ( remainder < 10 ) hexResults[i] = remainder + hexResults[i];
     else {
     switch( remainder ) {
     case 10 : ch = 'a'; break;
     case 11 : ch = 'b'; break;
     case 12 : ch = 'c'; break;
     case 13 : ch = 'd'; break;
     case 14 : ch = 'e'; break;
     case 15 : ch = 'f'; break;
     default: ch = ' ';break;
     }
     hexResults[i] = ch + hexResults[i];
     }
     bb = div;
     }
     if ( hexResults[i].Equals( "" ) ) hexResults[i] = "0";
     }
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #C." + iCountTestcases%primativeUInt.Length;
     m_strLoc = m_strLoc + "->" + currencyResults[i] + "<- Manufactured Results....";
     testUIntA = (UInt32) primativeUInt[i] ;
     if ( testUIntA.ToString( "C", nfi ).Equals( currencyResults[i] ) != true ) ErrorCode(testUIntA.ToString( "C", nfi ));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #D." + iCountTestcases%primativeUInt.Length;
     m_strLoc = m_strLoc + "->" + decimalResults[i];
     testUIntA = (UInt32) primativeUInt[i];
     if ( testUIntA.ToString( "D", nfi ).Equals( decimalResults[i] ) != true ) ErrorCode(testUIntA.ToString( "D", nfi ));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #E." + iCountTestcases%primativeUInt.Length;
     m_strLoc = m_strLoc + "->" + sciResults[i];
     testUIntA = (UInt32) primativeUInt[i];
     if ( testUIntA.ToString( "E", nfi ).Equals( sciResults[i] ) != true ) ErrorCode(testUIntA.ToString( "E", nfi ));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #F." + iCountTestcases%primativeUInt.Length;
     m_strLoc = m_strLoc + "->" + fixedResults[i];
     testUIntA = (UInt32) primativeUInt[i];
     if ( testUIntA.ToString( "F", nfi ).Equals( fixedResults[i] ) != true ) ErrorCode(testUIntA.ToString( "F", nfi ));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #G." + iCountTestcases%primativeUInt.Length;
     m_strLoc = m_strLoc + "->" + generalResults[i];
     testUIntA = (UInt32) primativeUInt[i];
     if ( testUIntA.ToString( "G", nfi ).Equals( generalResults[i] ) != true ) ErrorCode(testUIntA.ToString( "G", nfi ));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #N." + iCountTestcases%primativeUInt.Length;
     m_strLoc = m_strLoc + "->" + numberResults[i];
     testUIntA = (UInt32) primativeUInt[i];
     if ( testUIntA.ToString( "N", nfi ).Equals( numberResults[i] ) != true ) ErrorCode(testUIntA.ToString( "n", nfi ));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUInt.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #X." + iCountTestcases%primativeUInt.Length;
     m_strLoc = m_strLoc + "->" + hexResults[i];
     testUIntA = (UInt32) primativeUInt[i];
     if ( testUIntA.ToString( "x", nfi ).Equals( hexResults[i] ) != true ) ErrorCode(testUIntA.ToString( "X", nfi ));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "hexResults[" + i + "] = '" + hexResults[i] + "'" );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.WriteLine( "Error Err_8888yyy ("+ s_strTFAbbrev +")!  Unexpected exception thrown sometime after m_strLoc=="+ m_strLoc +" ,exc_general=="+ exc_general );
     }
   Console.Write(Environment.NewLine);
   Console.WriteLine( "Total Tests Ran: " + iCountTestcases + " Failed Tests: " + iCountErrors );
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
 public void ErrorCode( String erk ) {
 m_strLoc = m_strLoc + " tested function produced <" + erk + ">";
 throw new Exception( "Test failed." );
 }
 public void ErrorCode() {
 throw new Exception( "Test failed." );
 }
 public static void Main( String[] args ) 
   {
   Environment.ExitCode = 1;  
   Boolean bResult = false; 
   Co6028Format_str_ifp cbX= new Co6028Format_str_ifp();
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
