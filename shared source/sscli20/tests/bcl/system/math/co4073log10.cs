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
public class Co4073Log10
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4073Log10.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double doubleVal2 = 0;
   double expectedVal = 0;
   double exValHigh = 0;
   double exValLow = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       doubleVal1 = 10;
       expectedVal = 1;
       if ( Math.Log10( doubleVal1 ) != expectedVal )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_n0d2, Log10 not equal to expected Value == " + expectedVal );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 1e+308;
       doubleVal2 = 308.0;
       expectedVal = Math.Log10( doubleVal1 );
       if ( expectedVal != doubleVal2 )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_295b, Log10 not equal to expected Value == " + expectedVal );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 1e-308;
       doubleVal2 = -308.0;
       expectedVal = Math.Log10( doubleVal1 );
       if ( expectedVal != doubleVal2 )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_d3ka, Log10 not equal to expected Value == " + expectedVal );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 1e+0;
       doubleVal2 = 0.0;
       expectedVal = Math.Log10( doubleVal1 );
       if ( expectedVal != doubleVal2 )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_0l32, Log10 not equal to expected Value == " + expectedVal );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = 1e-0;
       doubleVal2 = 0.0;
       expectedVal = Math.Log10( doubleVal1 );
       if ( expectedVal != doubleVal2 )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_q25a, Log10 not equal to expected Value == " + expectedVal );
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
     Console.Error.Write( "Math\\Co4073Log10.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4073Log10.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4073Log10.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4073Log10 cbA = new Co4073Log10();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4073Log10.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
