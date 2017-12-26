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
using System.Globalization;
using System.Text;
using System;
public class Co4069Ceiling
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4069Ceiling.cs  runTest() started." );
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   double doubleVal1 = 0;
   double doubleexpectedValue = 0;
   String stVal = null;
   do
     {
     try
       {
       ++iCountTestcases;
       for ( int a = 1 ; a < 1000 ; ++a )
	 {
	 stVal = "1." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 doubleexpectedValue = 2.0;
	 if ( Math.Ceiling( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   sblMsg.Append( "POINTTOBREAK: find E_v82m, Ceiling not equal to expected Value == " + doubleexpectedValue);
	   sblMsg.Append( " Where in loop == " + a );
	   Console.Error.WriteLine( sblMsg.ToString() );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 1 ; a < 1000 ; ++a )
	 {
	 stVal = "-1." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 doubleexpectedValue = -1.0;
	 if ( Math.Ceiling( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   sblMsg.Append( "POINTTOBREAK: find E_v32k, Ceiling not equal to expected Value == " + doubleexpectedValue);
	   sblMsg.Append( "  Where in loop == " + a );
	   Console.Error.WriteLine( sblMsg.ToString() );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 1 ; a < 1000 ; ++a )
	 {
	 stVal = "2147483645." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 doubleexpectedValue = 2147483646;
	 if ( Math.Ceiling( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   sblMsg.Append( "POINTTOBREAK: find E_9v2h, Ceiling not equal to expected Value == " + doubleexpectedValue);
	   sblMsg.Append( "  Where in loop == " + a );
	   Console.Error.WriteLine( sblMsg.ToString() );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 1 ; a < 1000 ; ++a )
	 {
	 stVal = "-2147483647." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 doubleexpectedValue = -2147483647;
	 if ( Math.Ceiling( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   sblMsg.Append( "POINTTOBREAK: find E_b83j, Ceiling not equal to expected Value == " + doubleexpectedValue);
	   sblMsg.Append( "  Where in loop == " + a );
	   Console.Error.WriteLine( sblMsg.ToString() );
	   break;
	   }
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
     Console.Error.Write( "Math\\Co4069Ceiling.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4069Ceiling.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4069Ceiling.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4069Ceiling cbA = new Co4069Ceiling();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4069Ceiling.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
