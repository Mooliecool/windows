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
public class Co4264GetHashCode
{
 internal static String strName = "Double.GetHashCode";
 internal static String s_strTFName = "Co4264GetHashCode";
 public virtual bool runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFName );
   Console.Out.WriteLine( " runTest started..." );
   Double wrDouble;
   wrDouble = (Double)1.0f;
   iCountTestcases++;
   if ( wrDouble.GetHashCode() != GetExpectedHashCode( wrDouble ) )
     {
     Console.Error.Write( "GetHashCode error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( Math.Abs( (int) wrDouble ) );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble.GetHashCode() );
     Console.Error.Write( "> " );
     iCountErrors |= 0x01;
     }
   iCountTestcases++;
   wrDouble = (Double) Double.NaN;
   if ( wrDouble.GetHashCode() != GetExpectedHashCode( wrDouble ) )
     {
     Console.Error.Write( "GetHashCode error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( Math.Abs( (int) wrDouble ) );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble.GetHashCode() );
     Console.Error.Write( "> " );
     iCountErrors |= 0x01;
     }
   iCountTestcases++;
   wrDouble = (Double) Double.Epsilon;
   if ( wrDouble.GetHashCode() != GetExpectedHashCode( wrDouble ) )
     {
     Console.Error.Write( "GetHashCode error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( Math.Abs( (int) wrDouble ) );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble.GetHashCode() );
     Console.Error.Write( "> " );
     iCountErrors |= 0x01;
     }
   iCountTestcases++;
   wrDouble = (Double) Double.PositiveInfinity;
   if ( wrDouble.GetHashCode() != GetExpectedHashCode( wrDouble ) )
     {
     Console.Error.Write( "GetHashCode error: " );
     Console.Error.Write( "Expected Result = <" );
     Console.Error.Write( Math.Abs( (int) wrDouble ) );
     Console.Error.Write( "> " );
     Console.Error.Write( "Returned Result = <" );
     Console.Error.Write( wrDouble.GetHashCode() );
     Console.Error.Write( "> " );
     iCountErrors |= 0x01;
     }
   Console.Error.Write( strName );
   Console.Error.Write( ": " );
   Console.Error.Write( s_strTFName );
   Console.Error.Write( " " );
   Console.Error.Write( "iCountErrors==" );
   Console.Error.WriteLine( iCountErrors );
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
 public unsafe int GetExpectedHashCode(double m_value) {
 long value = *(long*)(&m_value);
 return ((int)value) ^ ((int)(value >> 32));
 }
 public static void Main(String[] args)
   {
   bool bResult = false;	
   Co4264GetHashCode oCbTest = new Co4264GetHashCode();
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
