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
public class Co4074Pow
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4074Pow.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double doubleVal2 = 0;
   double dubPOWVal = 0;
   double expectedVal = 0;
   double exValHigh = 0;
   double exValLow = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       doubleVal1 = 2.0;
       doubleVal2 = 10.0;
       expectedVal = 1024.0;
       exValHigh = (expectedVal+0.1);
       exValLow = (expectedVal-0.1);
       dubPOWVal = Math.Pow( doubleVal1, doubleVal2 );
       if ( ( exValLow <= dubPOWVal && dubPOWVal <= exValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_di13, Power not equal to expected Value == " + expectedVal);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 2.0;
       doubleVal2 = 100.0;
       expectedVal = 1.267650600228e+30;
       exValHigh = (1.367650600228e+30);   
       exValLow = (1.167650600228e+30);    
       dubPOWVal = Math.Pow( doubleVal1, doubleVal2 );
       if ( ( exValLow <= dubPOWVal && dubPOWVal <= exValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_cj84, Power not equal to expected Value == " + expectedVal);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 2.0;
       doubleVal2 = 1000.0;
       dubPOWVal = Math.Pow( doubleVal1, doubleVal2 );
       expectedVal = 1.071508607186e+301;
       exValHigh = ( 1.171508607186e+301 );  
       exValLow = ( 0.971508607186e+301 );   
       if ( ( exValLow <= dubPOWVal && dubPOWVal <= exValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_c8w1, Power not equal to expected Value == " + expectedVal);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 2.0;
       doubleVal2 = -10.0;
       expectedVal = 0.0009765625;
       exValHigh = (expectedVal+0.1);
       exValLow = (expectedVal-0.1);
       dubPOWVal = Math.Pow( doubleVal1, doubleVal2 );
       if ( ( exValLow <= dubPOWVal && dubPOWVal <= exValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_h92a, Power not equal to expected Value == " + expectedVal);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 2.0;
       doubleVal2 = -100.0;
       expectedVal = 7.88860905221e-31;
       exValHigh = (expectedVal+0.1);
       exValLow = (expectedVal-0.1);
       dubPOWVal = Math.Pow( doubleVal1, doubleVal2 );
       if ( ( exValLow <= dubPOWVal && dubPOWVal <= exValHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_a92k, Power not equal to expected Value == " + expectedVal);
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
     Console.Error.Write( "Math\\Co4074Pow.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4074Pow.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4074Pow.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4074Pow cbA = new Co4074Pow();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4074Pow.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
