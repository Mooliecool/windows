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
public class Co6016Format_u16_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "UInt16.ToString( UInt16, String )";
 public static String s_strTFName        = "Co6016Format_u16_str.cs";
 public static String s_strTFAbbrev      = "Co6016";
 public static String s_strTFPath        = "";
 int iCountErrors = 0;
 int iCountTestcases = 0;
 String m_strLoc="Loc_beforeRun";
 public Boolean runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   try
     {
     m_strLoc = "Loc_normalTests";
     NumberFormatInfo nfi = new NumberFormatInfo();
     UInt16[] primativeUShort = { 
       0,
       100,
       1000,
       10000,
       UInt16.MaxValue,
       UInt16.MinValue,
       unchecked((UInt16)(-10)),
       unchecked((UInt16)(-1 * UInt16.MaxValue))
     };
     String[] currencyResults = new String[ primativeUShort.Length ]; 
     String[] decimalResults = new String[ primativeUShort.Length ]; 
     String[] fixedResults = new String[ primativeUShort.Length ]; 
     String[] generalResults = new String[ primativeUShort.Length ]; 
     String[] numberResults = new String[ primativeUShort.Length ]; 
     String[] sciResults = new String[ primativeUShort.Length ]; 
     String[] hexResults = new String[ primativeUShort.Length ]; 
     UInt16 bb;
     UInt16 div;
     UInt16 remainder;
     Char ch;
     int[] Cgroups = nfi.CurrencyGroupSizes; 
     int[] Ngroups = nfi.NumberGroupSizes; 
     String currencyTemp;
     String numberTemp;
     String sciTemp;
     String sciExp;
     int sciExponent;
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     sciTemp =  "" + primativeUShort[i] ;
     sciResults[i] = sciTemp[0] + nfi.NumberDecimalSeparator;
     sciTemp = sciTemp.Substring( 1 ); 
     sciExponent = sciTemp.Length;
     sciTemp = sciTemp.PadRight( 6, '0' );
     sciTemp = sciTemp.Substring( 0 , 6 );
     sciExp =  "" + sciExponent ;
     sciExp = sciExp.PadLeft( 3, '0' );
     sciExp = sciExp.Substring( sciExp.Length - 3 );
     sciResults[i] = sciResults[i] + sciTemp + "E" + nfi.PositiveSign + sciExp;
     currencyTemp =  "" + primativeUShort[i] ;
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
     numberTemp =  "" + primativeUShort[i] ;
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
     decimalResults[i] =  primativeUShort[i] + "";
     fixedResults[i] =  primativeUShort[i] + "." + "".PadRight( nfi.NumberDecimalDigits, '0' );
     generalResults[i] =  primativeUShort[i] + "";
     bb = primativeUShort[i];
     hexResults[i] =  "" ;
     while ( bb > 0 ) {
     div = (UInt16)(bb/16);
     remainder = (UInt16)(bb%16);
     if ( remainder < 10 ) hexResults[i] = remainder + hexResults[i];
     else {
     switch( remainder ) {
     case 10 : ch = 'a'; break;
     case 11 : ch = 'b'; break;
     case 12 : ch = 'c'; break;
     case 13 : ch = 'd'; break;
     case 14 : ch = 'e'; break;
     case 15 : ch = 'f'; break;
     default: ch = ' '; break;
     }
     hexResults[i] = ch + hexResults[i];
     }
     bb = div;
     }
     if ( hexResults[i].Equals( "" ) ) hexResults[i] = "0";
     }
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #C." + iCountTestcases;
     m_strLoc = m_strLoc + "->" + currencyResults[i];
     String strPrimitiveCurrency = primativeUShort[i].ToString( "C") ;
     strPrimitiveCurrency = strPrimitiveCurrency.Replace("$" , new String( new char[]{(char)164} ) );
     if ( strPrimitiveCurrency.Equals( currencyResults[i] ) != true ) ErrorCode(primativeUShort[i].ToString( "C"));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #D." + iCountTestcases;
     m_strLoc = m_strLoc + "->" + decimalResults[i];
     if ( primativeUShort[i].ToString( "D").Equals( decimalResults[i] ) != true ) ErrorCode(primativeUShort[i].ToString( "d"));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #E." + iCountTestcases;
     m_strLoc = m_strLoc + "->" + sciResults[i];
     if ( primativeUShort[i].ToString( "E").Equals( sciResults[i] ) != true ) ErrorCode(primativeUShort[i].ToString( "E"));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #F." + iCountTestcases;
     m_strLoc = m_strLoc + "->" + fixedResults[i];
     if ( primativeUShort[i].ToString( "f").Equals( fixedResults[i] ) != true ) ErrorCode(primativeUShort[i].ToString( "F"));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #G." + iCountTestcases;
     m_strLoc = m_strLoc + "->" + generalResults[i];
     if ( primativeUShort[i].ToString( "G").Equals( generalResults[i] ) != true ) ErrorCode(primativeUShort[i].ToString( "g"));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #N." + iCountTestcases;
     m_strLoc = m_strLoc + "->" + numberResults[i];
     if ( primativeUShort[i].ToString( "n").Equals( numberResults[i] ) != true ) ErrorCode(primativeUShort[i].ToString( "n"));
     } catch( Exception e ) {
     iCountErrors++;
     Console.WriteLine( "Testcase["+iCountTestcases+"]" );
     Console.WriteLine( "Exception:" + e );
     Console.WriteLine( "StrLoc = '"+m_strLoc+"'" );
     }
     }
     for( int i = 0; i < primativeUShort.Length; i++ ) {
     try {
     iCountTestcases++; 
     m_strLoc = "Starting testgroup #X." + iCountTestcases;
     m_strLoc = m_strLoc + "->" + hexResults[i];
     if ( primativeUShort[i].ToString( "x").Equals( hexResults[i] ) != true ) ErrorCode(primativeUShort[i].ToString( "X"));
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
   Co6016Format_u16_str cbX= new Co6016Format_u16_str();
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
