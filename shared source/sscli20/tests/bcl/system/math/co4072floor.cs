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
public class Co4072Floor
{
 public virtual bool runTest()
   {
   Console.Out.WriteLine( "Co4072Floor.cs  runTest() started." );
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
	 doubleexpectedValue = 1.0;
	 if ( Math.Floor( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_v82m, Ceiligig not equal to expected Value == " + doubleexpectedValue);
	   Console.Error.WriteLine( " Where in loop == " + a );
	   Console.Error.WriteLine( sblMsg.ToString() );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 1 ; a < 1000 ; ++a ) 
	 {
	 stVal = "-1." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 doubleexpectedValue = -2.0;
	 if ( Math.Floor( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_v32k, Ceiligig not equal to expected Value == " + doubleexpectedValue);
	   Console.Error.WriteLine( "  Where in loop == " + a );
	   Console.Error.WriteLine( sblMsg.ToString() );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 1 ; a < 1000 ; ++a )
	 {
	 stVal = "2147483645." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 doubleexpectedValue = 2147483645;
	 if ( Math.Floor( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_9v2h, Ceiligig not equal to expected Value == " + doubleexpectedValue);
	   Console.Error.WriteLine( "  Where in loop == " + a );
	   Console.Error.WriteLine( sblMsg.ToString() );
	   break;
	   }
	 }
       ++iCountTestcases;
       for ( int a = 1 ; a < 1000 ; ++a )
	 {
	 stVal = "-2147483646." + a;
	 doubleVal1 = Double.Parse (stVal, CultureInfo.InvariantCulture);
	 doubleexpectedValue = -2147483647;
	 if ( Math.Floor( doubleVal1 ) != doubleexpectedValue )
	   {
	   ++iCountErrors;
	   Console.Error.Write( "POINTTOBREAK: find E_b83j, Ceiligig not equal to expected Value == " + doubleexpectedValue);
	   Console.Error.WriteLine( "  Where in loop == " + a );
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
     Console.Error.Write( "Math\\Co4072Floor.cs: paSs.  iCountTestcases==" );
     Console.Error.WriteLine( iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.Write( "Co4072Floor.cs iCountErrors==" );
     Console.Error.WriteLine( iCountErrors );
     Console.Error.WriteLine(  "Co4072Floor.cs   FAiL !"  );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblW = null;
   Co4072Floor cbA = new Co4072Floor();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc )
     {
     bResult = false;
     Console.Error.WriteLine(  "Co4072Floor.cs"  );
     sblW = new StringBuilder( "EXTENDEDINFO: (E_999zzz) " );
     sblW.Append( exc.ToString() );
     Console.Error.WriteLine(  sblW.ToString()  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
