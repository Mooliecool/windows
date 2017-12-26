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
public class Co4262ctor
{
 static String strName = "Double.Constructor";
 static String s_strTFName = "Co4262ctor";
 public Boolean runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   String strCheckpoint = "CHECKPOINT";
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFName );
   Console.Out.WriteLine( " runTest started..." );
   int ii = 0;
   Double wrDouble;
   long[] lWorkTable = 
   {
     155252, -155252, 1000000, -1000000, 0
   };
   double[] dblWorkTable = 
   {
     +155252.0, -155252.0, +1.0e+6, -1.0e+6, 0.0
   };
   Console.Out.WriteLine( "Construct and verify w.r.Double values from long");
   for ( ii = 0; ii < dblWorkTable.Length; ++ii )
     {
     iCountTestcases++;
     wrDouble = (Double)( lWorkTable[ii] );
     if ( wrDouble != dblWorkTable[ii] )
       {
       Console.Error.Write( "Double constructor error: " );
       Console.Error.Write( "Test #" );
       Console.Error.Write( ii );
       Console.Error.Write( " " );
       Console.Error.Write( "Expected Result = <" );
       Console.Error.Write( dblWorkTable[ii] );
       Console.Error.Write( "> " );
       Console.Error.Write( "Returned Result = <" );
       Console.Error.Write( wrDouble );
       Console.Error.WriteLine( "> " );
       iCountErrors |= 0x01;
       break;
       }
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
   Boolean bResult = false;	
   Co4262ctor oCbTest = new Co4262ctor();
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
