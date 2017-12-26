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
public class Co4063IEEERemainder
{
 private static double NegativeZero = BitConverter.Int64BitsToDouble(unchecked((long)0x8000000000000000));
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4063IEEERemainder.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double doubleVal2 = 0;
   double doubleexpectedValue = 0;
   Int64 l1;
   Int64 l2;
   do
     {
     try
       {
       ++iCountTestcases;
       doubleVal1 = (double)2.0;
       doubleVal2 = (double)21.0;
       doubleexpectedValue = (double)2.0;
       if ( Math.IEEERemainder( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Remainder not equal to expected doubleexpectedValue == " + doubleexpectedValue);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)21.0;
       doubleVal2 = (double)2.0;
       doubleexpectedValue = (double)1.0;
       if ( Math.IEEERemainder( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_vd33, Remainder not equal to expected doubleexpectedValue == " + doubleexpectedValue);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)1.79769e+307;
       doubleVal2 = (double)-1.79769e+307;
       doubleexpectedValue = (double)0.0;
       if ( Math.IEEERemainder( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_m49f, Remainder not equal to expected doubleexpectedValue == " + doubleexpectedValue);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)1.79769e+307;
       doubleVal2 = (double)-1.0;
       doubleexpectedValue = (double)0.0;
       if ( Math.IEEERemainder( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_fg43, Remainder not equal to expected doubleexpectedValue == " + doubleexpectedValue);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)1.79769e+307;
       doubleVal2 = (double)1.0;
       doubleexpectedValue = (double)0.0;
       if ( Math.IEEERemainder( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_ldq11, Remainder not equal to expected doubleexpectedValue == " + doubleexpectedValue);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)1.0;
       doubleVal2 = (double)1.79769e+307;
       doubleexpectedValue = (double)1.0;
       if ( Math.IEEERemainder( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g26, Remainder not equal to expected doubleexpectedValue == " + doubleexpectedValue);
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)1.0;
       doubleVal2 = (double)-1.79769e+307;
       doubleexpectedValue = (double)1.0;
       if ( Math.IEEERemainder( doubleVal1, doubleVal2) != doubleexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_sp25, Remainder not equal to expected doubleexpectedValue == " + doubleexpectedValue);
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
     ++iCountTestcases;
     if(Math.IEEERemainder(10, 6) != -2){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     ++iCountTestcases;
     if(Math.IEEERemainder(19, 2) != -1){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     if(Math.IEEERemainder(21, 2) != 1){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     ++iCountTestcases;
     if(Math.IEEERemainder(18, 2) != 0){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     l1 = unchecked((long)0x8000000000000000);
     l2 = BitConverter.DoubleToInt64Bits(Math.IEEERemainder(-18, 2));
     if(l1 != l2){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     if(Math.IEEERemainder(0, 2) != 0){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     l1 = unchecked((long)0x8000000000000000);
     l2 = BitConverter.DoubleToInt64Bits(Math.IEEERemainder(NegativeZero, 2));
     if(0 != l2 && l1 != l2){
     iCountErrors++;
     Console.WriteLine("Err_3947sdg! Wrong value returned, ");
     }
     ++iCountTestcases;
     if(!Math.IEEERemainder(18, 0).Equals(Double.NaN)){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     if(!Math.IEEERemainder(18, NegativeZero).Equals(Double.NaN)){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     if(!Math.IEEERemainder(0, 0).Equals(Double.NaN)){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     ++iCountTestcases;
     if(!Math.IEEERemainder(0, NegativeZero).Equals(Double.NaN)){
     iCountErrors++;
     Console.WriteLine("Err_3422897sg! Wrong value returned, ");
     }
     }
   while ( false );
   if ( iCountErrors == 0 )
     {
     Console.Error.Write( "Math\\Co4063IEEERemainder.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4063IEEERemainder.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4063IEEERemainder.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4063IEEERemainder cbA = new Co4063IEEERemainder();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4063IEEERemainder.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
