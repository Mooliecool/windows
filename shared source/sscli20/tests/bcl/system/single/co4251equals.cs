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
public class Co4251Equals
{
 internal static String strName = "Single.Equals";
 internal static String s_strTFName = "Co4251Equals";
 public virtual bool runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   String strCheckpoint = "CHECKPOINT";
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFName );
   Console.Out.WriteLine( " runTest started..." );
   Single	wrSingle1;
   Single	wrSingle2;
   Int32	wrInteger4a;
   Console.Out.WriteLine( "Equals:	Check two w.r.Single types with the same value for equality" );
   wrSingle1 = ( 1.0f );
   wrSingle2 = ( 1.0f );
   iCountTestcases++;
   if ( wrSingle1.Equals( wrSingle2 ) != true )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( true );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrSingle1.Equals( wrSingle2 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors++;
     }
   Console.Out.WriteLine( "Equals:	Check two w.r.Single types with different values" );
   wrSingle1 = ( 1.0f );
   wrSingle2 = ( -1.0f );
   iCountTestcases++;
   if ( wrSingle1.Equals( wrSingle2 ) != false )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrSingle1.Equals( wrSingle2 ) );
     Console.Error.WriteLine( "> " );
     iCountErrors++;
     }
   Console.Out.WriteLine( "Equals:	Check an w.r.Integer4 type with a w.r.Single type having the same value" );
   wrSingle1 = ( 1.0f );
   wrInteger4a = ( 1 );
   iCountTestcases++;
   if ( wrSingle1.Equals( wrInteger4a ) != true )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrSingle1.Equals( wrInteger4a ) );
     Console.Error.WriteLine( "> " );
     iCountErrors++;
     }
   Console.Out.WriteLine( "Equals:	Check an w.r.Integer4 type with a w.r.Single type with different values" );
   wrSingle1 = ( 1.0f );
   wrInteger4a = ( -1 );
   iCountTestcases++;
   if ( wrSingle1.Equals( wrInteger4a ) != false )
     {
     Console.Error.Write( "Equals error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( false );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrSingle1.Equals( wrInteger4a ) );
     Console.Error.WriteLine( "> " );
     iCountErrors++;
     }
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   Console.Error.Write( s_strTFName );
   Console.Error.Write( " " );
   Console.Error.Write( "iCountErrors==" );
   Console.Error.WriteLine( (iCountErrors) );
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
   Co4251Equals oCbTest = new Co4251Equals();
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
