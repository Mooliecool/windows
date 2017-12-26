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
public class Co6051ToString_ulong
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strLastModCoder  = "";
 public static String s_strOrigCoder     = "";
 public static String s_strComponentBeingTested
   = "UInt64.ToString( UInt64 )";
 public static String s_strTFName        = "Co6051ToString_ulong.cs";
 public static String s_strTFAbbrev      = "Co6051";
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
     XenoUInt64 xeno = new XenoUInt64();
     UInt64 x;
     while( xeno.HasMoreValues() ) {
     x = xeno.GetNextValue();
     iCountTestcases++;
     if ( x.ToString(  ).Equals(  x.ToString( "d")) != true ) iCountErrors++;
     }
     } catch ( Exception e ) {
     Console.WriteLine( "Unexpected exception " + e + " thrown during runTest try." );
     iCountErrors++;
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
   Co6051ToString_ulong cbX = new Co6051ToString_ulong();
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
public class XenoUInt64 {
 Double LowEpsilon = .85;
 Double HighEpsilon = .85;
 Boolean LowFinished;
 Boolean HighFinished;
 Boolean AllFinished;
 UInt64 Minimum;
 UInt64 Maximum;
 UInt64 CurrentMid;
 public Boolean IsValidMid() {
 UInt64 m = Minimum;
 UInt64 M = Maximum;
 if ( m > M ) throw new ArgumentException( "Xeno:Min > Max");
 while ( m < 0 ) {
 M = M + 1;
 M = M + 1;
 }
 while ( m > 0 ) {
 m = m -1;
 m = m -1;
 }
 CurrentMid = UInt64.Parse(  ((M-m)/2).ToString() );
 return true;
 }
 public XenoUInt64() {
 LowFinished = false;
 HighFinished = false;
 AllFinished = false;
 Minimum = UInt64.MinValue;
 Maximum = UInt64.MaxValue;
 if ( IsValidMid() == false ) AllFinished = true;
 }
 public Boolean HasMoreValues() {
 return ( !AllFinished );
 }
 public UInt64 GetNextValue() {
 if ( AllFinished == false ) {
 if ( LowFinished == false ) {
 CurrentMid = (UInt64) (Convert.ToDouble(CurrentMid)*LowEpsilon);
 if ( CurrentMid <= 0 ) {
 IsValidMid();
 LowFinished = true;
 return Minimum;
 }
 return UInt64.Parse(  (Minimum + CurrentMid).ToString() );
 }
 if ( HighFinished == false ) {
 CurrentMid = (UInt64) (Convert.ToDouble(CurrentMid)*HighEpsilon);
 if ( CurrentMid <= 0 ) {
 IsValidMid();
 HighFinished = true;
 return Maximum;
 }
 return UInt64.Parse(  (Maximum - CurrentMid).ToString() );
 }
 AllFinished = true;
 IsValidMid();
 return CurrentMid;
 }
 throw new OverflowException( "No more values in Range" );
 }
}
