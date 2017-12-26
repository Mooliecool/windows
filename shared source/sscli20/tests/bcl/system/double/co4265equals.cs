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
public class Co4265Equals
{
 internal static String strName = "Double.Equals";
 internal static String s_strTFName = "Co4265Equals";
 public virtual bool runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   String strCheckpoint = "CHECKPOINT";
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFName );
   Console.Out.WriteLine( " runTest started..." );
   Double	wrDouble1;
   Double	wrDouble2;
   Int32	wrInteger41;
   iCountTestcases++;
   Console.Out.WriteLine( "Equals:	Check two w.r.Double types with the same value for equality" );
   iCountTestcases++;
   wrDouble1 = (Double)( 1.0 );
   wrDouble2 = (Double)( 1.0 );
   if ( wrDouble1.Equals( wrDouble2 ) != true )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( true );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble1.Equals( wrDouble2 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors |= 0x01;
     }
   Console.Out.WriteLine( "Equals:	Check two w.r.Double types with different values" );
   iCountTestcases++;
   wrDouble1 = (Double)( 1.0 );
   wrDouble2 = (Double)( -1.0 );
   if ( wrDouble1.Equals( wrDouble2 ) != false )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble1.Equals( wrDouble2 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors |= 0x02;
     }
   Console.Out.WriteLine( "Equals:	Check an w.r.Integer4 type with a w.r.Double type having the same value" );
   iCountTestcases++;
   wrDouble1 = (Double)( 1.0 );
   wrInteger41 = (Int32)( 1 );
   if ( wrDouble1.Equals( wrInteger41 ) != true )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble1.Equals( wrInteger41 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors |= 0x04;
     }
   Console.Out.WriteLine( "Equals:	Check an w.r.Integer4 type with a w.r.Double type with different values" );
   iCountTestcases++;
   wrDouble1 = (Double)( 1.0 );
   wrInteger41 = (Int32)( -1 );
   if ( wrDouble1.Equals( wrInteger41 ) != false )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble1.Equals( wrInteger41 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors |= 0x08;
     }
   Console.Out.WriteLine( "Equals:	Check an w.r.Integer4 type with a w.r.Double type with NaN" );
   iCountTestcases++;
   wrDouble1 = Double.NaN;
   wrInteger41 = unchecked((int) Double.NaN);
   if ( wrDouble1.Equals( wrInteger41 ) != false )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble1.Equals( wrInteger41 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors |= 0x16;
     }
   Console.Out.WriteLine( "Equals:	Check an w.r.Double type with a w.r.Double type with NaN" );
   iCountTestcases++;
   wrDouble1 = Double.NaN;
   wrDouble2 = Double.NaN;
   if ( wrDouble1.Equals( wrDouble2 ) != true )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble1.Equals( wrDouble2 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors |= 0x32;
     }
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   Console.Error.Write( s_strTFName );
   Console.Error.Write( " " );
   Console.Error.Write( "iCountErrors==" );
   Console.Error.WriteLine( (Int32)(iCountErrors) );
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;	
   Co4265Equals oCbTest = new Co4265Equals();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     Console.Error.Write( s_strTFName );
     Console.Error.WriteLine(  ": Main() Uncaught exception" );
     Console.Error.WriteLine( ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
} 
