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
public class Co4071Exp
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4071Exp.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double doubleVal2 = 0;
   double doubleVal3 = 0;
   double dubExpected = 0;
   double dubHigh = 0;
   double dubLow = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       doubleVal1 = 1.0;
       doubleVal2 = Math.Exp( doubleVal1 );
       dubExpected = 2.718281828459;
       dubHigh = (dubExpected+0.1);
       dubLow = (dubExpected-0.1);
       if ( ( dubLow <= doubleVal2 && doubleVal2 <= dubHigh ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_d92j, Exp not equal to expected Value == " + dubExpected);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 100.0;
       doubleVal2 = Math.Exp( doubleVal1 );
       dubExpected = 2.688117141812e+43;
       dubHigh = (2.788117141812e+43);
       dubLow = (2.588117141812e+43);
       if ( ( dubLow <= doubleVal2 && doubleVal2 <= dubHigh  ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_fw6k, Exp not equal to expected Value == " + dubExpected);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = -100.0;
       doubleVal2 = Math.Exp( doubleVal1 );
       dubExpected = 3.720075976027e-44;
       dubHigh = ( dubExpected + 0.1 );
       dubLow = ( dubExpected - 0.1 );
       if ( ( dubLow <= doubleVal2 && doubleVal2 <= dubHigh  ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_2kh1, Exp not equal to expected Value == " + dubExpected);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 0.0;
       doubleVal2 = Math.Exp( doubleVal1 );
       dubExpected = 1;
       dubHigh = ( dubExpected + 0.1 );
       dubLow = ( dubExpected - 0.1 );
       if ( ( dubLow <= doubleVal2 && doubleVal2 <= dubHigh  ) == false )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ce68, Exp not equal to expected Value == " + dubExpected);
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
     Console.Error.Write( "Math\\Co4071Exp.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4071Exp.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4071Exp.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4071Exp cbA = new Co4071Exp();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4071Exp.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
