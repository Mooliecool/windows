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
public class Co6023ToString_uint
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strLastModCoder  = "";
 public static readonly String s_strOrigCoder     = "";
 public static readonly String s_strComponentBeingTested
   = "UInt16.ToString( UInt16 )";
 public static readonly String s_strTFName        = "Co6023ToString_uint.cs";
 public static readonly String s_strTFAbbrev      = "Co6023";
 public static readonly String s_strTFPath        = "";
 internal int iCountErrors = 0;
 internal int iCountTestcases = 0;
 internal String m_strLoc="Loc_beforeRun";
 internal bool m_verbose = false;
 public virtual bool runTest()
   {
   Console.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strComponentBeingTested +"  ,Source ver "+ s_strDtTmVer );
   String strBaseLoc;
   try
     {
     m_strLoc = "Loc_normalTests";
     String testStr = "";
     XenoUInt16 xeno = new XenoUInt16();
     UInt16 x;
     while( xeno.HasMoreValues() ) {
     x = ((UInt16) xeno.GetNextValue() );
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
 public virtual void ErrorCode( String erk ) {
 m_strLoc = m_strLoc + " tested function produced <" + erk + ">";
 throw new Exception( "Test failed." );
 }
 public virtual void ErrorCode() {
 throw new Exception( "Test failed." );
 }
 public static void Main( String[] args )
   {
   Environment.ExitCode = 1;  
   bool bResult = false; 
   Co6023ToString_uint cbX = new Co6023ToString_uint();
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
public class XenoUInt16 {
 internal Double LowEpsilon = ((Double) .85 );
 internal Double HighEpsilon = ((Double) .85 );
 internal Boolean LowFinished;
 internal Boolean HighFinished;
 internal Boolean AllFinished;
 internal UInt16 Minimum;
 internal UInt16 Maximum;
 internal UInt16 CurrentMid;
 public virtual Boolean IsValidMid() {
 UInt16 m = Minimum;
 UInt16 M = Maximum;
 if ( m > M ) throw new ArgumentException( "Xeno:Min > Max");
 while ( m < 0 ) {
 m++;
 M++;
 }
 while ( m > 0 ) {
 m--;
 M--;
 }
 CurrentMid = UInt16.Parse(  ((M-m)/2).ToString() );
 return true;
 }
 public XenoUInt16() {
 LowFinished = false;
 HighFinished = false;
 AllFinished = false;
 Minimum = UInt16.MinValue;
 Maximum = UInt16.MaxValue;
 if ( IsValidMid() == false ) AllFinished = true;
 }
 public virtual Boolean HasMoreValues() {
 return ( !AllFinished );
 }
 public virtual UInt16 GetNextValue() {
 if ( AllFinished == false ) {
 if ( LowFinished == false ) {
 CurrentMid = (UInt16) (Convert.ToDouble(CurrentMid)*LowEpsilon);
 if ( CurrentMid <= 0 ) {
 IsValidMid();
 LowFinished = true;
 return Minimum;
 }
 return UInt16.Parse(  (Minimum + CurrentMid).ToString() );
 }
 if ( HighFinished == false ) {
 CurrentMid = (UInt16) (Convert.ToDouble(CurrentMid)*HighEpsilon);
 if ( CurrentMid <= 0 ) {
 IsValidMid();
 HighFinished = true;
 return Maximum;
 }
 return UInt16.Parse(  (Maximum - CurrentMid).ToString() );
 }
 AllFinished = true;
 IsValidMid();
 return CurrentMid;
 }
 throw new OverflowException( "No more values in Range" );
 }
}
