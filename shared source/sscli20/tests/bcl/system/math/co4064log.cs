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
public class Co4064Log
{
 public Boolean runTest()
   {
   Console.Out.WriteLine( "Co4064Log.cs  runTest() started." );
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
       dubExpected = 0.773976;  
       dubOne = ( dubExpected - 0.01 );
       dubTwo = ( dubExpected + 0.01 );
       dubX = 5;
       dubY = 8;
       dubEquation = Math.Log( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_as25, Atan2 not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 0.0;  
       dubOne = ( dubExpected - 0.01 );
       dubTwo = ( dubExpected + 0.01 );
       dubX = 1;
       dubY = 1.79769e+308;
       dubEquation = Math.Log( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_g90h, Atan2 not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 9.0;  
       dubX = 1000000000;
       dubY = 10;
       dubEquation = Math.Log( dubX, dubY );
       ++iCountTestcases;
       if (
	   GetPropSmallDivByBig( dubEquation ,dubExpected ) > +0.01
	   ||  GetPropSmallDivByBig( dubEquation ,dubExpected ) < -0.01
	   )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_g53l, dubEquation ==" );
	 sblMsg.Append( dubEquation );
	 sblMsg.Append( ", dubExpected ==" );
	 sblMsg.Append( dubExpected );
	 sblMsg.Append( " ,Proportion==" );
	 sblMsg.Append( GetPropSmallDivByBig( dubEquation ,dubExpected ) );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 1.0;  
       dubX = 1000000000;
       dubY = 1000000000;
       dubEquation = Math.Log( dubX, dubY );
       ++iCountTestcases;
       if ( dubEquation != dubExpected )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_771d, Atan2 not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 2.0;  
       dubOne = ( dubExpected - 0.01 );
       dubTwo = ( dubExpected + 0.01 );
       dubX = 9;
       dubY = 3;
       dubEquation = Math.Log( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ur62, Atan2 not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 2.0;
       dubOne = ( dubExpected - 0.01 );
       dubTwo = ( dubExpected + 0.01 );
       dubX = 1.177901e+61;
       dubY = 3.433683e+30;
       dubEquation = Math.Log( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_lf67, Atan2 not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       dubExpected = 2.0;  
       dubOne = ( dubExpected - 0.01 );
       dubTwo = ( dubExpected + 0.01 );
       dubX = 1.79769e+308;
       dubY = 1.34078e+154;
       dubEquation = Math.Log( dubX, dubY );
       ++iCountTestcases;
       if ( ( dubOne <= dubEquation && dubEquation <= dubTwo ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ao33, Atan2 not equal to expected value" );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
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
     Console.Error.Write( "Math\\Co4064Log.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4064Log.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4064Log.cs   FAiL !"  );
     return false;
     }
   }
 public static double GetPropSmallDivByBig(double p_do8a,double p_do8b)
   {
   double do8Big;
   double do8Small;
   double do8Ret;
   double do8w;
   if ( p_do8a >= p_do8b )
     {
     do8Big = p_do8a;
     do8Small = p_do8b;
     }
   else
     {
     do8Big = p_do8b;
     do8Small = p_do8a;
     }
   if( do8Small == (double)0.0 && do8Big == (double)0.0)
     return (double)0.0;
   if ( do8Small <= (double)0.0  &&  do8Big <= (double)0.0 )
     {
     do8w = do8Big;
     do8Big = do8Small;
     do8Small = do8w;
     }
   if (
       ( do8Big < (double)0.0  &&  do8Small > (double)0.0 )
       ||  ( do8Big > (double)0.0  &&  do8Small < (double)0.0 )
       )
     {
     do8Ret = ( Double.NaN );  
     return do8Ret;
     }
   if ( do8Big == (double)0.0 )
     {
     do8Ret = ( Double.NaN );
     return do8Ret;
     }
   do8Ret = ( (double)(do8Small / do8Big) );
   do8Ret = (double)1.0 - do8Ret;
   return do8Ret;
   } 
 public static void Main( String[] args ) 
   {
   Boolean bResult = false; 
   StringBuilder sblW = null;
   Co4064Log cbA = new Co4064Log();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4064Log.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
