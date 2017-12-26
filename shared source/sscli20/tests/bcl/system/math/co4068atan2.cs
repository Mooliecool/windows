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
using System.Text;
public class Co4068Atan2
{
 public Boolean runTest()
   {
   Console.Out.WriteLine( "Co4068Atan2.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double dubExpected = 0;
   double dubEquation = 0;
   double dubLow = 0;
   double dubHigh = 0;
   double dubX = 0;
   double dubY = 0;
   do
     {
     try
       {
       dubX = -1;
       dubY = +1;
       dubExpected = 2.356194;  
       dubLow  = 2.356192 ;  
       dubHigh = 2.356196 ;
       dubEquation = Math.Atan2( dubY, dubX );
       ++iCountTestcases;
       Console.WriteLine( "Info_25720iee: dubEquation=="+ dubEquation.ToString() +" (dubExpected=="+ dubExpected.ToString() +")" );
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find Error_2495den, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubX = +1;
       dubY = +1;
       dubExpected = +0.78539816339744832;  
       dubLow  = +0.78538 ;
       dubHigh = +0.78541 ;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       Console.WriteLine( "Info_35720iee: dubEquation=="+ dubEquation.ToString() +" (dubExpected=="+ dubExpected.ToString() +")" );
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find Error_3495den, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubX = -1;
       dubY = -1;
       dubExpected = -2.356194;  
       dubLow  = -2.357 ;  
       dubHigh = -2.355 ;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       Console.WriteLine( "Info_45720iee: dubEquation=="+ dubEquation.ToString() +" (dubExpected=="+ dubExpected.ToString() +")" );
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find Error_4495den, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.558599;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 5;
       dubY = 8;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       Console.WriteLine( "Info_1863hin: dubEquation=="+ dubEquation.ToString() +" (dubExpected=="+ dubExpected.ToString() +")" );
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_d903m, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 1.00388;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = Math.PI;
       dubY = 2;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_xc85, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.808448;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = Math.PI;
       dubY = 3;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_c93n, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.66577;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = Math.PI;
       dubY = 4;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_d8n3, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.560982;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = Math.PI;
       dubY = 5;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_c93j, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.4823479;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = Math.PI;
       dubY = 6;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_n4f2, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.421854;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = Math.PI;
       dubY = 7;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_c9wq, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.308169;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 1;
       dubY = Math.PI;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_cn36, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.566911;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 2;
       dubY = Math.PI;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_95kg, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.762347;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 3;
       dubY = Math.PI;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_prl3, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.9050225;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 4;
       dubY = Math.PI;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_lh62, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 1.009814;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 5;
       dubY = Math.PI;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ssi3, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 1.08845; 
       dubLow = ( dubExpected - 0.01 );
       dubHigh = ( dubExpected + 0.01 );
       dubX = 6;
       dubY = Math.PI;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  0 ;
	 sblMsg.Append( "POINTTOBREAK: find E_d93h, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 sblMsg.Length =  0 ;
	 sblMsg.Append( "EXTENDEDINFO: find E_d98K, dubLow,dubEquation,dubHigh: " + dubLow + " ," + dubEquation + " ," + dubHigh );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 1.1489416;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 7;
       dubY = Math.PI;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_fi94, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.785398;  
       dubLow = ( dubExpected - 0.1 );
       dubHigh = ( dubExpected + 0.1 );
       dubX = 1;
       dubY = 1;
       dubEquation = Math.Atan2( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubLow <= dubEquation && dubEquation <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_34ht, Atan2 not equal to expected value" );
	 Console.WriteLine( sblMsg.ToString() );
	 break;
	 }
       }
     catch ( Exception Exc )
       {
       ++iCountErrors;
       sblMsg.Length =  99 ;
       sblMsg.Append( "POINTTOBREAK: find E_f3h5, Generic Exception Caught, Exc.ToString() == " );
       sblMsg.Append( Exc.ToString() );
       Console.WriteLine( sblMsg.ToString() );
       break;
       }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Write( "Math\\Co4068Atan2.cs: paSs.  iCountTestcases==" );
     Console.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Write( "Co4068Atan2.cs iCountErrors==" );
     Console.WriteLine( iCountErrors );
     Console.WriteLine( "PATHTOSOURCE: ClassLib\\Test\\Plain\\System\\Math\\Co4068Atan2.cs   FAiL !" );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co4068Atan2 cbA = new Co4068Atan2();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.WriteLine( "EXTENDEDINFO: FAiL!  Find E_999zzz, Uncaught Exception caught in main()! ClassLib\\Test\\Plain\\System\\Math\\Co4068Atan2.cs" );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.WriteLine( sblW.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
