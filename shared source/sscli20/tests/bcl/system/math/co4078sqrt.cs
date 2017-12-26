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
public class Co4078Sqrt
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4078Sqrt.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double expectedVal = 0;
   double dubValHigh = 0;
   double dubValLow = 0;
   double dubMade1 = 0;
   double dubMade2 = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       expectedVal = 3.0;
       dubMade1 = 9.0;
       dubMade2 = Math.Sqrt( dubMade1 );
       if ( dubMade2 != expectedVal )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ds25 Square Root not equal to expected Value == " + expectedVal);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       expectedVal = 1.34078e+154;
       dubValHigh = ( 1.44078e+154 ); 
       dubValLow = ( 1.24078e+154 );  
       dubMade1 = 1.79769e+308;
       dubMade2 = Math.Sqrt( dubMade1 );
       if ( ( dubValLow <= dubMade2 && dubMade2 <= dubValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_a94h, Square Root not equal to expected Value == " + expectedVal);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       expectedVal = 6.3340e+49;
       dubValHigh = ( 6.4340e+49 ); 
       dubValLow = ( 6.2340e+49 );  
       dubMade1 = 4.0119919e+99;
       dubMade2 = Math.Sqrt( dubMade1 );
       if ( ( dubValLow <= dubMade2 && dubMade2 <= dubValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_h66d, Square Root not equal to expected Value == " + expectedVal);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       expectedVal = 6.395123e+49;
       dubValHigh = ( 6.495123e+49 ); 
       dubValLow = ( 6.295123e+49 );  
       dubMade1 = 4.08976e+99;
       dubMade2 = Math.Sqrt( dubMade1 );
       if ( ( dubValLow <= dubMade2 && dubMade2 <= dubValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_f92o, Square Root not equal to expected Value == " + expectedVal);
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
     Console.Error.Write( "Math\\Co4078Sqrt.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4078Sqrt.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4078Sqrt.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4078Sqrt cbA = new Co4078Sqrt();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4078Sqrt.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
