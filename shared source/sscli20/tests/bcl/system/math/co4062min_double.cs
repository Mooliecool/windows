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
public class Co4062Min_double
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4062Min_double.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double doubleVal2 = 0;
   double doubleexpectedValue = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       doubleVal1 = (double)1.79769e+307;  
       doubleVal2 = (double)1.79769e+308;  
       doubleexpectedValue = (double)1.79769e+307;
       if ( Math.Min( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( " doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)1.79769e+308;  
       doubleVal2 = (double)1.79769e+307;  
       doubleexpectedValue = (double)1.79769e+307;
       if ( Math.Min( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Min value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)-1.79769e+306;  
       doubleVal2 = (double)-1.79769e+307;  
       doubleexpectedValue = (double)-1.79769e+307;
       if ( Math.Min( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Min value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)-1.79769e+307;  
       doubleVal2 = (double)-1.79769e+306;  
       doubleexpectedValue = (double)-1.79769e+307;
       if ( Math.Min( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ve73, Min value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)1.79769e+308;   
       doubleVal2 = (double)-1.79769e+307;  
       doubleexpectedValue = (double)-1.79769e+307;
       if ( Math.Min( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_s03p, Min value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)-1.79769e+307;  
       doubleVal2 = (double)1.79769e+308;   
       doubleexpectedValue = (double)-1.79769e+307;
       if ( Math.Min( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find f90k, Min value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)0;  
       doubleVal2 = (double)0;  
       doubleexpectedValue = (double)0;
       if ( Math.Min( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_67dq, Min value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
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
     Console.Error.Write( "Math\\Co4062Min_double.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4062Min_double.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4062Min_double.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4062Min_double cbA = new Co4062Min_double();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4062Min_double.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
