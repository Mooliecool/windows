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
public class Co4055Max_double
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4055Max_double.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double doubleVal2 = 0;
   double flexpectedValue = 0;
   do
     {
     try
       {
       ++iCountTestcases;
       doubleVal1 = (double)2147483646.0;  
       doubleVal2 = (double)2147483647.0;  
       flexpectedValue = (double)2147483647.0;
       if ( Math.Max( doubleVal1, doubleVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Max value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)2147483647.0;  
       doubleVal2 = (double)2147483646.0;  
       flexpectedValue = (double)2147483647.0;
       if ( Math.Max( doubleVal1, doubleVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Max value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)-2147483646.0;  
       doubleVal2 = (double)-2147483647.0;  
       flexpectedValue = (double)-2147483646.0;
       if ( Math.Max( doubleVal1, doubleVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_5g2f, Max value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)-2147483647.0;  
       doubleVal2 = (double)-2147483646.0;  
       flexpectedValue = (double)-2147483646.0;
       if ( Math.Max( doubleVal1, doubleVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_z55p, Max value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)2147483647.0;  
       doubleVal2 = (double)-2147483647.0;  
       flexpectedValue = (double)2147483647;
       if ( Math.Max( doubleVal1, doubleVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find E_s03p, Max value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)-2147483647.0;  
       doubleVal2 = (double)2147483647.0;  
       flexpectedValue = (double)2147483647;
       if ( Math.Max( doubleVal1, doubleVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Append( "POINTTOBREAK: find f90k, Max value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       doubleVal1 = (double)0.0;  
       doubleVal2 = (double)0.0;  
       flexpectedValue = (double)0.0;
       if ( Math.Max( doubleVal1, doubleVal2) != flexpectedValue )
	 {
	 ++iCountErrors;
	 sblMsg.Length =  99 ;
	 sblMsg.Append( "POINTTOBREAK: find E_67dq, Max value did not come up, Expected doubleVal1 == " + doubleVal1);
	 sblMsg.Append( "doubleVal2 == " + doubleVal2 );
	 Console.Error.WriteLine( sblMsg.ToString() );
	 break;
	 }
       ++iCountTestcases;
       if(!Double.NaN.Equals(Math.Max(Double.NaN, 5))){
       iCountErrors++;
       Console.WriteLine("Err_394tsg! Wrong value retunred");
       }
       if(!Double.NaN.Equals(Math.Max(5, Double.NaN))){
       iCountErrors++;
       Console.WriteLine("Err_394tsg! Wrong value retunred");
       }
       if(!Double.NaN.Equals(Math.Max(Double.NaN, Double.NaN))){
       iCountErrors++;
       Console.WriteLine("Err_394tsg! Wrong value retunred");
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
     Console.Error.Write( "Math\\Co4055Max_double.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4055Max_double.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4055Max_double.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4055Max_double cbA = new Co4055Max_double();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4055Max_double.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
