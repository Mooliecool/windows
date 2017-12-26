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
public class Co4070Cos
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4070Cos.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double dubExpected1 = 0;
   double expectedLow = 0;
   double expectedHigh = 0;
   double dubMade1 = 0;
   double dubMade2 = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       dubExpected1 = 0.5;
       expectedLow = ( dubExpected1-0.01 );
       expectedHigh = ( dubExpected1+0.01 );
       dubMade1 = ( Math.PI/3 );
       dubMade2 = ( Math.Cos( dubMade1 ) );
       if ( ( expectedLow <= dubMade2 && dubMade2 <= expectedHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_v82m, Cos not equal to expected Value == " + dubExpected1 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       dubExpected1 = 0.0;
       expectedLow = ( dubExpected1-0.01 );
       expectedHigh = ( dubExpected1+0.01 );
       dubMade1 = ( Math.PI/2 );
       dubMade2 = ( Math.Cos( dubMade1 ) );
       if ( ( expectedLow <= dubMade2 && dubMade2 <= expectedHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_9s75q, Cos not equal to expected Value == " + dubExpected1 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       dubExpected1 = 1.0;
       expectedLow = ( dubExpected1-0.01 );
       expectedHigh = ( dubExpected1+0.01 );
       dubMade1 = ( 0 );
       dubMade2 = ( Math.Cos( dubMade1 ) );
       if ( ( expectedLow <= dubMade2 && dubMade2 <= expectedHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ks92, Cos not equal to expected Value == " + dubExpected1 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       dubExpected1 = 0.86;
       expectedLow = ( dubExpected1-0.01 );
       expectedHigh = ( dubExpected1+0.01 );
       dubMade1 = ( Math.PI/6 );
       dubMade2 = ( Math.Cos( dubMade1 ) );
       if ( ( expectedLow <= dubMade2 && dubMade2 <= expectedHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_94fa, Cos not equal to expected Value == " + dubExpected1 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       dubExpected1 = 0.7071;
       expectedLow = ( dubExpected1-0.01 );
       expectedHigh = ( dubExpected1+0.01 );
       dubMade1 = ( Math.PI/4 );
       dubMade2 = ( Math.Cos( dubMade1 ) );
       if ( ( expectedLow <= dubMade2 && dubMade2 <= expectedHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_os46, Cos not equal to expected Value == " + dubExpected1 );
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
     Console.Error.Write( "Math\\Co4070Cos.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4070Cos.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4070Cos.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4070Cos cbA = new Co4070Cos();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4070Cos.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
