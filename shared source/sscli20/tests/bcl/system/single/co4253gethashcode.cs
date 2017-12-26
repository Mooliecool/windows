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
public class Co4253GetHashCode
{
 internal static System.String strName = "Single.GetHashCode";
 internal static System.String s_strTFName = "Co4253GetHashCode";
 public virtual bool runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   System.String strCheckpoint = "CHECKPOINT";
   System.Console.Out.Write( strName );
   System.Console.Out.Write( ": " );
   System.Console.Out.Write( s_strTFName );
   System.Console.Out.WriteLine( " runTest started..." );
   System.Single wrSingle = ( 1.0f );
   iCountTestcases++;
   System.Console.Out.WriteLine( "GetHashCode:	Verify Single object hash code" );
   if ( wrSingle.GetHashCode() != GetExpectedHashCode( wrSingle ) )
     {
     System.Console.Error.Write( "GetHashCode error: " );
     System.Console.Error.Write( "Expected Result = <" );
     System.Console.Error.Write( Math.Abs( (int) wrSingle ) );
     System.Console.Error.Write( "> " );
     System.Console.Error.Write( "Returned Result = <" );
     System.Console.Error.Write( wrSingle.GetHashCode() );
     System.Console.Error.Write( "> " );
     iCountErrors |= 0x01;
     }
   wrSingle = Single.Epsilon;
   iCountTestcases++;
   if ( wrSingle.GetHashCode() != GetExpectedHashCode( wrSingle ) )
     {
     System.Console.Error.Write( "GetHashCode error: " );
     System.Console.Error.Write( "Expected Result = <" );
     System.Console.Error.Write( Math.Abs( (int) wrSingle ) );
     System.Console.Error.Write( "> " );
     System.Console.Error.Write( "Returned Result = <" );
     System.Console.Error.Write( wrSingle.GetHashCode() );
     System.Console.Error.Write( "> " );
     iCountErrors |= 0x01;
     }
   wrSingle = Single.NaN;
   iCountTestcases++;
   if ( wrSingle.GetHashCode() != GetExpectedHashCode( wrSingle ) )
     {
     System.Console.Error.Write( "GetHashCode error: " );
     System.Console.Error.Write( "Expected Result = <" );
     System.Console.Error.Write( Math.Abs( (int) wrSingle ) );
     System.Console.Error.Write( "> " );
     System.Console.Error.Write( "Returned Result = <" );
     System.Console.Error.Write( wrSingle.GetHashCode() );
     System.Console.Error.Write( "> " );
     iCountErrors |= 0x01;
     }
   System.Console.Error.Write( strName );
   System.Console.Error.Write( ": " );
   System.Console.Error.Write( s_strTFName );
   System.Console.Error.Write( " " );
   System.Console.Error.Write( "iCountErrors==" );
   System.Console.Error.WriteLine( iCountErrors );
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
 public unsafe int GetExpectedHashCode(float value) {
 float f = value;
 int v = *(int*)(&f);
 return v;
 }
 public static void Main(System.String[] args)
   {
   bool bResult = false;	
   Co4253GetHashCode oCbTest = new Co4253GetHashCode();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( System.Exception ex )
     {
     System.Console.Error.Write( s_strTFName );
     System.Console.Error.WriteLine(  ": Main() Uncaught exception" );
     System.Console.Error.WriteLine( ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
} 
