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
using System.Text;
using System;
public class Co4082Log_double
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4082Log_double.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double dubExpected = 0;
   double dubEquation = 0;
   double dubOne = 0;
   double dubTwo = 0;
   double dubX = 0;
   double dubY = 0;
   do
     {
     try
       {
       dubExpected = 1.609437;  
       dubOne = ( dubExpected - 0.1 );
       dubTwo = ( dubExpected + 0.1 );
       dubX = 5;
       dubEquation = Math.Log( dubX );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_as25, Log not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.0;  
       dubOne = ( dubExpected - 0.01 );
       dubTwo = ( dubExpected + 0.01 );
       dubX = 1;
       dubEquation = Math.Log( dubX );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )	
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_g90h, Log not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 2.0794415;  
       dubOne = ( dubExpected - 0.1 );
       dubTwo = ( dubExpected + 0.1 );
       dubX = 8;
       dubEquation = Math.Log( dubX );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_g53l, Log not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 20.7232650;  
       dubOne = ( dubExpected - 0.1 );
       dubTwo = ( dubExpected + 0.1 );
       dubX = 1000000000;
       dubEquation = Math.Log( dubX );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_771d, Log not equal to expected value == " + dubEquation );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 666.0335943827;  
       dubOne = ( dubExpected - 0.1 );
       dubTwo = ( dubExpected + 0.1 );
       dubX = 1.79769e+289;
       dubEquation = Math.Log( dubX );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ur62, Log not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       try
	 {
	 dubX = 0;
	 dubEquation = Math.Log( dubX );
	 dubExpected = ( -1.0/0.0 ); 
	 if ( dubEquation != dubExpected )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine( "POINTTOBREAK: Find E_EX33, Log of 0 != Negative Infinity" );
	   }
	 }
       catch ( Exception Exc )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine( "POINTTOBREAK: Find E_EX44, Threw A generic Exception == " + Exc.ToString() );
	 }
       }
     catch ( Exception Exc )
       {
       ++iCountErrors;
       sblMsg.Length =  99 ;
       sblMsg.Append( "POINTTOBREAK: find E_f3h5, Generic Exception Caught, Exc.ToString() == " );
       sblMsg.Append( Exc.ToString() );
       Console.Error.WriteLine( sblMsg.ToString() );
       break;
       }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Math\\Co4082Log_double.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4082Log_double.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4082Log_double.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4082Log_double cbA = new Co4082Log_double();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4082Log_double.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
